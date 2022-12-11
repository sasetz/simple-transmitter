#ifndef SIMPLE_TRANSMITTER_BYTEDATA_HPP
#define SIMPLE_TRANSMITTER_BYTEDATA_HPP

#include <vector>
#include <cstddef>
#include <string>

// this class represents byte data that will be sent over the network in the data block of the packet
class ByteData {
private:
    std::vector<std::byte> data;
public:
    ByteData();
    explicit ByteData(unsigned long number);
    explicit ByteData(unsigned short number);
    explicit ByteData(bool value);
    // copy constructor
    ByteData(ByteData const &other);

    // move the data bytes to the object
    explicit ByteData(std::vector<std::byte> dataBlock);

    // copy string characters into the data block
    explicit ByteData(const std::string& string);

    std::vector<std::byte> getData();

    // converts host unsigned long into network unsigned long vector of bytes
    static std::vector<std::byte> longToBytes(unsigned long input);
    static unsigned long bytesToLong(std::vector<std::byte> input);
    static std::vector<std::byte> shortToBytes(unsigned short input);
    static unsigned short bytesToShort(std::vector<std::byte> input);

    // overload += operator, so that data of the second is added at the end of the first
    ByteData &operator+=(ByteData other);
    ByteData &operator+=(unsigned long other);
    ByteData &operator+=(unsigned short other);
    ByteData &operator+=(bool other);

    size_t size();

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
