#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "byteData.hpp"

TEST_CASE("long to bytes function work") {
    // setup
    unsigned long hostLong = 0xBB'69'5F'C8;
    std::byte supposed[4] {
            (std::byte)0xBB,
            (std::byte)0x69,
            (std::byte)0x5F,
            (std::byte)0xC8
    };
    // running
    std::vector<std::byte> output = ByteData::longToBytes(hostLong);
    // asserting
    CHECK_EQ(output.size(), 4);
    CHECK_EQ(output.at(0), supposed[0]);
    CHECK_EQ(output.at(1), supposed[1]);
    CHECK_EQ(output.at(2), supposed[2]);
    CHECK_EQ(output.at(3), supposed[3]);
}

TEST_CASE("bytes to long function work") {
    // setup
    unsigned long hostLong = 0xBB'69'5F'C8;
    std::vector<std::byte> bytes {
            (std::byte)0xBB,
            (std::byte)0x69,
            (std::byte)0x5F,
            (std::byte)0xC8
    };
    // running
    unsigned long output = ByteData::bytesToLong(bytes);
    // asserting
    CHECK_EQ(output, hostLong);
}

TEST_CASE("short to bytes function work") {
    // setup
    unsigned short hostShort = 0xF3'94;
    std::byte supposed[2] {
            (std::byte)0xF3,
            (std::byte)0x94
    };
    // running
    std::vector<std::byte> output = ByteData::shortToBytes(hostShort);
    // asserting
    CHECK_EQ(output.size(), 2);
    CHECK_EQ(output.at(0), supposed[0]);
    CHECK_EQ(output.at(1), supposed[1]);
}

TEST_CASE("bytes to short function work") {
    // setup
    unsigned short hostShort = 0xF3'94;
    std::vector<std::byte> bytes {
            (std::byte)0xF3,
            (std::byte)0x94
    };
    // running
    unsigned short output = ByteData::bytesToShort(bytes);
    // asserting
    CHECK_EQ(output, hostShort);
}
