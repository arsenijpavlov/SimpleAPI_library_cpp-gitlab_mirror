#include <SimpleAPI.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(CRC, CRC8_ENCODE_DECODE) {
    Packet prep_packet = utils::from_hex_string("00127B2248656C6C6F223A22574F524C4421227D");
    Packet crc_packet = prep_packet;

    bool b1 = utils::checkCrc8(crc_packet);
    bool b2 = utils::checkCrc8(crc_packet);

    EXPECT_EQ(b1, b2);
    EXPECT_EQ(prep_packet, crc_packet);
}

TEST(CRC, CRC16_ENCODE_DECODE) {
    Packet prep_packet = utils::from_hex_string("0000127B2248656C6C6F223A22574F524C4421227D");
    Packet crc_packet = prep_packet;

    bool b1 = utils::checkCrc16(crc_packet);
    bool b2 = utils::checkCrc16(crc_packet);

    EXPECT_EQ(b1, b2);
    EXPECT_EQ(prep_packet, crc_packet);
}

TEST(CRC, CRC32_ENCODE_DECODE) {
    Packet prep_packet = utils::from_hex_string("00000000127B2248656C6C6F223A22574F524C4421227D");
    Packet crc_packet = prep_packet;

    bool b1 = utils::checkCrc32(crc_packet);
    bool b2 = utils::checkCrc32(crc_packet);

    EXPECT_EQ(b1, b2);
    EXPECT_EQ(prep_packet, crc_packet);
}
