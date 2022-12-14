#include <iostream>
#include "filePacketConsumer.hpp"

FilePacketConsumer::FilePacketConsumer() = default;

bool FilePacketConsumer::consumePacket(Packet packet) {
    if(!this->fileStream.is_open() && packet.isFile()) {
        this->filename = packet.getData().toString();
        this->fileStream.open(this->filename, std::ofstream::out | std::ofstream::binary);

        if(!this->fileStream.is_open())
            throw FileConstructionException("Failed to open a file");

        return true;
    } else if(this->fileStream.is_open() && packet.isFile() ||
    !this->fileStream.is_open() && !packet.isFile()) {
        if(this->fileStream.is_open())
            this->fileStream.close();
        throw FileConstructionException("Unexpected packet");
    }

    // file is open, we got a fragment. write it to the file

    if(!packet.isFragment()) {
        if(this->fileStream.is_open())
            this->fileStream.close();
        throw FileConstructionException("Passed a wrong packet");
    }

    this->fileStream.write(reinterpret_cast<const char *>(packet.getData().getData().data()), packet.getLength());

    this->isFinished = packet.getLength() < packet.getFragmentLength();

    if(this->isFinished)
        this->result = this->filename;

    return !this->isFinished;
}

DataEntity FilePacketConsumer::getResult() const {
    return {DataEntity::InputType::File, this->filename};
}

