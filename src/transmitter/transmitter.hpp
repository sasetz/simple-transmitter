#ifndef TRANSMITTER
#define TRANSMITTER

#include <queue>
#include <optional>
#include <thread>
#include <mutex>
#include <utility>
#include <memory>
#include "socketAddress.hpp"
#include "socket.hpp"
#include "packetBuilder.hpp"
#include "dataEntity.hpp"
#include "transmissionController.hpp"

class Transmitter {
private:
    std::thread runningThread;

    bool closing;

    std::shared_ptr<std::mutex> inputMutex; // mutex for input queue
    std::shared_ptr<std::mutex> outputMutex; // mutex for output queue

    std::shared_ptr<std::queue<DataEntity>> inputDataQueue; // queue that contains data to send
    std::shared_ptr<std::queue<DataEntity>> outputDataQueue; // queue that contains data received by the transmitter

    std::unique_ptr<TransmissionController> transmissionController{};

    bool hotConnection = false;
    bool textFragmented = false;
    bool hotClose = false;
    SocketAddress socketAddress;
public:
    explicit Transmitter(SocketAddress address);

    void run(bool activeOpen);

    Transmitter &addFile(const std::string& path);
    Transmitter &addText(const std::string& text);
    Transmitter &setHotConnection();
    Transmitter &setTextFragmented();
    Transmitter &setHotClose();

    ~Transmitter();
};

#endif
