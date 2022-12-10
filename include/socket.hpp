#ifndef SOCKET
#define SOCKET

#define MAX_BYTES 1024

#include <exception>
#include <netinet/in.h>
#include <string>
#include <list>
#include <utility>
#include <vector>
#include <sys/socket.h>

class Socket {
public:
    enum ReceiveStatus {
        RECEIVED, // if the message is received
        EXPIRED, // if the timer has expired
    };
    struct SocketException : public std::exception {
        std::string message;
        explicit SocketException(std::string message) {
            this->message = std::move(message);
        }
        [[nodiscard]] const char * what() const noexcept override {
            return this->message.data();
        }
    };
    Socket() noexcept(false);
    explicit Socket(unsigned short port) noexcept(false);
    Socket(unsigned long ipAddress, unsigned short port) noexcept(false); // initialize a socket file
    // accepts ipAddress, port and vector of data. All in network form
    void send(unsigned long ipAddress, unsigned short port, const std::vector<char>& data) const;
    // overrides the ipAddress, port and data with its own values. All in network form
    ReceiveStatus listen(unsigned long &ipAddress, unsigned short &port, std::vector<char> &data, int size, int timeout) const;

    ~Socket(); // close the socket file

private:
    // in host format
    unsigned long ipAddress;
    unsigned short port;
    struct sockaddr_in internalSocketAddress;
    struct sockaddr_in externalSocketAddress;
    int socketDescriptor; // it's not just a number, it's a descriptor!!!!!!!
    void initialize(unsigned long ipAddress, unsigned short port);
};


#endif
