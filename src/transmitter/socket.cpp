#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshadow"
#include "socket.hpp"
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/poll.h>
#include <cstring>
#include <unistd.h>
#include <optional>

using namespace std::chrono_literals;

Socket::Socket(SocketAddress address): socketAddress(address) {
    this->initialize();
}

void Socket::initialize() noexcept(false) {
    // zero out the internal socket address
    struct sockaddr_in internalSocketAddress{};
    memset((void *)&internalSocketAddress, 0, sizeof(internalSocketAddress));

    // assign proper values
    internalSocketAddress.sin_family = AF_INET;
    internalSocketAddress.sin_addr.s_addr = this->socketAddress.getNetworkInternalIp();
    internalSocketAddress.sin_port = this->socketAddress.getNetworkInternalPort();

    this->socketDescriptor = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(this->socketDescriptor == -1) {
        // couldn't create a socket, throw an error
        throw SocketException("System call of creating a socket has failed");
    }

    // bind the socket
    if( bind(
                this->socketDescriptor,
                (struct sockaddr*)&internalSocketAddress,
                sizeof(internalSocketAddress)
                )
            == -1) {
        // couldn't bind the socket, throw an error
        throw SocketException("Could not bind the socket to the port");
    }
}

void Socket::send(Packet packet) const {
    int bytesSent = 0;
    int code; // variable for output codes
    ByteData rawPacket = packet.build();
    auto totalBytes = rawPacket.size(); // size of the packet that needs to be sentPackets
    std::vector<std::byte> toSend = std::move(rawPacket.getData());

    struct sockaddr_in externalSocketAddress{};
    memset(&externalSocketAddress, 0, sizeof(struct sockaddr_in));

    externalSocketAddress.sin_family = PF_INET;
    externalSocketAddress.sin_port = this->socketAddress.getNetworkExternalPort();
    externalSocketAddress.sin_addr.s_addr = this->socketAddress.getNetworkExternalIp();

    while(bytesSent < totalBytes) {
        code = sendto(this->socketDescriptor, toSend.data(), toSend.size(),
                      0, (struct sockaddr*)&externalSocketAddress, sizeof(struct sockaddr));
        if(code == -1)
            throw SocketException("Could not send data");
        bytesSent += code;
        toSend.erase(toSend.begin(), toSend.begin() + bytesSent);
    }
}

std::optional<Packet> Socket::receive(const std::chrono::duration<int, std::milli> &timeout) const {
    auto pollOutput = this->poll(timeout.count());
    if(!pollOutput)
        return std::nullopt;

    auto externalSocketAddress = pollOutput->second;
    if(externalSocketAddress.sin_addr.s_addr != this->socketAddress.getNetworkExternalIp() ||
    externalSocketAddress.sin_port != this->socketAddress.getNetworkExternalPort()) {
        throw SocketException("Received a packet from another client than the connected one");
    }
    return pollOutput->first;
}

std::optional<Packet> Socket::listen(const std::chrono::duration<int, std::milli> &timeout) {
    auto pollOutput = this->poll(timeout.count());
    if(!pollOutput)
        return std::nullopt;

    auto externalSocketAddress = pollOutput->second;
    this->socketAddress.setNetworkExternalIp(externalSocketAddress.sin_addr.s_addr);
    this->socketAddress.setNetworkExternalPort(externalSocketAddress.sin_port);

    return pollOutput->first;
}

std::optional<std::pair<Packet, struct sockaddr_in>> Socket::poll(int timeout) const {
    struct pollfd fds{};
    fds.events = POLLIN;
    fds.fd = this->socketDescriptor;

    ssize_t code; // return codes

    code = ::poll(&fds, POLLIN, timeout);

    if(code == -1)
        throw SocketException("Could not receive to data");

    if (code == 0)
        return std::nullopt;

    if(fds.revents & POLLIN) {
        // we received some data!
        char *buffer = new char[RECEIVE_BUFFER_SIZE];
        for(int i = 0; i < RECEIVE_BUFFER_SIZE; i++)
            buffer[i] = 0;
        struct sockaddr_in externalSocketAddress{};
        socklen_t addressLength = sizeof(externalSocketAddress);
        code = ::recvfrom(this->socketDescriptor, buffer, RECEIVE_BUFFER_SIZE, 0,
                        (struct sockaddr*)&externalSocketAddress, &addressLength);
        // code contains number of bytes that have been received
        if(code <= 0)
            throw SocketException("Could not receive the data");

        // transform chars into byte data, delete the buffer
        ByteData data(buffer, RECEIVE_BUFFER_SIZE);
        delete[] buffer;

        return std::make_pair(Packet(data), externalSocketAddress);
    }

    if(fds.revents & POLLERR) {
        throw SocketException("An error occurred during polling: Error condition");
    } else if(fds.revents & POLLHUP) {
        throw SocketException("An error occurred during polling: Hung up");
    } else {
        // POLLNVAL
        throw SocketException("An error occurred during polling: Invalid polling request");
    }
}

Socket::~Socket() {
    close(this->socketDescriptor);
}

#pragma clang diagnostic pop