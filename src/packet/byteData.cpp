#include "byteData.hpp"
#include <netinet/in.h>

ByteData::ByteData() {
    this->data = {};
}

ByteData::ByteData(const ByteData &other) {
    this->data = std::vector<std::byte>(other.data);
}

ByteData::ByteData(std::vector<std::byte> dataBlock) {
    if(dataBlock.size() > MAXIMUM_DATA_SIZE) {
        throw ByteException("Data block of the packet has exceeded maximum capacity!");
    }
    this->data = std::move(dataBlock);
}

ByteData::ByteData(uint32_t number) {
    this->data = longToBytes(number);
}

ByteData::ByteData(uint16_t number) {
    this->data = shortToBytes(number);
}

ByteData::ByteData(bool value) {
    this->data.push_back((std::byte)(value ? 1 : 0));
}

std::vector<std::byte> ByteData::getData() {
    return this->data;
}

ByteData::ByteData(const std::string& string) {
    if(string.size() > MAXIMUM_DATA_SIZE) {
        throw ByteException("Data block of the packet has exceeded maximum capacity!");
    }
    for(auto character : string) {
        this->data.push_back((std::byte)character);
    }
}

std::vector<std::byte> ByteData::longToBytes(uint32_t input) {
    std::vector<std::byte> output;
    uint32_t networkLong = htonl(input);

    // magic trick to make you think that the long is actually 4 chars
    char* pointer = reinterpret_cast<char*>(&networkLong);
    for (int i = 0; i < 4; ++i) {
        output.push_back((std::byte)pointer[i]);
    }
    return output;
}

uint32_t ByteData::bytesToLong(std::vector<std::byte> input) {
    uint32_t hostLong = 0;
    // magic trick to iterate over single byte of the number
    char* pointer = reinterpret_cast<char*>(&hostLong);

    // iterate over each byte
    for (int i = 0; i < 4; ++i) {
        pointer[i] = (char) input.at(i);
    }

    return ntohl(hostLong);
}

std::vector<std::byte> ByteData::shortToBytes(uint16_t input) {
    std::vector<std::byte> output;
    uint16_t networkLong = htons(input);
    // magic trick to iterate over single byte of the number
    char* pointer = reinterpret_cast<char*>(&networkLong);

    // iterate over each byte
    for (int i = 0; i < 2; ++i) {
        output.push_back((std::byte)pointer[i]);
    }
    return output;
}

uint16_t ByteData::bytesToShort(std::vector<std::byte> input) {
    uint16_t hostShort = 0;
    // magic trick to iterate over single byte of the number
    char* pointer = reinterpret_cast<char*>(&hostShort);

    // iterate over each byte
    for (int i = 0; i < 2; ++i) {
        pointer[i] = (char) input.at(i);
    }

    return ntohs(hostShort);
}

ByteData &ByteData::operator+=(ByteData other) {
    this->data.insert(this->data.end(), other.data.begin(), other.data.end());
    return *this;
}

ByteData &ByteData::operator+=(uint32_t other) {
    auto temp = longToBytes(other);
    this->data.insert(this->data.end(), temp.begin(), temp.end());
    return *this;
}

ByteData &ByteData::operator+=(uint16_t other) {
    auto temp = shortToBytes(other);
    this->data.insert(this->data.end(), temp.begin(), temp.end());
    return *this;
}

size_t ByteData::size() const {
    return this->data.size();
}

ByteData &ByteData::operator+=(std::byte other) {
    this->data.push_back(other);
    return *this;
}

std::byte ByteData::operator[](int index) {
    return this->data.at(index);
}

ByteData ByteData::slice(int from, int to) {
    return ByteData(std::vector<std::byte>(this->data.begin() + from, this->data.begin() + to));
}

ByteData ByteData::slice(int from) {
    return ByteData(std::vector<std::byte>(this->data.begin() + from, this->data.end()));
}

ByteData::ByteData(char *buffer, size_t length) {
    this->data.reserve(length);
    for(size_t i = 0; i < length; i++) {
        this->data.push_back((std::byte)buffer[i]);
    }
}

std::string ByteData::toString() const {
    std::string output;
    for(std::byte byte : this->data) {
        output.push_back(static_cast<char>(byte));
    }
    return output;
}
