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
    static SocketAddress getServerAddress(unsigned short port);
    static SocketAddress getClientAddress(unsigned long externalIp, unsigned short externalPort);

    SocketAddress(unsigned long internalIp, unsigned short internalPort,
                  unsigned long externalIp, unsigned short externalPort);
    SocketAddress(unsigned long externalIp, unsigned short externalPort);
    SocketAddress();
    explicit SocketAddress(unsigned short port);

    SocketAddress(const std::string& internal, const std::string& external);

    unsigned long getNetworkExternalIp() const;
    unsigned long getNetworkInternalIp() const;
    unsigned short getNetworkExternalPort() const;
    unsigned short getNetworkInternalPort() const;

    unsigned long getHostExternalIp() const;
    unsigned long getHostInternalIp() const;
    unsigned short getHostExternalPort() const;
    unsigned short getHostInternalPort() const;

    void setNetworkExternalIp(unsigned long ip);
    void setNetworkExternalPort(unsigned short port);
};

#endif
