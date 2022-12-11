#include <iostream>
#include <getopt.h> // getting the arguments from cli
#include "socket.hpp"
#include "packetBuilder.hpp"

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
        std::cout << "Listening for packets...\n";
        std::optional<Packet> packet = socket.listen(TIMEOUT * 1000);

        while(packet) {
            std::cout << "\nReceived a packet:" << std::endl;
            std::cout << packet.value().dump() << std::endl;
            packet = socket.listen(TIMEOUT * 1000);
        }

        std::cout << "No packet received, exiting\n";
        std::exit(0);
    } else {
        // server
        Socket socket((SocketAddress(0x7f'00'00'01UL, PORT))); // don't care what address or port is assigned
        PacketBuilder builder;
        for (int i = 0; i < 10; ++i) {
            std::cout << "Sending packet " << i << "...\n";
            socket.send(builder.getKeepAlive());
        }
    }
    return 0;
}
