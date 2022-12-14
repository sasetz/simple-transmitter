#ifndef SIMPLE_TRANSMITTER_PACKETPRODUCER_HPP
#define SIMPLE_TRANSMITTER_PACKETPRODUCER_HPP


#include <optional>
#include <fstream>
#include "packet.hpp"
#include "packetBuilder.hpp"

// class that represents data that needs to be sent to the other host
class PacketProducer {
public:
    // method that generates the next packet that needs to be sent
    virtual std::optional<Packet>
    producePacket(PacketBuilder &builder, bool isHotConnection, bool isHotClose) = 0;
};

#endif //SIMPLE_TRANSMITTER_PACKETPRODUCER_HPP
