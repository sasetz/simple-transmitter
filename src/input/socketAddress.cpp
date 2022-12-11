#include "socketAddress.hpp"
#include <netinet/in.h>

SocketAddress::SocketAddress(unsigned long internalIp, unsigned short internalPort,
                             unsigned long externalIp, unsigned short externalPort) :
        externalIp(externalIp),
        externalPort(externalPort),
        internalIp(internalIp),
        internalPort(internalPort) {}

SocketAddress::SocketAddress(const std::string &internal,
                             const std::string &external) {
    char addressDelimiter = ':';
    auto delimiterPosition = internal.find(addressDelimiter);
    this->internalIp = inet_addr(internal.substr(0, delimiterPosition).c_str());
    // this line might drop an exception because of wrong port number, but that ok
    this->internalPort = htons(std::stoi(internal.substr(delimiterPosition, internal.size())));


    delimiterPosition = external.find(addressDelimiter);
    this->externalIp = inet_addr(external.substr(0, delimiterPosition).c_str());
    // this line might drop an exception because of wrong port number, but that ok
    this->externalPort = htons(std::stoi(external.substr(delimiterPosition, external.size())));
}

SocketAddress::SocketAddress(unsigned long externalIp, unsigned short externalPort) :
        externalIp(externalIp), externalPort(externalPort), internalIp(INADDR_ANY), internalPort(0) {}

SocketAddress::SocketAddress() :
        externalIp(0), externalPort(0), internalIp(INADDR_ANY), internalPort(0) {}

SocketAddress::SocketAddress(unsigned short port) :
        externalIp(0), externalPort(0), internalIp(INADDR_ANY), internalPort(port) {}

unsigned long SocketAddress::getNetworkExternalIp() const {
    return htonl(this->externalIp);
}

unsigned long SocketAddress::getNetworkInternalIp() const {
    return htonl(this->internalIp);
}

unsigned short SocketAddress::getNetworkExternalPort() const {
    return htons(this->externalPort);
}

unsigned short SocketAddress::getNetworkInternalPort() const {
    return htons(this->internalPort);
}

unsigned long SocketAddress::getHostExternalIp() const {
    return this->externalIp;
}

unsigned long SocketAddress::getHostInternalIp() const {
    return this->internalIp;
}

unsigned short SocketAddress::getHostExternalPort() const {
    return this->externalPort;
}

unsigned short SocketAddress::getHostInternalPort() const {
    return this->internalPort;
}

void SocketAddress::setNetworkExternalIp(unsigned long ip) {
    this->externalIp = ntohl(ip);
}

void SocketAddress::setNetworkExternalPort(unsigned short port) {
    this->externalPort = ntohs(port);
}

