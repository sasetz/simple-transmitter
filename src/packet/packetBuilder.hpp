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
    [[nodiscard]] unsigned short getFragmentLength() const;
    unsigned long getSequenceNumber() {
        return this->sequenceNumber;
    };

    Packet getStart();
    Packet getHotFileStart(const std::string &name);
    Packet getHotTextStart();
    Packet getHotTextFragmentStart(const std::string &textBlock);

    Packet getFileStart(const std::string &name);
    Packet getTextStart();
    Packet getTextFragmentStart(const std::string &textBlock);

    Packet getAcknowledgement(unsigned long acknowledgeNumber);
    Packet getNak(unsigned long nakNumber);

    Packet getKeepAlive();

    Packet getFragment(const ByteData &dataBlock);
    Packet getTextFragment(const std::string &textBlock);

    Packet getFragmentStop(const ByteData &dataBlock);
    Packet getTextFragmentStop(const std::string &textBlock);
    Packet getStop();
};


#endif //SIMPLE_TRANSMITTER_PACKETBUILDER_HPP
