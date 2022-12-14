#ifndef SIMPLE_TRANSMITTER_TEXTPACKETPRODUCER_HPP
#define SIMPLE_TRANSMITTER_TEXTPACKETPRODUCER_HPP


#include "packetProducer.hpp"

class TextPacketProducer : public PacketProducer{
private:
    std::string remainingText;
    bool hasClosingPacket = false;
    bool hadStarted = false;
    bool isHotFragment;

public:
    TextPacketProducer(const std::string &text, bool isHotFragment);
    std::optional<Packet> producePacket(PacketBuilder &builder, bool isHotConnection, bool isHotClose) override;
};


#endif //SIMPLE_TRANSMITTER_TEXTPACKETPRODUCER_HPP
