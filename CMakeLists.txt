cmake_minimum_required(VERSION 3.22.3)
project(simple-transmitter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(${PROJECT_NAME} src/main/main.cpp src/transmitter/transmitter.cpp src/transmitter/socket.cpp src/main/input.cpp src/input/socketAddress.cpp src/packet/packet.cpp src/packet/packetBuilder.cpp src/packet/byteData.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE include)
add_subdirectory(tests)
