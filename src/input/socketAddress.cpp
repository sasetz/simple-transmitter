#include "socketAddress.hpp"
#include <netinet/in.h>

SocketAddress::SocketAddress(uint32_t internalIp, uint16_t internalPort,
                             uint32_t externalIp, uint16_t externalPort) :
        externalIp(externalIp),
        externalPort(externalPort),
        internalIp(internalIp),
        internalPort(internalPort) {}

SocketAddress::SocketAddress(const std::string &internal,
                             const std::string &external) {
    char addressDelimiter = ':';
    auto delimiterPosition = internal.find(addressDelimiter);
    this->internalIp = ntohl(inet_addr(internal.substr(0, delimiterPosition).c_str()));
    // this line might drop an exception because of wrong port number, but that ok
    this->internalPort = std::stoi(internal.substr(delimiterPosition + 1, internal.size()));


    delimiterPosition = external.find(addressDelimiter);
    this->externalIp = ntohl(inet_addr(external.substr(0, delimiterPosition).c_str()));
    // this line might drop an exception because of wrong port number, but that ok
    this->externalPort = std::stoi(external.substr(delimiterPosition + 1, external.size()));
}

SocketAddress::SocketAddress(const std::string &external, uint16_t internalPort) {
    char addressDelimiter = ':';
    auto delimiterPosition = external.find(addressDelimiter);
    this->externalIp = ntohl(inet_addr(external.substr(0, delimiterPosition).c_str()));
    auto str = external.substr(delimiterPosition, external.size());
    // this line might drop an exception because of wrong port number, but that ok
    this->externalPort = std::stoi(external.substr(delimiterPosition + 1, external.size()));

    this->internalPort = internalPort;
    this->internalIp = INADDR_ANY;
}

SocketAddress::SocketAddress(uint32_t externalIp, uint16_t externalPort) :
        externalIp(externalIp), externalPort(externalPort), internalIp(INADDR_ANY), internalPort(0) {}

SocketAddress::SocketAddress() :
        externalIp(0), externalPort(0), internalIp(INADDR_ANY), internalPort(0) {}

SocketAddress::SocketAddress(uint16_t port) :
        externalIp(0), externalPort(0), internalIp(INADDR_ANY), internalPort(port) {}

uint32_t SocketAddress::getNetworkExternalIp() const {
    return htonl(this->externalIp);
}

uint32_t SocketAddress::getNetworkInternalIp() const {
    return htonl(this->internalIp);
}

uint16_t SocketAddress::getNetworkExternalPort() const {
    return htons(this->externalPort);
}

uint16_t SocketAddress::getNetworkInternalPort() const {
    return htons(this->internalPort);
}

uint32_t SocketAddress::getHostExternalIp() const {
    return this->externalIp;
}

uint32_t SocketAddress::getHostInternalIp() const {
    return this->internalIp;
}

uint16_t SocketAddress::getHostExternalPort() const {
    return this->externalPort;
}

uint16_t SocketAddress::getHostInternalPort() const {
    return this->internalPort;
}

void SocketAddress::setNetworkExternalIp(uint32_t ip) {
    this->externalIp = ntohl(ip);
}

void SocketAddress::setNetworkExternalPort(uint16_t port) {
    this->externalPort = ntohs(port);
}

SocketAddress SocketAddress::getServerAddress(uint16_t port) {
    return SocketAddress(port);
}

SocketAddress SocketAddress::getClientAddress(uint32_t externalIp, uint16_t externalPort) {
    return {externalIp, externalPort};
}

