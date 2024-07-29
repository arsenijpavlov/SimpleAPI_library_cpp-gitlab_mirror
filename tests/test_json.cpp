#include <SimpleAPI.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(JSON, put_and_get_elements) {
    std::string test_str    = "abc";
    double test_num         = 15;
    bool test_bool          = true;
    //inner json
    //inner array

    Json json;
    json.put("string", test_str);
    json.put("number", test_num);
    json.put("bool", test_bool);

    EXPECT_EQ(test_str, json["string"].getString());
    EXPECT_EQ(test_num, json["number"].getNum());
    EXPECT_EQ(test_bool, json["bool"].getBool());

    Array array;
    array.push_back("str");
    array.push_back((double)15);

    Json json2;
    json2.put("json", json);
    json2.put("array", array);

//TODO:    EXPECT_EQ(json, json2["json"].getJson());
//TODO:    EXPECT_EQ(json["string"], json2["json"].getJson()["string"]);
    EXPECT_EQ(json["string"].getString(), json2["json"].getJson()["string"].getString());

//TODO:    EXPECT_EQ(array, json2["array"].getArray());
//TODO:    EXPECT_EQ(array[0], json2["array"].getArray()[0]);
    EXPECT_EQ(array[0].getString(), json2["array"].getArray()[0].getString());
}
