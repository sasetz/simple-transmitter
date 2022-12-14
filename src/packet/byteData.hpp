#ifndef SIMPLE_TRANSMITTER_BYTEDATA_HPP
#define SIMPLE_TRANSMITTER_BYTEDATA_HPP

#include <vector>
#include <cstddef>
#include <string>

// this class represents byte data that will be sentPackets over the network in the data block of the packet
class ByteData {
private:
    std::vector<std::byte> data;
public:
    ByteData();
    explicit ByteData(uint32_t number);
    explicit ByteData(uint16_t number);
    explicit ByteData(bool value);
    explicit ByteData(char *buffer, size_t length);
    // copy constructor
    ByteData(ByteData const &other);

    // move the data bytes to the object
    explicit ByteData(std::vector<std::byte> dataBlock);

    // copy string characters into the data block
    explicit ByteData(const std::string& string);

    [[nodiscard]] std::string toString() const;

    std::vector<std::byte> getData();

    // converts host unsigned long into network unsigned long vector of bytes
    static std::vector<std::byte> longToBytes(uint32_t input);
    static uint32_t bytesToLong(std::vector<std::byte> input);
    static std::vector<std::byte> shortToBytes(uint16_t input);
    static uint16_t bytesToShort(std::vector<std::byte> input);

    // overload += operator, so that data of the second is added at the end of the first
    ByteData &operator+=(ByteData other);
    ByteData &operator+=(uint32_t other);
    ByteData &operator+=(uint16_t other);
    ByteData &operator+=(std::byte other);
    std::byte operator[](int index);
    ByteData slice(int from, int to);
    ByteData slice(int from);

    size_t size() const;

    // exceptions that can occur while transforming byte data
    class ByteException : public std::exception {
        std::string message;

        [[nodiscard]] const char * what() const noexcept override {
            return this->message.data();
        }

    public:
        explicit ByteException(std::string msg) {
            this->message = std::move(msg);
        }
    };
};

#endif //SIMPLE_TRANSMITTER_BYTEDATA_HPP
