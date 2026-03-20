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

TEST(ELEMENT, create_empty) {
    Config el_null;
    EXPECT_EQ(el_null.getType(), ValueType::eNull);
    EXPECT_TRUE(el_null.isNull());

    Config el_null_2(ValueType::eNull);
    EXPECT_EQ(el_null_2.getType(), ValueType::eNull);
    EXPECT_TRUE(el_null_2.isNull());
}

TEST(ELEMENT, create_bool) {
    Config el_bool_1(ValueType::eBool);
    EXPECT_EQ(el_bool_1.getType(), ValueType::eBool);
    EXPECT_TRUE(el_bool_1.isBool());
    EXPECT_EQ(el_bool_1.getBool(), false);

    Config el_bool_2(false);
    EXPECT_EQ(el_bool_2.getType(), ValueType::eBool);
    EXPECT_TRUE(el_bool_2.isBool());
    EXPECT_EQ(el_bool_2.getBool(), false);

    Config el_bool_3(true);
    EXPECT_EQ(el_bool_3.getType(), ValueType::eBool);
    EXPECT_TRUE(el_bool_3.isBool());
    EXPECT_EQ(el_bool_3.getBool(), true);
}

TEST(ELEMENT, create_number) {
    Config el_int(1);
    EXPECT_EQ(el_int.getType(), ValueType::eNumber);
    EXPECT_TRUE(el_int.isNumber());

    Config el_uint(11);
    EXPECT_TRUE(el_uint.isNumber());
    EXPECT_EQ(el_uint.getType(), ValueType::eNumber);

    Config el_double(1.1);
    EXPECT_TRUE(el_double.isNumber());
    EXPECT_EQ(el_double.getType(), ValueType::eNumber);

    Config el_float(11.f);
    EXPECT_TRUE(el_float.isNumber());
    EXPECT_EQ(el_float.getType(), ValueType::eNumber);
}

TEST(ELEMENT, create_string) {
    Config el_string(std::string("asd"));
    EXPECT_EQ(el_string.getType(), ValueType::eString);
    EXPECT_TRUE(el_string.isString());

    Config el_string_2(ValueType::eString);
    EXPECT_EQ(el_string_2.getType(), ValueType::eString);
    EXPECT_TRUE(el_string_2.isString());

    Config el_string_3("asd");
    EXPECT_EQ(el_string_3.getType(), ValueType::eString);
    EXPECT_TRUE(el_string_3.isString());

    const char* chr = "asd";
    Config el_string_4(chr);
    EXPECT_EQ(el_string_4.getType(), ValueType::eString);
    EXPECT_TRUE(el_string_4.isString());
}

TEST(ELEMENT, create_array) {
    Config ar(ValueType::eArray);
    EXPECT_EQ(ar.getType(), ValueType::eArray);

    Config el_array(ValueType::eArray, ar);
    ASSERT_TRUE(el_array.isArray());
    ASSERT_FALSE(el_array.isEmpty());

    EXPECT_EQ(ValueType::eArray, el_array.get_front().getType());
}

TEST(ELEMENT, create_json) {
    Config js(ValueType::eJson);
    EXPECT_EQ(js.getType(), ValueType::eJson);

    Config el_json(ValueType::eJson, "json", js);
    ASSERT_TRUE(el_json.isJson());
    ASSERT_FALSE(el_json.isEmpty());

    ASSERT_FALSE(el_json.isEmpty());
    EXPECT_EQ(ValueType::eJson, el_json.get_front().getType());
}

TEST(ELEMENT, compare_all_types) {
    Config el_num(15.0);
    ASSERT_EQ(el_num.getType(), ValueType::eNumber);
    ASSERT_TRUE(el_num.isNumber());
    EXPECT_EQ(el_num, 15.0);

    Config el_bool(true);
    ASSERT_EQ(el_bool.getType(), ValueType::eBool);
    ASSERT_TRUE(el_bool.isBool());
    EXPECT_EQ(el_bool, true);

    Config el_string("true");
    ASSERT_EQ(el_string.getType(), ValueType::eString);
    ASSERT_TRUE(el_string.isString());
    EXPECT_EQ(el_string, "true");

    Config el_array(ValueType::eArray);
    ASSERT_EQ(el_array.getType(), ValueType::eArray);
    ASSERT_TRUE(el_array.isArray());
    EXPECT_EQ(el_array, Config(ValueType::eArray));

    Config el_json(ValueType::eJson);
    ASSERT_EQ(el_json.getType(), ValueType::eJson);
    ASSERT_TRUE(el_json.isJson());
    EXPECT_EQ(el_json, Config(ValueType::eJson));
}

TEST(ELEMENT, not_compare_all_types) {
    Config el_num(15.0);
    ASSERT_EQ(el_num.getType(), ValueType::eNumber);
    ASSERT_TRUE(el_num.isNumber());
    EXPECT_EQ(el_num, 15.0);

    Config el_bool(true);
    ASSERT_EQ(el_bool.getType(), ValueType::eBool);
    ASSERT_TRUE(el_bool.isBool());
    EXPECT_EQ(el_bool, true);

    Config el_string("true");
    ASSERT_EQ(el_string.getType(), ValueType::eString);
    ASSERT_TRUE(el_string.isString());
    EXPECT_EQ(el_string, "true");
    EXPECT_EQ(el_string, std::string("true"));

    Config el_json(ValueType::eJson);
    ASSERT_EQ(el_json.getType(), ValueType::eJson);
    ASSERT_TRUE(el_json.isJson());
    EXPECT_EQ(el_json, Config(ValueType::eJson));

    Config el_array(ValueType::eArray);
    ASSERT_EQ(el_array.getType(), ValueType::eArray);
    ASSERT_TRUE(el_array.isArray());
    EXPECT_EQ(el_array, Config(ValueType::eArray));

    EXPECT_NE(el_num,       1.0);
    EXPECT_NE(el_bool,      false);
    EXPECT_NE(el_string,    "asd");
    EXPECT_NE(el_json,      Config(ValueType::eJson, "asd", "asd"));            // вариант без обёртки
    EXPECT_NE(el_json,      Config(ValueType::eJson, "asd", Config("asd")));    // вариант с обёрткой
    EXPECT_NE(el_array,     Config(ValueType::eArray, "asd"));          // вариант без обёртки
    EXPECT_NE(el_array,     Config(ValueType::eArray, Config("asd")));  // вариант с обёрткой
}

TEST(ELEMENT, create_null_from_variadic) {
    Config config(ValueType::eNull);
    EXPECT_TRUE(config.isNull());

    EXPECT_ANY_THROW({ Config config2(ValueType::eNull, 123); });
}

TEST(ELEMENT, create_bool_from_variadic) {
    Config config(ValueType::eBool);
    EXPECT_TRUE(config.isBool());

    Config config2(ValueType::eBool, true);
    EXPECT_TRUE(config2.isBool());
    EXPECT_EQ(config2.getBool(), true);

    EXPECT_ANY_THROW({ Config config3(ValueType::eBool, true, false); });
    EXPECT_ANY_THROW({ Config config3(ValueType::eBool, "true"); });
}

TEST(ELEMENT, create_number_from_variadic) {
    Config config(ValueType::eNumber);
    EXPECT_TRUE(config.isNumber());

    Config config2(ValueType::eNumber, 14);
    EXPECT_TRUE(config2.isNumber());
    EXPECT_EQ(config2.getNumber(), 14);

    EXPECT_ANY_THROW({ Config config3(ValueType::eNumber, 14, 15); });
    EXPECT_ANY_THROW({ Config config3(ValueType::eNumber, true); });
}

TEST(ELEMENT, create_string_from_variadic) {
    Config config(ValueType::eString);
    EXPECT_TRUE(config.isString());

    Config config2(ValueType::eString, "true");
    EXPECT_TRUE(config2.isString());
    EXPECT_EQ(config2.getString(), "true");

    EXPECT_ANY_THROW({ Config config3(ValueType::eString, "true", "false"); });
    EXPECT_ANY_THROW({ Config config3(ValueType::eString, true); });
}
