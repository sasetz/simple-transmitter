#ifndef SOCKET
#define SOCKET

#include <exception>
#include <netinet/in.h>
#include <string>
#include <list>
#include <utility>
#include <vector>
#include <sys/socket.h>
#include <optional>
#include <chrono>
#include "socketAddress.hpp"
#include "packet.hpp"

class Socket {
public:
    struct SocketException : public std::exception {
        std::string message;

        explicit SocketException(std::string message) {
            this->message = std::move(message);
        }

        [[nodiscard]] const char *what() const noexcept override {
            return this->message.data();
        }
    };

    explicit Socket(SocketAddress address);

    // accepts ipAddress, port and vector of data. All in network form
    void send(Packet packet) const;

    // receive for a packet with an established client
    [[nodiscard]] std::optional<Packet> receive(const std::chrono::duration<int, std::milli> &timeout) const;

    std::optional<Packet> listen(const std::chrono::duration<int, std::milli> &timeout);

    ~Socket(); // close the socket file

private:
    SocketAddress socketAddress;
    int socketDescriptor = 0; // it's not just a number, it's a descriptor
    void initialize();
    [[nodiscard]] std::optional<std::pair<Packet, struct sockaddr_in>> poll(int timeout) const;
};


#endif
