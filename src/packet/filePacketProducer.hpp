#ifndef SIMPLE_TRANSMITTER_FILEPACKETPRODUCER_HPP
#define SIMPLE_TRANSMITTER_FILEPACKETPRODUCER_HPP


#include "packet.hpp"
#include <optional>
#include <fstream>
#include "packetProducer.hpp"
#include "packetBuilder.hpp"

class FilePacketProducer : public PacketProducer {
private:
    std::string filePath;
    std::ifstream fileStream;
    // true if we still have to send empty fragment packet to end the file transmission
    // this packet needs to be sent if end-of-file was reached and last fragment had full length
    bool hasClosingPacket = false;
    static std::optional<ByteData> getBytes(std::ifstream &stream, uint16_t fragmentLength){
        std::vector<std::byte> bytes (fragmentLength);

        // check if the file is closed or eof is already reached
        if(!stream.is_open() || !stream.read(reinterpret_cast<char *>(bytes.data()), fragmentLength)) {
            return std::nullopt;
        }

        // resize the vector if eof was reached while reading
        bytes.resize(stream.gcount());
        return ByteData(bytes);
    }
public:
    FilePacketProducer(const std::string &path);
    ~FilePacketProducer();

    std::optional<Packet>
    producePacket(PacketBuilder &builder, bool isHotConnection, bool isHotClose) override;

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


#endif //SIMPLE_TRANSMITTER_FILEPACKETPRODUCER_HPP
