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

TEST(UTILS, string_len) {
    using namespace simpleapi;

    std::string example = "12345";
    EXPECT_EQ(5, utils::GetStringCharCount(example));

    example = "string";
    EXPECT_EQ(6, utils::GetStringCharCount(example));

    example = "строка";
    EXPECT_EQ(6, utils::GetStringCharCount(example));

    example = "string12345строка";
    EXPECT_EQ(17, utils::GetStringCharCount(example));

    example = "Ř"; //2-byte UNICODE symbol
    EXPECT_EQ(1, utils::GetStringCharCount(example));

    example = "炗"; //3-byte UNICODE symbol
    EXPECT_EQ(1, utils::GetStringCharCount(example));

    example = "🚵"; //4-byte UNICODE symbol
    EXPECT_EQ(1, utils::GetStringCharCount(example));
}

TEST(UTILS, EndToEndCounter_set_overflowing) {
    using namespace simpleapi;
    EECounter ee(256);

    ee.set_pos(300);
    EXPECT_EQ(ee.get(), 44);

    ee.reset();
    ee.set_glob_pos(300);
    EXPECT_EQ(ee.get_glob(), 44);

    ee.reset();
    ee.set_pos(300, 300);
    EXPECT_EQ(ee.get_glob(), 45);
    EXPECT_EQ(ee.get(), 44);
}

TEST(UTILS, EndToEndCounter_add_overflowing) {
    using namespace simpleapi;
    EECounter ee(256);

    ee.add(256);
    EXPECT_EQ(ee.get(), 0);

    ee.reset();
    ee.set_pos(255);
    ee.add(1);
    EXPECT_EQ(ee.get(), 0);
}

TEST(UTILS, EndToEndCounter_sub_overflowing) {
    using namespace simpleapi;
    EECounter ee(256);

    ee.set_pos(0);
    ee.sub(256);
    EXPECT_EQ(ee.get(), 0);

    ee.reset();
    ee.set_pos(255);
    ee.sub(256);
    EXPECT_EQ(ee.get(), 255);
}

// FIXME: внедрить проверку компилируемости для simpleapi::utils::Variant
//using Variant1 = Variant<int, std::string, std::nullptr_t, int8_t, uint16_t, MyStruct>;
//using Variant2 = Variant<int, std::string, int8_t, uint16_t, MyStruct>;
//using Variant3 = Variant<int, std::string, int8_t, uint16_t>;
//int int_val = 156;

//// с std::nullptr_t
//const Variant1 var1(static_cast<int>(156));
//Variant1       var2(static_cast<int>(156));

//// без std::nullptr_t
//const Variant2 var3(static_cast<int>(156));
//Variant2       var4(static_cast<int>(156));

//// проверка конструкторов
//{
//    const Variant1 var1_1(var1);
//    Variant1       var2_1(var1);
//    Variant1       var2_2(var2);

//    const Variant2 var3_1(var1);
//    Variant2       var3_2(var2);
//    Variant2       var3_3(var3);
//    Variant2       var3_4(var4);

//    const Variant3 var_finish_1(var1);
//    Variant3       var_finish_2(var2);
//    Variant3       var_finish_3(var3);
//    Variant3       var_finish_4(var4);
//}

//// "сырые" контейнеры не должны компилироваться
//int int_arr[5];
//static_assert(!std::is_constructible<Variant<int[5]>, int[5]>::value, "SimpleAPI: incorrect Variant<> logic");

//// наследованный тип (подали наследника - должен присвоиться наследник)
//struct Base {};
//struct A : Base {} example_A;
//Variant<Base, A> var_base_base = example_A;
//Variant<A, Base> var_base_a    = example_A;
//std::cout << utils::ToString(var_base_base.index() == 1) << std::endl;
//std::cout << utils::ToString(var_base_a.index() == 0) << std::endl;
//std::cout << "--------------------------------------------------" << std::endl;

//// "сырые" строки разной длины
//const char* str_1 = "first";
//const char* str_2 = "second";
//Variant<int, const char*> var_with_strings = str_1;
//std::cout << utils::ToString(var_with_strings.index() == 1) << std::endl;
//var_with_strings = str_2;
//std::cout << utils::ToString(var_with_strings.index() == 1) << std::endl;
//std::cout << "--------------------------------------------------" << std::endl;

//// дублирование(проверяется внутри Variant) и перекрытие типов
//Variant<int, uint8_t, size_t> var_dup = (uint8_t)15;
//std::cout << utils::ToString(var_dup.index() == 1) << std::endl;
//var_dup = (size_t)16;
//std::cout << utils::ToString(var_dup.index() == 2) << std::endl;
//std::cout << "--------------------------------------------------" << std::endl;

//// cv-модификаторы
//// (const int и int - не дубликаты, но будет выбран индекс первого типа из указанных)
//Variant<const int, std::string, int> var_cv_1 = 15;
//std::cout << var_cv_1.index() << std::endl;
//const int ci = 15;
//Variant<const int, std::string, int> var_cv_2(ci);
//std::cout << var_cv_2.index() << std::endl;
//Variant<const int, std::string, int> var_cv_3(std::move(ci));
//std::cout << var_cv_3.index() << std::endl;
//int i = 15;
//Variant<const int, std::string, int> var_cv_4(i);
//std::cout << var_cv_4.index() << std::endl;
//Variant<const int, std::string, int> var_cv_5(std::move(i));
//std::cout << var_cv_5.index() << std::endl;
//uint8_t ui = 15;
//Variant<const int, std::string, uint8_t> var_cv_6(ui);
//std::cout << var_cv_6.index() << std::endl;
//var_cv_6 = static_cast<int>(165);
//std::cout << var_cv_6.index() << std::endl;
//std::cout << "--------------------------------------------------" << std::endl;

//// const char[4] - сырые строки, переданные напрямую
//Variant<bool, std::string> var_bool_string("asd");
//std::cout << utils::ToString(var_bool_string.index() == 1) << std::endl;

//using FindType = decltype("asd");
//std::cout << "is_constructible: " << utils::ToString(std::is_constructible<std::string, FindType>::value) << std::endl;
//std::cout << "compiler_type_resolver: "
//          << utils::ToString(decltype(tools::compiler_type_resolver<0, FindType, std::string>::
//                                      template match<FindType>(std::declval<FindType>()))::value != static_cast<size_t>(-1))
//          << std::endl;
//std::cout << "--------------------------------------------------" << std::endl;

//// проверка указателей
//Variant<int, std::string> var_ptr_val;
//std::cout << utils::ToString(var_ptr_val.get_if<std::string>() == nullptr) << std::endl;
//std::cout << utils::ToString(var_ptr_val.get_if<int>() != nullptr) << std::endl;
//std::cout << utils::ToString(var_ptr_val.get_if<uint8_t>() != nullptr) << std::endl;
//Variant<const int, const std::string> var_const_ptr_val("asd");
//std::cout << utils::ToString(var_const_ptr_val.get_if<std::string>() != nullptr) << std::endl;
//std::cout << utils::ToString(var_const_ptr_val.get_if<int>() == nullptr) << std::endl;
//std::cout << utils::ToString(var_const_ptr_val.get_if<uint8_t>() == nullptr) << std::endl;
