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

std::string example = "";

TEST(XML, parse_simple_config) {
    using namespace simpleapi;

    std::string input = "<a>\n"
                        "b\n"
                        "</a>\n";

    Config xml = ParseXml(example);

    EXPECT_EQ(xml.size(), 1); // a(eString) = b

}

// TODO: добавить тест незакрытого тега
// TODO: добавить тест грубой ошибки в именах переменных (нельзя использовать спец. символы в начале слова)
