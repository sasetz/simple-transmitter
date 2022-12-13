#include <iostream>
#include <thread>
#include "packetController.hpp"

using namespace std::chrono_literals;

PacketController::PacketController(const Socket &socket, bool isActive) : socket(socket) {
    // initialize connection or die
    if (isActive) {
        std::cout << "sending start\n";
        this->socket.send(builder.getStart());
        std::cout << "listening for ack\n";
        auto response = this->socket.listen(5'000); // wait for the ack
        if (!this->accept(response) || !response->isAck()) {
            throw ConnectionException("Connection establishment failed");
        }
        this->nextSequenceNumber = response->getSequenceNumber();
        std::cout << "connection established\n";
        // connection established
    } else {
        std::cout << "listening for starts\n";
        auto response = this->socket.listen(30'000);
        if (!this->accept(response) || !response->isOpen()) {
            throw ConnectionException("Connection establishment failed");
        }
        this->nextSequenceNumber = response->getSequenceNumber() + 1;
        std::cout << "sending ack\n";
        this->socket.send(builder.getAcknowledgement(response->getSequenceNumber()));
        std::cout << "connection established\n";
    }
}

// this method deals with pretty much all communication
// it manages ARQ, ACKing in general, queues input and output packets
// also manages keep-alive and how many times transmission is retried
Packet PacketController::await(int timeout) {
    // first, send/resend all queued packets
    this->flushQueue();

    // if we have the next packet in queue to process, send it
    if(this->hasNext()) {
        Packet output = this->inputPackets.front();
        this->inputPackets.pop_front();
        return output;
    }

    // count how many invalid packet are received
    int retryCount = 0;
    // we don't have the next packet yet, listen to them until the next packet comes up
    while(!this->hasNext()) {
        if(retryCount > 15)
            throw ConnectionException("Connection timed out");
        auto option = this->socket.receive(timeout);
        retryCount++;

        if(!option) {
            // if no packet is accepted, send a keep-alive to check if the connection is functional
            this->socket.send(this->builder.getKeepAlive());
            continue;
        }

        // some packet is received, check its checksum
        if(!option->validate()) {
            // the packet received is damaged
            this->socket.send(builder.getNak(option->getSequenceNumber()));
            continue;
        }

        // process ack packets
        if(option->isAck()) {
            if(this->acknowledgement(option.value())) {
                retryCount = 15;
            }
            continue;
        }

        // process nak packets
        if(option->isNak()) {
            this->resend(option->getAckNumber());
        }

        // process keep-alive packets
        if (option->isKeepAlive()) {
            std::cout << "keep-alive received, sending ack\n";
            this->acknowledgePacket(option.value());
            retryCount = 0;
        }

        // process previous packets
        if(option->getSequenceNumber() < this->nextSequenceNumber) {
            // re-acknowledgement the packet
            this->socket.send(this->builder.getAcknowledgement(option->getSequenceNumber()));
            continue;
        }

        // a valid packet is received
        if(this->inputPackets.empty())
            this->inputPackets.push_front(option.value());
        // insert the packet so its place is ordered
        for(auto iterator = this->inputPackets.begin(); iterator != this->inputPackets.end(); iterator++) {
            if(iterator->getSequenceNumber() > option->getSequenceNumber()) {
                this->inputPackets.insert(iterator, option.value());
                break;
            }
        }
    }
    // since the cycle ended, we have a packet queued, take it

    Packet output = this->inputPackets.front();
    this->inputPackets.pop_front();
    return output;
}

// returns true if input queue contains a packet
bool PacketController::hasNext() {
    return !this->inputPackets.empty() && this->nextSequenceNumber == this->inputPackets.front().getSequenceNumber();
}

// todo: delete this method
bool PacketController::accept(std::optional<Packet> inputPacket) {
    if (!inputPacket)
        return false;

    if (inputPacket->getSequenceNumber() < this->nextSequenceNumber) {
        std::cout << "sending an ack for already processed packet\n";
        this->socket.send(this->builder.getAcknowledgement(inputPacket->getSequenceNumber()));
        return false;
    }
    return this->nextSequenceNumber == 0 || inputPacket->getSequenceNumber() == this->nextSequenceNumber;
}

void PacketController::run() {
    while (this->isRunning) {
        Packet packet = this->await(5'000);
        std::this_thread::yield();
    }
}

// resends already sentPackets packets
// sends packets from the queue
void PacketController::flushQueue() {
    for (const auto &pair: sentPackets) {
        if (std::chrono::steady_clock::now() - pair.second >= std::chrono::seconds(3s)) {
            // flushQueue the packet
            socket.send(pair.first);
        }
    }
    while (this->sentPackets.size() < PacketController::windowSize && !this->outputPackets.empty()) {
        socket.send(this->outputPackets.front());
        this->sentPackets.emplace_back(this->outputPackets.front(), std::chrono::steady_clock::now());
        this->outputPackets.pop();
    }
}

// this method gets an acknowledgement from another host and acknowledges a packet that has been set
bool PacketController::acknowledgement(const Packet &packet) {
    for (auto sentPacket = this->sentPackets.begin(); sentPacket != this->sentPackets.end(); sentPacket++) {
        if (packet.getAckNumber() == sentPacket->first.getSequenceNumber()) {
            // if we get an ack, the packet that we got will be processed
            this->sentPackets.erase(sentPacket);
            return true;
        }
    }
    return false;
}

// this method sends an acknowledgement packet to the other host
void PacketController::acknowledgePacket(const Packet &packet) {
    this->nextSequenceNumber += packet.getLength();
    this->socket.send(builder.getAcknowledgement(packet.getSequenceNumber()));
}

void PacketController::send(const Packet &packet) {
    this->outputPackets.push(packet);
}

void PacketController::resend(unsigned long sequenceNumber) {
    for (auto & sentPacket : this->sentPackets) {
        if (sequenceNumber == sentPacket.first.getSequenceNumber()) {
            sentPacket.second = std::chrono::steady_clock::now();
            this->socket.send(sentPacket.first);
            return;
        }
    }
}

// this method checks if there are any input data and pushes its packets if there is
void PacketController::flushInput() {
    // check input queue
    // generate packet sequence
    // push the sequence to output packets queue
}
