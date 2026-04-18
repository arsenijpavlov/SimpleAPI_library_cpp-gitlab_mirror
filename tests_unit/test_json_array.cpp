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


using namespace simpleapi;

//проверка должна затрагивать все вариации значений (number, bool, string, array, json, null)
Config array_example(ValueType::eArray,
                     "first",
                     2,
                     3.1,
                     true,
                     Config(ValueType::eJson),
                     Config(ValueType::eArray),
                     Config()
                     );

TEST(ARRAY, copy) {
    Config a1(ValueType::eArray, "asd", 123);
    Config a2 = a1;

    ASSERT_EQ(a1.getType(), a2.getType());
    ASSERT_EQ(a1.size(), a2.size());
    EXPECT_EQ(a2[0].getString(), a1[0].getString());
    EXPECT_EQ(a2[1].getNumber(), a1[1].getNumber());
    EXPECT_EQ(a1.size(), a2.size());
}

TEST(ARRAY, append_array) {
    Config a1 = array_example;
    uint8_t pre_size = a1.size();

    Config a2;
    a2.push_back(15);
    a2.push_back(1.5);
    a2.push_back(true);

    a1.append(a2);

    EXPECT_EQ(pre_size + a2.size(), a1.size());
}

TEST(ARRAY, parse) {
    std::string string_array = "[15, true, \"string\"]";
    Config json;

    // парсинг разрешён только в составе других контейнеров, например Json
    json.parseJson("array:" + string_array);
    Config& array = json[0];

    ASSERT_TRUE(array.isArray());
    EXPECT_EQ(3, array.size());
    EXPECT_EQ(15, array[0].getNumber());
    EXPECT_EQ(true, array[1].getBool());
    EXPECT_EQ("string", array[2].getString());
}

TEST(ARRAY, parse_error) {
    //нет запятой между элементами
    std::string string_array = "[15 true, \"string\"]";

    // парсинг разрешён только в составе других контейнеров, например Json
    Config json;
    try {
        json.parseJson("array:" + string_array);
    } catch (...) {
        FAIL(); //метод не должен генерировать исключений
    }
    EXPECT_FALSE(json["array"].size() == 3); //будет прочитано только значение 15
    EXPECT_EQ(json["array"][0], 15);
}

TEST(ARRAY, get_index) {
    Config array = array_example;

    EXPECT_EQ(array[0].getString(), "first");
    EXPECT_EQ(array[1].getNumber(), 2);
    EXPECT_EQ(array[2].getNumber(), 3.1);
    EXPECT_EQ(array[3].getBool(), true);
}

TEST(ARRAY, get_complex_name) {
    //да, способ инициализации мерзкий, проще: Config cfg; cfg.push_back("json_num", 15);
    Config j(std::vector<std::pair<std::string, Config>>{std::make_pair("json_num", Config(15))});
    Config a;
    a.push_back(j);

    double d = a[{0, "json_num"}].getNumber();
    EXPECT_EQ(15, d);
}

TEST(ARRAY, get_index_error) {
    Config array = array_example;

    try {
        array[7];
    } catch (std::out_of_range& e) {
        return SUCCEED();
    }

    FAIL();
}

TEST(ARRAY, insert) {
    Config array = array_example;

    array.insert_at(1, "insert1");
    EXPECT_EQ(array[1].getString(), "insert1");

    array.insert_at(15, "insert999");
    EXPECT_EQ(array.get_back().getString(), "insert999");
}

TEST(ARRAY, insert_iterator) {
    Config array = array_example;
    Config::Range range = array.getRange();

    array.insert_at(range.begin() + 1, "insert1");
    EXPECT_EQ(array[1].getString(), "insert1");

    array.insert_at(range.begin() + 15, "insert999");
    EXPECT_EQ(array.get_back().getString(), "insert999");
}

TEST(ARRAY, erase_it) {
    Config array = array_example;

    array.erase_at(1);
    EXPECT_EQ(array[1].getNumber(), 3.1);
}

TEST(ARRAY, erase_iterator) {
    Config array = array_example;
    Config::Range range = array.getRange();

    array.erase_at(range.begin() + 1);
    EXPECT_EQ(array[1].getNumber(), 3.1);
}

TEST(ARRAY, contains) {
    Config array = array_example;
    Config inner = array[0];

    EXPECT_TRUE(array.containsValue(inner));
}
