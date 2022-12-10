#include "socketAddress.hpp"

SocketAddress::SocketAddress(unsigned long internalIp, unsigned short internalPort,
                             unsigned long externalIp, unsigned short externalPort):
    externalIp(externalIp),
    externalPort(externalPort),
    internalIp(internalIp),
    internalPort(internalPort) {};

SocketAddress::SocketAddress(const std::string& internal,
                             const std::string& external) {
    char addressDelimiter = ':';
    auto delimiterPosition = internal.find(addressDelimiter);
    this->internalIp = inet_addr(internal.substr(0, delimiterPosition).c_str());
    // this line might drop an exception because of wrong port number, but that ok
    this->internalPort = htons(std::stoi(internal.substr(delimiterPosition, internal.size())));


    delimiterPosition = external.find(addressDelimiter);
    this->externalIp = inet_addr(external.substr(0, delimiterPosition).c_str());
    // this line might drop an exception because of wrong port number, but that ok
    this->externalPort = htons(std::stoi(external.substr(delimiterPosition, external.size())));
};

