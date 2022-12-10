#ifndef SOCKET_ADDRESS
#define SOCKET_ADDRESS

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
class SocketAddress {
private:
    // external IP address and port in NETWORK format
    unsigned long externalIp;
    unsigned long internalIp;

    // internal IP address and port in NETWORK format
    unsigned short externalPort;
    unsigned short internalPort;
public:
    SocketAddress(unsigned long internalIp, unsigned short internalPort, unsigned long externalIp, unsigned short externalPort);

    SocketAddress(const std::string& internal, const std::string& external);
};

#endif
