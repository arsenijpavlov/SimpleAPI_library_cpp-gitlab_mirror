#include <SimpleAPI.h>
#include <iostream>

std::string test_string = "\n"
//                          "/*######################\n*/"
//                          "/# 1;losdihfg2;\n#/"
//                          "<# slopighsd3;pogihvd4;\n#>"
//                          "<-# pfgvibhdfns5;\n->"
//                          "!.# ipnbedf6 7;\n.!"
//                          "?.# ipnbedf6 7;\n.?"
//                          "%# voihnaern8 som9\n"
//                          "# word1...\n"
//                          "!######################\n"
//                          ";######################\n"
//                          "?######################\n"
//                          "//######################\n"
                          "{\n"
//                          "    string2 : \"str\\\"ing_\"value,/*some...\n*/"
//                          "    \"string2\" : stringvalue,/*some...\n*/"
//                          "    \"json\" : { \"string\" : \"inner string_value\" },\n"
//                          "    \"string\" : \"string_value\",\n"
                          "    \"array\" : [\n"
                          "\"str\\\"ing_\""//value"
//                          "           \"string_value\",\n"
//                          "           true\n"
                          "    ]\n"
//                          "    \"number\": 182,\n"
//                          "\n"
//                          "    /*######################\n"
//                          "    # some words...\n"
//                          "    ######################*/\n"
//                          "    \"bool\" : true,\n"
                          "}\n";

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

std::string temp_string = "\"asd\\\"asd\"";
std::string test_string_2 = "{string:inner_string_value}";
std::string string_array = "[15, true, \"string\"]";
std::string string_array_2 = "[\"string_value\"\n true]\n";

std::string test_json_string = "{key:\\\\}";
std::string jarray_string = "[" + temp_string + "]";
std::string json_string = "{ array: " + jarray_string + "\nstring:" + temp_string + "}";

int main() {

    std::string test_1 = "часть коммента №1\n"
                         "часть коммента №2\n"
                         "часть коммента №3\n";
    std::string test_2 = "#####################\n"
                         "# часть коммента №1\n"
                         "# часть коммента №2\n"
                         "# часть коммента №3\n"
                         "#####################";
    std::string test_5 = "=====================\n"
                         "| часть коммента №1\n"
                         "| часть коммента №2\n"
                         "| часть коммента №3\n"
                         "=====================";
//    std::string test_3 = "####\n" //нет границы, весь контент - комментарий
//                         "# часть коммента №1\n"
//                         "# часть коммента №2\n"
//                         "# часть коммента №3\n"
//                         "#####################";
//    std::string test_4 = "#####\n"
//                         "# часть коммента №1\n"
//                         "# часть коммента №2\n"
//                         "# часть коммента №3\n"
//                         "#####################";
    uint8_t size = 0;
    char sym = 0;
    std::string result = test_string;
//    result = FromComment(test_1, size, sym);
//    result = FromComment(test_2, size, sym);
//    result = FromComment(test_3, size, sym);
    result = FromComment(test_5, size, sym);
//    std::cout << result
//              << ", sym:" << (char)sym
//              << ", size:" << std::to_string(size)
//              << std::endl;

    std::cout << "-=--=--=--=--=--=--=--=--=--=--=--=--=-" << std::endl;

    result = ToComment(result, 0, size, sym);
    std::cout << result << std::endl;

    std::string eng_string = "hello";   //5
    std::string rus_string = "привет";  //6

    std::cout << "size eng_string:" << utils::getStrignSize(eng_string)
              << std::endl
              << "size rus_string:" << utils::getStrignSize(rus_string)
              << std::endl;

//TODO: std::stod("1.2e15") закончить идею

    return 0;
}
