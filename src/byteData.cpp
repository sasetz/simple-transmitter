#include "packet/byteData.hpp"
#include "parameters.hpp"

ByteData::ByteData() {
    this->data = {};
}

ByteData::ByteData(std::vector<std::byte> &dataBlock) {
    if(dataBlock.size() > MAXIMUM_DATA_SIZE) {
        throw ByteException("Data block of the packet has exceeded maximum capacity!");
    }
    this->data = std::move(dataBlock);
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
