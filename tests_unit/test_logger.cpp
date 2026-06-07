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

//TODO: нужны тесты, которые проверяют корректное указание цветового вывода в консоль
//TEST(LOGGER, custom_color) {
//    using namespace simpleapi;
//    using namespace logs;

//    Color color_1(ColorType::eForeground, 150);
//    Color color_2(COLOR::eBRIGHT_BLUE_FG);
//    Color color_3(ColorType::eBackground, 255, 153, 51);
//    std::cout << color_1.getColorString() << "TEST" << "\e[0m" << std::endl;
//    std::cout << color_2.getColorString() << "TEST" << "\e[0m" << std::endl;
//    std::cout << color_3.getColorString() << "TEST" << "\e[0m" << std::endl;
//}

