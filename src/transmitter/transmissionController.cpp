#include <iostream>
#include <memory>
#include <thread>
#include <utility>
#include "transmissionController.hpp"
#include "filePacketConsumer.hpp"
#include "filePacketProducer.hpp"
#include "textPacketProducer.hpp"

using namespace std::chrono_literals;

TransmissionController::TransmissionController(SocketAddress address,
                                               std::shared_ptr<std::queue<DataEntity>> inputDataQueue,
                                               std::shared_ptr<std::queue<DataEntity>> outputDataQueue,
                                               std::shared_ptr<std::mutex> inputMutex,
                                               std::shared_ptr<std::mutex> outputMutex,
                                               bool &isClosing) : socket(address), closing(isClosing) {
    this->inputDataQueue = std::move(inputDataQueue);
    this->outputDataQueue = std::move(outputDataQueue);
    this->inputMutex = std::move(inputMutex);
    this->outputMutex = std::move(outputMutex);
}

void TransmissionController::establishConnection() {
    // if the connection is set to hot open that means we don't need to do anything to initialize connection
    // packet producers will do the job themselves
    if (this->hot && this->activeOpen)
        return;

    if (this->activeOpen) {
        this->socket.send(builder.getStart());
        // debug
        std::cout << "listening for ack\n";
        auto response = this->socket.listen(5s); // wait for the ack
        if (!response || !response->isAck()) {
            throw ConnectionException("Connection establishment failed");
        }
        this->nextSequenceNumber = response->getSequenceNumber();
        // debug
        std::cout << "connection established\n";
        // connection established
    } else {
        // debug
        std::cout << "listening for starts\n";
        auto response = this->socket.listen(30s);
        if (!response || !response->isOpen()) {
            throw ConnectionException("Connection establishment failed");
        }
        this->nextSequenceNumber = response->getSequenceNumber() + 1;
        // debug
        std::cout << "sending ack\n";
        this->socket.send(builder.getAcknowledgement(response->getSequenceNumber()));
        // debug
        std::cout << "connection established\n";

        // if received connection is hot open, transfer the packet to the input queue ready for processing
        if (response->isFile() || response->isText()) {
            this->inputPackets.push_back(response.value());
        }
    }
}

// this method deals with pretty much all communication
// it manages ARQ, ACKing in general, queues input and output packets
// also manages keep-alive and how many times transmission is retried
void TransmissionController::run(std::chrono::duration<int, std::milli> timeout) {
    this->isRunning = true;
    this->establishConnection();
    while (this->isRunning) {
        // debug
        std::cout << "foreign: " << this->nextSequenceNumber << "; ";
        std::cout << "mine: " << this->builder.getSequenceNumber() << "\n";
        retryCount++;
        if(this->closing && this->isProducingFinished() && this->isConsumingFinished()) {
            // initiate closing sequence
            this->closeConnection();
            this->isRunning = false;
            return;
        }
        // push data packets to the queue
        this->produceDataPackets();
        // resend all expired packets
        this->resendAll();
        // send packets from the queue
        this->flushQueue();

        // if we have a packet waiting to be processed, process it
        if (this->hasNext()) {
            Packet output = this->inputPackets.front();
            this->inputPackets.pop_front();
            this->consumeDataPacket(output);
        } else {
            if (this->retryCount > TransmissionController::maximumRetries)
                throw ConnectionException("Connection timed out");
            auto option = this->socket.receive(timeout);

            this->processPacket(option);
        }
    }
    this->closeConnection();
}

void TransmissionController::processPacket(std::optional<Packet> optionalPacket) {
    if (!optionalPacket) {
        // if no packet is accepted, send a keep-alive to check if the connection is functional
        this->send(this->builder.getKeepAlive());
        // debug
        std::cout << "sending keep-alive\n";
        return;
    }

    // some packet is received, check its checksum
    if (!optionalPacket->validate()) {
        // the packet received is damaged
        this->socket.send(builder.getNak(optionalPacket->getSequenceNumber()));
        // debug
        std::cout << "sending nak\n";
        return;
    }

    // process previous packets
    if (optionalPacket->getSequenceNumber() < this->nextSequenceNumber) {
        // re-acknowledgement the packet
        this->socket.send(this->builder.getAcknowledgement(optionalPacket->getSequenceNumber()));
        // debug
        std::cout << "sending ack for previous\n";
        return;
    }

    this->retryCount = 0; // if a packet was received, it is valid, and it is not a duplicate

    // process ack packets
    if (optionalPacket->isAck()) {
        // debug
        std::cout << "ack received\n";
        if (this->processAcknowledgement(optionalPacket.value())) {
            this->retryCount = 0;
        }
        return;
    }

    // process nak packets
    if (optionalPacket->isNak()) {
        this->resend(optionalPacket->getAckNumber());
    }

    // process keep-alive packets
    if (optionalPacket->isKeepAlive()) {
        // debug
        std::cout << "keep-alive received, sending ack\n";
        this->acknowledgePacket(optionalPacket.value());
        return;
    }

    if (optionalPacket->isClose()) {
        // debug
        std::cout << "received closing packet\n";
        this->acknowledgePacket(optionalPacket.value());
        this->closing = true;

        // closing packet may be also hot closing packet
        if (!optionalPacket->isFile() && !optionalPacket->isText() && !optionalPacket->isFragment())
            return;
    }

    // a valid packet is received
    if (this->inputPackets.empty()) {
        this->inputPackets.push_front(optionalPacket.value());
        return;
    }

    // insert the packet so its place is ordered
    for (auto iterator = this->inputPackets.begin(); iterator != this->inputPackets.end(); iterator++) {
        if (iterator->getSequenceNumber() > optionalPacket->getSequenceNumber()) {
            this->inputPackets.insert(iterator, optionalPacket.value());
            return;
        }
    }
}

// returns true if input queue contains a packet
bool TransmissionController::hasNext() {
    return !this->inputPackets.empty() && this->nextSequenceNumber == this->inputPackets.front().getSequenceNumber();
}

// sends packets from the queue
void TransmissionController::flushQueue() {
    // send new packets
    while (this->sentPackets.size() < TransmissionController::windowSize && !this->outputPackets.empty()) {
        socket.send(this->outputPackets.front());
        this->sentPackets.emplace_back(this->outputPackets.front(), std::chrono::steady_clock::now());
        this->outputPackets.pop();
    }
}

void TransmissionController::resendAll() {
    // resend packets
    for (const auto &pair: sentPackets) {
        if (std::chrono::steady_clock::now() - pair.second >= std::chrono::seconds(3s)) {
            this->resend(pair.first);
        }
    }
}

// this method gets an acknowledgement from another host and acknowledges a packet that has been set
bool TransmissionController::processAcknowledgement(const Packet &packet) {
    for (auto sentPacket = this->sentPackets.begin(); sentPacket != this->sentPackets.end(); sentPacket++) {
        if (packet.getAckNumber() == sentPacket->first.getSequenceNumber()) {
            // if we get an ack, the packet that we got will be processed
            this->sentPackets.erase(sentPacket);
            // debug
            std::cout << "acknowledged\n";
            return true;
        }
    }
    return false;
}

// this method sends an acknowledgement packet to the other host
void TransmissionController::acknowledgePacket(const Packet &packet) {
    this->nextSequenceNumber += packet.getLength();
    if (packet.isKeepAlive() || packet.isText() || packet.isOpen() || packet.isClose() || packet.isFile() ||
        packet.isFragment())
        this->nextSequenceNumber++;
    this->socket.send(builder.getAcknowledgement(packet.getSequenceNumber()));
}

void TransmissionController::send(const Packet &packet) {
    this->outputPackets.push(packet);
}

void TransmissionController::resend(uint32_t sequenceNumber) {
    for (auto &sentPacket: this->sentPackets) {
        if (sequenceNumber == sentPacket.first.getSequenceNumber()) {
            sentPacket.second = std::chrono::steady_clock::now();
            this->socket.send(sentPacket.first);
            return;
        }
    }
}

void TransmissionController::resend(const Packet &packet) {
    this->socket.send(packet);
}

void TransmissionController::consumeDataPacket(const Packet& packet) {
    if (packet.isFile()) {
        // we got a packet that begins transmitting a file
        this->consumer = std::make_unique<FilePacketConsumer>();
    }

    if (packet.isText()) {
        // we got a packet that begins transmitting text
        this->consumer = std::make_unique<FilePacketConsumer>();

        // IMPORTANT ///////////////////////////////////////////////////////////////
        // do not return: go forward and process the packet if it is also a fragment
        ////////////////////////////////////////////////////////////////////////////
    }

    if (packet.isFragment() || packet.isFile()) {
        // we got a packet that carries some data

        std::cout << "fragment received\n";
        auto response = this->consumer->consumePacket(packet);
        if (!response) {
            // whole data is received
            this->outputMutex->lock();
            this->outputDataQueue->push(this->consumer->getResult());
            this->outputMutex->unlock();
            this->consumer.reset();
        }
    }
    this->acknowledgePacket(packet);
}

void TransmissionController::produceDataPackets() {
    if (this->outputPackets.size() >= TransmissionController::queuedPacketsCount)
        return;

    // if we are not currently producing anything, skip
    if (!this->producer) {
        std::cout << "checking queue\n";
        this->inputMutex->lock();
        if(this->inputDataQueue->empty()) {
            this->inputMutex->unlock();
            return;
        }
        DataEntity next = this->inputDataQueue->front();
        this->inputDataQueue->pop();
        switch (next.type) {
            case DataEntity::InputType::File:
                this->producer = std::make_unique<FilePacketProducer>(next.payload);
                break;
            case DataEntity::InputType::Text:
                this->producer = std::make_unique<TextPacketProducer>(next.payload, this->textFragmented);
                break;
        }
        this->inputMutex->unlock();
    }

    while (this->outputPackets.size() < TransmissionController::queuedPacketsCount) {
        auto packet = this->producer->producePacket(
                this->builder, this->hot && !this->established, this->quickClose
        );
        this->established = true;
        if (packet) {
            if (packet->isClose())
                this->closing = true;
            this->send(packet.value());
        } else { // this data transmission is over
            this->producer.reset();
            return;
        }
    }
}

void TransmissionController::closeConnection() {
    auto packet = builder.getStop();

    while(this->retryCount < this->maximumRetries) {
        this->socket.send(packet);
        this->retryCount++;
        auto response = this->socket.receive(5s);
        if(response && response->isAck() && packet.getSequenceNumber() == response->getAckNumber()) {
            // connection termination successful
            return;
        }
    }
    std::cout << "connection terminated badly\n";
}

bool TransmissionController::isHot() const {
    return hot;
}

void TransmissionController::setHot(bool hot) {
    TransmissionController::hot = hot;
}

bool TransmissionController::isTextFragmented() const {
    return textFragmented;
}

void TransmissionController::setTextFragmented(bool textFragmented) {
    TransmissionController::textFragmented = textFragmented;
}

bool TransmissionController::isQuickClose() const {
    return quickClose;
}

void TransmissionController::setQuickClose(bool quickClose) {
    TransmissionController::quickClose = quickClose;
}

bool TransmissionController::isActiveOpen() const {
    return activeOpen;
}

void TransmissionController::setActiveOpen(bool activeOpen) {
    TransmissionController::activeOpen = activeOpen;
}

int TransmissionController::getWindowSize() {
    return windowSize;
}

int TransmissionController::getMaximumRetries() {
    return maximumRetries;
}

int TransmissionController::getQueuedPacketsCount() {
    return queuedPacketsCount;
}

void TransmissionController::setWindowSize(int windowSize) {
    TransmissionController::windowSize = windowSize;
}

void TransmissionController::setMaximumRetries(int maximumRetries) {
    TransmissionController::maximumRetries = maximumRetries;
}

void TransmissionController::setQueuedPacketsCount(int queuedPacketsCount) {
    TransmissionController::queuedPacketsCount = queuedPacketsCount;
}

bool TransmissionController::isProducingFinished() {
    this->inputMutex->lock();
    bool result = this->inputDataQueue->empty();
    this->inputMutex->unlock();
    return result && !this->producer;
}

bool TransmissionController::isConsumingFinished() {
    return !this->consumer;
}
