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

int main() {
    system("tabs 4");

    std::string path = "../test_writer_with_comments.json";
    Json json;
    bool b = json.readFile(path, true, ConfigFormat::eJSON); //по умолчанию считывается JSON формат

    std::cout << "json size: " << json.size() << std::endl << std::endl;

    std::cout << "PC: " << "\"" << json.getPreviewComment().before << "\"" << std::endl;
    for(uint8_t i = 0; i < json.size(); i++) {
        try{
            Comment comment = json.getComment(i);
            std::cout << "[" << (int)i << "]" << std::endl
                      //<< TODO: json.key
                      << "\tbefore: " << comment.before << std::endl
                      << "\tafter: " << comment.after << std::endl;
        } catch (...) {
            std::cout << "[" << (int)i << "]"
                      << "comments not found" << std::endl;
        }
    }

    return 0;
}
