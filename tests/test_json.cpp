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

TEST(ELEMENT, create_num) {
    Element el_int(1);
    Element el_double(1.1);
    Element el_uint(11);
    Element el_float(11.f);

    EXPECT_EQ(eNumber, el_int.first);
    EXPECT_EQ(eNumber, el_double.first);
    EXPECT_EQ(eNumber, el_uint.first);
    EXPECT_EQ(eNumber, el_float.first);
}

TEST(ELEMENT, create_srting) {
    Element el_string(std::string("asd"));
    Element el_char_arr("asd");
    const char* chr = "asd";
    Element el_char_star(chr);

    EXPECT_EQ(eString, el_string.first);
    EXPECT_EQ(eString, el_char_arr.first);
    EXPECT_EQ(eString, el_char_star.first);
}

TEST(ELEMENT, create_bool) {
    Element el_bool(true);

    EXPECT_EQ(eBool, el_bool.first);
}

TEST(ELEMENT, create_json) {
    Json js;
    Element el_json(js);

    EXPECT_EQ(eJson, el_json.first);
}

TEST(ELEMENT, create_array) {
    JArray ar;
    Element el_array(ar);

    EXPECT_EQ(eArray, el_array.first);
}

TEST(ELEMENT, compare_all_types) {
    Element el_num(15.0);
    Element el_bool(true);
    Element el_string("true");
    Element el_json(Json{});
    Element el_array(JArray{});

    EXPECT_EQ(el_num.first,     eNumber);
    EXPECT_EQ(el_bool.first,    eBool);
    EXPECT_EQ(el_string.first,  eString);
    EXPECT_EQ(el_json.first,    eJson);
    EXPECT_EQ(el_array.first,   eArray);

    EXPECT_EQ(el_num,   Element(15.0));
    EXPECT_EQ(el_bool,  Element(true));
    EXPECT_EQ(el_string,Element("true"));
    EXPECT_EQ(el_json,  Element(Json{}));
    EXPECT_EQ(el_array, Element(JArray{}));
}

TEST(ELEMENT, not_compare_all_types) {
    Element el_num(15.0);
    Element el_bool(true);
    Element el_string("true");
    Element el_json(Json{});
    Element el_array(JArray{});

    EXPECT_EQ(el_num.first,     eNumber);
    EXPECT_EQ(el_bool.first,    eBool);
    EXPECT_EQ(el_string.first,  eString);
    EXPECT_EQ(el_json.first,    eJson);
    EXPECT_EQ(el_array.first,   eArray);

    EXPECT_NE(el_num,   Element(1.0));
    EXPECT_NE(el_bool,  Element(false));
    EXPECT_NE(el_string,Element("asd"));
    EXPECT_NE(el_json,  Element(Json().put("asd", "asd")));
    EXPECT_NE(el_array, Element(JArray().push_front("asd")));
}

//========================================================================================
Json json_example({{"key_0", "first"},
                   {"key_1", 2},
                   {"key_2", 3.1},
                   {"key_3", true}
});
std::string json_string_example = "{"
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
                                  "\"json\":{\"string\":\"inner_string_value\",\n \"string2\" : 150 },\n"
                                  //перенос строки равнозначен разделителю ',' (массивы)
                                  "\"array\":[\"string_value\"\n true]\n"
                                  "}";


TEST(JSON, copy) {
    Json j1("asd", "123");
    j1.put("asa", "asdd");
    Json j2 = j1;

    EXPECT_EQ(j1.size(), j2.size());
}

TEST(JSON, append_json) {
    Json j1("asd", "asd");
    uint8_t pre_size = j1.size();

    Json j2;
    j2.put("num1", 15);
    j2.put("num2", 1.5);
    j2.put("bool", true);

    j1.append(j2);

    EXPECT_EQ(pre_size + j2.size(), j1.size());
}

TEST(JSON, parse) {
    std::string string_json = json_string_example;
    Json json;
    json.parseJSON(string_json);

    EXPECT_EQ(5, json.size());
    EXPECT_EQ(182, json["number"].getNum());
    EXPECT_EQ(true, json["bool"].getBool());
    EXPECT_EQ("string_value", json["string"].getString());

    EXPECT_EQ(1, json["json"].getJson().size());
    EXPECT_EQ("inner_string_value", json["json"].getJson()["string"].getString());

    EXPECT_EQ(2, json["array"].getArray().size());
    EXPECT_EQ("string_value", json["array"].getArray()[0].getString());
    EXPECT_EQ(true, json["array"].getArray()[1].getBool());

    //повторная обработка (очистка, новое заполнение)
    json.parseJSON(string_json);
    EXPECT_EQ(5, json.size());
}

TEST(JSON, parse2) {
    std::string string_json = R"({"get":["chip_key"]})";
    Json json;
    json.parseJSON(string_json);

    EXPECT_EQ(1, json.size());
}

TEST(JSON, parse3) {
    std::string string_json = R"({"Hello":"WORLD!"})";
    Json json;
    json.parseJSON(string_json);

    EXPECT_EQ(1, json.size());
}

TEST(JSON, parse_custom_string_elements) {
    std::string temp_string = "\"\\\"\\b\\f\\n\\r\\t\"";
    std::string jarray_string = "[" + temp_string + "]";
    std::string json_string = "{ string:" + temp_string + ", array:[" + temp_string + "]" + "}";

    Json json(json_string);
    ASSERT_EQ(2, json.size());
    EXPECT_EQ(temp_string, json["string"].to_string());
    ASSERT_EQ(1, json["array"].getArray().size());
    EXPECT_EQ(temp_string, json["array"].getArray()[0].to_string());
}

TEST(JSON, parse_error) {
    std::string string_json;
    Json json;

    //некорректное значение числа, пробелов нет ==> это строка!
    string_json = "{key:15.4.3}";
    json.parseJSON(string_json);
    EXPECT_EQ(1, json.size());
    EXPECT_EQ(eString, json[0].first);

    //некорректное значение числа, пробелов нет ==> это строка!
    string_json = "{key:15e43}";
    try {
        json.parseJSON(string_json);
    } catch(...) {}
    EXPECT_EQ(1, json.size());
    EXPECT_EQ(eNumber, json[0].first);

    //пробелы в значении bool
    string_json = "{key:tru e}";
    try {
        json.parseJSON(string_json);
    } catch(...) {}
    EXPECT_EQ(0, json.size());

    //некорректное значение bool ==> это строка!
    string_json = "{key:truee}";
    json.parseJSON(string_json);
    EXPECT_EQ(1, json.size());
    EXPECT_EQ(eString, json[0].first);
}

TEST(JSON, write_file) {
    Json json = json_example;

    json.writeFile("../tests/test_writer_with_spaces.json");
    json.writeFile("../tests/test_writer.json", -1);
}

//TODO: тест не завершён
TEST(JSON, write_file_comment) {
    Json json(json_string_example);

    json.setCommentColumnSize(20);
    json.setCommentSymbol('#');
    json.addPreviewComment("1;losdihfg2;slopighsd3;pogihvd4;pfgvibhdfns5;ipnbedf6 7;voihnaern8 som9 word1...");
    json.addComment_before("bool", "some \nwords...");
    json.addComment_before("string", "some many words1...");
    json.addComment_after("array", "some many words2...");

//    json.clearPreviewComment();
//    json.clearComment("bool");

    json["json"].getJson().addComment_before(0, "json element\n comment");
    json["array"].getArray().addComment_before(0, "array element\n comment_");

    std::string path = "../tests/test_writer_with_comments.json";

    std::ofstream file(path);
    if (!file.is_open())
        FAIL();

    json.erase("number");
    file << json.to_string(0, true, json.getCommentColumnSize()) << std::endl;

    file.flush();
    file.close();

    return SUCCEED();
}

TEST(JSON, read_file) {
    std::string path = "../tests/test_reader.json";

    std::ofstream file_example_creator(path);
    if(!file_example_creator.is_open())
        FAIL();
    file_example_creator.write(json_string_example.c_str(), json_string_example.size());
    file_example_creator.close();

    Json json;
    json.readFile(path);

    EXPECT_EQ(json.size(), Json(json_string_example).size());
}

TEST(JSON, read_file_comment) {
    Json json(json_string_example);
    json.setCommentColumnSize(20);

    std::string preview_comment = "1;losdihfg2;slopighsd3;pogihvd4;pfgvibhdfns5;ipnbedf6 7;voihnaern8 som9 word1...";
    std::string preview_comment_result = "1;losdihfg2;\n"
                                         "slopighsd3;pogihvd4;\n"
                                         "pfgvibhdfns5;\n"
                                         "ipnbedf6 7;\n"
                                         "voihnaern8 som9\n"
                                         "word1...";
    json.addPreviewComment(preview_comment);
    std::string comment1 = "some words...";
    std::string comment2 = "some many words1...";
    std::string comment3 = "some many words2...";
    std::string comment4 = "json element comment";
    std::string comment5 = "array element comment_";
    json.addComment("bool",         comment1);
    json.addComment("string",       comment2);
    json.addComment_after("array",  comment3);
    json["json"].getJson().addComment(0,    comment4);
    json["json"].getJson().addComment(1,    comment4);
    json["array"].getArray().addComment(0,  comment5);
    std::string path = "../tests/test_writer_with_comments.json";

    std::ofstream file(path);
    if (!file.is_open())
        FAIL();
    file << json.to_string(0, true, json.getCommentColumnSize()) << std::endl;
    file.close();
    //========================================================================

    Json json2;
    bool b = json2.readFile(path, true, ConfigFormat::eJSON); //по умолчанию считывается JSON формат
    ASSERT_EQ(true, b);

    EXPECT_EQ(json2.getPreviewComment().before, preview_comment_result);
    ASSERT_EQ(json2.contains("bool"), true);
    EXPECT_EQ(json2.getComment("bool").before, comment1);
    ASSERT_EQ(json2.contains("string"), true);
    EXPECT_EQ(json2.getComment("string").before, comment2);
    ASSERT_EQ(json2.contains("array"), true);
    EXPECT_EQ(json2.getComment("array").after, comment3);
    ASSERT_EQ(json2.contains("json"), true);
    ASSERT_EQ(json2["json"].getJson().size() > 0, true);
    EXPECT_EQ(json2["json"].getJson().getComment(0).before, comment4);
    EXPECT_EQ(json2["json"].getJson().getComment(1).before, comment4);
    ASSERT_EQ(json2.contains("array"), true);
    EXPECT_EQ(json2["array"].getArray().getComment(0).before, comment5);
}

TEST(JSON, read_file_error) {
    //файла не существует
    Json json;
    json.readFile("json_file_not_found.json");

    EXPECT_EQ(json.size(), 0);
}

TEST(JSON, put_and_get_elements) {
    std::string test_str    = "abc";
    double test_num         = 15;
    bool test_bool          = true;

    Json json;
    json.put("string", test_str);
    json.put("number", test_num);
    json.put("bool", test_bool);

    EXPECT_EQ(test_str, json["string"].getString());
    EXPECT_EQ(test_num, json["number"].getNum());
    EXPECT_EQ(test_bool, json["bool"].getBool());

    JArray array;
    array.push_back("str");
    array.push_back(15);

    Json json2;
    json2.put("json", json);
    json2.put("array", array);

    EXPECT_EQ(json, json2["json"].getJson());
    EXPECT_EQ(json["string"], json2["json"].getJson()["string"]);
    EXPECT_EQ(json["string"].getString(), json2["json"].getJson()["string"].getString());

    EXPECT_EQ(array, json2["array"].getArray());
    EXPECT_EQ(array[0], json2["array"].getArray()[0]);
    EXPECT_EQ(array[0].getString(), json2["array"].getArray()[0].getString());
}

TEST(JSON, update_value) {
    Json json("key", 15);

    json.put("key", 20);
    EXPECT_EQ(json["key"].getNum(), 20);

    json.put("key", 30, false);
    EXPECT_EQ(json["key"].getNum(), 20);

    json.updateValue("key", 40);
    EXPECT_EQ(json["key"].getNum(), 40);

    json.updateValue("key", true);
    EXPECT_EQ(json["key"].getBool(), true);
}

TEST(JSON, get_index) {
    Json json = json_example;

    EXPECT_EQ(json.getValue(0).getString(), "first");
    EXPECT_EQ(json.getValue(1).getNum(), 2);
    EXPECT_EQ(json.getValue(2).getNum(), 3.1);
    EXPECT_EQ(json.getValue(3).getBool(), true);
}

TEST(JSON, get_index_error) {
    Json json = json_example;

    try {
        json[5];
    } catch (std::out_of_range& e) {
        return SUCCEED();
    }

    FAIL();
}

TEST(JSON, get_complex_name) {
    Json j("json_num", 15);
    JArray a;
    a.push_back(j);
    Json j_main;
    j_main.put("array", a);

    double d = j_main[{"array", "0", "json_num"}].getNum();
    EXPECT_EQ(15, d);
}

TEST(JSON, get_key) {
    Json json = json_example;

    EXPECT_EQ(json.getValue("key_0").getString(), "first");
    EXPECT_EQ(json.getValue("key_1").getNum(), 2);
    EXPECT_EQ(json.getValue("key_2").getNum(), 3.1);
    EXPECT_EQ(json.getValue("key_3").getBool(), true);
}

TEST(JSON, get_key_error) {
    Json json = json_example;

    try {
        Element el = json.getValue("azaza");
    } catch (std::invalid_argument& e) {
        return SUCCEED();
    }

    FAIL();
}

TEST(JSON, insert) {
    Json json = json_example;

    json.insert(json.begin() + 2, "inner_json1", json_example);
    EXPECT_EQ(json[2].first, ValueType::eJson);

    json.insert(4, "inner_json2", json_example);
    EXPECT_EQ(json[4].first, ValueType::eJson);
}

TEST(JSON, insert_before) {
    Json json = json_example;
    json.insertBefore("key_1", "inner_json", json_example);

    EXPECT_EQ(json[1].first, ValueType::eJson);
}

TEST(JSON, insert_after) {
    Json json = json_example;
    json.insertAfter("key_1", "inner_json", json_example);

    EXPECT_EQ(json[2].first, ValueType::eJson);
}

TEST(JSON, erase_it) {
    Json json = json_example;
    json.erase(json.begin() + 2);
    EXPECT_EQ(json[2].first, ValueType::eBool);

    json = json_example;
    json.erase(2);
    EXPECT_EQ(json[2].first, ValueType::eBool);

    json = json_example;
    json.erase(json.begin(), json.begin() + 3);
    EXPECT_EQ(json[0].first, ValueType::eBool);
}

TEST(JSON, erase_key) {
    Json json = json_example;

    json.erase("key_0");
    EXPECT_EQ(json[0].first, ValueType::eNumber);
}

TEST(JSON, erase_keys) {
    Json json = json_example;

    json.erase({"key_0", "key_1", "key_2"});
    EXPECT_EQ(json[0].first, ValueType::eBool);
}

TEST(JSON, check_numbers) {
    std::map<std::string, bool> map_numbers;
    map_numbers.insert(std::make_pair("1",          true));
    map_numbers.insert(std::make_pair("1.1",        true));
    map_numbers.insert(std::make_pair("1e1",        true));
    map_numbers.insert(std::make_pair("1.1e1",      true));
    map_numbers.insert(std::make_pair("1.1.1",      false));
    map_numbers.insert(std::make_pair("e1",         false));
    map_numbers.insert(std::make_pair("f1",         false));
    map_numbers.insert(std::make_pair(".1",         true));
    map_numbers.insert(std::make_pair("a1e2",       false));
    map_numbers.insert(std::make_pair("145o",       false));
    map_numbers.insert(std::make_pair("1eu2",       false));
    map_numbers.insert(std::make_pair("1E5",        true));
    map_numbers.insert(std::make_pair("1f",         true));
    map_numbers.insert(std::make_pair("1e",         false));
    map_numbers.insert(std::make_pair("1ef",        false));
    map_numbers.insert(std::make_pair("1e1.1",      false));
    map_numbers.insert(std::make_pair("1.1e-1",     true));
    map_numbers.insert(std::make_pair("-1.1e-1",    true));
    map_numbers.insert(std::make_pair("-1.1e+1",    true));
    map_numbers.insert(std::make_pair("-1.1e+1e",   false));
    map_numbers.insert(std::make_pair("f",          false));

    for(auto it = map_numbers.cbegin(); it != map_numbers.cend(); it++) {
        Json json("{number:" + it->first + "}");
        EXPECT_EQ((json[0].first == eNumber), it->second);
    }
}
//========================================================================================
JArray array_example("first", 2, 3.1, true);


TEST(ARRAY, copy) {
    JArray a1("asd", 123);
    JArray a2 = a1;

    EXPECT_EQ(a1.size(), a2.size());
}

TEST(ARRAY, append_array) {
    JArray a1 = array_example;
    uint8_t pre_size = a1.size();

    JArray a2;
    a2.push_back(15);
    a2.push_back(1.5);
    a2.push_back(true);

    a1.append(a2);

    EXPECT_EQ(pre_size + a2.size(), a1.size());
}

TEST(ARRAY, parse) {
    std::string string_array = "[15, true, \"string\"]";
    JArray array; //JArray(std::string) нельзя, т.к. положит это в значения, а не распарсит
    //TODO: JArray(std::string, SettingFormat::eJSON)
    array.parseArray(string_array);

    EXPECT_EQ(3, array.size());
    EXPECT_EQ(15, array[0].getNum());
    EXPECT_EQ(true, array[1].getBool());
    EXPECT_EQ("string", array[2].getString());
}

TEST(ARRAY, parse_error) {
    //нет запятой между элементами
    std::string string_array = "[15 true, \"string\"]";
    JArray array;
    try {
        array.parseArray(string_array);
    } catch (...) {}
    EXPECT_EQ(0, array.size());
}

TEST(ARRAY, get_index) {
    JArray array = array_example;

    EXPECT_EQ(array.getValue(0).getString(), "first");
    EXPECT_EQ(array.getValue(1).getNum(), 2);
    EXPECT_EQ(array.getValue(2).getNum(), 3.1);
    EXPECT_EQ(array.getValue(3).getBool(), true);
}

TEST(ARRAY, get_complex_name) {
    Json j("json_num", 15);
    JArray a;
    a.push_back(j);

    double d = a[{"0", "json_num"}].getNum();
    EXPECT_EQ(15, d);
}

TEST(ARRAY, get_index_error) {
    JArray array = array_example;

    try {
        array[5];
    } catch (std::out_of_range& e) {
        return SUCCEED();
    }

    FAIL();
}

TEST(ARRAY, insert) {
    JArray array = array_example;

    array.insert(1, "insert1");
    EXPECT_EQ(array[1].getString(), "insert1");

    array.insert(array.begin() + 3, "insert2");
    EXPECT_EQ(array[3].getString(), "insert2");
}

TEST(ARRAY, erase_it) {
    JArray array = array_example;

    array.erase(1);
    EXPECT_EQ(array[1].getNum(), 3.1);
 }

TEST(ARRAY, erase_index) {
    JArray array = array_example;

    array.erase(array.begin() + 1);
    EXPECT_EQ(array[1].getNum(), 3.1);
}

//========================================================================================
