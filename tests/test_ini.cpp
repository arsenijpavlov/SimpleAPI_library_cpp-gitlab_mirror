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
                                       "json = {inner_json:[a, 4.0, \"asd string\"]}\n"
                                       "\n"
                                       "[group 1]; ещё комментарий\n"
                                       "g1_key : value\n"
                                       "g1_key2 = g1_key3 = value2\n"
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

//TODO: некорректный вывод однострочных комментариев {
//          "group 2" : {
//              "key" : {
//                  "inner_key" : {
//                      /*
//          				вложенные значения
//          				и гр
//          				уппы значений
//          				*/
//                      "inner_inner_key" : "inner_inner_value" }
//              }
//          }
//      }

TEST(INI, parse) {
    Json json;
    json.parseINI(ini_example_string, true);

    std::cout << json.to_string(0, true, 0, ConfigFormat::eJSON) << std::endl;

    ASSERT_EQ(json.size(), 9);

//    EXPECT_EQ(json["key"].first, eNumber);
//    EXPECT_EQ(json["key"].getNum(), 15);

//    EXPECT_EQ(json["array"].first, eArray);
//    EXPECT_EQ(json["array"].getArray().size(), 3);

//    EXPECT_EQ(json["json"].first, eJson);
//    EXPECT_EQ(json["json"].getJson().size(), 1);

//    EXPECT_EQ(json["key2"].first, eArray);
//    EXPECT_EQ(json["key2"].getArray().size(), 2);

//    EXPECT_EQ(json["g1_key"].first, eString);
//    EXPECT_EQ(json["g1_key"].getString(), "value");

//    EXPECT_EQ(json["string2"].first, eString);
//    EXPECT_EQ(json["string2"].getString(), "big\nline string");

//    ASSERT_EQ(json["group_3"].first, eJson);
//    EXPECT_EQ(json["group_3"].getJson()["key"].getArray()[0].first, eNumber);
//    EXPECT_EQ(json["group_3"].getJson()["key"].getArray()[0].getNum(), 15);
//    EXPECT_EQ(json["group_3"].getJson()["key"].getArray()[1].first, eNull);
//    EXPECT_EQ(json["group_3"].getJson()["key"].getArray()[1], Element());
//    EXPECT_EQ(json["group_3"].getJson()["key"].getArray()[2].first, eBool);
//    EXPECT_EQ(json["group_3"].getJson()["key"].getArray()[2].getBool(), true);
}
