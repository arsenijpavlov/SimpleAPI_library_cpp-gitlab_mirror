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
                                       "key =  value\n"
                                       "\n"
                                       "[group 1]; ещё комментарий\n"
                                       "key : value\n"
                                       "key2 = key3 = value2\n"
                                       "\n"
                                       "[group 2]\n"
                                       "string = one line string\n"
                                       "string2 = big\\\n"
                                       "            line string\n"
                                       "key\\inner_key\\inner_inner_key = inner_inner_value ; вложенные значения и группы значений\n"
                                       "\n"
                                       "key2\\inner_key2 = a\n"
                                       "key2\\inner_key2 = b\n"
                                       "key3\\inner_key3 = a\n"
                                       "key3\\inner_key33 = b\n"
                                       "\n"
                                       "[group_3]\n"
                                       "key[0] = a\n"
                                       "key[2] = c\n";

TEST(INI, parse) {
    Json json;
    json.parseINI(ini_example_string, true);

    EXPECT_EQ(json.size(), 4);
}


CheckComments(is_one_line, is_multi_line, is_quotes) //TODO: перенести из функций парсинга
