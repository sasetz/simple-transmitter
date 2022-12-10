#ifndef SIMPLE_TRANSMITTER_PARAMETERS_HPP
#define SIMPLE_TRANSMITTER_PARAMETERS_HPP

// this value is calculated by subtracting from maximum MTU value sizes of IP and UDP headers
#define MAXIMUM_TOTAL_PACKET_SIZE 1472
// this is the maximum length of a data block, calculated by subtracting header size from the packet size
#define MAXIMUM_DATA_SIZE 1452
// maximum length of the file name
#define FILENAME_LENGTH 255

#endif //SIMPLE_TRANSMITTER_PARAMETERS_HPP
