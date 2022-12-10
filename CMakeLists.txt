cmake_minimum_required(VERSION 3.22.3)
project(simple-transmitter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(${PROJECT_NAME} src/main.cpp src/transmitter.cpp src/socket.cpp src/input.cpp src/socketAddress.cpp src/packet.cpp src/packetBuilder.cpp src/byteData.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE include)
