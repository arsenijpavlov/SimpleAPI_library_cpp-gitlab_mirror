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
    Element el_int(1);
    Element el_double(1.1);
    Element el_uint(11);
    Element el_float(11.f);

    EXPECT_EQ(eNumber,  el_int.first);
    EXPECT_EQ(eNumber,  el_double.first);
    EXPECT_EQ(eNumber,  el_uint.first);
    EXPECT_EQ(eNumber,  el_float.first);
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

TEST(ELEMENT, create_null) {
    Element el_null;

    EXPECT_EQ(eNull,    el_null.first);
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
