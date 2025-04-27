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
    IElement el_int(1);
    IElement el_double(1.1);
    IElement el_uint(11);
    IElement el_float(11.f);

    EXPECT_EQ(ValueType::eNumber,  el_int.getType());
    EXPECT_EQ(ValueType::eNumber,  el_double.getType());
    EXPECT_EQ(ValueType::eNumber,  el_uint.getType());
    EXPECT_EQ(ValueType::eNumber,  el_float.getType());
}

TEST(ELEMENT, create_srting) {
    IElement el_string(std::string("asd"));
    IElement el_char_arr("asd");
    const char* chr = "asd";
    IElement el_char_star(chr);

    EXPECT_EQ(ValueType::eString, el_string.getType());
    EXPECT_EQ(ValueType::eString, el_char_arr.getType());
    EXPECT_EQ(ValueType::eString, el_char_star.getType());
}

TEST(ELEMENT, create_bool) {
    IElement el_bool(true);

    EXPECT_EQ(ValueType::eBool, el_bool.getType());
}

TEST(ELEMENT, create_null) {
    IElement el_null;

    EXPECT_EQ(ValueType::eNull,    el_null.getType());
}

//TEST(ELEMENT, create_json) {
//    Json js;
//    Element el_json(js);

//    EXPECT_EQ(ValueType::eJson, el_json.first);
//}

TEST(ELEMENT, create_array) {
    ElementArray ar;
    IElement el_array(ar);

    EXPECT_EQ(ValueType::eArray, el_array.getType());
}

//TEST(ELEMENT, compare_all_types) {
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

//    EXPECT_EQ(el_num,   Element(15.0));
//    EXPECT_EQ(el_bool,  Element(true));
//    EXPECT_EQ(el_string,Element("true"));
//    EXPECT_EQ(el_json,  Element(Json{}));
//    EXPECT_EQ(el_array, Element(ElementArray{}));
//}

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
