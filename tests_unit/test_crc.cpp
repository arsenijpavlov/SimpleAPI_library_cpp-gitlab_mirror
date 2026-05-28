#include <SimpleAPI.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);

    return RUN_ALL_TESTS();
}

std::string data_template("127B2248656C6C6F223A22574F524C4421227D");

TEST(CRC, CRC8_ENCODE_DECODE) {
    using namespace simpleapi;

    Packet prep_packet = utils::FromHexString("00" + data_template);
    Packet crc_packet = prep_packet;

    bool b1 = utils::CheckCrc8(crc_packet);
    bool b2 = utils::CheckCrc8(crc_packet);

    EXPECT_EQ(b1, b2);
    EXPECT_EQ(prep_packet, crc_packet);
}

TEST(CRC, CRC8_incorrect_ENCODE_DECODE) {
    using namespace simpleapi;

    Packet prep_packet = utils::FromHexString("00" + data_template);
    Packet crc_packet = prep_packet;

    bool b1 = utils::CheckCrc8(crc_packet);
    crc_packet[0] = 14;
    bool b2 = utils::CheckCrc8(crc_packet);

    EXPECT_NE(b1, b2);
    EXPECT_NE(prep_packet, crc_packet);
}

TEST(CRC, CRC8_Packet_incorrect_ENCODE_DECODE) {
    using namespace simpleapi;

    Packet prep_packet = utils::FromHexString("00" + data_template);
    Packet crc_packet = prep_packet;

    bool b1 = utils::CheckCrc8(crc_packet);
    crc_packet[10] = 14;
    bool b2 = utils::CheckCrc8(crc_packet);

    EXPECT_NE(b1, b2);
    EXPECT_NE(prep_packet, crc_packet);
}

TEST(CRC, CRC16_ENCODE_DECODE) {
    using namespace simpleapi;

    Packet prep_packet = utils::FromHexString("0000" + data_template);
    Packet crc_packet = prep_packet;

    bool b1 = utils::CheckCrc16(crc_packet);
    bool b2 = utils::CheckCrc16(crc_packet);

    EXPECT_EQ(b1, b2);
    EXPECT_EQ(prep_packet, crc_packet);
}

TEST(CRC, CRC16_incorrect_ENCODE_DECODE) {
    using namespace simpleapi;

    Packet prep_packet = utils::FromHexString("0000" + data_template);
    Packet crc_packet = prep_packet;

    bool b1 = utils::CheckCrc16(crc_packet);
    crc_packet[0] = 14;
    bool b2 = utils::CheckCrc16(crc_packet);

    EXPECT_NE(b1, b2);
    EXPECT_NE(prep_packet, crc_packet);
}

TEST(CRC, CRC16_Packet_incorrect_ENCODE_DECODE) {
    using namespace simpleapi;

    Packet prep_packet = utils::FromHexString("0000" + data_template);
    Packet crc_packet = prep_packet;

    bool b1 = utils::CheckCrc16(crc_packet);
    crc_packet[10] = 14;
    bool b2 = utils::CheckCrc16(crc_packet);

    EXPECT_NE(b1, b2);
    EXPECT_NE(prep_packet, crc_packet);
}

TEST(CRC, CRC32_ENCODE_DECODE) {
    using namespace simpleapi;

    Packet prep_packet = utils::FromHexString("00000000" + data_template);
    Packet crc_packet = prep_packet;

    bool b1 = utils::CheckCrc32(crc_packet);
    bool b2 = utils::CheckCrc32(crc_packet);

    EXPECT_EQ(b1, b2);
    EXPECT_EQ(prep_packet, crc_packet);
}

TEST(CRC, CRC32_incorrect_ENCODE_DECODE) {
    using namespace simpleapi;

    Packet prep_packet = utils::FromHexString("00000000" + data_template);
    Packet crc_packet = prep_packet;

    bool b1 = utils::CheckCrc32(crc_packet);
    crc_packet[0] = 14;
    bool b2 = utils::CheckCrc32(crc_packet);

    EXPECT_NE(b1, b2);
    EXPECT_NE(prep_packet, crc_packet);
}

TEST(CRC, CRC32_Packet_incorrect_ENCODE_DECODE) {
    using namespace simpleapi;

    Packet prep_packet = utils::FromHexString("00000000" + data_template);
    Packet crc_packet = prep_packet;

    bool b1 = utils::CheckCrc32(crc_packet);
    crc_packet[10] = 14;
    bool b2 = utils::CheckCrc32(crc_packet);

    EXPECT_NE(b1, b2);
    EXPECT_NE(prep_packet, crc_packet);
}
