#include <iostream>
#include <getopt.h> // getting the arguments from cli
#include "socket.hpp"
#include "packetBuilder.hpp"
#include "transmissionController.hpp"
#include "transmitter.hpp"

#define PORT 23423

using namespace std::chrono_literals;

int main(int argc, char *argv[]) {
    int opt;
    bool isReceiver = false;

    while ((opt = getopt(argc, argv, "r")) != -1) {
        if (opt == 'r')
            isReceiver = true;
        else {
            std::cout << "Unexpected option! Exiting..." << std::endl;
            std::exit(1);
        }
    }

    if (isReceiver) {
        // client
        Transmitter transmitter(SocketAddress::getServerAddress(PORT));
        // since we don't know our host, yet
        std::cout << "Listening for connections...\n";

        transmitter.run(false);

        std::cout << "No packet received, exiting\n";
    } else {
        // server
        Transmitter transmitter(SocketAddress::getClientAddress(0x7f'00'00'01UL,
                                                                 PORT)); // don't care what address or port is assigned
        std::cout << "Connecting...\n";

        transmitter.run(true);
    }
    return 0;
}
