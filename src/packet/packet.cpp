#include "packet.hpp"
#include "zlib.h"
#include <iomanip>

Packet::Packet(uint32_t seqNumber) : sequenceNumber(seqNumber) {}

void Packet::setOpen() {
    this->opn = true;
}

void Packet::setClose() {
    this->rst = true;
}

void Packet::setAcknowledged(uint32_t ackNumber) {
    this->ack = true;
    this->acknowledgementNumber = ackNumber;
}

void Packet::setNotAcknowledged(uint32_t ackNumber) {
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
    this->length = this->data.size();
    this->fragLength = this->length;
}

void Packet::setText() {
    this->txt = true;
}

void Packet::setFragment(const ByteData& dataBlock, uint16_t fragmentLength) {
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
    // size of the header is at least 20 bytes
    if(bytePacket.size() < 20)
        throw PacketInsufficientDataException();
    this->sequenceNumber = ByteData::bytesToLong(bytePacket.slice(0, 4).getData());
    this->acknowledgementNumber = ByteData::bytesToLong(bytePacket.slice(4, 8).getData());
    this->checksum = ByteData::bytesToLong(bytePacket.slice(8, 12).getData());

    auto flags = bytePacket[12];
    this->frg = (flags & std::byte{1}) == std::byte{1};
    flags >>= 1;
    this->txt = (flags & std::byte{1}) == std::byte{1};
    flags >>= 1;
    this->fil = (flags & std::byte{1}) == std::byte{1};
    flags >>= 1;
    this->liv = (flags & std::byte{1}) == std::byte{1};
    flags >>= 1;
    this->nak = (flags & std::byte{1}) == std::byte{1};
    flags >>= 1;
    this->ack = (flags & std::byte{1}) == std::byte{1};
    flags >>= 1;
    this->rst = (flags & std::byte{1}) == std::byte{1};
    flags >>= 1;
    this->opn = (flags & std::byte{1}) == std::byte{1};
    flags >>= 1;

    this->length = ByteData::bytesToShort(bytePacket.slice(16, 18).getData());
    this->fragLength = ByteData::bytesToShort(bytePacket.slice(18, 20).getData());

    // check if the header length is larger than the actual data block length
    if(bytePacket.size() < 20 + this->length)
        throw PacketInsufficientDataException();
    this->data = bytePacket.slice(20, this->length + 20);
}

uint32_t Packet::generateChecksum(ByteData data) {
    uint32_t crc = crc32(0L, Z_NULL, 0);

    for(int i = 0; i < data.size(); i++) {
        crc = crc32(crc, reinterpret_cast<const Bytef *>(&(data.getData()[i])), 1);
    }
    return crc;
}

std::string Packet::dump() {
    std::string output;
    output.append("Sequence number: ");
    output.append(std::to_string(this->sequenceNumber));
    output.append("\nAck number: ");
    output.append(std::to_string(this->acknowledgementNumber));
    output.append("\nChecksum: ");
    output.append(std::to_string(this->checksum));
    output.append("\nFlags: ");

    if(this->opn)
        output.append("OPN ");
    if(this->rst)
        output.append("RST ");
    if(this->ack)
        output.append("ACK ");
    if(this->nak)
        output.append("NAK ");
    if(this->liv)
        output.append("LIV ");
    if(this->fil)
        output.append("FIL ");
    if(this->txt)
        output.append("TXT ");
    if(this->frg)
        output.append("FRG ");

    output.append("\nLength: ");
    output.append(std::to_string(this->length));
    output.append("\n[Actual data block length]: ");
    output.append(std::to_string(this->data.size()));
    output.append("\nFragment length: ");
    output.append(std::to_string(this->fragLength));

    output.append("\nData block:");

    for(int i = 0; i < this->data.size(); i++) {
        if(i % 16 == 0)
            output.append("\n");
        output.append(toHex(this->data[i]) + " ");
    }

    return output;
}

std::string Packet::toHex(std::byte number) {
    std::stringstream stream;
    stream << std::setfill('0') << std::setw(2) << std::hex << static_cast<unsigned char>(number);
    return stream.str();
}

bool Packet::isOpen() const {
    return this->opn;
}

bool Packet::isClose() const {
    return this->rst;
}

bool Packet::isAck() const {
    return this->ack;
}

bool Packet::isNak() const {
    return this->nak;
}

bool Packet::isKeepAlive() const {
    return this->liv;
}

bool Packet::isFile() const {
    return this->fil;
}

bool Packet::isText() const {
    return this->txt;
}

bool Packet::isFragment() const {
    return this->frg;
}

uint32_t Packet::getSequenceNumber() const {
    return this->sequenceNumber;
}

uint32_t Packet::getAckNumber() const {
    return this->acknowledgementNumber;
}

bool Packet::validate() {
    return this->checksum == 0 || this->generateChecksum(this->data) == this->checksum;
}

uint16_t Packet::getLength() const {
    return this->length;
}

ByteData Packet::getData() const {
    return this->data;
}

uint16_t Packet::getFragmentLength() const {
    return this->fragLength;
}
