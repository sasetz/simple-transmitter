#ifndef SIMPLE_TRANSMITTER_TEXTDATA_HPP
#define SIMPLE_TRANSMITTER_TEXTDATA_HPP


#include "data.hpp"

class TextData : Data{
private:
    std::string remainingText;
    bool hasClosingPacket = false;
    bool hadStarted = false;
    bool isHotFragment;
public:
    TextData(const std::string &text, bool isHotFragment);
    std::optional<Packet> nextPacket(PacketBuilder &builder, bool isHotConnection) override;
};


#endif //SIMPLE_TRANSMITTER_TEXTDATA_HPP
