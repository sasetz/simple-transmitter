#include "packetBuilder.hpp"
#include <random>

PacketBuilder::PacketBuilder(uint16_t fragLength) {
    std::random_device entropy; // generate entropy from the device
    std::default_random_engine generator(entropy());
    std::uniform_int_distribution<uint32_t> distribution;
    this->sequenceNumber = distribution(generator);
    this->fragmentLength = fragLength;
}

Packet PacketBuilder::getStart() {
    Packet temp(this->sequenceNumber);
    temp.setOpen();
    this->sequenceNumber++; // add one because OPN flag adds a fantom byte to data
    return temp;
}

void PacketBuilder::setFragmentLength(uint16_t fragLength) {
    this->fragmentLength = fragLength;
}

Packet PacketBuilder::getHotFileStart(const std::string &name) {
    Packet temp(this->sequenceNumber);
    // add the data block size to the sequence number to indicate how much we've sentPackets
    this->sequenceNumber += name.size();
    this->sequenceNumber++;
    temp.setOpen();
    temp.setFile(name);
    return temp;
}

Packet PacketBuilder::getHotTextStart() {
    Packet temp(this->sequenceNumber);
    this->sequenceNumber++;
    temp.setOpen();
    temp.setText();
    return temp;
}

Packet PacketBuilder::getHotTextFragmentStart(const std::string &textBlock) {
    // add the data block size to the sequence number to indicate how much we've sentPackets
    Packet temp(this->sequenceNumber);
    temp.setOpen();
    temp.setText();
    ByteData data(textBlock);
    this->sequenceNumber += data.size();
    this->sequenceNumber++;
    temp.setFragment(data, this->fragmentLength);
    return temp;
}

Packet PacketBuilder::getFileStart(const std::string &name) {
    // add the data block size to the sequence number to indicate how much we've sent
    Packet temp(this->sequenceNumber);
    this->sequenceNumber += name.size();
    this->sequenceNumber++;
    temp.setFile(name);
    return temp;
}

Packet PacketBuilder::getTextStart() {
    Packet temp(this->sequenceNumber);
    this->sequenceNumber++;
    temp.setText();
    return temp;
}

Packet PacketBuilder::getTextFragmentStart(const std::string &textBlock) {
    // add the data block size to the sequence number to indicate how much we've sentPackets
    Packet temp(this->sequenceNumber);
    temp.setText();
    ByteData data(textBlock);
    this->sequenceNumber += data.size();
    this->sequenceNumber++;
    temp.setFragment(data, this->fragmentLength);
    return temp;
}

Packet PacketBuilder::getAcknowledgement(uint32_t acknowledgeNumber) {
    Packet temp(this->sequenceNumber);
    temp.setAcknowledged(acknowledgeNumber);
    return temp;
}

Packet PacketBuilder::getNak(uint32_t nakNumber) {
    Packet temp(this->sequenceNumber);
    temp.setNotAcknowledged(nakNumber);
    return temp;
}

Packet PacketBuilder::getKeepAlive() {
    Packet temp(this->sequenceNumber);
    temp.setKeepAlive();
    this->sequenceNumber++; // add one because LIV flag adds a fantom byte to data
    return temp;
}

Packet PacketBuilder::getFragment(const ByteData &dataBlock) {
    Packet temp(this->sequenceNumber);
    this->sequenceNumber += dataBlock.size();
    this->sequenceNumber++;
    temp.setFragment(dataBlock, this->fragmentLength);
    return temp;
}

Packet PacketBuilder::getTextFragment(const std::string &textBlock) {
    Packet temp(this->sequenceNumber);
    ByteData data(textBlock);
    this->sequenceNumber += data.size();
    this->sequenceNumber++;
    temp.setFragment(data, this->fragmentLength);
    return temp;
}

Packet PacketBuilder::getFragmentStop(const ByteData &dataBlock) {
    Packet temp(this->sequenceNumber);
    temp.setClose();
    this->sequenceNumber += dataBlock.size();
    this->sequenceNumber++;
    temp.setFragment(dataBlock, this->fragmentLength);
    return temp;
}

Packet PacketBuilder::getTextFragmentStop(const std::string &textBlock) {
    Packet temp(this->sequenceNumber);
    ByteData data(textBlock);
    this->sequenceNumber += data.size();
    this->sequenceNumber++;
    temp.setFragment(data, this->fragmentLength);
    temp.setClose();
    return temp;
}

Packet PacketBuilder::getStop() {
    Packet temp(this->sequenceNumber);
    temp.setClose();
    this->sequenceNumber++; // add one because RST flag adds a fantom byte to data
    return temp;
}

uint16_t PacketBuilder::getFragmentLength() const {
    return this->fragmentLength;
}
