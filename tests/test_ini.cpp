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
/*ПРИМЕР:
; простой комментарий
key = value

[group_1] ; строго БЕЗ пробелов!
key = value         ; даст итоговый объект { key : value }
key = key1 = value  ; даст итоговый объект { key : value, key1 : value }

[group_2]
key\inner_key\inner_inner_key = inner_inner_value ; вложенные значения и группы значений
=> даст итоговый объект
    { key : { inner_key : { inner_inner_key : inner_inner_value } } }

key\key1\key2 = a
key\key1\key2 = b
=> даст итоговый объект
    {
        key : {
            key1 : {
                key2 : [
                    a,
                    b
                ]
            }
        }
    }

key\key1\key2 = a
key\key1\key22 = b
=> даст итоговый объект
    {
        key : {
            key1 : {
                key2 : a,
                key22 : b
            }
        }
    }

; строка, число, отсутсвие значения (NULL), bool

key = simple string
key = many \
    line \
    string ; пробелы от начала строки до первого не пустого символа будут удалены при чтении

key[0] = value
key[2] = value
=> даст итоговый объект
    { key : [value, null, value]}

*/
//========================================================================================
const std::string ini_example_string = "; комментарий\n"
                                       "; вторая строка комментария\n"
                                       "key =  15\n"
                                       "array = [a, 15, true]\n"
                                       "json = {inner_array:[a, 4.0, \"asd string\"]}\n"
                                       "\n"
                                       "[group 1]; ещё комментарий\n"
                                       "g1_key : value\n"
                                       "g1_key2 = g1_key3 = 152\n"
                                       //пустая строка после описания группы завершает группу
                                       "\n"
                                       "key2 = value2\n"
                                       "[group 2]\n"
                                       "g2_string = one line string\n"
                                       "g2 string2 = big\\\n"
                                       "            line string\n"
                                       //комментарий применится только для конечного элемента
                                       "; коммент ДО переменной\n"
                                       "g2_key\\inner_key\\inner_inner_key = inner_inner_value ; вложенные значения и группы значений\n"
                                       "\n"
                                       "key2\\inner_key2 = a\n"
                                       "key2/inner_key2 = b\n" // "/" и "\" равнозначны при обработке
                                       "key3\\inner_key3 = a\n"
                                       "key3/inner_key33 = b\n"
                                       "\n"
                                       "[group_3]\n"
                                       "g3_key = 15\n"
                                       "g3_key = \n"
                                       "g3_key = true\n"
    ;

//NOTE: можно сделать более читабельным, но пока и так норм
TEST(INI, main_parser) {
    Json json;
    json.parseINI(ini_example_string, true);

    std::cout << json.to_string(0, true, 0, ConfigFormat::eJSON) << std::endl;

    ASSERT_EQ(json.size(), 9);

    ASSERT_EQ(json.contains("key"), true); {
        ASSERT_EQ(json["key"].first, eNumber);
        EXPECT_EQ(json["key"].getNum(), 15);
    }

    ASSERT_EQ(json.contains("array"), true); {
        ASSERT_EQ(json["array"].first, eArray);
        EXPECT_EQ(json["array"].getArray().size(), 3);
        EXPECT_EQ(json["array"].getArray()[0].first, eString);
        EXPECT_EQ(json["array"].getArray()[1].first, eNumber);
        EXPECT_EQ(json["array"].getArray()[2].first, eBool);
    }

    ASSERT_EQ(json.contains("json"), true); {
        ASSERT_EQ(json["json"].first, eJson);
        EXPECT_EQ(json["json"].getJson().size(), 1);
        ASSERT_EQ(json["json"].getJson().contains("inner_array"), true);
        ASSERT_EQ(json["json"].getJson()["inner_array"].first, eArray);
        EXPECT_EQ(json["json"].getJson()["inner_array"].getArray().size(), 3);
        EXPECT_EQ(json["json"].getJson()["inner_array"].getArray()[0].first, eString);
        EXPECT_EQ(json["json"].getJson()["inner_array"].getArray()[1].first, eNumber);
        EXPECT_EQ(json["json"].getJson()["inner_array"].getArray()[2].first, eString);
    }

    ASSERT_EQ(json.contains("key2"), true); {
        ASSERT_EQ(json["key2"].first, eArray);
        ASSERT_EQ(json["key2"].getArray().size(), 2);
        ASSERT_EQ(json["key2"].getArray()[0].first, eString);
        EXPECT_EQ(json["key2"].getArray()[0].getString(), "value2");
        ASSERT_EQ(json["key2"].getArray()[1].first, eJson);
        ASSERT_EQ(json["key2"].getArray()[1].getJson().size(), 1);
        ASSERT_EQ(json["key2"].getArray()[1].getJson().contains("inner_key2"), true);
        ASSERT_EQ(json["key2"].getArray()[1].getJson()["inner_key2"].first, eArray);
        EXPECT_EQ(json["key2"].getArray()[1].getJson()["inner_key2"].getArray().size(), 2);
        ASSERT_EQ(json["key2"].getArray()[1].getJson()["inner_key2"].getArray()[0].first, eString);
        EXPECT_EQ(json["key2"].getArray()[1].getJson()["inner_key2"].getArray()[0].getString(), "a");
        ASSERT_EQ(json["key2"].getArray()[1].getJson()["inner_key2"].getArray()[1].first, eString);
        EXPECT_EQ(json["key2"].getArray()[1].getJson()["inner_key2"].getArray()[1].getString(), "b");
    }

    ASSERT_EQ(json.contains("key3"), true); {
        ASSERT_EQ(json["key3"].first, eJson);
        ASSERT_EQ(json["key3"].getJson().size(), 2);
        ASSERT_EQ(json["key3"].getJson().contains("inner_key3"), true);
        ASSERT_EQ(json["key3"].getJson()["inner_key3"].first, eString);
        EXPECT_EQ(json["key3"].getJson()["inner_key3"].getString(), "a");
        ASSERT_EQ(json["key3"].getJson().contains("inner_key33"), true);
        ASSERT_EQ(json["key3"].getJson()["inner_key33"].first, eString);
        ASSERT_EQ(json["key3"].getJson()["inner_key33"].getString(), "b");
    }


    ASSERT_EQ(json.contains("group 1"), true); {
        ASSERT_EQ(json["group 1"].first, eJson);
        ASSERT_EQ(json["group 1"].getJson().size(), 3);
        ASSERT_EQ(json["group 1"].getJson().contains("g1_key"), true);
        ASSERT_EQ(json["group 1"].getJson()["g1_key"].first, eString);
        EXPECT_EQ(json["group 1"].getJson()["g1_key"].getString(), "value");
        ASSERT_EQ(json["group 1"].getJson().contains("g1_key2"), true);
        ASSERT_EQ(json["group 1"].getJson()["g1_key2"].first, eNumber);
        EXPECT_EQ(json["group 1"].getJson()["g1_key2"].getNum(), 152);
        ASSERT_EQ(json["group 1"].getJson().contains("g1_key3"), true);
        ASSERT_EQ(json["group 1"].getJson()["g1_key3"].first, eNumber);
        EXPECT_EQ(json["group 1"].getJson()["g1_key3"].getNum(), 152);
    }

    ASSERT_EQ(json.contains("group 2"), true); {
        ASSERT_EQ(json["group 2"].first, eJson);
        ASSERT_EQ(json["group 2"].getJson().size(), 2);
        ASSERT_EQ(json["group 2"].getJson().contains("g2_string"), true);
        ASSERT_EQ(json["group 2"].getJson()["g2_string"].first, eString);
        EXPECT_EQ(json["group 2"].getJson()["g2_string"].getString(), "one line string");
        ASSERT_EQ(json["group 2"].getJson().contains("g2 string2"), true);
        ASSERT_EQ(json["group 2"].getJson()["g2 string2"].first, eString);
        EXPECT_EQ(json["group 2"].getJson()["g2 string2"].getString(), "big\nline string");
    }

    ASSERT_EQ(json.contains("group_3"), true); {
        ASSERT_EQ(json["group_3"].first, eJson);
        ASSERT_EQ(json["group_3"].getJson().size(), 1);
        ASSERT_EQ(json["group_3"].getJson().contains("g3_key"), true);
        ASSERT_EQ(json["group_3"].getJson()["g3_key"].first, eArray);
        ASSERT_EQ(json["group_3"].getJson()["g3_key"].getArray().size(), 3);
        EXPECT_EQ(json["group_3"].getJson()["g3_key"].getArray()[0].first, eNumber);
        ASSERT_EQ(json["group_3"].getJson()["g3_key"].getArray()[0].getNum(), 15);
        ASSERT_EQ(json["group_3"].getJson()["g3_key"].getArray()[1].first, eNull);
        ASSERT_EQ(json["group_3"].getJson()["g3_key"].getArray()[2].first, eBool);
        EXPECT_EQ(json["group_3"].getJson()["g3_key"].getArray()[2].getBool(), true);
    }

}
