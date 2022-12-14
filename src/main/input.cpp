#include <getopt.h> // getting the arguments from cli
#include <iostream>
#include "transmitter.hpp"
#include "socketAddress.hpp"

void listen(uint16_t port) {
    Transmitter transmitter(SocketAddress::getServerAddress(port));
    // since we don't know our host, yet
    std::cout << "Listening for connections...\n";

    transmitter.run(false);

    int option;
    while(!transmitter.isClosing()) {
        std::cout << "Please, choose your option:\n";
        std::cout << "[0] Close connection\n";
        std::cout << "[1] Send a file\n";
        std::cout << "[2] Send text\n";
        std::cout << "[3] Check if something arrived\n";

        std::cin >> option;
        if(option == 0) {
            std::cout << "Closing the connection...\n";
            transmitter.close();
        } else if(option == 1) {
            std::cout << "Input file path: ";
            std::string path;
            std::cin >> path;
            transmitter.addFile(path);
            std::cout << "Sending your file...\n";
        } else if(option == 2) {
            std::cout << "Input text: ";
            std::string text;
            std::cin >> text;
            transmitter.addText(text);
            std::cout << "Sending your text...\n";
        } else {
            auto output = transmitter.getOutput();
            if(!output) {
                std::cout << "No data received\n";
            } else {
                std::cout << output.value();
            }
        }
    }
}

void connect(const std::string& externalSocket) {
    Transmitter transmitter(SocketAddress(externalSocket, 0));
    // since we don't know our host, yet
    transmitter.run(true);

    int option;
    while(!transmitter.isClosing()) {
        std::cout << "Please, choose your option:\n";
        std::cout << "[0] Close connection\n";
        std::cout << "[1] Send a file\n";
        std::cout << "[2] Send text\n";
        std::cout << "[3] Check if something arrived\n";

        std::cin >> option;
        if(option == 0) {
            std::cout << "Closing the connection...\n";
            transmitter.close();
        } else if(option == 1) {
            std::cout << "Input file path: ";
            std::string path;
            std::cin >> path;
            transmitter.addFile(path);
            std::cout << "Sending your file...\n";
        } else if(option == 2) {
            std::cout << "Input text: ";
            std::string text;
            std::cin >> text;
            transmitter.addText(text);
            std::cout << "Sending your text...\n";
        } else {
            auto output = transmitter.getOutput();
            if(!output) {
                std::cout << "No data received\n";
            } else {
                std::cout << output.value();
            }
        }
    }
}

enum class InputMode {
    Interactive,
    Listening,
    SendFile,
    SendText,
    SendInput
};

void input(int argc, char* argv[]) {
    int opt;
    InputMode mode;
    bool listen = false, file = false, text = false, input = false, socket = false, port = false, address = false;

    while((opt = getopt(argc, argv, "l:f:t:is:p:a:")) != -1) {
        //case 'l':
            //listen = true;
    }
}

void interactivePrompt() {
    uint16_t port = 25565;
    std::string externalSocket = "127.0.0.1:25565";
    int option = -1;

    while(option) {
        std::cout << "Please, choose your option:\n";
        std::cout << "[0] Exit\n";
        std::cout << "[1] Start listening for connections\n";
        std::cout << "[2] Set internal port number\n";
        std::cout << "[3] Set external socket\n";
        std::cout << "[4] Start connection\n";

        std::cin >> option;

        switch (option) {
            case 0:
                std::cout << "Exiting...\n";
                break;
            case 1:
                listen(port);
                break;
            case 2:
                std::cout << "Input your port number: ";
                std::cin >> port;
                break;
            case 3:
                std::cout << "Input socket value (127.0.0.1:1111): ";
                std::cin >> externalSocket;
                break;
            case 4:
                std::cout << "Connecting to the client...\n";
                connect(externalSocket);
                break;
            default:
                std::cout << "Invalid option!\n";
        }
    }
}
