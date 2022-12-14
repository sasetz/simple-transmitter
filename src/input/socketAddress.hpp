#ifndef SOCKET_ADDRESS
#define SOCKET_ADDRESS

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
class SocketAddress {
private:
    // external IP address and port in NETWORK format
    uint32_t externalIp;
    uint32_t internalIp;

    // internal IP address and port in NETWORK format
    uint16_t externalPort;
    uint16_t internalPort;
public:
    static SocketAddress getServerAddress(uint16_t port);
    static SocketAddress getClientAddress(uint32_t externalIp, uint16_t externalPort);

    SocketAddress(uint32_t internalIp, uint16_t internalPort,
                  uint32_t externalIp, uint16_t externalPort);
    SocketAddress(uint32_t externalIp, uint16_t externalPort);
    SocketAddress();
    explicit SocketAddress(uint16_t port);

    SocketAddress(const std::string& internal, const std::string& external);

    uint32_t getNetworkExternalIp() const;
    uint32_t getNetworkInternalIp() const;
    uint16_t getNetworkExternalPort() const;
    uint16_t getNetworkInternalPort() const;

    uint32_t getHostExternalIp() const;
    uint32_t getHostInternalIp() const;
    uint16_t getHostExternalPort() const;
    uint16_t getHostInternalPort() const;

    void setNetworkExternalIp(uint32_t ip);
    void setNetworkExternalPort(uint16_t port);
};

#endif
