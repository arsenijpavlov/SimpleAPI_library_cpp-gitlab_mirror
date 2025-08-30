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

TEST(ELEMENT, create_num) {
    Config el_int(1);
    EXPECT_EQ(ValueType::eNumber,  el_int.getType());

    Config el_double(1.1);
    EXPECT_EQ(ValueType::eNumber,  el_double.getType());

    Config el_uint(11);
    EXPECT_EQ(ValueType::eNumber,  el_uint.getType());

    Config el_float(11.f);
    EXPECT_EQ(ValueType::eNumber,  el_float.getType());
}

TEST(ELEMENT, create_string) {
    Config el_string(std::string("asd"));
    EXPECT_EQ(ValueType::eString, el_string.getType());

    Config el_char_arr("asd");
    EXPECT_EQ(ValueType::eString, el_char_arr.getType());

    const char* chr = "asd";
    Config el_char_star(chr);
    EXPECT_EQ(ValueType::eString, el_char_star.getType());
}

TEST(ELEMENT, create_bool) {
    Config el_bool(true);
    EXPECT_EQ(ValueType::eBool, el_bool.getType());
}

TEST(ELEMENT, create_null) {
    Config el_null;
    EXPECT_EQ(ValueType::eNull,    el_null.getType());
}

TEST(ELEMENT, create_json) {
    Config js(ValueType::eJson);
    EXPECT_EQ(js.getType(), ValueType::eJson);

    Config el_json(ValueType::eJson, "json", js);
    ASSERT_EQ(el_json.getType(), ValueType::eJson);

    ASSERT_FALSE(el_json.isEmpty());
    EXPECT_EQ(ValueType::eJson, el_json.get_front().getType());
}

TEST(ELEMENT, create_array) {
    Config ar(ValueType::eArray);
    EXPECT_EQ(ar.getType(), ValueType::eArray);

    Config el_array(ar);
    ASSERT_EQ(el_array.getType(), ValueType::eArray);

    EXPECT_EQ(ValueType::eArray, el_array.get_front().getType());
}

TEST(ELEMENT, compare_all_types) {
    Config el_num(15.0);
    EXPECT_EQ(el_num.getType(),     ValueType::eNumber);
    EXPECT_EQ(el_num,               15.0);

    Config el_bool(true);
    EXPECT_EQ(el_bool.getType(),    ValueType::eBool);
    EXPECT_EQ(el_bool,              true);

    Config el_string("true");
    EXPECT_EQ(el_string.getType(),  ValueType::eString);
    EXPECT_EQ(el_string,            "true");

    Config el_json(ValueType::eJson);
    EXPECT_EQ(el_json.getType(),    ValueType::eJson);
    EXPECT_EQ(el_json,              Config(ValueType::eJson));

    Config el_array(ValueType::eArray);
    EXPECT_EQ(el_array.getType(),   ValueType::eArray);
    EXPECT_EQ(el_array,             Config(ValueType::eArray));
}

//TEST(ELEMENT, not_compare_all_types) {
//    Element el_num(15.0);
//    Element el_bool(true);
//    Element el_string("true");
//    Element el_json(Json{});
//    Element el_array(ElementArray{});

//    EXPECT_EQ(el_num.first,     ValueType::eNumber);
//    EXPECT_EQ(el_bool.first,    ValueType::eBool);
//    EXPECT_EQ(el_string.first,  ValueType::eString);
//    EXPECT_EQ(el_json.first,    ValueType::eJson);
//    EXPECT_EQ(el_array.first,   ValueType::eArray);

//    EXPECT_NE(el_num,   Element(1.0));
//    EXPECT_NE(el_bool,  Element(false));
//    EXPECT_NE(el_string,Element("asd"));
//    EXPECT_NE(el_json,  Element(Json().put("asd", "asd")));
//    EXPECT_NE(el_array, Element(ElementArray().push_front("asd")));
//}

//TODO: TEST(ELEMENT, read_file) {
//    Element el;
//}
