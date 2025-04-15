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

TEST(COMMENT, default_wrappers) {
    EXPECT_EQ("//", GetOnelineCommentStr(CommentDesign()));
    EXPECT_EQ("/*", GetMultilineCommentStartStr(CommentDesign()));
    EXPECT_EQ("*/", GetMultilineCommentStopStr(CommentDesign()));
}

TEST(COMMENT, tabulation_level) {
    EXPECT_EQ("\t// asd", ToComment("asd", CommentDesign(), 1));
    EXPECT_EQ("\t\t\t\t// asd", ToComment("asd", CommentDesign(), 4));
}

TEST(COMMENT, multiline_chopper) {

}

