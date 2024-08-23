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
    uint8_t size = 0;
    char sym = 0;
    std::string result = test_string;


//    result = ToComment(result, 0, size, sym);
//    std::cout << result << std::endl;
//    std::cout << "-=--=--=--=--=--=--=--=--=--=--=--=--=-" << std::endl;
//    result.pop_back();
//    result.pop_back();
//    result.erase(0, 2);
//    result = FromComment(result, size, sym);
//    std::cout << result << std::endl;
//    std::cout << "-=--=--=--=--=--=--=--=--=--=--=--=--=-" << std::endl;
//    result = ToComment(result, 0, size, sym);
//    std::cout << result << std::endl;

//    std::string eng_string = "hello";   //5
//    std::string rus_string = "привет";  //6

//    std::cout << "size eng_string:" << utils::getStringSize(eng_string)
//              << std::endl
//              << "size rus_string:" << utils::getStringSize(rus_string)
//              << std::endl;

//    result = utils::SeparateString(eng_string, 3);
//    std::cout << "sub3: " << result << ", " << eng_string << std::endl;
//    result = utils::SeparateString(rus_string, 3);
//    std::cout << "sub3: " << result << ", " << rus_string << std::endl;

//TODO: std::stod("1.2e15") закончить идею
    std::vector<std::string> strings = {"1", "1.1",
                                        "1e1", "1.1e1",
                                        "1.1.1", "e1",
                                        ".1", "a1e2"};
    for(size_t i = 0; i < strings.size(); i++) {
        std::string res;
        try{
            res = std::to_string(std::stod(strings[i]));
        }
        catch(...){
            res = "exc";
        }
        std::cout << "[" << i << "]" << res << std::endl;
    }

//    std::string temp_string = "\"asd\\\"asd\"";
//    std::string jarray_string = "[" + temp_string + "]";
//    std::string json_string = "{ array: " + jarray_string + "\nstring:" + temp_string + "}";
//    std::cout << "temp_string: " << temp_string << std::endl
//              << "jarray_string: " << jarray_string << std::endl
//              << "json_string: " << json_string << std::endl
//              << std::endl;

//    Json json("123", temp_string);
//    std::cout << json[0].to_string() << std::endl;

//    Json json2(json_string);
//    std::cout << json2.to_JSON_string(-1) << std::endl;
//    std::cout << json2["string"].to_string() << std::endl;
//    std::cout << json2["array"].getArray()[0].to_string() << std::endl;

    return 0;
}
