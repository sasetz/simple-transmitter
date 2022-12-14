#include "textPacketConsumer.hpp"

bool TextPacketConsumer::consumePacket(Packet packet) {
    if(!packet.isFragment() && !packet.isText()) {
        throw TextConstructionException("Unexpected packet");
    }

    if(packet.isFragment()) {
        this->result += packet.getData().toString();
    }

    this->isFinished = packet.getLength() < packet.getFragmentLength();
    return !this->isFinished;
}

DataEntity TextPacketConsumer::getResult() const {
    return {DataEntity::InputType::Text, this->result};
}

TextPacketConsumer::TextPacketConsumer() = default;
