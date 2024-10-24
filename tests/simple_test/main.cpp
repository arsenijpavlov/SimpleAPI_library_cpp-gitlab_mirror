#include <SimpleAPI.h>
#include <iomanip>
#include <iostream>
#include <fstream>


int main() {
//    system("tabs 4");
    const std::string ini_example_string = ""
//                                           "; комментарий\n"
//                                           "; вторая строка комментария\n"
//                                           "key =  15\n"
//                                           "array = [a, 15, true]\n"
//                                           "json = {inner_array:[a, 4.0, \"asd string\"]}\n"
//                                           "\n"
//                                           "[group 1]; ещё комментарий\n"
//                                           "g1_key : value\n"
//                                           "g1_key2 = g1_key3 = 152\n"
//                                           //пустая строка после описания группы завершает группу
//                                           "\n"
//                                           "key2 = value2\n"
//                                           "[group 2]\n"
//                                           "g2_string = one line string\n"
//                                           "g2 string2 = big\\\n"
//                                           "            line string\n"
//                                           //комментарий применится только для конечного элемента
//                                           "; коммент ДО переменной inner_inner_key\n"
//                                           "g2_key\\inner_key\\inner_inner_key = inner_inner_value ; вложенные значения и группы значений\n"
//                                           "\n"
//                                           "; многострочный коммент\n"
//                                           "; ДО переменной inner_key2\n"
//                                           "key2\\inner_key2 = a /*многострочный коммент\nпосле переменной inner_key2*/\n"
//                                           "key2/inner_key2 = b\n" // "/" и "\" равнозначны при обработке
                                           "key3\\inner_key3 = a\n"
                                           "key3/inner_key33 = b\n"
//                                           "\n"
//                                           "[group_3]\n"
//                                           "g3_key = 15\n"
//                                           "g3_key = \n"
//                                           "g3_key = true\n"
        ;
    Json json;
    json.parseINI(ini_example_string, true);

    std::cout << std::endl << utils::RepeatSymToStr('-', 30) << std::endl;
    std::cout << json.to_INI_string(0, true) << std::endl;

    return 0;
}
