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
    //однострочные комментарии (используются знаки по умолчанию)
    "//\n"
    //многострочные комментарии (используются знаки по умолчанию)
    "/*\n*/"
    //поля
    "\"number\":182,\n"
    //иной вариант разделителя '='
    "\"bool\"=true,\n"
    "\"null\"=NuLL,\n"
    "\"null2\"=,\n"
    //перенос строки равнозначен разделителю ','
    "\"string\":\"string_value\"\n"
    "\"json\" : {"
    "\"string\":\"inner_string_value\",\n \"string2\" : 150"
    "}"
    ",\n"
    //перенос строки равнозначен разделителю ',' (массивы)
    "\"array\":[\"string_value\"\n true]\n"
    "}"
    );

std::string inner_json_example = "{\"string\":\"inner_string_value\",\n \"string2\" : 150 }";
std::string inner_array_example = "[\"string_value\"\n true]";


TEST(JSON, copy) {
    Config j1(std::vector<std::pair<std::string, Config>>{
               std::make_pair("asd", Config("123"))
    });
    j1.push_at("asa", "asdd");
    Config j2 = j1;

    EXPECT_EQ(j1.size(), j2.size());
}

TEST(JSON, append_json) {
    Config j1(ValueType::eJson, "asd", "asd");
    uint8_t pre_size = j1.size();

    Config j2(ValueType::eJson);
    j2.push_at("num1", 15);
    j2.push_at("num2", 1.5);
    j2.push_at("bool", true);

    j1.append(j2);

    EXPECT_EQ(pre_size + j2.size(), j1.size());
}

TEST(JSON, parse) {
    std::string string_json = json_string_example;
    std::string string_json2 = json_string_example;
    Config json;

    Config inner_json;
    inner_json.parseJson(inner_json_example);
    ASSERT_EQ(inner_json.size(), 2);
    EXPECT_TRUE(inner_json["string"].isString());
    EXPECT_TRUE(inner_json["string2"].isNumber());

    Config inner_array;
    inner_array.parseArray(inner_array_example);
    ASSERT_EQ(inner_array.size(), 2);
    EXPECT_TRUE(inner_array[0].isString());
    EXPECT_TRUE(inner_array[1].isBool());

    json.parseJson(string_json);
    EXPECT_EQ(json.size(), 7);

    EXPECT_TRUE(json["number"].isNumber());
    EXPECT_EQ(json["number"].getNumber(),           182);
    EXPECT_EQ(json["number"],                       182);

    EXPECT_TRUE(json["bool"].isBool());
    EXPECT_EQ(json["bool"].getBool(),               true);
    EXPECT_EQ(json["bool"].getBool(),               true);

    EXPECT_TRUE(json["null"].isNull());
    EXPECT_EQ(json["null"],                         Config());
    EXPECT_EQ(json["null"],                         Config(ValueType::eNull));

    EXPECT_TRUE(json["null2"].isNull());
    EXPECT_EQ(json["null2"],                        Config());
    EXPECT_EQ(json["null2"],                        Config(ValueType::eNull));

    EXPECT_TRUE(json["string"].isString());
    EXPECT_EQ(json["string"].getString(),           "string_value");
    EXPECT_EQ(json["string"],                       "string_value");

    EXPECT_FALSE(json["json"].isNull());
    EXPECT_TRUE(json["json"].isJson());
    ASSERT_EQ(json["json"].size(),                  2);
    EXPECT_EQ(json["json"]["string"].getString(),   "inner_string_value");

    EXPECT_TRUE(json["array"].isArray());
    ASSERT_EQ(json["array"].size(),                 2);
    EXPECT_EQ(json["array"][0].getString(),         "string_value");
    EXPECT_EQ(json["array"][1].getBool(),           true);

    //повторная обработка (очистка, новое заполнение)
    json.parseJson(string_json2);
    EXPECT_EQ(json.size(), 7);

    json = Config::CreateElementFromString(std::move(string_json), ConfigFormat::eJSON);
    ASSERT_EQ(json.size(), 7);
}

TEST(JSON, parse2) {
    std::string string_json = R"({"get":["chip_key"]})";
    Config json;
    json.parseJson(string_json);

    EXPECT_EQ(json.size(), 1);
}

TEST(JSON, parse3) {
    std::string string_json = R"({"Hello":"WORLD!"})";
    Config json;
    json.parseJson(string_json);

    EXPECT_EQ(json.size(), 1);
}

TEST(JSON, parse_custom_string_elements) {
    //ВСЕ экранированные символы должны попасть в значение без изменений
    std::string temp_string = "\"asd\\\"\\b\\f\\n\\r\\tdsa\"";
    std::string json_string = std::string("{ ")
                              + "string:" + temp_string
                              + ", array:[" + temp_string + "]"
                              + "}";

    //исходная строка без кавычек по бокам
    std::string temp_string_without_quotes = temp_string.substr(1, temp_string.length() - 2);

    Config inner_array;
    std::string inner_array_str = "[" + temp_string + "]";
    inner_array.parseArray(inner_array_str);
    ASSERT_EQ(inner_array.size(),           1);

    Config json;
    json.parseJson(json_string);

    ASSERT_TRUE(json.isJson());
    EXPECT_EQ(json.size(),                  2);

    EXPECT_TRUE(json["string"].isString());
    EXPECT_EQ(json["string"],               temp_string_without_quotes);
    EXPECT_EQ(json["string"].toString(),    temp_string_without_quotes);

    ASSERT_TRUE(json["array"].isArray());
    ASSERT_EQ(json["array"].size(),         1);
    EXPECT_EQ(json["array"][0],             temp_string_without_quotes);
    EXPECT_EQ(json["array"][0].toString(),  temp_string_without_quotes);
}

TEST(JSON, parse_error) {
    std::string string_json;
    Config json;

    //некорректное значение числа, пробелов нет ==> это строка!
    string_json = "{key:15.4.3}";
    json.parseJson(string_json);
    EXPECT_EQ(json.size(), 1);
    EXPECT_TRUE(json[0].isString());
    EXPECT_EQ(json["key"], "15.4.3");

    //некорректное значение числа, пробелов нет ==> это строка!
    string_json = "{key:15e43}";
    try {
        json.parseJson(string_json);
    } catch(...) {}
    EXPECT_EQ(json.size(), 1);
    EXPECT_TRUE(json[0].isNumber());

    //пробелы в значении bool
    string_json = "{key:tru e}";
    try {
        json.parseJson(string_json);
    } catch(...) {}
    EXPECT_EQ(json.size(), 1);
    EXPECT_TRUE(json["key"].isString());

    //некорректное значение bool ==> это строка!
    string_json = "{key:truee}";
    json.parseJson(string_json);
    EXPECT_EQ(json.size(), 1);
    EXPECT_TRUE(json[0].isString());
}

//TODO: writeFile   custom_tabulation_level

TEST(JSON, write_file) {
    Config json = json_example;

    bool ret = json.writeFile("../tests/test_writer_with_spaces.json", ConfigFormat::eJSON);
    EXPECT_TRUE(ret);

    ret = json.writeFile("../tests/test_writer.json", ConfigFormat::eONLY_VALUE);
    EXPECT_TRUE(ret);
}

TEST(JSON, parse_with_comments) {
    std::string test_input = "{a:15,/*s_comment*//*p1_comment*//*p2_comment*/b:120}";
    Config json;
    json.parseJson(test_input, true);

    EXPECT_EQ(json.get_suffix_comment("a"), "s_comment");
    EXPECT_EQ(json.get_prefix_comment("b"), "p1_comment\n"
                                            "p2_comment");

    test_input = "{a:15, //s1_comment\n"
                 "//p2_comment\n"
                 "b:20 //s2_comment\n"
                 "} //sf_comment\n";
    json.parseJson(test_input, true);

    EXPECT_EQ(json.get_suffix_comment("a"), "s1_comment");
    EXPECT_EQ(json.get_prefix_comment("b"), "p2_comment");
    EXPECT_EQ(json.get_suffix_comment("b"), "s2_comment");
    EXPECT_EQ(json.getSuffixComment(), "sf_comment");
}

TEST(JSON, write_and_read_file_comment) {
    Config json(ValueType::eJson);
    json.parseJson(json_string_example);

    CommentDesign& cd = json.getCommentDesign();
    cd.opt_multiline_column_size = /*20*/0;
    cd.opt_multiline_border = '#';
    cd.with_comments = true;
//    cd.multiline_comment_variants.push_back(std::array<char, 3>{'/', '*', 0});

    std::string main_comment = "1;losdihfg2;slopighsd3;pogihvd4;pfgvibhdfns5;ipnbedf6 7;voihnaern8 som9 word1...";
    json.addPrefixComment(main_comment);
    json.addSuffixComment(main_comment);
    json.add_prefix_comment("bool", "some \nwords...");
    json.add_suffix_comment("bool", "some \nwords...");
    json.add_prefix_comment("string", "<string> prefix comment");
    json.add_prefix_comment("array", "<array> prefix comment");
    json.add_suffix_comment("array", "<array> suffix comment");
    json.add_prefix_comment("json", "<json> prefix comment");
    json.add_suffix_comment("json", "<json> suffix comment");

    json["json"].add_prefix_comment(0, "json first element\n comment");
    json["array"].add_prefix_comment(1, "array second element\n comment_");

    std::string path = "../tests/test_writer_with_comments.json";
    json.writeFile(path, ConfigFormat::eJSON, cd, 0);
//    SUCCEED();
//    return;

    Config json2;
    try {
        json2.readFileJson(path, true);
    } catch (const std::exception& e) {
        FAIL() << e.what();
    }
    EXPECT_EQ(json2.size(), json.size());
    EXPECT_EQ(json2.getCommentDesign(), json.getCommentDesign());

    //FIXME: проверка комментариев должна учитывать переносы строк (преобразование комментариев при чтении/записи)

//    main_comment = FromComment(ToComment(main_comment, json.getCommentDesign()), json2.getCommentDesign());
    main_comment = FromComment(ToComment(main_comment, cd), json2.getCommentDesign());
    EXPECT_EQ(json2.getComment(), json.getComment());
    EXPECT_EQ(json2.getPrefixComment(), json.getPrefixComment());
    EXPECT_EQ(json2.getSuffixComment(), json.getSuffixComment());
    EXPECT_EQ(json2.getPrefixComment(), main_comment);
    EXPECT_EQ(json2.getSuffixComment(), main_comment);

    EXPECT_EQ(json2.get_comment("bool"), json.get_comment("bool"));
    EXPECT_EQ(json2.get_prefix_comment("bool"), json.get_prefix_comment("bool"));
    EXPECT_EQ(json2.get_suffix_comment("bool"), json.get_suffix_comment("bool"));

//    EXPECT_EQ(json2.get_comment("string"), json.get_comment("string"));
//    EXPECT_EQ(json2.get_prefix_comment("string"), json.get_prefix_comment("string"));
//    EXPECT_EQ(json2.get_suffix_comment("string"), json.get_suffix_comment("string"));

//    EXPECT_EQ(json2.get_comment("array"), json.get_comment("array"));
//    EXPECT_EQ(json2.get_prefix_comment("array"), json.get_prefix_comment("array"));
//    EXPECT_EQ(json2.get_suffix_comment("array"), json.get_suffix_comment("array"));

//    EXPECT_EQ(json2["json"].get_comment(0), json["json"].get_comment(0));
//    EXPECT_EQ(json2["json"].get_prefix_comment(0),
//              json["json"].get_prefix_comment(0));
//    EXPECT_EQ(json2["json"].get_suffix_comment(0),
//              json["json"].get_suffix_comment(0));

//    EXPECT_EQ(json2["array"].get_comment(0), json["array"].get_comment(0));
//    EXPECT_EQ(json2["array"].get_prefix_comment(0),
//              json["array"].get_prefix_comment(0));
//    EXPECT_EQ(json2["array"].get_suffix_comment(0),
//              json["array"].get_suffix_comment(0));
}

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
