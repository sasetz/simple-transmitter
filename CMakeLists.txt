cmake_minimum_required(VERSION 3.22.3)
project(simple-transmitter)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set some parameters at build time
set(MAXIMUM_PACKET_LENGTH 1472)
set(MAXIMUM_DATA_SIZE 1452)
set(MAXIMUM_FILENAME_LENGTH 255)
find_package(ZLIB REQUIRED)
add_subdirectory(src)
add_subdirectory(dependencies/doctest)
add_subdirectory(tests)
