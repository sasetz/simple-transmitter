add_executable(tests_run PacketTest.cpp ../src/transmitter/transmitter.cpp ../src/transmitter/socket.cpp ../src/main/input.cpp ../src/input/socketAddress.cpp ../src/packet/packet.cpp ../src/packet/packetBuilder.cpp ../src/packet/byteData.cpp)
target_include_directories(tests_run PRIVATE ../include)
