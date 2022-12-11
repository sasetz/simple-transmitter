#include "packet.hpp"

Packet::Packet(unsigned long seqNumber) : sequenceNumber(seqNumber) {}

void Packet::setOpen() {
    this->opn = true;
}

void Packet::setClose() {
    this->rst = true;
}

void Packet::setAcknowledged(unsigned long ackNumber) {
    this->ack = true;
    this->acknowledgementNumber = ackNumber;
}

void Packet::setNotAcknowledged(unsigned long ackNumber) {
    this->nak = true;
    this->acknowledgementNumber = ackNumber;
}

void Packet::setKeepAlive() {
    this->liv = true;
}

void Packet::setFile(const std::string &name) {
    if (name.size() > FILENAME_MAX) {
        throw PacketException("File name has exceeded the maximum filename length");
    }

    this->fil = true;
    this->data = ByteData(name);
}

void Packet::setText() {
    this->txt = true;
}

void Packet::setFragment(ByteData &dataBlock, unsigned short fragmentLength) {
    this->frg = true;
    this->data = std::move(dataBlock);
    this->fragLength = fragmentLength;
}

ByteData Packet::build() {
    ByteData bytes(sequenceNumber);
    bytes += ByteData(acknowledgementNumber);
    bytes += ByteData(checksum); // todo: checksum calculation using zlib

    // insert flags
    bytes += opn;
    bytes += rst;
    bytes += ack;
    bytes += nak;
    bytes += liv;
    bytes += fil;
    bytes += txt;
    bytes += frg;

    bytes += this->data;
    return bytes;
}
