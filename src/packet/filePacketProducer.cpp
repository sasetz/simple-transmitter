#include "filePacketProducer.hpp"
#include "packetBuilder.hpp"

FilePacketProducer::FilePacketProducer(const std::string &path) {
    this->filePath = path;
}

std::optional<Packet>
FilePacketProducer::nextPacket(PacketBuilder &builder, bool isHotConnection) {
    if (!this->fileStream.is_open()) {
        // try to open the file in binary input mode
        this->fileStream.open(this->filePath, std::ios::in | std::ios::binary);
        if (!this->fileStream.is_open())
            throw FileDataException("Failed to open the file");

        // if the file has been opened, proceed to send the first bytes
        if (isHotConnection) {
            return builder.getHotFileStart(this->filePath);
        } else {
            return builder.getFileStart(this->filePath);
        }
    }
    // the file is open and ok, construct the bytes
    auto bytes = PacketProducer::getBytes(this->fileStream, builder.getFragmentLength());

    if (!bytes) {
        if(!this->hasClosingPacket)
            return std::nullopt;
        this->hasClosingPacket = false;
        // if the function is called again, but there are no
        return builder.getFragment(ByteData());
    }
    this->hasClosingPacket = this->fileStream.eof() && bytes->size() == builder.getFragmentLength();
    return builder.getFragment(bytes.value());
}
