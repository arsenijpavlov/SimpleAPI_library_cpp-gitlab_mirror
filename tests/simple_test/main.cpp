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
    system("tabs 4");

    std::string test_1 = "часть коммента №1\n"
                         "часть коммента №2\n"
                         "часть коммента №3\n";
    std::string test_2 = "/*" + test_1 + "/";
    std::string test_3 = "часть коммента №1";
    uint8_t size = 20;
    char sym = 0;
    std::string result = test_1;

//    result = ToComment(result, 1, size, sym);
//    std::cout << result << std::endl;
//    std::cout << "-=--=--=--=--=--=--=--=--=--=--=--=--=-" << std::endl;
//    result.pop_back();
//    result.pop_back();
//    result.erase(0, 3);
//    result = FromComment(result, size, sym);
//    std::cout << result << std::endl;
//    std::cout << "-=--=--=--=--=--=--=--=--=--=--=--=--=-" << std::endl;
//    result = ToComment(result, 1, size, sym);
//    std::cout << result << std::endl;

//    result = "some many words2...";
//    std::cout << result << std::endl;
//    std::cout << "-=--=--=--=--=--=--=--=--=--=--=--=--=-" << std::endl;
//    result = ToComment(result, 1, size, sym);
//    std::cout << result << std::endl;
//    std::cout << "-=--=--=--=--=--=--=--=--=--=--=--=--=-" << std::endl;

    Json json(json_string_example);

    json.setCommentColumnSize(20);
    json.setCommentSymbol('#');
    json.addPreviewComment("очень даже невероятно такие большие комментарии");
    json.addComment_before("bool", "some \nwords...");
    json.addComment_before("string", "some many words1...");
    json.addComment_after("array", "some many words2...");

    json["json"].getJson().addComment_before(0, "json element\n comment");
    json["array"].getArray().addComment_before(0, "array element\n comment_");

//    json.erase("number");
    std::cout << json.to_string(0, true, json.getCommentColumnSize()) << std::endl;

    return 0;
}
