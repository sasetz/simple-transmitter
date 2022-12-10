#include <arpa/inet.h>
#include <bits/getopt_core.h>
#include <cstdlib>
#include <ios>
#include <iostream>
#include <memory>
#include <iomanip>
#include <getopt.h> // getting the arguments from cli
#include <ostream>
#include "../transmitter/socket.hpp"
// #include "transmitter.hpp"

#define BUFFER_LENGTH 512
#define TIMEOUT 30

int main(int argc, char* argv[]) {
    unsigned long externalIp;
    unsigned short internalPort, externalPort;
    int opt;
    bool isReceiver = false, hasInternals = false, hasExternals = false;

    while((opt = getopt(argc, argv, "rp:o:e:")) != -1) {
        switch (opt) {
            case 'p':
                // internal port
                hasInternals = true;
                internalPort = atoi(optarg);
                break;
            case 'o':
                // external IP
                hasExternals = true;
                externalIp = ntohl(inet_addr(optarg));
                break;
            case 'e':
                // external port
                hasExternals = true;
                externalPort = atoi(optarg);
                break;
            case 'r':
                // this is a receiver
                isReceiver = true;
                break;
            default:
                std::cout << "Unexpected option! Exiting..." << std::endl;
                std::exit(1);
        }
    }

    std::unique_ptr<Socket> socket(hasInternals ? new Socket(internalPort) : new Socket());
    if(isReceiver) {
        std::cout << "Init complete. Listening for " << TIMEOUT << " seconds" << std::endl;

        unsigned long receivedIp;
        unsigned short receivedPort;
        std::vector<char> data;

        Socket::ReceiveStatus status;
        do{
            status = socket->listen(receivedIp, receivedPort, data, 512, TIMEOUT * 1000);
            if(status != Socket::RECEIVED)
                break;
            std::cout << "Received packet:\n";
            int count = 0;
            for(auto i = data.begin(); i != data.end(); i++, count++){
                if(count == 16) {
                    std::cout << std::endl;
                    count = 0;
                }
                count++;
                std::cout << std::setw(2) << std::setfill('0') << std::hex << (int) *i << " ";
            } 

            std::cout << std::endl << "Interpreted as characters:" << std::endl;
            for(char & i : data) {
                std::cout << i;
            }
        } while(true);
    } else {
        if(!hasExternals) {
            std::cerr << "To send, please, enter external IP and port\n";
            std::exit(1);
        }
        std::cout << "Init complete. ";
        std::string inputData;
        while(true) {
            std::cout << "Input data to send:" << std::endl;
            std::cin >> inputData;
            if(inputData == "exit")
                break;
            inputData.push_back('\n');
            std::cout << "Sending your data..." << std::endl;
            socket->send(externalIp, externalPort, std::vector<char>(inputData.begin(), inputData.end()));
        }
    }
    return 0;
}
