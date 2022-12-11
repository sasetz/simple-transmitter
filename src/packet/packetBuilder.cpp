#include "packetBuilder.hpp"
#include <random>

PacketBuilder::PacketBuilder() {
    std::random_device entropy; // generate entropy from the device
    std::default_random_engine generator(entropy());
    std::uniform_int_distribution<unsigned long> distribution;
    this->sequenceNumber = distribution(generator);
    this->fragmentLength = 512;
}

Packet PacketBuilder::getStart() {
    Packet temp(this->sequenceNumber);
    temp.setOpen();
    this->sequenceNumber++; // add one because OPN flag adds a fantom byte to data
    return temp;
}

void PacketBuilder::setFragmentLength(unsigned short fragLength) {
    this->fragmentLength = fragLength;
}

Packet PacketBuilder::getHotFileStart(std::string& name) {
    // add the data block size to the sequence number to indicate how much we've sent
    this->sequenceNumber += name.size();
    Packet temp(this->sequenceNumber);
    temp.setOpen();
    temp.setFile(name);
    return temp;
}

Packet PacketBuilder::getHotTextStart() {
    Packet temp(this->sequenceNumber);
    temp.setOpen();
    temp.setText();
    return temp;
}

Packet PacketBuilder::getHotTextFragmentStart(std::string textBlock) {
    // add the data block size to the sequence number to indicate how much we've sent
    this->sequenceNumber += textBlock.size();
    Packet temp(this->sequenceNumber);
    temp.setOpen();
    temp.setText();
    ByteData data(textBlock);
    temp.setFragment(data, this->fragmentLength);
    return temp;
}

Packet PacketBuilder::getFileStart(std::string name) {
    // add the data block size to the sequence number to indicate how much we've sent
    this->sequenceNumber += name.size();
    Packet temp(this->sequenceNumber);
    temp.setFile(name);
    return temp;
}

Packet PacketBuilder::getTextStart() {
    Packet temp(this->sequenceNumber);
    temp.setText();
    return temp;
}

Packet PacketBuilder::getTextFragmentStart(std::string textBlock) {
    // add the data block size to the sequence number to indicate how much we've sent
    this->sequenceNumber += textBlock.size();
    Packet temp(this->sequenceNumber);
    temp.setText();
    ByteData data(textBlock);
    temp.setFragment(data, this->fragmentLength);
    return temp;
}

Packet PacketBuilder::getAcknowledgement(unsigned long acknowledgeNumber) {
    Packet temp(this->sequenceNumber);
    temp.setAcknowledged(acknowledgeNumber);
    return temp;
}

Packet PacketBuilder::getNak(unsigned long nakNumber) {
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

Packet PacketBuilder::getFragment(ByteData &dataBlock) {
    Packet temp(this->sequenceNumber);
    temp.setFragment(dataBlock, this->fragmentLength);
    return temp;
}

Packet PacketBuilder::getTextFragment(std::string textBlock) {
    Packet temp(this->sequenceNumber);
    ByteData data(textBlock);
    this->sequenceNumber += data.size();
    temp.setFragment(data, this->fragmentLength);
    return temp;
}

Packet PacketBuilder::getFragmentStop(ByteData dataBlock) {
    Packet temp(this->sequenceNumber);
    temp.setClose();
    this->sequenceNumber += dataBlock.size();
    temp.setFragment(dataBlock, this->fragmentLength);
    return temp;
}

Packet PacketBuilder::getTextFragmentStop(std::string textBlock) {
    Packet temp(this->sequenceNumber);
    ByteData data(textBlock);
    this->sequenceNumber += data.size();
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
