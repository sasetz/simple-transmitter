#include <iostream>
#include <getopt.h> // getting the arguments from cli
#include "socket.hpp"
#include "packetBuilder.hpp"
#include "transmitter.hpp"
#include "input.hpp"

#define PORT 23423

using namespace std::chrono_literals;

int main(int argc, char *argv[]) {
    int opt;
    bool isReceiver = false;
    bool interactive = true;
    bool listen = false, file = false, text = false, input = false, socket = false, port = false, address = false;
    std::string socketAddress;

    // r = receive, binary
    // f = send file
    // t = send text
    // a = other client's address, if the intent is to send data
    // s = fragment size

    // combinations:
    // r
    // af
    // at
    // a (send text from standard input)
    // saf
    // sat
    // sa (send text from standard input)
    while ((opt = getopt(argc, argv, "r")) != -1) {
        switch (opt) {
            case 'r':
                isReceiver = true;
                listen = true;
                break;
            default:
                std::cout << "Unexpected option! Exiting..." << std::endl;
                std::exit(1);
        }
    }

//    if(interactive) {
//        // launch interactive menu
//        interactivePrompt();
//    }

    if (isReceiver) {
        // receiver
        Transmitter transmitter(SocketAddress::getServerAddress(PORT));
        // since we don't know our host, yet
        std::cout << "Listening for connections...\n";

        transmitter.run(false);

        while(!transmitter.isClosing()) {
            auto output = transmitter.getOutput();
            if(!output) {
                std::this_thread::yield();
                continue;
            }
            std::cout << output.value();
        }

        std::cout << "No packet received, exiting\n";
    } else {
        // sender
        Transmitter transmitter(SocketAddress::getClientAddress(0x7f'00'00'01UL,
                                                                 PORT)); // don't care what address or port is assigned
        std::cout << "Connecting...\n";
//        transmitter.addFile("3.pdf");
        transmitter.run(true);

        std::string line;
        do {
            std::cout << "input your text: \n";
            std::cin >> line;

            transmitter.addText(line);
        } while (!line.empty());

    }
    return 0;
}
