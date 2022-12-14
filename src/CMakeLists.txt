add_library(input STATIC
        input/inputParameters.hpp
        input/socketAddress.hpp
        input/socketAddress.cpp)
target_include_directories(input
        PUBLIC input)
target_compile_definitions(input
        PRIVATE MAXIMUM_PACKET_LENGTH=${MAXIMUM_PACKET_LENGTH}
        PRIVATE MAXIMUM_DATA_SIZE=${MAXIMUM_DATA_SIZE}
        PRIVATE MAXIMUM_FILENAME_LENGTH=${MAXIMUM_FILENAME_LENGTH}
        PRIVATE RECEIVE_BUFFER_SIZE=${RECEIVE_BUFFER_SIZE})

add_library(packet STATIC
        packet/byteData.hpp
        packet/byteData.cpp
        packet/packet.hpp
        packet/packet.cpp
        packet/packetBuilder.hpp
        packet/packetBuilder.cpp packet/packetProducer.hpp packet/filePacketProducer.cpp packet/filePacketProducer.hpp packet/textPacketProducer.cpp packet/textPacketProducer.hpp packet/packetConsumer.hpp packet/filePacketConsumer.cpp packet/filePacketConsumer.hpp packet/textPacketConsumer.cpp packet/textPacketConsumer.hpp)
target_include_directories(packet
        PUBLIC packet)
target_link_libraries(packet input ZLIB::ZLIB)
target_compile_definitions(packet
        PRIVATE MAXIMUM_PACKET_LENGTH=${MAXIMUM_PACKET_LENGTH}
        PRIVATE MAXIMUM_DATA_SIZE=${MAXIMUM_DATA_SIZE}
        PRIVATE MAXIMUM_FILENAME_LENGTH=${MAXIMUM_FILENAME_LENGTH}
        PRIVATE RECEIVE_BUFFER_SIZE=${RECEIVE_BUFFER_SIZE})

add_library(transmitter STATIC
        transmitter/socket.hpp
        transmitter/socket.cpp
        transmitter/transmitter.hpp
        transmitter/transmitter.cpp transmitter/transmissionController.cpp transmitter/transmissionController.hpp transmitter/dataEntity.hpp)
target_include_directories(transmitter
        PUBLIC transmitter)
target_link_libraries(transmitter packet input)
target_compile_definitions(transmitter
        PRIVATE MAXIMUM_PACKET_LENGTH=${MAXIMUM_PACKET_LENGTH}
        PRIVATE MAXIMUM_DATA_SIZE=${MAXIMUM_DATA_SIZE}
        PRIVATE MAXIMUM_FILENAME_LENGTH=${MAXIMUM_FILENAME_LENGTH}
        PRIVATE RECEIVE_BUFFER_SIZE=${RECEIVE_BUFFER_SIZE})

add_executable(${PROJECT_NAME} main/main.cpp)
target_link_libraries(${PROJECT_NAME} transmitter)