#include <iostream>
#include <getopt.h> // getting the arguments from cli
#include "socket.hpp"
#include "packetBuilder.hpp"
#include "packetController.hpp"

#define TIMEOUT 30
#define PORT 27565

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
        Socket socket((SocketAddress(PORT))); // initialize the socket with empty address,
        // since we don't know our host, yet
        std::cout << "Listening for connections...\n";

        PacketController controller(socket, false);
        controller.run();

        std::cout << "No packet received, exiting\n";
    } else {
        // server
        Socket socket((SocketAddress(0x7f'00'00'01UL, PORT))); // don't care what address or port is assigned
        std::cout << "Starting server...\n";

        PacketController controller(socket, true);
        controller.run();
    }
    return 0;
}
