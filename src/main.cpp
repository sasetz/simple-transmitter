#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <getopt.h> // getting the arguments from cli
#include <unistd.h> // for closing the socket descriptor
#include <string.h> // for memset
// #include "transmitter.h"

#define BUFFER_LENGTH 512
#define SERVER "127.0.0.1"
#define PORT 8888

int main(int argc, char* argv[]) {
    struct sockaddr_in inputSocketAddress, outputSocketAddress;
    char buffer[BUFFER_LENGTH] = { 0 };
    socklen_t addressLength = sizeof(outputSocketAddress);

    int socketDescriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(socketDescriptor == -1) {
        std::cerr << "Could not open the socket!" << std::endl;
        std::exit(1);
    }

    inputSocketAddress = {0, 0, 0, 0};

    inputSocketAddress.sin_family = AF_INET;
    inputSocketAddress.sin_port = htons(PORT);
    inputSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    if( bind(socketDescriptor, (struct sockaddr*)&inputSocketAddress, sizeof(inputSocketAddress)) == -1) {
        std::cerr << "Could not bind the socket!" << std::endl;
        std::exit(1);
    }

    int receivedLength;
    while(true) {
        std::cout << "Waiting for data..." << std::endl;

        receivedLength = recvfrom(socketDescriptor, buffer, BUFFER_LENGTH, 0, (struct sockaddr *) &outputSocketAddress,
                &addressLength);

        if(receivedLength == -1) {
            std::cerr << "Could not wait for data" << std::endl;
            std::exit(1);
        }

        std::cout << "Received data packet from: " << inet_ntoa(outputSocketAddress.sin_addr) << ":"
            << ntohs(outputSocketAddress.sin_port) << std::endl;
        std::cout << "Data: " << buffer << std::endl;

        if(sendto(socketDescriptor, buffer, BUFFER_LENGTH, 0, (struct sockaddr*)&outputSocketAddress, addressLength)
                == -1) {
            std::cerr << "Could not send the data back!" << std::endl;
            std::exit(1);
        }
    }

    close(socketDescriptor);
    return 0;
}
