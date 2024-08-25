#include <SimpleAPI.h>
#include <iomanip>
#include <iostream>

std::string json_string_example = "{"                               //ПРИМЕР ИЗ ТЕСТОВ
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
                                  "\"number\":182,\n"
                                  //иной вариант разделителя '='
                                  "\"bool\"=true,\n"
                                  //перенос строки равнозначен разделителю ','
                                  "\"string\":\"string_value\"\n"
                                  "\"json\":{\"string\":\"inner_string_value\"},\n"
                                  //перенос строки равнозначен разделителю ',' (массивы)
                                  "\"array\":[\"string_value\"\n true]\n"
                                  "}";

int main() {

    std::string test_1 = "часть коммента №1\n"
                         "часть коммента №2\n"
                         "часть коммента №3\n";
    uint8_t size = 0;
    char sym = 0;
    std::string result;

//    result = ToComment(result, 1, size, sym);
//    std::cout << result << std::endl;
//    std::cout << "-=--=--=--=--=--=--=--=--=--=--=--=--=-" << std::endl;
//    result.pop_back();
//    result.pop_back();
//    result.erase(0, 2);
//    result = FromComment(result, size, sym);
//    std::cout << result << std::endl;
//    std::cout << "-=--=--=--=--=--=--=--=--=--=--=--=--=-" << std::endl;
//    result = ToComment(result, 1, size, sym);
//    std::cout << result << std::endl;

    return 0;
}
