#include "transmitter.hpp"
#include "file.hpp"
#include "packetBuilder.hpp"

Transmitter::Transmitter(SocketAddress address) : socket(address) {
}

Transmitter::Transmitter(): socket(SocketAddress()) {
}

Transmitter &Transmitter::setHotConnection() {
    this->isHot = true;
    return *this;
}

Transmitter &Transmitter::addFile(const std::string& path) {
    this->inputMutex.lock();
    this->input.emplace(path, true);
    this->inputMutex.unlock();
    return *this;
}

Transmitter &Transmitter::addText(const std::string& text) {
    this->inputMutex.lock();
    this->input.emplace(text, false);
    this->inputMutex.unlock();
    return *this;
}

Transmitter &Transmitter::connect() {
    this->isRunning = true;
    this->isPassive = false;
    this->runningThread = std::thread(&Transmitter::transmit, this);
    return *this;
}

Transmitter &Transmitter::listen() {
    this->isRunning = true;
    this->isPassive = true;
    this->runningThread = std::thread(&Transmitter::transmit, this);
    return *this;
}

std::optional<DataEntity> Transmitter::consumeInput() {
    if(this->output.empty())
        return std::nullopt;

    this->outputMutex.lock();
    DataEntity data = this->output.front();
    this->output.pop();
    this->outputMutex.unlock();
    return data;
}

void Transmitter::close() {
    this->isRunning = false;
    this->runningThread.join();
}

Transmitter::~Transmitter() {
    this->isRunning = false;
    this->runningThread.detach();
}

void Transmitter::transmit() {
    // this is on a new thread
    if(this->isPassive)
        runClient();
    else
        runServer();
}

void Transmitter::runServer() {
    PacketBuilder builder;
    std::optional<Packet> packet;
    Packet sent(0);
    int retryNumber = 5;
    while(this->isRunning && retryNumber) {
        switch (this->currentState) {
            case State::Closed:
                sent = builder.getStart();
                socket.send(sent);
                packet = socket.listen(30'000);

                if(!packet){
                    this->isRunning = false;
                    break;
                }
                if (!packet->isAck()) {
                    retryNumber--;
                    break;
                }
                if(packet->getAckNumber() != sent.getSequenceNumber()) {
                    retryNumber--;
                    break;
                }
                retryNumber = 5;
                this->currentState = State::Established;
                this->nextSequenceNumber++;
                break;
            case State::Established:
                packet = socket.listen(1'000);

                if(!packet) {
                    // check if our queue is empty
                    // start uploading data if it isn't
                    break;
                }
                if(packet->getSequenceNumber() != nextSequenceNumber) {
                    retryNumber--;
                    break;
                }
                if(packet->isKeepAlive()) {
                    socket.send(builder.getAcknowledgement(packet->getSequenceNumber()));
                    nextSequenceNumber++;
                    retryNumber = 5;
                    break;
                }
                retryNumber--;
                break;
            case State::SendingFile:
                break;
            case State::SendingText:
                break;
            case State::ReceivingFile:
                break;
            case State::ReceivingText:
                break;
        }
    }
}

void Transmitter::runClient() {
    std::optional<Packet> packet;
    Packet sent(0);
    while(this->isRunning && retryNumber) {
        switch (this->currentState) {
            case State::Closed:
                packet = socket.listen(30'000);

                if(!packet){
                    this->isRunning = false;
                    break;
                }
                if (!packet->isOpen()) {
                    retryNumber--;
                    break;
                }
                sentPackets.push_back(builder.getAcknowledgement(packet->getSequenceNumber()));
                retryNumber = 5;
                this->currentState = State::Established;
                this->nextSequenceNumber++;
                break;
            case State::Established:
                establishedState();
                break;
            case State::SendingFile:
                break;
            case State::SendingText:
                break;
            case State::ReceivingFile:
                break;
            case State::ReceivingText:
                break;
        }
    }
}

void Transmitter::establishedState() {
    auto packet = socket.listen(1'000);

    if(!packet) {
        // check if our queue is empty
        // start uploading data if it isn't
        return;
    }
    if(packet->getSequenceNumber() != nextSequenceNumber) {
        retryNumber--;
        return;
    }
    if(packet->isKeepAlive()) {
        socket.send(builder.getAcknowledgement(packet->getSequenceNumber()));
        nextSequenceNumber++;
        retryNumber = 5;
        return;
    }
    retryNumber--;
}

