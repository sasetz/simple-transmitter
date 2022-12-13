#ifndef SIMPLE_TRANSMITTER_PACKETCONTROLLER_HPP
#define SIMPLE_TRANSMITTER_PACKETCONTROLLER_HPP

#include <vector>
#include <queue>
#include <chrono>
#include <unordered_set>
#include "packet.hpp"
#include "socket.hpp"
#include "packetBuilder.hpp"

class PacketController {
private:
    // list of all packets that has been sent, but ack hasn't come yet
    std::vector< std::pair< Packet, std::chrono::time_point<std::chrono::steady_clock> > > sentPackets;

    std::queue<Packet> outputPackets;
    std::list<Packet> inputPackets; // queue for input packets

//    std::queue<>

    Socket socket;
    PacketBuilder builder;
    unsigned long nextSequenceNumber = 0;

    [[nodiscard]] bool accept(std::optional<Packet> inputPacket);

    void flushQueue();
    bool acknowledgement(const Packet& packet);
    void acknowledgePacket(const Packet& packet);
    void flushInput();
    void send(const Packet& packet);
    void resend(unsigned long sequenceNumber);
    static const int windowSize = 10;
    // function that guarantees that the packet it returns is relevant to the conversation
    // watches the sequence numbers, validates the packet checksums
    Packet await(int timeout);
    bool hasNext();
public:
    PacketController(const Socket &socket, bool isActive);

    void run();

    bool isRunning = true;

    class ConnectionException : public std::exception {
    public:
        std::string message;

        [[nodiscard]] const char * what() const noexcept override {
            return this->message.data();
        }

        explicit ConnectionException(std::string msg) {
            this->message = std::move(msg);
        }
    };
};


#endif //SIMPLE_TRANSMITTER_PACKETCONTROLLER_HPP
