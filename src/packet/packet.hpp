#ifndef SIMPLE_TRANSMITTER_PACKET_HPP
#define SIMPLE_TRANSMITTER_PACKET_HPP

#include <string>
#include "byteData.hpp"

class Packet {
private:
    // flags
    bool opn = false;
    bool rst = false;
    bool ack = false;
    bool nak = false;
    bool liv = false;
    bool fil = false;
    bool txt = false;
    bool frg = false;

    // other fields, all in HOST format
    unsigned long sequenceNumber = 0;
    unsigned long acknowledgementNumber = 0;
    unsigned long checksum = 0;
    unsigned short length = 0;
    unsigned short fragLength = 0;

    ByteData data;

    Packet() = default;


public:
    explicit Packet(unsigned long seqNumber);
    explicit Packet(ByteData bytePacket);

    void setOpen();

    void setClose();

    void setAcknowledged(unsigned long ackNumber);

    void setNotAcknowledged(unsigned long ackNumber);

    void setKeepAlive();

    void setFile(const std::string &name);

    void setText();

    void setFragment(const ByteData& dataBlock, unsigned short fragmentLength);

    // checks if the packet is valid and assembles it to bytes
    ByteData build();
    static unsigned long generateChecksum(ByteData data);

    // exception that signals that something went wrong while creating a packet
    class PacketException : public std::exception {
        std::string message;

        [[nodiscard]] const char * what() const noexcept override {
            return this->message.data();
        }

    public:
        explicit PacketException(std::string msg) {
            this->message = std::move(msg);
        }
    };
};

#endif //SIMPLE_TRANSMITTER_PACKET_HPP
