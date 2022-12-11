#ifndef SIMPLE_TRANSMITTER_PACKETBUILDER_HPP
#define SIMPLE_TRANSMITTER_PACKETBUILDER_HPP

#include "packet.hpp"
#include "byteData.hpp"

// can be used to generate particular packets
// manages seq# and provides an interface for constructing packets
class PacketBuilder {
private:
    unsigned long sequenceNumber;
    unsigned short fragmentLength;
public:
    PacketBuilder();
    void setFragmentLength(unsigned short fragLength);

    Packet getStart();
    Packet getHotFileStart(std::string& name);
    Packet getHotTextStart();
    Packet getHotTextFragmentStart(std::string textBlock);

    Packet getFileStart(std::string name);
    Packet getTextStart();
    Packet getTextFragmentStart(std::string textBlock);

    Packet getAcknowledgement(unsigned long acknowledgeNumber);
    Packet getNak(unsigned long nakNumber);

    Packet getKeepAlive();

    Packet getFragment(ByteData &dataBlock);
    Packet getTextFragment(std::string textBlock);

    Packet getFragmentStop(ByteData dataBlock);
    Packet getTextFragmentStop(std::string textBlock);
    Packet getStop();
};


#endif //SIMPLE_TRANSMITTER_PACKETBUILDER_HPP
