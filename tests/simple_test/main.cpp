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
//                                  "\"number\":182,\n"
                                  //иной вариант разделителя '='
                                  "\"bool\"=true,\n"
                                  //перенос строки равнозначен разделителю ','
//                                  "\"string\":\"string_value\"\n"
//                                  "\"json\":{\"string\":\"inner_string_value\"},\n"
                                  //перенос строки равнозначен разделителю ',' (массивы)
//                                  "\"array\":[\"string_value\"\n true]\n"
                                  "}";

int main() {
    system("tabs 4");

    Json json(json_string_example, ConfigFormat::eJSON);
    std::cout << "json size: " << json.size() << std::endl << std::endl;

    json.setCommentColumnSize(20);

    std::string preview_comment = "1;losdihfg2;slopighsd3;pogihvd4;pfgvibhdfns5;ipnbedf6 7;voihnaern8 som9 word1...";
    std::string preview_comment_result = "1;losdihfg2;\n"
                                         "slopighsd3;pogihvd4;\n"
                                         "pfgvibhdfns5;\n"
                                         "ipnbedf6 7;\n"
                                         "voihnaern8 som9\n"
                                         "word1...";
    json.addPreviewComment(preview_comment);
    std::string comment = "some words...";
    json.addComment("bool", comment);
    //    json.addComment("string",   "some many words1...",   CommentType::eBeforeValue);
    //    json.addComment("array",    "some many words2...",   CommentType::eAfterValueOneLine);
    //    json["json"].getJson().addComment(0, "json element comment", CommentType::eBeforeValueMultiLine);
    //    json["array"].getArray().addComment(0, "array element comment_", CommentType::eBeforeValueMultiLine);
    std::string path = "../test_writer_with_comments.json";

    std::ofstream file(path);
    if (!file.is_open())
        return 1;
    file << json.to_string(0, true, json.getCommentColumnSize()) << std::endl;
    file.close();
    //========================================================================
    Json json2;
    bool b = json2.readFile(path, false, ConfigFormat::eJSON); //по умолчанию считывается JSON формат

    std::cout << "json2 size: " << json2.size() << " (" << utils::to_string(b) << ")"<< std::endl;
//    EXPECT_EQ(preview_comment_result, json2.getPreviewComment().before);
//    EXPECT_EQ(comment, json2.getComment("bool").before);

    return 0;
}
