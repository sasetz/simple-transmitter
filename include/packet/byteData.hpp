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

    // move the data bytes to the object
    explicit ByteData(std::vector<std::byte> &dataBlock);

    // copy string characters into the data block
    explicit ByteData(const std::string& string);

    std::vector<std::byte> getData();

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
