#include <SimpleAPI.h>
#include "../SimpleAPI/config/Comment.h"
#include <iostream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);

    return RUN_ALL_TESTS();
}

//========================================================================================

TEST(TEST_TEST, ToComment) {
    std::string input = "abcdef";
    CommentDesign cd;

    input = ToComment(input, cd, 0);

    EXPECT_TRUE(false) << input;
}
