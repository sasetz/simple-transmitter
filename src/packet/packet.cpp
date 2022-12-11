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

void Packet::setFragment(const ByteData& dataBlock, unsigned short fragmentLength) {
    this->frg = true;
    this->data = dataBlock;
    this->fragLength = fragmentLength;
    this->length = this->data.size();
}

ByteData Packet::build() {
    ByteData bytes(sequenceNumber);
    bytes += ByteData(acknowledgementNumber);
    bytes += ByteData(checksum); // todo: checksum calculation using zlib

    // insert flags
    std::byte flags{0};

    flags = (flags << 1) | (std::byte)opn;
    flags = (flags << 1) | (std::byte)opn;
    flags = (flags << 1) | (std::byte)rst;
    flags = (flags << 1) | (std::byte)ack;
    flags = (flags << 1) | (std::byte)nak;
    flags = (flags << 1) | (std::byte)liv;
    flags = (flags << 1) | (std::byte)fil;
    flags = (flags << 1) | (std::byte)txt;
    flags = (flags << 1) | (std::byte)frg;

    bytes += flags;

    // 3 reserved bytes
    bytes += (std::byte)0;
    bytes += (std::byte)0;
    bytes += (std::byte)0;

    bytes += this->length;
    bytes += this->fragLength;

    bytes += this->data;
    return bytes;
}
