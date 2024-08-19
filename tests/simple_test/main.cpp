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
                          "    string2 : \"string\"value,/*some...\n*/"
//                          "    \"string2\" : stringvalue,/*some...\n*/"
//                          "    \"json\" : { \"string\" : \"inner string_value\" },\n"
//                          "    \"string\" : \"string_value\",\n"
//                          "    \"array\" : [\n"
//                          "           \"string_value\",\n"
//                          "           true\n"
//                          "    ]\n"
//                          "    \"number\": 182,\n"
//                          "\n"
//                          "    /*######################\n"
//                          "    # some words...\n"
//                          "    ######################*/\n"
//                          "    \"bool\" : true,\n"
                          "}\n";

std::string test_string_2 = "{string:inner_string_value}";

int main() {
    Json json;
    try {
        json.parseJSON(test_string);//, true); //FIXME: не работает
    } catch (std::invalid_argument e) {
        std::cout << "exception: " << e.what() << std::endl;
    }

//    json.parseJSON(test_string_2);//, true);

//    std::string test_1 = "часть коммента №1\n"
//                         "часть коммента №2\n"
//                         "часть коммента №3\n";
//    std::string test_2 = "#####################\n"
//                         "# часть коммента №1\n"
//                         "# часть коммента №2\n"
//                         "# часть коммента №3\n"
//                         "#####################";
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
//    uint8_t size = 0;
//    std::string result = test_string;
//    result = FromComment(test_1, size);
//    result = FromComment(test_2, size);
//    result = FromComment(test_3, size);
//    std::cout << result << std::endl;

    return 0;
}
