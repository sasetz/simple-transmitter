#include "socket.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/poll.h>
#include <string.h>
#include <unistd.h>

Socket::Socket() {
    this->ipAddress = INADDR_ANY;
    this->port = 0;
    
    // zero out the internal socket address
    memset((void *)&this->internalSocketAddress, 0, sizeof(this->internalSocketAddress));

    // assign proper values
    this->internalSocketAddress.sin_family = AF_INET;
    this->internalSocketAddress.sin_addr.s_addr = htonl(ipAddress);
    this->internalSocketAddress.sin_port = htons(port);

    // zero out external socket address, too
    memset((void *)&this->externalSocketAddress, 0, sizeof(this->externalSocketAddress));

    this->socketDescriptor = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(this->socketDescriptor == -1) {
        // couldn't create a socket, throw an error
        throw SocketException();
    }

    // bind the socket
    if( bind(
                this->socketDescriptor,
                (struct sockaddr*)&this->internalSocketAddress,
                sizeof(this->internalSocketAddress)
                )
            == -1) {
        // couldn't bind the socket, throw an error
        throw SocketException();
    }
}

Socket::Socket(const unsigned long ipAddress, const short port) {
    this->ipAddress = ipAddress;
    this->port = port;
    
    // zero out the internal socket address
    memset((void *)&this->internalSocketAddress, 0, sizeof(this->internalSocketAddress));

    // assign proper values
    this->internalSocketAddress.sin_family = AF_INET;
    this->internalSocketAddress.sin_addr.s_addr = htonl(ipAddress);
    this->internalSocketAddress.sin_port = htons(port);

    // zero out external socket address, too
    memset((void *)&this->externalSocketAddress, 0, sizeof(this->externalSocketAddress));

    this->socketDescriptor = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(this->socketDescriptor == -1) {
        // couldn't create a socket, throw an error
        throw SocketException();
    }

    // bind the socket
    if( bind(
                this->socketDescriptor,
                (struct sockaddr*)&this->internalSocketAddress,
                sizeof(this->internalSocketAddress)
                )
            == -1) {
        // couldn't bind the socket, throw an error
        throw SocketException();
    }
}

void Socket::send(const unsigned long ipAddress, const short port, const std::vector<char> data) {
    int bytesSent = 0;
    int code; // variable for output codes
    std::vector<char> toSend(data);
    struct sockaddr_in externalSocketAddress;
    memset(&externalSocketAddress, 0, sizeof(struct sockaddr_in));

    externalSocketAddress.sin_family = PF_INET;
    externalSocketAddress.sin_port = htons(port);
    externalSocketAddress.sin_addr.s_addr = htonl(ipAddress);
    
    while(bytesSent < data.size()) {
        code = sendto(this->socketDescriptor, toSend.data(), toSend.size(), 0, (struct sockaddr*)&externalSocketAddress, sizeof(struct sockaddr));
        if(code == -1)
            throw SocketException();
        bytesSent += code;
        toSend.erase(toSend.begin(), toSend.begin() + bytesSent);
    }
}

Socket::ReceiveStatus Socket::poll(unsigned long &ipAddress, short &port, std::vector<char> &data, const int size, const int timeout) {
    struct pollfd fds;
    fds.events = POLLIN;
    fds.fd = this->socketDescriptor;

    int code;

    code = ::poll(&fds, 1, timeout);

    if(code == -1)
        throw SocketException();

    if(code == 0) {
        return ReceiveStatus::EXPIRED;
    }

    if(fds.revents & POLLIN) {
        // we received some data!
        char *buffer = new char[size];
        for(int i = 0; i < size; i++)
            buffer[i] = 0;
        struct sockaddr_in externalSocketAddress;
        socklen_t addressLength;
        code = recvfrom(this->socketDescriptor, buffer, size, 0, (struct sockaddr*)&externalSocketAddress, &addressLength);
        if(code <= 0)
            throw SocketException();

        // fill ip and port fields
        ipAddress = externalSocketAddress.sin_addr.s_addr;
        port = externalSocketAddress.sin_port;

        // clean the vector, then copy all the data from the array to it
        data.clear();
        data.insert(data.end(), &buffer[0], &buffer[size]);

        delete[] buffer;
        return Socket::ReceiveStatus::RECEIVED;
    }

    throw SocketException();
}

Socket::~Socket() {
    close(this->socketDescriptor);
}
