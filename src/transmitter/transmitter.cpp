#include "transmitter.hpp"
#include "packetBuilder.hpp"
#include "transmissionController.hpp"

using namespace std::chrono_literals;
Transmitter &Transmitter::setHotConnection() {
    this->hotConnection = true;
    return *this;
}

Transmitter &Transmitter::addFile(const std::string& path) {
    this->inputMutex->lock();
    this->inputDataQueue->push(DataEntity(DataEntity::InputType::File, path));
    this->inputMutex->unlock();
    return *this;
}

Transmitter &Transmitter::addText(const std::string& text) {
    this->inputMutex->lock();
    this->inputDataQueue->push(DataEntity(DataEntity::InputType::Text, text));
    this->inputMutex->unlock();
    return *this;
}

Transmitter &Transmitter::setTextFragmented() {
    this->textFragmented = true;
    return *this;
}

Transmitter::Transmitter(SocketAddress address) {
    this->socketAddress = address;
    this->closing = false;

    this->inputDataQueue = std::make_unique<std::queue<DataEntity>>();
    this->outputDataQueue = std::make_unique<std::queue<DataEntity>>();

    this->inputMutex = std::make_unique<std::mutex>();
    this->outputMutex = std::make_unique<std::mutex>();

    this->transmissionController = nullptr;
}

void Transmitter::run(bool activeOpen) {
    this->transmissionController = make_unique<TransmissionController>(this->socketAddress,
                                                                       this->inputDataQueue,
                                                                       this->outputDataQueue,
                                                                       this->inputMutex,
                                                                       this->outputMutex,
                                                                       std::ref(this->closing));
    transmissionController->setActiveOpen(activeOpen);
    transmissionController->setHot(this->hotConnection);
    transmissionController->setQuickClose(this->hotClose);
    transmissionController->setTextFragmented(this->textFragmented);
    this->runningThread = std::thread(&TransmissionController::run, transmissionController.get(), 5s);
}

Transmitter &Transmitter::setHotClose() {
    this->hotClose = true;
    return *this;
}


Transmitter::~Transmitter() {
    this->runningThread.join();
}

bool Transmitter::isClosing() const {
    return closing;
}

void Transmitter::close() {
    Transmitter::closing = true;
}

std::optional<std::string> Transmitter::getOutput() {
    this->outputMutex->lock();
    if(this->outputDataQueue->empty()) {
        this->outputMutex->unlock();
        return std::nullopt;
    }

    DataEntity data = this->outputDataQueue->front();
    this->outputDataQueue->pop();
    this->outputMutex->unlock();
    switch(data.type) {
        case DataEntity::InputType::File:
            return "File on path: " + data.payload + "\n";
        case DataEntity::InputType::Text:
            return "Text: " + data.payload + "\n";
    }
}

