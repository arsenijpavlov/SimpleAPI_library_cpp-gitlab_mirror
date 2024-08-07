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
                                  "//\n"
                                  "#\n"
                                  ";\n"
                                  "!\n"
                                  //многострочные комментарии
                                  "/*\n*/"
                                  "<-\n->"
                                  "<#\n#>"
                                  "/#\n#/"
                                  "\"number\":182,\n"
                                  "\"bool\":true,\n"
                                  "\"string\":\"string_value\",\n"
                                  "\"json\":{\"string\":\"inner_string_value\"},\n"
                                  "\"array\":[\"string_value\", true]\n"
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
    json.parseJson(string_json);

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
    json.parseJson(string_json);
    EXPECT_EQ(5, json.size());
}

TEST(JSON, parse_error) {
    //строка не обрамлена кавычками
    std::string string_json = "{\"key\":string_value}";
    Json json(string_json);
    EXPECT_EQ(0, json.size());

    //ключ не обрамлён кавычками
    string_json = "{key:\"string_value\"}";
    json.parseJson(string_json);
    EXPECT_EQ(0, json.size());

    //некорректное значение числа
    string_json = "{key:15.4.3}";
    json.parseJson(string_json);
    EXPECT_EQ(0, json.size());

    //некорректное значение числа
    string_json = "{key:15e43}"; //TODO: добавить поддержку экспоненты в значении числа
    json.parseJson(string_json);
    EXPECT_EQ(0, json.size());

    //пробелы в значении bool
    string_json = "{key:tru e}";
    json.parseJson(string_json);
    EXPECT_EQ(0, json.size());
}

TEST(JSON, write_file) {
    Json json = json_example;

    json.writeFile("../tests/test_writer_with_spaces.json");
    json.writeFile("../tests/test_writer.json", -1);
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
    JArray array; //TODO: JArray(std::string)
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
    array.parseArray(string_array);
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
