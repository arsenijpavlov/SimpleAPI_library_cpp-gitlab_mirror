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
const std::string ini_example_string = ""
                                       "; комментарий\n"
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
                                       "; коммент ДО переменной inner_inner_key\n"
                                       "g2_key\\inner_key\\inner_inner_key = inner_inner_value ; вложенные значения и группы значений\n"
                                       "\n"
                                       "; многострочный коммент\n"
                                       "; ДО переменной inner_key2\n"
                                       "key2\\inner_key2 = a /*многострочный коммент\nпосле переменной inner_key2*/\n"
                                       "key2/inner_key2 = b\n" // "/" и "\" равнозначны при обработке
                                       "key3\\inner_key3 = a\n"
                                       "key3/inner_key33 = b\n"
                                       "\n"
                                       "[group_3]\n"
                                       "g3_key = 15\n"
                                       "g3_key = \n"
                                       "g3_key = true\n"
    ;

TEST(INI, main_parser) {
    Json json;
    json.parseINI(ini_example_string, true);
//    std::cout << json.to_string(0, true, 0, ConfigFormat::eJSON) << std::endl;
    std::cout << json.to_string(0, true, 0, ConfigFormat::eINI) << std::endl;

    EXPECT_EQ(json.size(), 8);

    ASSERT_EQ(json.contains("key"), true); {
        ASSERT_EQ(json["key"].first, eNumber);
        EXPECT_EQ(json["key"].getNum(), 15);
        EXPECT_EQ(json.getComment("key").before,
                  std::string("комментарий\nвторая строка комментария"));
    }

    ASSERT_EQ(json.contains("array"), true); {
        ASSERT_EQ(json["array"].first, eArray);
        JArray ja = json["array"].getArray();
        {
            EXPECT_EQ(ja.size(), 3);
            EXPECT_EQ(ja[0].first, eString);
            EXPECT_EQ(ja[1].first, eNumber);
            EXPECT_EQ(ja[2].first, eBool);
        }
    }

    ASSERT_EQ(json.contains("json"), true); {
        ASSERT_EQ(json["json"].first, eJson);
        Json j = json["json"].getJson();
        {
            EXPECT_EQ(j.size(), 1);
            ASSERT_EQ(j.contains("inner_array"), true);
            ASSERT_EQ(j["inner_array"].first, eArray);
            JArray ja = j["inner_array"].getArray();
            {
                EXPECT_EQ(ja.size(), 3);
                EXPECT_EQ(ja[0].first, eString);
                EXPECT_EQ(ja[1].first, eNumber);
                EXPECT_EQ(ja[2].first, eString);
            }
        }
    }

    ASSERT_EQ(json.contains("key2"), true); {
        ASSERT_EQ(json["key2"].first, eArray);
        JArray ja = json["key2"].getArray();
        {
            ASSERT_EQ(ja.size(), 2);
            ASSERT_EQ(ja[0].first, eString);
            EXPECT_EQ(ja[0].getString(), "value2");
            ASSERT_EQ(ja[1].first, eJson);
            Json j = ja[1].getJson();
            {
                ASSERT_EQ(j.size(), 1);
                ASSERT_EQ(j.contains("inner_key2"), true);
                ASSERT_EQ(j["inner_key2"].first, eArray);
                JArray ja2 = j["inner_key2"].getArray();
                {
                    EXPECT_EQ(ja2.size(), 2);
                    ASSERT_EQ(ja2[0].first, eString);
                    EXPECT_EQ(ja2[0].getString(), "a");
                    ASSERT_EQ(ja2[1].first, eString);
                    EXPECT_EQ(ja2[1].getString(), "b");
                }

                EXPECT_EQ(j.getComment("inner_key2").before,
                          std::string("многострочный коммент\nДО переменной inner_key2"));
                EXPECT_EQ(j.getComment("inner_key2").after,
                          std::string("многострочный коммент\nпосле переменной inner_key2"));
            }
        }
    }

    ASSERT_EQ(json.contains("key3"), true); {
        ASSERT_EQ(json["key3"].first, eJson);
        Json j = json["key3"].getJson();
        {
            ASSERT_EQ(j.size(), 2);
            ASSERT_EQ(j.contains("inner_key3"), true);
            ASSERT_EQ(j["inner_key3"].first, eString);
            EXPECT_EQ(j["inner_key3"].getString(), "a");
            ASSERT_EQ(j.contains("inner_key33"), true);
            ASSERT_EQ(j["inner_key33"].first, eString);
            ASSERT_EQ(j["inner_key33"].getString(), "b");
        }
    }

    //группы значений
    ASSERT_EQ(json.contains("group 1"), true); {
        ASSERT_EQ(json["group 1"].first, eJson);
        Json j = json["group 1"].getJson();
        {
            ASSERT_EQ(j.size(), 3);
            ASSERT_EQ(j.contains("g1_key"), true);
            ASSERT_EQ(j["g1_key"].first, eString);
            EXPECT_EQ(j["g1_key"].getString(), "value");
            ASSERT_EQ(j.contains("g1_key2"), true);
            ASSERT_EQ(j["g1_key2"].first, eNumber);
            EXPECT_EQ(j["g1_key2"].getNum(), 152);
            ASSERT_EQ(j.contains("g1_key3"), true);
            ASSERT_EQ(j["g1_key3"].first, eNumber);
            EXPECT_EQ(j["g1_key3"].getNum(), 152);

            EXPECT_EQ(j.getComment("g1_key").before,
                      std::string("ещё комментарий"));
        }
    }

    ASSERT_EQ(json.contains("group 2"), true); {
        ASSERT_EQ(json["group 2"].first, eJson);
        Json j = json["group 2"].getJson();
        {
            ASSERT_EQ(j.size(), 3);
            ASSERT_EQ(j.contains("g2_string"), true);
            ASSERT_EQ(j["g2_string"].first, eString);
            EXPECT_EQ(j["g2_string"].getString(), "one line string");
            ASSERT_EQ(j.contains("g2 string2"), true);
            ASSERT_EQ(j["g2 string2"].first, eString);
            EXPECT_EQ(j["g2 string2"].getString(), "big\nline string");
            ASSERT_EQ(j.contains("g2_key"), true);
            ASSERT_EQ(j["g2_key"].first, eJson);
            Json j2 = j["g2_key"].getJson();
            {
                ASSERT_EQ(j2["inner_key"].first, eJson);
                Json j3 = j2["inner_key"].getJson();
                {
                    EXPECT_EQ(j3.getComment("inner_inner_key").before,
                              std::string("коммент ДО переменной inner_inner_key"));
                    EXPECT_EQ(j3.getComment("inner_inner_key").after,
                              std::string("вложенные значения и группы значений"));
                }
            }
        }
    }

    ASSERT_EQ(json.contains("group_3"), true); {
        ASSERT_EQ(json["group_3"].first, eJson);
        Json j = json["group_3"].getJson();
        {
            ASSERT_EQ(j.size(), 1);
            ASSERT_EQ(j.contains("g3_key"), true);
            ASSERT_EQ(j["g3_key"].first, eArray);
            JArray ja = j["g3_key"].getArray();
            {
                ASSERT_EQ(ja.size(), 3);
                EXPECT_EQ(ja[0].first, eNumber);
                ASSERT_EQ(ja[0].getNum(), 15);
                ASSERT_EQ(ja[1].first, eNull);
                ASSERT_EQ(ja[2].first, eBool);
                EXPECT_EQ(ja[2].getBool(), true);
            }
        }
    }

}
