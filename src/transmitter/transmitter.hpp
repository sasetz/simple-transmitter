#ifndef TRANSMITTER
#define TRANSMITTER

#include <queue>
#include <optional>
#include <thread>
#include <mutex>
#include <utility>
#include "socketAddress.hpp"
#include "dataEntity.hpp"
#include "socket.hpp"
#include "packetBuilder.hpp"

class Transmitter {
private:
    class InputData {
    public:
        bool isFile;
        std::string value;
        InputData(std::string value, bool isFile): value(std::move(value)), isFile(isFile) {}
    };

    void transmit();
    std::thread runningThread;
    std::atomic<bool> isRunning = false;

    std::queue<InputData> input;
    std::mutex inputMutex;

    std::queue<DataEntity> output;
    std::mutex outputMutex;

    bool isHot = false;
    bool isPassive = true;

    enum class State {
        Closed,
        Established,
        SendingFile,
        SendingText,
        ReceivingFile,
        ReceivingText
    };
    State currentState = State::Closed;
    Socket socket;

    std::vector<Packet> sentPackets;
    unsigned long nextSequenceNumber;

    void runServer();
    void runClient();
    void establishedState();
    PacketBuilder builder;
    int retryNumber;
public:
    Transmitter();
    explicit Transmitter(SocketAddress address);

    Transmitter &setHotConnection();
    Transmitter &addFile(const std::string& path);
    Transmitter &addText(const std::string& text);
    Transmitter &connect();
    Transmitter &listen();

    std::optional<DataEntity> consumeInput();
    void close();
    ~Transmitter();
};

#endif
