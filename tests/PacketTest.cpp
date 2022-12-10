#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "../src/packet/byteData.hpp"

TEST_CASE("testing long to host long function") {
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