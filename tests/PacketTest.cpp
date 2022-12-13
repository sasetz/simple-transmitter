#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "byteData.hpp"
#include "packetBuilder.hpp"
#include "fileData.hpp"
#include "textData.hpp"

TEST_SUITE_BEGIN("byte data");
TEST_CASE("long to bytes function works") {
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

TEST_CASE("bytes to long function works") {
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

TEST_CASE("short to bytes function works") {
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

TEST_CASE("bytes to short function works") {
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

TEST_CASE("slicing works") {
    ByteData data(0xBB'69'5F'C8UL);
    REQUIRE_EQ(data.size(), 4);
    SUBCASE("index operator"){
        CHECK_EQ(data[0], std::byte{0xBB});
        CHECK_EQ(data[1], std::byte{0x69});
        CHECK_EQ(data[2], std::byte{0x5F});
        CHECK_EQ(data[3], std::byte{0xC8});
    }
    SUBCASE("one number slicing") {
        auto sliced = data.slice(2);
        CHECK_EQ(sliced.size(), 2);
        CHECK_EQ(sliced[0], std::byte{0x5F});
        CHECK_EQ(sliced[1], std::byte{0xC8});
    }
    SUBCASE("two number slicing") {
        auto sliced = data.slice(1, 3);
        CHECK_EQ(sliced.size(), 2);
        CHECK_EQ(sliced[0], std::byte{0x69});
        CHECK_EQ(sliced[1], std::byte{0x5F});
    }
}

TEST_CASE("add operator works") {
    ByteData data(0xBB'69'5F'C8UL);
    SUBCASE("other byte data") {
        ByteData other(0x7B'12'FF'05UL);
        data += other;
        CHECK_EQ(data.size(), 8);
        CHECK_EQ(data[0], std::byte{0xBB});
        CHECK_EQ(data[1], std::byte{0x69});
        CHECK_EQ(data[2], std::byte{0x5F});
        CHECK_EQ(data[3], std::byte{0xC8});
        CHECK_EQ(data[4], std::byte{0x7B});
        CHECK_EQ(data[5], std::byte{0x12});
        CHECK_EQ(data[6], std::byte{0xFF});
        CHECK_EQ(data[7], std::byte{0x05});
    }
    SUBCASE("unsigned long") {
        data += 0x7B'12'FF'05UL;
        CHECK_EQ(data.size(), 8);
        CHECK_EQ(data[0], std::byte{0xBB});
        CHECK_EQ(data[1], std::byte{0x69});
        CHECK_EQ(data[2], std::byte{0x5F});
        CHECK_EQ(data[3], std::byte{0xC8});
        CHECK_EQ(data[4], std::byte{0x7B});
        CHECK_EQ(data[5], std::byte{0x12});
        CHECK_EQ(data[6], std::byte{0xFF});
        CHECK_EQ(data[7], std::byte{0x05});
    }
    SUBCASE("unsigned short") {
        data += (unsigned short)0x7B'12;
        CHECK_EQ(data.size(), 6);
        CHECK_EQ(data[0], std::byte{0xBB});
        CHECK_EQ(data[1], std::byte{0x69});
        CHECK_EQ(data[2], std::byte{0x5F});
        CHECK_EQ(data[3], std::byte{0xC8});
        CHECK_EQ(data[4], std::byte{0x7B});
        CHECK_EQ(data[5], std::byte{0x12});
    }
    SUBCASE("byte") {
        data += std::byte{0x5C};
        CHECK_EQ(data.size(), 5);
        CHECK_EQ(data[0], std::byte{0xBB});
        CHECK_EQ(data[1], std::byte{0x69});
        CHECK_EQ(data[2], std::byte{0x5F});
        CHECK_EQ(data[3], std::byte{0xC8});
        CHECK_EQ(data[4], std::byte{0x5C});
    }
}
TEST_SUITE_END();

TEST_SUITE_BEGIN("packets");

TEST_CASE("creating an open packet") {
    PacketBuilder test;
    Packet packet = test.getStart();
    ByteData data = packet.build();

    CHECK_EQ(data.size(), 20);
    CHECK_EQ(data[4], std::byte{0x00});
    CHECK_EQ(data[5], std::byte{0x00});
    CHECK_EQ(data[6], std::byte{0x00});
    CHECK_EQ(data[7], std::byte{0x00});
    CHECK_EQ(data[8], std::byte{0x00});
    CHECK_EQ(data[9], std::byte{0x00});
    CHECK_EQ(data[10], std::byte{0x00});
    CHECK_EQ(data[11], std::byte{0x00});
    CHECK_EQ(data[12], std::byte{0x80});
    CHECK_EQ(data[13], std::byte{0x00});
    CHECK_EQ(data[14], std::byte{0x00});
    CHECK_EQ(data[15], std::byte{0x00});
    CHECK_EQ(data[16], std::byte{0x00});
    CHECK_EQ(data[17], std::byte{0x00});
    CHECK_EQ(data[18], std::byte{0x00});
    CHECK_EQ(data[19], std::byte{0x00});
}

TEST_CASE("checksum calculation works") {
    ByteData data(0xBB'69'5F'C8UL);
    unsigned long output = Packet::generateChecksum(data);
    CHECK_EQ(output, 78507313UL);
}

TEST_SUITE_END();

TEST_SUITE_BEGIN("data transferring to packets");

TEST_CASE("file data to packets works") {
    FileData data("../test.txt");
    PacketBuilder builder;
    builder.setFragmentLength(8);

    std::optional<Packet> anOptional = data.nextPacket(builder, false);
    CHECK(anOptional->isFile());
    anOptional = data.nextPacket(builder, false);
    int counter = 0;
    while(anOptional) {
        CHECK(anOptional->isFragment());
        if(counter == 3) {
            CHECK_EQ(anOptional->getLength(), 2);
        } else {
            CHECK_EQ(anOptional->getLength(), 8);
        }
        counter++;
        anOptional = data.nextPacket(builder, false);
    }
}

TEST_CASE("text data to packets works") {
    TextData data("Testing text transmission\n", false);
    PacketBuilder builder;
    builder.setFragmentLength(8);

    std::optional<Packet> anOptional = data.nextPacket(builder, false);
    CHECK(anOptional->isText());
    anOptional = data.nextPacket(builder, false);
    int counter = 0;
    while(anOptional) {
        CHECK(anOptional->isFragment());
        if(counter == 3) {
            CHECK_EQ(anOptional->getLength(), 2);
        } else {
            CHECK_EQ(anOptional->getLength(), 8);
        }
        counter++;
        anOptional = data.nextPacket(builder, false);
    }
}

TEST_SUITE_END();