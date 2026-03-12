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
std::string yaml_string_example = "{"
                                  //пробелы и табуляции
                                  "   \t"
                                  //однострочные комментарии
                                  "%\n"
                                  "#\n"
                                  "!\n"
                                  ";\n"
                                  "?\n"
                                  "//\n"
                                  //многострочные комментарии
                                  "/*\n*/"
                                  "/#\n#/"
                                  "<-\n->"
                                  "<#\n#>"
                                  "!.\n.!"
                                  "?.\n.?"
                                  //поля
                                  "number:182\n"
                                  //иной вариант разделителя '='
                                  "bool=true\n"
                                  "null  = NuLL\n" //количество пробелов произвольное
                                  "string:string_value\n"
                                  "string2 : > string_value\n" //многострочные значения
                                  "json:\n"
                                  "\tstring:inner_string_value\n"
                                  "\tstring2 : 150\n"
                                  "array:\n"
                                  "\t-\t\"string_value\"\n true]\n"
                                  "}";

//TODO (потом): написать тесты для YAML
//TEST(YAML, copy) {
//}
