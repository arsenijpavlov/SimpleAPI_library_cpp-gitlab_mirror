#include <SimpleAPI.h>

std::string test_string = "\n"
                          "/*######################\n*/"
                          "/# 1;losdihfg2;\n#/"
                          "<# slopighsd3;pogihvd4;\n#>"
                          "<-# pfgvibhdfns5;\n->"
                          "!.# ipnbedf6 7;\n.!"
                          "?.# ipnbedf6 7;\n.?"
                          "%# voihnaern8 som9\n"
                          "# word1...\n"
                          "!######################\n"
                          ";######################\n"
                          "?######################\n"
                          "//######################\n"
                          "{\n"
                          "    \"string2\" : stringvalue,/*some...\n*/"
                          "    \"json\" : { \"string\" : \"inner string_value\" },\n"
                          "    \"string\" : \"string_value\",\n"
                          "    \"array\" : [\n"
                          "           \"string_value\",\n"
                          "           true\n"
                          "    ]\n"
                          "    \"number\": 182,\n"
                          "\n"
                          "    /*######################\n"
                          "    # some words...\n"
                          "    ######################*/\n"
                          "    \"bool\" : true,\n"
                          "}\n";

int main() {
    Json json;
    json.parseJsonWithComment(test_string, PrintType::eWithComment);

    return 0;
}
