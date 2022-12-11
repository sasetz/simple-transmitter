#include "packet.hpp"
#include "zlib.h"

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
    this->checksum = generateChecksum(this->data);
}

ByteData Packet::build() {
    ByteData bytes(sequenceNumber);
    bytes += ByteData(acknowledgementNumber);
    bytes += ByteData(checksum);

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
    bytes += std::byte{0};
    bytes += std::byte{0};
    bytes += std::byte{0};

    bytes += this->length;
    bytes += this->fragLength;

    bytes += this->data;
    return bytes;
}

Packet::Packet(ByteData bytePacket) {
    this->sequenceNumber = ByteData::bytesToLong(bytePacket.slice(0, 4).getData());
    this->acknowledgementNumber = ByteData::bytesToLong(bytePacket.slice(4, 8).getData());
    this->checksum = ByteData::bytesToLong(bytePacket.slice(8, 12).getData());

    auto flags = bytePacket[12];
    this->frg = (flags & std::byte{1}) == std::byte{1};
    flags >>= 1;
    this->rst = (flags & std::byte{1}) == std::byte{1};
    flags >>= 1;
    this->ack = (flags & std::byte{1}) == std::byte{1};
    flags >>= 1;
    this->nak = (flags & std::byte{1}) == std::byte{1};
    flags >>= 1;
    this->liv = (flags & std::byte{1}) == std::byte{1};
    flags >>= 1;
    this->fil = (flags & std::byte{1}) == std::byte{1};
    flags >>= 1;
    this->txt = (flags & std::byte{1}) == std::byte{1};
    flags >>= 1;
    this->frg = (flags & std::byte{1}) == std::byte{1};
    flags >>= 1;

    this->length = ByteData::bytesToShort(bytePacket.slice(16, 18).getData());
    this->fragLength = ByteData::bytesToShort(bytePacket.slice(18, 20).getData());

    this->data = bytePacket.slice(20);
}

unsigned long Packet::generateChecksum(ByteData data) {
    unsigned long crc = crc32(0L, Z_NULL, 0);

    for(int i = 0; i < data.size(); i++) {
        crc = crc32(crc, reinterpret_cast<const Bytef *>(&(data.getData()[i])), 1);
    }
    return crc;
}
