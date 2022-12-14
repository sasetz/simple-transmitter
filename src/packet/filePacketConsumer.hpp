#ifndef SIMPLE_TRANSMITTER_FILEPACKETCONSUMER_HPP
#define SIMPLE_TRANSMITTER_FILEPACKETCONSUMER_HPP


#include <fstream>
#include "packetConsumer.hpp"

class FilePacketConsumer : public PacketConsumer {
private:
    std::string filename;
    std::ofstream fileStream;
public:
    FilePacketConsumer();
    ~FilePacketConsumer();

    bool consumePacket(Packet packet) override;

    DataEntity getResult() const override;

    class FileConstructionException : public std::exception {
    public:
        std::string message;

        [[nodiscard]] const char * what() const noexcept override {
            return this->message.data();
        }

        explicit FileConstructionException(std::string msg) {
            this->message = std::move(msg);
        }
    };
};


#endif //SIMPLE_TRANSMITTER_FILEPACKETCONSUMER_HPP
