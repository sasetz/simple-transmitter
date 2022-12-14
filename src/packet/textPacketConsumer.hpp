#ifndef SIMPLE_TRANSMITTER_TEXTPACKETCONSUMER_HPP
#define SIMPLE_TRANSMITTER_TEXTPACKETCONSUMER_HPP


#include "packetConsumer.hpp"

class TextPacketConsumer : public PacketConsumer {
public:
    TextPacketConsumer();
    bool consumePacket(Packet packet) override;

    DataEntity getResult() const override;

    class TextConstructionException : public std::exception {
    public:
        std::string message;

        [[nodiscard]] const char * what() const noexcept override {
            return this->message.data();
        }

        explicit TextConstructionException(std::string msg) {
            this->message = std::move(msg);
        }
    };
};


#endif //SIMPLE_TRANSMITTER_TEXTPACKETCONSUMER_HPP
