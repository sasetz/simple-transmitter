#ifndef SIMPLE_TRANSMITTER_PACKETCONSUMER_HPP
#define SIMPLE_TRANSMITTER_PACKETCONSUMER_HPP


#include <string>
#include "packet.hpp"
#include "../transmitter/dataEntity.hpp"

class PacketConsumer {
protected:
    bool isFinished = false;
    std::string result;
public:
    // returns true if there are packets to go
    virtual bool consumePacket(Packet packet) = 0;
    virtual DataEntity getResult() const = 0;

    [[nodiscard]] bool isReady() const {
        return this->isFinished;
    }
};


#endif //SIMPLE_TRANSMITTER_PACKETCONSUMER_HPP
