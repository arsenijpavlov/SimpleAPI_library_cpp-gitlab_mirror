#include <SimpleAPI.h>
#include <iomanip>
#include <iostream>
#include <fstream>

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
std::string array_string_example = "[\"string_value\"\n true]";

int main() {
    system("tabs 4");
    Json json;
    json.parseJSON(json_string_example);
    std::cout << "size: " << json.size() << std::endl;

//    JArray array;
//    array.parseJSON_array(array_string_example);
//    std::cout << "size: " << array.size() << std::endl;

    return 0;
}
