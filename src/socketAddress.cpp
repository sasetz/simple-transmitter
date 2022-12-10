#include "input/socketAddress.hpp"

SocketAddress::SocketAddress(unsigned long internalIp, unsigned short internalPort, unsigned long externalIp, unsigned short externalPort):
    externalIp(externalIp),
    externalPort(externalPort),
    internalIp(internalIp),
    internalPort(internalPort) {};

SocketAddress::SocketAddress(std::string internal, std::string external) {
    char addressDelimeter = ':';
    auto delimeterPosition = internal.find(addressDelimeter);
    this->internalIp = inet_addr(internal.substr(0, delimeterPosition).c_str());
    // this line might drop an exception because of wrong port number, but that ok
    this->internalPort = htons(std::stoi(internal.substr(delimeterPosition, internal.size())));


    delimeterPosition = external.find(addressDelimeter);
    this->externalIp = inet_addr(external.substr(0, delimeterPosition).c_str());
    // this line might drop an exception because of wrong port number, but that ok
    this->externalPort = htons(std::stoi(external.substr(delimeterPosition, external.size())));
};

