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
std::string ini_example_string = std::string(
    ""
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
    "g2 string2 = big\n"
    "            line string\n" //пробелы после переноса строки должны стать равны одному пробелу
                                //при записи в файл, дополнить пробелами до знака "равно"
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
    );


TEST(INI, main_parser) {
    Config json;
    CommentDesign cd;
    cd.with_comments = true;
    cd.oneline_comment_variants.push_back({';', 0});

    json.parseIni(ini_example_string, cd);
    std::cout << json.toString(ConfigFormat::eJSON) << std::endl;
    std::cout << json.toString(ConfigFormat::eINI) << std::endl;

    EXPECT_EQ(json.size(), 8);

    ASSERT_EQ(json.containsKey("key"), true); {
        ASSERT_TRUE(json["key"].isNumber());
        EXPECT_EQ(json["key"].getNumber(), 15);
        EXPECT_EQ(json.get_comment("key").prefix(),
                  std::string("комментарий\nвторая строка комментария"));
    }

    ASSERT_EQ(json.containsKey("array"), true); {
        ASSERT_TRUE(json["array"].isArray());
        Config ja = json["array"];
        {
            EXPECT_EQ(ja.size(), 3);
            EXPECT_TRUE(ja[0].isString());
            EXPECT_TRUE(ja[1].isNumber());
            EXPECT_TRUE(ja[2].isBool());
        }
    }

    ASSERT_EQ(json.containsKey("json"), true); {
        ASSERT_TRUE(json["json"].isJson());
        Config j = json["json"];
        {
            EXPECT_EQ(j.size(), 1);
            ASSERT_EQ(j.containsKey("inner_array"), true);
            ASSERT_TRUE(j["inner_array"].isArray());
            Config ja = j["inner_array"];
            {
                EXPECT_EQ(ja.size(), 3);
                EXPECT_TRUE(ja[0].isString());
                EXPECT_TRUE(ja[1].isNumber());
                EXPECT_TRUE(ja[2].isString());
            }
        }
    }

    ASSERT_EQ(json.containsKey("key2"), true); {
        ASSERT_TRUE(json["key2"].isArray());
        Config ja = json["key2"];
        {
            ASSERT_EQ(ja.size(), 2);
            ASSERT_TRUE(ja[0].isString());
            EXPECT_EQ(ja[0].getString(), "value2");
            ASSERT_TRUE(ja[1].isJson());
            Config j = ja[1];
            {
                ASSERT_EQ(j.size(), 1);
                ASSERT_EQ(j.containsKey("inner_key2"), true);
                ASSERT_TRUE(j["inner_key2"].isArray());
                Config ja2 = j["inner_key2"];
                {
                    EXPECT_EQ(ja2.size(), 2);
                    ASSERT_TRUE(ja2[0].isString());
                    EXPECT_EQ(ja2[0].getString(), "a");
                    ASSERT_TRUE(ja2[1].isString());
                    EXPECT_EQ(ja2[1].getString(), "b");
                }

                EXPECT_EQ(j.get_comment("inner_key2").prefix(),
                          std::string("многострочный коммент\nДО переменной inner_key2"));
                EXPECT_EQ(j.get_comment("inner_key2").suffix(),
                          std::string("многострочный коммент\nпосле переменной inner_key2"));
            }
        }
    }

    ASSERT_EQ(json.containsKey("key3"), true); {
        ASSERT_TRUE(json["key3"].isJson());
        Config j = json["key3"];
        {
            ASSERT_EQ(j.size(), 2);
            ASSERT_EQ(j.containsKey("inner_key3"), true);
            ASSERT_TRUE(j["inner_key3"].isString());
            EXPECT_EQ(j["inner_key3"].getString(), "a");
            ASSERT_EQ(j.containsKey("inner_key33"), true);
            ASSERT_TRUE(j["inner_key33"].isString());
            ASSERT_EQ(j["inner_key33"].getString(), "b");
        }
    }

    //группы значений
    ASSERT_EQ(json.containsKey("group 1"), true); {
        ASSERT_TRUE(json["group 1"].isJson());
        Config j = json["group 1"];
        {
            ASSERT_EQ(j.size(), 3);
            ASSERT_EQ(j.containsKey("g1_key"), true);
            ASSERT_TRUE(j["g1_key"].isString());
            EXPECT_EQ(j["g1_key"].getString(), "value");
            ASSERT_EQ(j.containsKey("g1_key2"), true);
            ASSERT_TRUE(j["g1_key2"].isNumber());
            EXPECT_EQ(j["g1_key2"].getNumber(), 152);
            ASSERT_EQ(j.containsKey("g1_key3"), true);
            ASSERT_TRUE(j["g1_key3"].isNumber());
            EXPECT_EQ(j["g1_key3"].getNumber(), 152);

            EXPECT_EQ(j.get_comment("g1_key").prefix(),
                      std::string("ещё комментарий"));
        }
    }

    ASSERT_EQ(json.containsKey("group 2"), true); {
        ASSERT_TRUE(json["group 2"].isJson());
        Config j = json["group 2"];
        {
            ASSERT_EQ(j.size(), 3);
            ASSERT_EQ(j.containsKey("g2_string"), true);
            ASSERT_TRUE(j["g2_string"].isString());
            EXPECT_EQ(j["g2_string"].getString(), "one line string");
            ASSERT_EQ(j.containsKey("g2 string2"), true);
            ASSERT_TRUE(j["g2 string2"].isString());
            EXPECT_EQ(j["g2 string2"].getString(), "big\nline string");
            ASSERT_EQ(j.containsKey("g2_key"), true);
            ASSERT_TRUE(j["g2_key"].isJson());
            Config j2 = j["g2_key"];
            {
                ASSERT_TRUE(j2["inner_key"].isJson());
                Config j3 = j2["inner_key"];
                {
                    EXPECT_EQ(j3.get_comment("inner_inner_key").prefix(),
                              std::string("коммент ДО переменной inner_inner_key"));
                    EXPECT_EQ(j3.get_comment("inner_inner_key").suffix(),
                              std::string("вложенные значения и группы значений"));
                }
            }
        }
    }

    ASSERT_EQ(json.containsKey("group_3"), true); {
        ASSERT_TRUE(json["group_3"].isJson());
        Config j = json["group_3"];
        {
            ASSERT_EQ(j.size(), 1);
            ASSERT_EQ(j.containsKey("g3_key"), true);
            ASSERT_TRUE(j["g3_key"].isArray());
            Config ja = j["g3_key"];
            {
                ASSERT_EQ(ja.size(), 3);
                EXPECT_TRUE(ja[0].isNumber());
                ASSERT_EQ(ja[0].getNumber(), 15);
                ASSERT_TRUE(ja[1].isNull());
                ASSERT_TRUE(ja[2].isBool());
                EXPECT_EQ(ja[2].getBool(), true);
            }
        }
    }

}
