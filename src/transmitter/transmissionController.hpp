#ifndef SIMPLE_TRANSMITTER_TRANSMISSIONCONTROLLER_HPP
#define SIMPLE_TRANSMITTER_TRANSMISSIONCONTROLLER_HPP

#include <vector>
#include <queue>
#include <chrono>
#include <unordered_set>
#include <memory>
#include "packet.hpp"
#include "socket.hpp"
#include "packetBuilder.hpp"
#include "packetConsumer.hpp"
#include "packetProducer.hpp"
#include "dataEntity.hpp"

class TransmissionController {
private:
    // list of all packets that has been sent, but ack hasn't come yet
    std::vector< std::pair< Packet, std::chrono::time_point<std::chrono::steady_clock> > > sentPackets;

    std::queue<Packet> outputPackets; // packets queued for sending
    std::list<Packet> inputPackets; // packets queued for processing

    std::shared_ptr<std::mutex> inputMutex; // mutex for input queue
    std::shared_ptr<std::mutex> outputMutex; // mutex for output queue

    std::shared_ptr<std::queue<DataEntity>> inputDataQueue; // queue that contains data to send
    std::shared_ptr<std::queue<DataEntity>> outputDataQueue; // queue that contains data received by the transmitter

    // input parameters ////////////////////////////////////////////////////////////////////////////////////////////////
    bool hot = false; // whether the connection has hot open enabled                                                  //
    bool textFragmented = false; // whether the first text packet should be a fragment too                            //
    bool quickClose = false; // whether the connection needs to be closed after the transmission                      //
    bool activeOpen = true; // whether the client will try to connect actively or wait for input connection           //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool established = false;
    bool &closing;

    Socket socket;
    PacketBuilder builder;
    unsigned long nextSequenceNumber = 0;

    void flushQueue();
    bool processAcknowledgement(const Packet& packet);
    void acknowledgePacket(const Packet& packet);
    void send(const Packet& packet);
    void resendAll();
    void resend(unsigned long sequenceNumber);
    void resend(const Packet& packet);

    int windowSize = 10;
    int maximumRetries = 15;
    int queuedPacketsCount = 50;

    // current consumer and producer of packets
    std::unique_ptr<PacketConsumer> consumer;
    std::unique_ptr<PacketProducer> producer;

    int retryCount = 0;

    void establishConnection();
    void closeConnection();

    // check if there is a packet that needs to be consumed
    bool hasNext();

    // main processing point of every packet
    // fully manages ARQ, ACKs and this low-level stuff
    // has a queue of input packets, as well as queue of packets that need to be sent
    void processPacket(std::optional<Packet> optionalPacket);

    // processes request packets, i.e. packets that send data
    void consumeDataPacket(Packet packet);

    // checks if there is new data that needs to be sent
    void produceDataPackets();

    bool isProducingFinished();
    bool isConsumingFinished();
public:
    TransmissionController(SocketAddress address, std::shared_ptr<std::queue<DataEntity>> inputDataQueue,
                           std::shared_ptr<std::queue<DataEntity>> outputDataQueue,
                           std::shared_ptr<std::mutex> inputMutex, std::shared_ptr<std::mutex> outputMutex,
                           bool &isClosing);

    // main cycle of the packet flow
    // manages connection establishment and termination
    void run(std::chrono::duration<int, std::milli> timeout);

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

    [[nodiscard]] bool isHot() const;

    void setHot(bool hot);

    [[nodiscard]] bool isTextFragmented() const;

    void setTextFragmented(bool textFragmented);

    [[nodiscard]] bool isQuickClose() const;

    void setQuickClose(bool quickClose);

    [[nodiscard]] bool isActiveOpen() const;

    void setActiveOpen(bool activeOpen);

    void setWindowSize(int windowSize);

    void setMaximumRetries(int maximumRetries);

    void setQueuedPacketsCount(int queuedPacketsCount);

    int getWindowSize();

    int getMaximumRetries();

    int getQueuedPacketsCount();
};


#endif //SIMPLE_TRANSMITTER_TRANSMISSIONCONTROLLER_HPP
