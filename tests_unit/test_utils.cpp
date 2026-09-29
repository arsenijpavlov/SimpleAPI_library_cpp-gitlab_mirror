#include <SimpleAPI.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);

    return RUN_ALL_TESTS();
}

//========================================================================================

TEST(UTILS, string_len) {
    using namespace simpleapi;

    std::string example = "12345";
    EXPECT_EQ(5, utils::GetStringCharCount(example));

    example = "string";
    EXPECT_EQ(6, utils::GetStringCharCount(example));

    example = "строка";
    EXPECT_EQ(6, utils::GetStringCharCount(example));

    example = "string12345строка";
    EXPECT_EQ(17, utils::GetStringCharCount(example));

    example = "Ř"; //2-byte UNICODE symbol
    EXPECT_EQ(1, utils::GetStringCharCount(example));

    example = "炗"; //3-byte UNICODE symbol
    EXPECT_EQ(1, utils::GetStringCharCount(example));

    example = "🚵"; //4-byte UNICODE symbol
    EXPECT_EQ(1, utils::GetStringCharCount(example));
}

TEST(UTILS, EndToEndCounter_set_overflowing) {
    using namespace simpleapi;
    EECounter ee(256);

    ee.set_pos(300);
    EXPECT_EQ(ee.get(), 44);

    ee.reset();
    ee.set_glob_pos(300);
    EXPECT_EQ(ee.get_glob(), 44);

    ee.reset();
    ee.set_pos(300, 300);
    EXPECT_EQ(ee.get_glob(), 45);
    EXPECT_EQ(ee.get(), 44);
}

TEST(UTILS, EndToEndCounter_add_overflowing) {
    using namespace simpleapi;
    EECounter ee(256);

    ee.add(256);
    EXPECT_EQ(ee.get(), 0);

    ee.reset();
    ee.set_pos(255);
    ee.add(1);
    EXPECT_EQ(ee.get(), 0);
}

TEST(UTILS, EndToEndCounter_sub_overflowing) {
    using namespace simpleapi;
    EECounter ee(256);

    ee.set_pos(0);
    ee.sub(256);
    EXPECT_EQ(ee.get(), 0);

    ee.reset();
    ee.set_pos(255);
    ee.sub(256);
    EXPECT_EQ(ee.get(), 255);
}
