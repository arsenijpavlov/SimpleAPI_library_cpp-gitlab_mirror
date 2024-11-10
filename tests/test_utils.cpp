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
    std::string example = "12345";
    EXPECT_EQ(5, utils::GetStringSize(example));

    example = "string";
    EXPECT_EQ(6, utils::GetStringSize(example));

    example = "строка";
    EXPECT_EQ(6, utils::GetStringSize(example));

    example = "string12345строка";
    EXPECT_EQ(17, utils::GetStringSize(example));
}
