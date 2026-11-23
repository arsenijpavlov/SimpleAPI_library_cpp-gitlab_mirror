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
    ASSERT_FALSE(json["array"].size() == 3); //будет прочитано только значение 15
    EXPECT_TRUE(json.error());
    ASSERT_TRUE(json.containsKey("array"));
    ASSERT_TRUE(json["array"].isArray());
    ASSERT_EQ(json["array"].size(), 1);
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

TEST(ARRAY, parse_json_simple_array) {
    Config cfg = ParseJson("[a, b, c]");
    EXPECT_EQ(cfg, Config(ValueType::eArray, "a", "b", "c"));
}

TEST(ARRAY, parse_json_array_with_empty_element) {
    Config cfg = ParseJson("[a,, b, c]");
    ASSERT_EQ(cfg.size(), 3);
    Config cfg2 = Config(ValueType::eArray, "a", "b", "c");
    EXPECT_EQ(cfg, cfg2);
}

TEST(ARRAY, parse_correct) {
    CommentDesign cd;
    Config cfg = ParseJson("[[\"a\naaa\", b],{c=[d,{e=f}]},g]", cd);
    EXPECT_FALSE(cfg.error());
}

TEST(ARRAY, parse_empty_array) {
    Config cfg;

    cfg.parseJson("[]");
    EXPECT_TRUE(cfg.isArray());

    cfg.parseJson("[   ]");
    EXPECT_TRUE(cfg.isArray());

    cfg.parseJson("[ \n ]");
    EXPECT_TRUE(cfg.isArray());
}

TEST(ARRAY, null_between_value) {
    using namespace simpleapi;

    Config cfg = ParseJson("[a,,b]");
    EXPECT_EQ(cfg.size(), 2);
    EXPECT_TRUE(cfg.containsValue("a"));
    EXPECT_TRUE(cfg.containsValue("b"));

    // вариант с переносами строк
    cfg = ParseJson("[a\n"
                    "\n"
                    "b\n]");
    EXPECT_EQ(cfg.size(), 2);
    EXPECT_TRUE(cfg.containsValue("a"));
    EXPECT_TRUE(cfg.containsValue("b"));
}

// префиксом считается всё до конца считывания значения
//  - многострочное представление: в суффиксный комментарий попадёт всё, что было от конца значения до переноса строки
TEST(ARRAY, parser_of_comments_multiline_view_1) {
    using namespace simpleapi;

    //-----------------------------------------------
    // суффиксный комментарий умещается в одну строку
    //-----------------------------------------------
    std::string input = "[\n"
                        "/*prefix_1*//*prefix_2*/\n"
                        "/*prefix_3*//*prefix_4*/value/*suffix_1*//*suffix_2*/,/*suffix_3*/\n"
                        "//prefix ignore from value \"value2\"\n"
                        "value2"
                        "]\n";
    CommentDesign cd;
    cd.with_comments = true;

    Config cfg = ParseJson(input, cd);
    EXPECT_EQ(cfg.get_prefix_comment(0), "prefix_1\n"
                                         "prefix_2\n"
                                         "prefix_3\n"
                                         "prefix_4");
    EXPECT_EQ(cfg.get_suffix_comment(0), "suffix_1\n"
                                         "suffix_2\n"
                                         "suffix_3");
}

// префиксом считается всё до конца считывания значения
//  - многострочное представление: в суффиксный комментарий попадёт всё, что было от конца значения до переноса строки
TEST(ARRAY, parser_of_comments_multiline_view_2) {
    using namespace simpleapi;

    //---------------------------------------------
    // суффиксный комментарий многострочный внутри
    //---------------------------------------------
    std::string input = "[\n"
                        "/*prefix_1*//*prefix_2*/\n"
                        "/*prefix_3*//*prefix_4*/value/*suffix_1*//*suffix_2*/,/*suf\n"
                        "fix_3*/\n"
                        "//prefix ignore from value \"value2\"\n"
                        "value2"
                        "]\n";
    CommentDesign cd;
    cd.with_comments = true;

    Config cfg = ParseJson(input, cd);
    EXPECT_EQ(cfg.get_prefix_comment(0), "prefix_1\n"
                                         "prefix_2\n"
                                         "prefix_3\n"
                                         "prefix_4");
    EXPECT_EQ(cfg.get_suffix_comment(0), "suffix_1\n"
                                         "suffix_2\n"
                                         "suf\n"
                                         "fix_3");
}

// префиксом считается всё до конца считывания значения
//  - однострочное представление: так же, как в многострочном, но если был суффиксный комментарий до запятой, то
//  следующий комментарий (после запятой) будет воспринят как префикс следующего поля
TEST(ARRAY, parser_of_comments_oneline_view_1) {
    using namespace simpleapi;

    //-------------------------------------------------------------------------------------------------
    // NOTE: переноса строки в исходном JSON-ARRAY нет (перенос только в коде для визуального разделения)!
    //-------------------------------------------------------------------------------------------------

    // случай с описанием "суффиксного" комментария до и после запятой
    std::string input = "["
                        "/*prefix_1*//*prefix_2*/value/*suffix_1*//*suffix_2*/,/*prefix_ignore*/value2"
                        "]";
    CommentDesign cd;
    cd.with_comments = true;

    Config cfg = ParseJson(input, cd);
    EXPECT_EQ(cfg.get_prefix_comment(0), "prefix_1\n"
                                         "prefix_2");
    EXPECT_EQ(cfg.get_suffix_comment(0), "suffix_1\n"
                                         "suffix_2");
}

// префиксом считается всё до конца считывания значения
//  - однострочное представление: так же, как в многострочном, но если был суффиксный комментарий до запятой, то
//  следующий комментарий (после запятой) будет воспринят как префикс следующего поля
TEST(ARRAY, parser_of_comments_oneline_view_2) {
    using namespace simpleapi;

    //-------------------------------------------------------------------------------------------------
    // NOTE: переноса строки в исходном JSON-ARRAY нет (перенос только в коде для визуального разделения)!
    //-------------------------------------------------------------------------------------------------

    // случай с описанием суффиксного комментария только после запятой
    std::string input = "["
                        "/*prefix_1*//*prefix_2*/value,/*suffix*//*prefix_ignore_1*//*prefix_ignore_2*/value2"
                        "]";
    CommentDesign cd;
    cd.with_comments = true;

    Config cfg = ParseJson(input, cd);
    EXPECT_EQ(cfg.get_prefix_comment(0), "prefix_1\n"
                                         "prefix_2");
    EXPECT_EQ(cfg.get_suffix_comment(0), "suffix");
}

TEST(ARRAY, another_comments_test) {
    using namespace simpleapi;

    std::string input =
        "[\n"
        "	/*######################\n"
        "	# bool some            #\n"
        "	# words...             #\n"
        "	######################*/\n"
        "	true, /*###########\n"
        "	              # bool some #\n"
        "	              # words...  #\n"
        "	              ###########*/\n"
        "	null\n"
        "]\n";
    CommentDesign cd;
    cd.with_comments = true;
    Config cfg = ParseJson(input, cd);

    EXPECT_EQ(cfg.get_prefix_comment(0), cfg.get_suffix_comment(0));
}

// несколько префиксов
// несколько суффиксов на одной строке для последнего элемента
TEST(ARRAY, parse_element_with_many_comments_1) {
    CommentDesign cd;
    cd.with_comments = true;

    std::string test_file_string = "[/*b1*/\n"
                                   "/*b2*/\n"
                                   "k/*a1*//*a2*//*a3\n"
                                   "a3*//*a4*//*a5*///a6\n"
                                   "]";
    Config json;
    json.parseJson(test_file_string, cd);

    EXPECT_EQ(json.size(), 1);
    EXPECT_TRUE(json.getComment().prefix().empty());
    EXPECT_FALSE(json[0].getComment().prefix().empty());
    EXPECT_EQ(json[0].getComment().prefix(), "b1\n"
                                             "b2");
    EXPECT_FALSE(json[0].getComment().suffix().empty());
    EXPECT_EQ(json[0].getComment().suffix(), "a1\n"
                                             "a2\n"
                                             "a3a3\n"
                                             "a4\n"
                                             "a5\n"
                                             "a6");
}

// несколько префиксов
// несколько суффиксов на одной строке для последнего элемента
// (случай с лишним замыкающим комментарием на другой строке)
TEST(ARRAY, parse_element_with_many_comments_2) {
    CommentDesign cd;
    cd.with_comments = true;

    std::string test_file_string = "[/*b1*/\n"
                                   "/*b2*/\n"
                                   "k/*a1*//*a2*//*a3\n"
                                   "a3*//*a4*//*a5*///a6\n"
                                   "//ignore comment\n"
                                   "]";
    Config json;
    json.parseJson(test_file_string, cd);

    EXPECT_EQ(json.size(), 1);
    EXPECT_TRUE(json.getComment().prefix().empty());
    EXPECT_FALSE(json[0].getComment().prefix().empty());
    EXPECT_EQ(json[0].getComment().prefix(), "b1\n"
                                             "b2");
    EXPECT_FALSE(json[0].getComment().suffix().empty());
    EXPECT_EQ(json[0].getComment().suffix(), "a1\n"
                                             "a2\n"
                                             "a3a3\n"
                                             "a4\n"
                                             "a5\n"
                                             "a6");
}
