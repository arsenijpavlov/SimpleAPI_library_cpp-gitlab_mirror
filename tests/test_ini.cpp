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
                                       "key =  15\n"
                                       "array = [a, b, c]\n"
                                       "json = {json:[a, b, c]}\n"
                                       "\n"
                                       "[group 1]; ещё комментарий\n"
                                       "g1_key : value\n"
                                       "g1_key2 = g1_key3 = value2\n"
                                       "\n" //пустая строка после описания группы завершает группу
                                       "key2 = value2\n"
                                       "[group 2]\n"
                                       "g2_string = one line string\n"
                                       "g2 string2 = big\\\n"
                                       "            line string\n"
                                       "key\\inner_key\\inner_inner_key = inner_inner_value ; вложенные значения и группы значений\n"
                                       "\n"
                                       "key2\\inner_key2 = a\n"
                                       "key2/inner_key2 = b\n" // "/" и "\" равнозначны при обработке
                                       "key3\\inner_key3 = a\n"
                                       "key3/inner_key33 = b\n"
                                       "\n"
                                       "[group_3]\n"
                                       "key = a\n"
                                       "key = \n"
                                       "key = c\n"
    ;

//TODO: переделать парсинг под схему:
//      прочесть строку ПОЛНОСТЬЮ
//      обработать комментарии к ней
//      обработать строку на поиск значений
//          * srting, null, number, JSON, JArray, group, key=key2=value, ini_array
TEST(INI, parse) {
    Json json;
    json.parseINI(ini_example_string, true);

    std::cout << json.to_string(0, true, 0, ConfigFormat::eJSON) << std::endl;

    ASSERT_EQ(json.size(), 8);

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
}
