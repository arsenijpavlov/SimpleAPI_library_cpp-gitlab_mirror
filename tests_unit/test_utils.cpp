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

    example = "Ř"; //2-byte UNICODE symbol
    EXPECT_EQ(1, utils::GetStringSize(example));

    example = "炗"; //3-byte UNICODE symbol
    EXPECT_EQ(1, utils::GetStringSize(example));

    example = "🚵"; //4-byte UNICODE symbol
    EXPECT_EQ(1, utils::GetStringSize(example));
}

TEST(UTILS, from_text_to_escape_symbols) {
    EXPECT_EQ(utils::getEscChar2("\\\""),           "\"");
    EXPECT_EQ(utils::getEscChar2("\\b"),            "\b");
    EXPECT_EQ(utils::getEscChar2("\\f"),            "\f");
    EXPECT_EQ(utils::getEscChar2("\\n"),            "\n");
    EXPECT_EQ(utils::getEscChar2("\\r"),            "\r");
    EXPECT_EQ(utils::getEscChar2("\\t"),            "\t");
    EXPECT_EQ(utils::getEscChar2("\\u00a5"),        "\\u00a5");
    EXPECT_EQ(utils::getEscChar2("\\u000F12345"),   "\\u000F12345");
}

TEST(UTILS, from_escape_symbols_to_text) {
    EXPECT_EQ(utils::getFromEscChar2("\""),         "\\\"");
    EXPECT_EQ(utils::getFromEscChar2("\b"),         "\\\b");
    EXPECT_EQ(utils::getFromEscChar2("\f"),         "\\\f");
    EXPECT_EQ(utils::getFromEscChar2("\n"),         "\\\n");
    EXPECT_EQ(utils::getFromEscChar2("\r"),         "\\\r");
    EXPECT_EQ(utils::getFromEscChar2("\t"),         "\\\t");
    EXPECT_EQ(utils::getFromEscChar2("\u00a5"),     "\u00a5");
    EXPECT_EQ(utils::getFromEscChar2("\u000F12345"),"\u000F12345");
}
