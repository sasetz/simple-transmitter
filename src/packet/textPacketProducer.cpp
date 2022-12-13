#include "textPacketProducer.hpp"

TextPacketProducer::TextPacketProducer(const std::string &text, bool isHotFragment) {
    this->remainingText = text;
    this->isHotFragment = isHotFragment;
}

std::optional<Packet> TextPacketProducer::nextPacket(PacketBuilder &builder, bool isHotConnection) {
    // if all text has been sent, return nothing
    if (this->remainingText.empty()) {
        if (this->hasClosingPacket) {
            this->hasClosingPacket = false;
            return builder.getFragment(ByteData());
        }
        return std::nullopt;
    }

    if (!this->hadStarted) {
        this->hadStarted = true;
        // tells if the start of the text should be a fragment too or not
        if (!this->isHotFragment) {
            if (isHotConnection)
                return builder.getHotTextStart();
            else
                return builder.getTextStart();
        }

        // send a text start fragment

        std::string output(this->remainingText.substr(0, builder.getFragmentLength()));
        if (output.size() == builder.getFragmentLength())
            this->remainingText = this->remainingText.substr(builder.getFragmentLength());
        else
            this->remainingText = "";

        this->hasClosingPacket =
                this->remainingText.empty() && this->remainingText.size() == builder.getFragmentLength();

        if (isHotConnection)
            return builder.getHotTextFragmentStart(output);
        else
            return builder.getTextFragmentStart(output);
    }

    // we have something to send
    std::string output(this->remainingText.substr(0, builder.getFragmentLength()));
    if (output.size() == builder.getFragmentLength())
        this->remainingText = this->remainingText.substr(builder.getFragmentLength());
    else
        this->remainingText = "";

    this->hasClosingPacket = this->remainingText.empty() && this->remainingText.size() == builder.getFragmentLength();

    return builder.getTextFragment(output);
}
