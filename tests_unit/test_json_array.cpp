#include "config/ElementArray.h"
//#include <SimpleAPI.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);

    return RUN_ALL_TESTS();
}

//========================================================================================
ElementArray array_example("first", 2, 3.1, true);


//TEST(ARRAY, copy) {
//    JArray a1("asd", 123);
//    JArray a2 = a1;

//    EXPECT_EQ(a1.size(), a2.size());
//}

//TEST(ARRAY, append_array) {
//    JArray a1 = array_example;
//    uint8_t pre_size = a1.size();

//    JArray a2;
//    a2.push_back(15);
//    a2.push_back(1.5);
//    a2.push_back(true);

//    a1.append(a2);

//    EXPECT_EQ(pre_size + a2.size(), a1.size());
//}

//TEST(ARRAY, parse) {
//    std::string string_array = "[15, true, \"string\"]";
//    JArray array; //JArray(std::string) нельзя, т.к. положит это в значения, а не распарсит
//    array.parseArray(string_array);

//    EXPECT_EQ(3, array.size());
//    EXPECT_EQ(15, array[0].getNum());
//    EXPECT_EQ(true, array[1].getBool());
//    EXPECT_EQ("string", array[2].getString());
//}

TEST(ARRAY, NEW_parse) {
    std::string string_array = "[15, true, \"string\"]";
    ElementArray array; //JArray(std::string) нельзя, т.к. положит это в значения, а не распарсит
    array.parseArray(string_array);

    EXPECT_EQ(3, array.size());
//    EXPECT_EQ(15, array[0].getNum());
//    EXPECT_EQ(true, array[1].getBool());
//    EXPECT_EQ("string", array[2].getString());
}

//TEST(ARRAY, parse_error) {
//    //нет запятой между элементами
//    std::string string_array = "[15 true, \"string\"]";
//    JArray array;
//    try {
//        array.parseArray(string_array);
//    } catch (...) {}
//    EXPECT_EQ(0, array.size());
//}

//TEST(ARRAY, get_index) {
//    JArray array = array_example;

//    EXPECT_EQ(array.getValue(0).getString(), "first");
//    EXPECT_EQ(array.getValue(1).getNum(), 2);
//    EXPECT_EQ(array.getValue(2).getNum(), 3.1);
//    EXPECT_EQ(array.getValue(3).getBool(), true);
//}

//TEST(ARRAY, get_complex_name) {
//    Json j("json_num", 15);
//    JArray a;
//    a.push_back(j);

//    double d = a[{"0", "json_num"}].getNum();
//    EXPECT_EQ(15, d);
//}

//TEST(ARRAY, get_index_error) {
//    JArray array = array_example;

//    try {
//        array[5];
//    } catch (std::out_of_range& e) {
//        return SUCCEED();
//    }

//    FAIL();
//}

//TEST(ARRAY, insert) {
//    JArray array = array_example;

//    array.insert(1, "insert1");
//    EXPECT_EQ(array[1].getString(), "insert1");

//    array.insert(array.begin() + 3, "insert2");
//    EXPECT_EQ(array[3].getString(), "insert2");
//}

//TEST(ARRAY, erase_it) {
//    JArray array = array_example;

//    array.erase(1);
//    EXPECT_EQ(array[1].getNum(), 3.1);
//}

//TEST(ARRAY, erase_index) {
//    JArray array = array_example;

//    array.erase(array.begin() + 1);
//    EXPECT_EQ(array[1].getNum(), 3.1);
//}
