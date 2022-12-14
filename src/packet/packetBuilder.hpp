#ifndef SIMPLE_TRANSMITTER_PACKETBUILDER_HPP
#define SIMPLE_TRANSMITTER_PACKETBUILDER_HPP

#include "packet.hpp"
#include "byteData.hpp"

// can be used to generate particular packets
// manages seq# and provides an interface for constructing packets
class PacketBuilder {
private:
    uint32_t sequenceNumber;
    uint16_t fragmentLength;
public:
    PacketBuilder();
    void setFragmentLength(uint16_t fragLength);
    [[nodiscard]] uint16_t getFragmentLength() const;
    uint32_t getSequenceNumber() {
        return this->sequenceNumber;
    };

    Packet getStart();
    Packet getHotFileStart(const std::string &name);
    Packet getHotTextStart();
    Packet getHotTextFragmentStart(const std::string &textBlock);

    Packet getFileStart(const std::string &name);
    Packet getTextStart();
    Packet getTextFragmentStart(const std::string &textBlock);

    Packet getAcknowledgement(uint32_t acknowledgeNumber);
    Packet getNak(uint32_t nakNumber);

    Packet getKeepAlive();

    Packet getFragment(const ByteData &dataBlock);
    Packet getTextFragment(const std::string &textBlock);

    Packet getFragmentStop(const ByteData &dataBlock);
    Packet getTextFragmentStop(const std::string &textBlock);
    Packet getStop();
};


#endif //SIMPLE_TRANSMITTER_PACKETBUILDER_HPP
