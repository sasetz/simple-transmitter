#ifndef SOCKET
#define SOCKET

#define MAX_BYTES 1024

#include <exception>
#include <netinet/in.h>
#include <string>
#include <vector>
#include <sys/socket.h>

class Socket {
public:
    enum ReceiveStatus {
        RECEIVED, // if the message is received
        EXPIRED, // if the timer has expired
    };
    struct SocketException : public std::exception {
        const char * what() const throw () {
            return "Socket creation has failed";
        }
    };
    Socket();
    Socket(const unsigned long ipAddress, const short port) noexcept(false); // initialize a socket file
    // accepts ipAddress, port and vector of data. All in network form
    void send(const unsigned long ipAddress, const short port, const std::vector<char> data);
    // overrides the ipAddress, port and data with its own values. All in network form
    ReceiveStatus poll(unsigned long &ipAddress, short &port, std::vector<char> &data, const int size, const int timeout);

    ~Socket(); // close the socket file

private:
    // in host format
    unsigned long ipAddress;
    short port;
    struct sockaddr_in internalSocketAddress;
    int socketDescriptor; // it's not just a number, it's a descriptor!!!!!!!
};


#endif
