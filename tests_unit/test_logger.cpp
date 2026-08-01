#include <SimpleAPI.h>
#include <regex>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);

    return RUN_ALL_TESTS();
}

//========================================================================================

//TODO: нужны тесты, которые проверяют корректное указание цветового вывода в консоль
//TEST(LOGGER, custom_color) {
//    using namespace simpleapi;
//    using namespace logs;

//    Color color_1(ColorType::eForeground, 150);
//    Color color_2(COLOR::eBRIGHT_BLUE_FG);
//    Color color_3(ColorType::eBackground, 255, 153, 51);
//    std::cout << color_1.getColorString() << "TEST" << "\033[0m" << std::endl;
//    std::cout << color_2.getColorString() << "TEST" << "\033[0m" << std::endl;
//    std::cout << color_3.getColorString() << "TEST" << "\033[0m" << std::endl;
//}

TEST(LOGGER, rm_colors_example) {
    using namespace simpleapi;
    static std::regex reg("\\x1B\[[0-9;]*m");

    std::string str = logs::to_color_string(logs::COLOR::eYELLOW_FG, "example");

    str = std::regex_replace(str, reg, "");
    EXPECT_EQ(str, "example");
}
