#ifndef SIMPLE_TRANSMITTER_PACKETPRODUCER_HPP
#define SIMPLE_TRANSMITTER_PACKETPRODUCER_HPP


#include <optional>
#include <fstream>
#include "packet.hpp"
#include "packetBuilder.hpp"

// class that represents data that needs to be sent to the other host
class PacketProducer {
protected:
    static std::optional<ByteData> getBytes(std::ifstream &stream, unsigned short fragmentLength){
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
    // method that generates the next packet that needs to be sent
    virtual std::optional<Packet>
    nextPacket(PacketBuilder &builder, bool isHotConnection) = 0;
};

#endif //SIMPLE_TRANSMITTER_PACKETPRODUCER_HPP
