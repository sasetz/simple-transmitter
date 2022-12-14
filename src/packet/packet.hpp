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
    uint32_t sequenceNumber = 0;
    uint32_t acknowledgementNumber = 0;
    uint32_t checksum = 0;
    uint16_t length = 0;
    uint16_t fragLength = 0;

    ByteData data;

    Packet() = default;

    static std::string toHex(std::byte number);

public:
    explicit Packet(uint32_t seqNumber);
    explicit Packet(ByteData bytePacket);

    void setOpen();

    void setClose();

    void setAcknowledged(uint32_t ackNumber);

    void setNotAcknowledged(uint32_t ackNumber);

    void setKeepAlive();

    void setFile(const std::string &name);

    void setText();

    void setFragment(const ByteData& dataBlock, uint16_t fragmentLength);

    [[nodiscard]] bool isOpen() const;
    [[nodiscard]] bool isClose() const;
    [[nodiscard]] bool isAck() const;
    [[nodiscard]] bool isNak() const;
    [[nodiscard]] bool isKeepAlive() const;
    [[nodiscard]] bool isFile() const;
    [[nodiscard]] bool isText() const;
    [[nodiscard]] bool isFragment() const;

    bool validate();

    [[nodiscard]] uint32_t getSequenceNumber() const;
    [[nodiscard]] uint32_t getAckNumber() const;
    [[nodiscard]] uint16_t getLength() const;
    [[nodiscard]] uint16_t getFragmentLength() const;
    [[nodiscard]] ByteData getData() const;

    // checks if the packet is valid and assembles it to bytes
    ByteData build();
    static uint32_t generateChecksum(ByteData data);
    std::string dump();

    // exception that signals that something went wrong while creating a packet
    class PacketException : public std::exception {
    public:
        std::string message;

        [[nodiscard]] const char * what() const noexcept override {
            return this->message.data();
        }

        explicit PacketException(std::string msg) {
            this->message = std::move(msg);
        }
    };

    // exception that is thrown when the packet needs more byte data to be created
    // e.g. byte data is smaller than 20 bytes (packet header), or the length in the header is bigger than the actual
    // data block
    class PacketInsufficientDataException: public std::exception {
    public:
        std::string message;

        [[nodiscard]] const char * what() const noexcept override {
            return this->message.data();
        }

        explicit PacketInsufficientDataException() {
            this->message = "The byte data passed is not enough to create a whole packet!";
        }
    };
};

#endif //SIMPLE_TRANSMITTER_PACKET_HPP
