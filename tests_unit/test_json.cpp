#include <SimpleAPI.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);

    return RUN_ALL_TESTS();
}

//========================================================================================
Config json_example(ValueType::eJson,
                    "key_0", Config("first"), //NOTE: ручной каст к Config обязателен, из-за рекурсивной передачи variadic
                    "key_1", Config(2),
                    "key_2", Config(3.1),
                    "key_3", Config(true)
                    );
Config json2_example(
    std::vector<std::pair<std::string, Config>>{
        std::make_pair("key_0", Config("first")),
        std::make_pair("key_1", Config(2)),
        std::make_pair("key_2", Config(3.1)),
        std::make_pair("key_3", Config(true))
    }
);

std::string json_string_example = std::string(
    "{"
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
    "\"null\"=NuLL,\n"
    "\"null2\"=,\n"
    //перенос строки равнозначен разделителю ','
    "\"string\":\"string_value\"\n"
    "\"json\":{\"string\":\"inner_string_value\",\n \"string2\" : 150 },\n"
    //перенос строки равнозначен разделителю ',' (массивы)
    "\"array\":[\"string_value\"\n true]\n"
    "}"
    );


TEST(JSON, copy) {
    Config j1(std::vector<std::pair<std::string, Config>>{
               std::make_pair("asd", Config("123"))
    });
    j1.push_at("asa", "asdd");
    Config j2 = j1;

    EXPECT_EQ(j1.size(), j2.size());
}

//TEST(JSON, append_json) {
//    Config j1(ValueType::eJson, "asd", "asd");
//    uint8_t pre_size = j1.size();

//    Config j2(ValueType::eJson);
//    j2.push_at("num1", 15);
//    j2.push_at("num2", 1.5);
//    j2.push_at("bool", true);

//    j1.append(j2);

//    EXPECT_EQ(pre_size + j2.size(), j1.size());
//}

TEST(JSON, parse) {
    std::string string_json = json_string_example;
    Config json;
    json.parseJson(string_json);

    EXPECT_EQ(7, json.size());
    EXPECT_EQ(182, json["number"].getNumber());
    EXPECT_EQ(true, json["bool"].getBool());
    EXPECT_EQ(ValueType::eNull, json["null"].getType());
    EXPECT_EQ(ValueType::eNull, json["null2"].getType());
    EXPECT_EQ("string_value", json["string"].getString());

    EXPECT_EQ(2, json["json"].size());
    EXPECT_EQ("inner_string_value", json["json"]["string"].getString());

    EXPECT_EQ(2, json["array"].size());
    EXPECT_EQ("string_value", json["array"][0].getString());
    EXPECT_EQ(true, json["array"][1].getBool());

    //повторная обработка (очистка, новое заполнение)
    json.parseJson(string_json);
    EXPECT_EQ(7, json.size());
}

//TEST(JSON, parse2) {
//    std::string string_json = R"({"get":["chip_key"]})";
//    Json json;
//    json.parseJSON(string_json);

//    EXPECT_EQ(1, json.size());
//}

//TEST(JSON, parse3) {
//    std::string string_json = R"({"Hello":"WORLD!"})";
//    Json json;
//    json.parseJSON(string_json);

//    EXPECT_EQ(1, json.size());
//}

//TEST(JSON, parse_custom_string_elements) {
//    //ВСЕ экранированные символы должны попасть в значение без изменений
//    std::string temp_string = "\"asd\\\"\\b\\f\\n\\r\\tdsa\"";
//    std::string jarray_string = "[" + temp_string + "]";
//    std::string json_string = "{ string:" + temp_string + ", array:[" + temp_string + "]" + "}";

//    //исходная строка без кавычек по бокам
//    temp_string = temp_string.substr(1, temp_string.length() - 2);

//    Json json(json_string);
//    ASSERT_EQ(2, json.size());
//    EXPECT_EQ(temp_string, json["string"].to_string());
//    ASSERT_EQ(1, json["array"].getArray().size());
//    EXPECT_EQ(temp_string, json["array"].getArray()[0].to_string());
//}

//TEST(JSON, parse_error) {
//    std::string string_json;
//    Json json;

//    //некорректное значение числа, пробелов нет ==> это строка!
//    string_json = "{key:15.4.3}";
//    json.parseJSON(string_json);
//    EXPECT_EQ(1, json.size());
//    EXPECT_EQ(ValueType::eString, json[0].first);

//    //некорректное значение числа, пробелов нет ==> это строка!
//    string_json = "{key:15e43}";
//    try {
//        json.parseJSON(string_json);
//    } catch(...) {}
//    EXPECT_EQ(1, json.size());
//    EXPECT_EQ(ValueType::eNumber, json[0].first);

//    //пробелы в значении bool
//    string_json = "{key:tru e}";
//    try {
//        json.parseJSON(string_json);
//    } catch(...) {}
//    EXPECT_EQ(0, json.size());

//    //некорректное значение bool ==> это строка!
//    string_json = "{key:truee}";
//    json.parseJSON(string_json);
//    EXPECT_EQ(1, json.size());
//    EXPECT_EQ(ValueType::eString, json[0].first);
//}

//TEST(JSON, write_file) {
//    Json json = json_example;

//    json.writeFile("../tests/test_writer_with_spaces.json");
//    json.writeFile("../tests/test_writer.json", -1);
//}

//TEST(JSON, write_and_read_file_comment) {
//    Json json(json_string_example);

//    json.setCommentColumnSize(20);
//    json.setCommentSymbol('#');
//    json.addPreviewComment("1;losdihfg2;slopighsd3;pogihvd4;pfgvibhdfns5;ipnbedf6 7;voihnaern8 som9 word1...");
//    json.addComment_before("bool", "some \nwords...");
//    json.addComment_before("string", "some many words1...");
//    json.addComment_after("array", "some many words2...");

//    json["json"].getJson().addComment_before(0, "json element\n comment");
//    json["array"].getArray().addComment_before(0, "array element\n comment_");

//    std::string path = "../tests/test_writer_with_comments.json";
//    json.writeFile(path, 0, true, ConfigFormat::eJSON);

//    Json json2;
//    json2.readFileJSON(path, true);
//    EXPECT_EQ(json2.size(), json.size());

//    //TODO: проверка комментариев должна учитывать переносы строк (преобразование комментариев при чтении/записи)
////    EXPECT_EQ(json2.getPreviewComment(), json.getPreviewComment());
////    EXPECT_EQ(json2.getPreviewComment().prefix(), json.getPreviewComment().prefix);
////    EXPECT_EQ(json2.getPreviewComment().suffix(), json.getPreviewComment().suffix);

////    EXPECT_EQ(json2.getComment("bool"), json.getComment("bool"));
////    EXPECT_EQ(json2.getComment("bool").prefix(), json.getComment("bool").prefix);
////    EXPECT_EQ(json2.getComment("bool").suffix(), json.getComment("bool").suffix);

////    EXPECT_EQ(json2.getComment("string"), json.getComment("string"));
////    EXPECT_EQ(json2.getComment("string").prefix(), json.getComment("string").prefix);
////    EXPECT_EQ(json2.getComment("string").suffix(), json.getComment("string").suffix);

////    EXPECT_EQ(json2.getComment("array"), json.getComment("array"));
////    EXPECT_EQ(json2.getComment("array").prefix(), json.getComment("array").prefix);
////    EXPECT_EQ(json2.getComment("array").suffix(), json.getComment("array").suffix);

////    EXPECT_EQ(json2["json"].getJson().getComment(0), json["json"].getJson().getComment(0));
////    EXPECT_EQ(json2["json"].getJson().getComment(0).prefix(),
////              json["json"].getJson().getComment(0).prefix);
////    EXPECT_EQ(json2["json"].getJson().getComment(0).suffix(),
////              json["json"].getJson().getComment(0).suffix);

////    EXPECT_EQ(json2["array"].getArray().getComment(0), json["array"].getArray().getComment(0));
////    EXPECT_EQ(json2["array"].getArray().getComment(0).prefix(),
////              json["array"].getArray().getComment(0).prefix);
////    EXPECT_EQ(json2["array"].getArray().getComment(0).suffix(),
////              json["array"].getArray().getComment(0).suffix);
//}

//TEST(JSON, read_file) {
//    std::string path = "../tests/test_reader.json";

//    std::ofstream file_example_creator(path);
//    if(!file_example_creator.is_open())
//        FAIL();
//    file_example_creator.write(json_string_example.c_str(), json_string_example.size());
//    file_example_creator.close();

//    Json json;
//    json.readFile(path);

//    EXPECT_EQ(json.size(), Json(json_string_example).size());
//}

//TEST(JSON, read_file_comment) {
//    Json json(json_string_example);
//    json.setCommentColumnSize(20);

//    std::string preview_comment = "1;losdihfg2;slopighsd3;pogihvd4;pfgvibhdfns5;ipnbedf6 7;voihnaern8 som9 word1...";
//    std::string preview_comment_result = "1;losdihfg2;\n"
//                                         "slopighsd3;pogihvd4;\n"
//                                         "pfgvibhdfns5;\n"
//                                         "ipnbedf6 7;\n"
//                                         "voihnaern8 som9\n"
//                                         "word1...";
//    json.addPreviewComment(preview_comment);
//    std::string comment1 = "some words...";
//    std::string comment2 = "some many words1...";
//    std::string comment3 = "some many words2...";
//    std::string comment4 = "json element comment";
//    std::string comment5 = "array element comment_";
//    json.addComment("bool",         comment1);
//    json.addComment("string",       comment2);
//    json.addComment_after("array",  comment3);
//    json["json"].getJson().addComment(0,    comment4);
//    json["json"].getJson().addComment(1,    comment4);
//    json["array"].getArray().addComment(0,  comment5);
//    std::string path = "../tests/test_writer_with_comments.json";

//    std::ofstream file(path);
//    if (!file.is_open())
//        FAIL();
//    file << json.to_string(0, true, json.getCommentColumnSize()) << std::endl;
//    file.close();
//    //========================================================================

//    Json json2;
//    bool b = json2.readFile(path, true, ConfigFormat::eJSON); //по умолчанию считывается JSON формат
//    ASSERT_EQ(true, b);

//    EXPECT_EQ(json2.getPreviewComment().prefix(), preview_comment_result);
//    ASSERT_EQ(json2.contains("bool"), true);
//    EXPECT_EQ(json2.getComment("bool").prefix(), comment1);
//    ASSERT_EQ(json2.contains("string"), true);
//    EXPECT_EQ(json2.getComment("string").prefix(), comment2);
//    ASSERT_EQ(json2.contains("array"), true);
//    EXPECT_EQ(json2.getComment("array").suffix(), comment3);
//    ASSERT_EQ(json2.contains("json"), true);
//    ASSERT_EQ(json2["json"].getJson().size() > 0, true);
//    EXPECT_EQ(json2["json"].getJson().getComment(0).prefix(), comment4);
//    EXPECT_EQ(json2["json"].getJson().getComment(1).prefix(), comment4);
//    ASSERT_EQ(json2.contains("array"), true);
//    EXPECT_EQ(json2["array"].getArray().getComment(0).prefix(), comment5);
//}

//TEST(JSON, read_file_error) {
//    //файла не существует
//    Json json;
//    json.readFile("json_file_not_found.json");

//    EXPECT_EQ(json.size(), 0);
//}

//TEST(JSON, put_and_get_elements) {
//    std::string test_str    = "abc";
//    double test_num         = 15;
//    bool test_bool          = true;

//    Json json;
//    json.put("string", test_str);
//    json.put("number", test_num);
//    json.put("bool", test_bool);
//    EXPECT_EQ(json.size(), 3);

//    EXPECT_EQ(test_str, json["string"].getString());
//    EXPECT_EQ(test_num, json["number"].getNum());
//    EXPECT_EQ(test_bool, json["bool"].getBool());

//    ElementArray array;
//    array.push_back("str");
//    array.push_back(15);

//    Json json2;
//    json2.put("json", json);
//    json2.put("array", array);

//    EXPECT_EQ(json, json2["json"].getJson());
//    EXPECT_EQ(json["string"], json2["json"].getJson()["string"]);
//    EXPECT_EQ(json["string"].getString(), json2["json"].getJson()["string"].getString());

//    EXPECT_EQ(array, json2["array"].getArray());
//    EXPECT_EQ(array[0], json2["array"].getArray()[0]);
//    EXPECT_EQ(array[0].getString(), json2["array"].getArray()[0].getString());
//}

//TEST(JSON, update_value) {
//    Json json("key", 15);

//    json.put("key", 20);
//    EXPECT_EQ(json["key"].getNum(), 20);

//    json.put("key", 30, false);
//    EXPECT_EQ(json["key"].getNum(), 20);

//    json.updateValue("key", 40);
//    EXPECT_EQ(json["key"].getNum(), 40);

//    json.updateValue("key", true);
//    EXPECT_EQ(json["key"].getBool(), true);
//}

//TEST(JSON, get_index) {
//    Json json = json_example;

//    EXPECT_EQ(json.getValue(0).getString(), "first");
//    EXPECT_EQ(json.getValue(1).getNum(), 2);
//    EXPECT_EQ(json.getValue(2).getNum(), 3.1);
//    EXPECT_EQ(json.getValue(3).getBool(), true);
//}

//TEST(JSON, get_index_error) {
//    Json json = json_example;

//    try {
//        json[5];
//    } catch (std::out_of_range& e) {
//        return SUCCEED();
//    }

//    FAIL();
//}

//TEST(JSON, get_complex_name) {
//    Json j("json_num", 15);
//    ElementArray a;
//    a.push_back(j);
//    Json j_main;
//    j_main.put("array", a);

//    double d = j_main[{"array", "0", "json_num"}].getNum();
//    EXPECT_EQ(15, d);
//}

//TEST(JSON, get_key) {
//    Json json = json_example;

//    EXPECT_EQ(json.getValue("key_0").getString(), "first");
//    EXPECT_EQ(json.getValue("key_1").getNum(), 2);
//    EXPECT_EQ(json.getValue("key_2").getNum(), 3.1);
//    EXPECT_EQ(json.getValue("key_3").getBool(), true);
//}

////больше не актуален, т.к. переменные добавляются автоматически
////TEST(JSON, get_key_error) {
////    Json json = json_example;

////    try {
////        Element el = json.getValue("azaza");
////    } catch (std::invalid_argument& e) {
////        return SUCCEED();
////    }

////    FAIL();
////}

//TEST(JSON, insert) {
//    Json json = json_example;

//    json.insert(json.begin() + 2, "inner_json1", json_example);
//    EXPECT_EQ(json[2].first, ValueType::eJson);

//    json.insert(4, "inner_json2", json_example);
//    EXPECT_EQ(json[4].first, ValueType::eJson);
//}

//TEST(JSON, insert_before) {
//    Json json = json_example;
//    json.insertBefore("key_1", "inner_json", json_example);

//    EXPECT_EQ(json[1].first, ValueType::eJson);
//}

//TEST(JSON, insert_after) {
//    Json json = json_example;
//    json.insertAfter("key_1", "inner_json", json_example);

//    EXPECT_EQ(json[2].first, ValueType::eJson);
//}

//TEST(JSON, erase_it) {
//    Json json = json_example;
//    json.erase(json.begin() + 2);
//    EXPECT_EQ(json[2].first, ValueType::eBool);

//    json = json_example;
//    json.erase(2);
//    EXPECT_EQ(json[2].first, ValueType::eBool);

//    json = json_example;
//    json.erase(json.begin(), json.begin() + 3);
//    EXPECT_EQ(json[0].first, ValueType::eBool);
//}

//TEST(JSON, erase_key) {
//    Json json = json_example;

//    json.erase("key_0");
//    EXPECT_EQ(json[0].first, ValueType::eNumber);
//}

//TEST(JSON, erase_keys) {
//    Json json = json_example;

//    json.erase({"key_0", "key_1", "key_2"});
//    EXPECT_EQ(json[0].first, ValueType::eBool);
//}

//TEST(JSON, check_numbers) {
//    std::map<std::string, bool> map_numbers;
//    map_numbers.insert(std::make_pair("1",          true));
//    map_numbers.insert(std::make_pair("1.1",        true));
//    map_numbers.insert(std::make_pair("1e1",        true));
//    map_numbers.insert(std::make_pair("1.1e1",      true));
//    map_numbers.insert(std::make_pair("1.1.1",      false));
//    map_numbers.insert(std::make_pair("e1",         false));
//    map_numbers.insert(std::make_pair("f1",         false));
//    map_numbers.insert(std::make_pair(".1",         true));
//    map_numbers.insert(std::make_pair("a1e2",       false));
//    map_numbers.insert(std::make_pair("145o",       false));
//    map_numbers.insert(std::make_pair("1eu2",       false));
//    map_numbers.insert(std::make_pair("1E5",        true));
//    map_numbers.insert(std::make_pair("1f",         true));
//    map_numbers.insert(std::make_pair("1e",         false));
//    map_numbers.insert(std::make_pair("1ef",        false));
//    map_numbers.insert(std::make_pair("1e1.1",      false));
//    map_numbers.insert(std::make_pair("1.1e-1",     true));
//    map_numbers.insert(std::make_pair("-1.1e-1",    true));
//    map_numbers.insert(std::make_pair("-1.1e+1",    true));
//    map_numbers.insert(std::make_pair("-1.1e+1e",   false));
//    map_numbers.insert(std::make_pair("f",          false));

//    for(auto it = map_numbers.cbegin(); it != map_numbers.cend(); it++) {
//        Json json("{number:" + it->first + "}");
//        EXPECT_EQ((json[0].first == ValueType::eNumber), it->second);
//    }
//}

//TEST(JSON, parse_simple_element) {
//    std::string test_file_string = "k = 1";
//    Json json;
//    json.parseJSON(test_file_string);
//    EXPECT_EQ(json.size(), 1);

//    test_file_string = "k = \"string line\"";
//    json.parseJSON(test_file_string);
//    EXPECT_EQ(json.size(), 1);

//    test_file_string = "k = true";
//    json.parseJSON(test_file_string);
//    EXPECT_EQ(json.size(), 1);

//    test_file_string = "k = ";
//    json.parseJSON(test_file_string);
//    EXPECT_EQ(json.size(), 1);

//    test_file_string = "k = null";
//    json.parseJSON(test_file_string);
//    EXPECT_EQ(json.size(), 1);

//    test_file_string = "k = word";
//    json.parseJSON(test_file_string);
//    EXPECT_EQ(json.size(), 1);

//    test_file_string = "k = {a=b}";
//    json.parseJSON(test_file_string);
//    EXPECT_EQ(json.size(), 1);

//    test_file_string = "k = [a,b]";
//    json.parseJSON(test_file_string);
//    EXPECT_EQ(json.size(), 1);
//}
