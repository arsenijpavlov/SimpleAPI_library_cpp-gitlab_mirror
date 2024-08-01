#include <SimpleAPI.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(ELEMENT, create_num) {
    Element el_int(1);
    Element el_double(1.1);
    Element el_uint(11);
    Element el_float(11.f);

    EXPECT_EQ(eNumber, el_int.first);
    EXPECT_EQ(eNumber, el_double.first);
    EXPECT_EQ(eNumber, el_uint.first);
    EXPECT_EQ(eNumber, el_float.first);

    delete el_int.second;
    delete el_double.second;
    delete el_uint.second;
    delete el_float.second;
}

TEST(ELEMENT, create_srting) {
    Element el_string(std::string("asd"));
    Element el_char_arr("asd");
    const char* chr = "asd";
    Element el_char_star(chr);

    EXPECT_EQ(eString, el_string.first);
    EXPECT_EQ(eString, el_char_arr.first);
    EXPECT_EQ(eString, el_char_star.first);

    delete el_string.second;
    delete el_char_arr.second;
    delete el_char_star.second;
}

TEST(ELEMENT, create_bool) {
    Element el_bool(true);

    EXPECT_EQ(eBool, el_bool.first);

    delete el_bool.second;
}

TEST(ELEMENT, create_json) {
    Json js;
    Element el_json(js);

    EXPECT_EQ(eJson, el_json.first);

    delete el_json.second;
}

TEST(ELEMENT, create_array) {
    Array ar;
    Element el_array(ar);

    EXPECT_EQ(eArray, el_array.first);

    delete el_array.second;
}

TEST(ELEMENT, compare) {
    Element el_num(15.0);
    Element el_bool(true);
    Element el_string("true");
    Element el_json(Json{});
    Element el_array(Array{});

    EXPECT_EQ(el_num.first,     eNumber);
    EXPECT_EQ(el_bool.first,    eBool);
    EXPECT_EQ(el_string.first,  eString);
    EXPECT_EQ(el_json.first,    eJson);
    EXPECT_EQ(el_array.first,   eArray);

    EXPECT_EQ(el_num,   Element(15.0));
    EXPECT_EQ(el_bool,  Element(true));
    EXPECT_EQ(el_string,Element("true"));
    EXPECT_EQ(el_json,  Element(Json{}));
    EXPECT_EQ(el_array, Element(Array{}));

    delete el_num.second;
    delete el_bool.second;
    delete el_string.second;
    delete el_json.second;
    delete el_array.second;
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

    Array array;
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
