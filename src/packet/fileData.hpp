#ifndef SIMPLE_TRANSMITTER_FILEDATA_HPP
#define SIMPLE_TRANSMITTER_FILEDATA_HPP


#include <fstream>
#include "data.hpp"
#include "packetBuilder.hpp"

class FileData : public Data {
private:
    std::string filePath;
    std::ifstream fileStream;
    // true if we still have to send empty fragment packet to end the file transmission
    // this packet needs to be sent if end-of-file was reached and last fragment had full length
    bool hasClosingPacket = false;
public:
    FileData(const std::string &path);

    std::optional<Packet>
    nextPacket(PacketBuilder &builder, bool isHotConnection) override;

    // exception for if something wrong went with the file
    class FileDataException : public std::exception {
    public:
        std::string message;

        [[nodiscard]] const char * what() const noexcept override {
            return this->message.data();
        }

        explicit FileDataException(std::string msg) {
            this->message = std::move(msg);
        }
    };
};


#endif //SIMPLE_TRANSMITTER_FILEDATA_HPP
