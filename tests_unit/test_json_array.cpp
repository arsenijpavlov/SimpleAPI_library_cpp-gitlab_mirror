#include <SimpleAPI.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);

    return RUN_ALL_TESTS();
}

//========================================================================================
//TODO: проверка должна затрагивать все вариации значений (number, bool, string, array, json, null)
ElementArray array_example("first", 2, 3.1, true);


TEST(ARRAY, copy) {
    ElementArray a1("asd", 123);
    ElementArray a2 = a1;

    EXPECT_EQ(a2[0].getString(), a1[0].getString());
    EXPECT_EQ(a2[1].getNumber(), a1[1].getNumber());
    EXPECT_EQ(a1.size(), a2.size());
}

TEST(ARRAY, append_array) {
    ElementArray a1 = array_example;
    uint8_t pre_size = a1.size();

    ElementArray a2;
    a2.push_back(15);
    a2.push_back(1.5);
    a2.push_back(true);

    a1.appendArray(a2);

    EXPECT_EQ(pre_size + a2.size(), a1.size());
}

TEST(ARRAY, parse) {
    std::string string_array = "[15, true, \"string\"]";
    ElementArray array; //ElementArray(std::string) нельзя, т.к. положит это в значения, а не распарсит
    array.parseArray(string_array);

    EXPECT_EQ(3, array.size());
    EXPECT_EQ(15, array[0].getNumber());
    EXPECT_EQ(true, array[1].getBool());
    EXPECT_EQ("string", array[2].getString());
}

TEST(ARRAY, parse_error) {
    //нет запятой между элементами
    std::string string_array = "[15 true, \"string\"]";
    ElementArray array;
    try {
        array.parseArray(string_array);
    } catch (...) {}
    EXPECT_EQ(0, array.size());
}

TEST(ARRAY, get_index) {
    ElementArray array = array_example;

    EXPECT_EQ(array.get_value(0).getString(), "first");
    EXPECT_EQ(array.get_value(1).getNumber(), 2);
    EXPECT_EQ(array.get_value(2).getNumber(), 3.1);
    EXPECT_EQ(array.get_value(3).getBool(), true);
}

//TEST(ARRAY, get_complex_name) {
//    Json j("json_num", 15);
//    ElementArray a;
//    a.push_back(j);

//    double d = a[{"0", "json_num"}].getNum();
//    EXPECT_EQ(15, d);
//}

TEST(ARRAY, get_index_error) {
    ElementArray array = array_example;

    try {
        array[5];
    } catch (std::out_of_range& e) {
        return SUCCEED();
    }

    FAIL();
}

TEST(ARRAY, insert) {
    ElementArray array = array_example;

    array.insert(1, "insert1");
    EXPECT_EQ(array[1].getString(), "insert1");

    array.insert(array.begin() + 3, "insert2");
    EXPECT_EQ(array[3].getString(), "insert2");
}

//TEST(ARRAY, erase_it) {
//    ElementArray array = array_example;

//    array.erase(1);
//    EXPECT_EQ(array[1].getNum(), 3.1);
//}

//TEST(ARRAY, erase_index) {
//    ElementArray array = array_example;

//    array.erase(array.begin() + 1);
//    EXPECT_EQ(array[1].getNum(), 3.1);
//}
