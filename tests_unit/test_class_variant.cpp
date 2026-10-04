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
#define FIELDS(X, Y) \
    X(int,     field_int,     0) \
    X(uint8_t, field_uint8_t, 1)
SAPI_REGISTER_CONFIG(MyStruct, FIELDS)

// данный тест проверяет только компилируемость написанного
TEST(VARIANT, constructors) {
    using namespace simpleapi;

    using Variant1 = Variant<int, std::string, std::nullptr_t, int8_t, uint16_t, MyStruct>;
    using Variant2 = Variant<int, std::string, int8_t, uint16_t, MyStruct>;
    using Variant3 = Variant<int, std::string, int8_t, uint16_t>;
    int int_val = 156;

    // с std::nullptr_t
    const Variant1 var1(static_cast<int>(156));
    Variant1       var2(static_cast<int>(156));

    // без std::nullptr_t
    const Variant2 var3(static_cast<int>(156));
    Variant2       var4(static_cast<int>(156));

    // проверка конструкторов
    {
        const Variant1 var1_1(var1);
        Variant1       var2_1(var1);
        Variant1       var2_2(var2);

        const Variant2 var3_1(var1);
        Variant2       var3_2(var2);
        Variant2       var3_3(var3);
        Variant2       var3_4(var4);

        const Variant3 var_finish_1(var1);
        Variant3       var_finish_2(var2);
        Variant3       var_finish_3(var3);
        Variant3       var_finish_4(var4);
    }

    // "сырые" контейнеры не должны компилироваться
    int int_arr[5];
    static_assert(!std::is_constructible<Variant<int[5]>, int[5]>::value, "SimpleAPI: incorrect Variant<> logic");

    SUCCEED(); // исключительно для завершения теста
}

// наследник класса не должен преобразовываться сам к предку
TEST(VARIANT, parent_and_heir) {
    using namespace simpleapi;

    // наследованный тип (подали наследника - должен присвоиться наследник)
    struct Base {};
    struct A : Base {} example_A;

    Variant<Base, A> var_base_base = example_A;
    Variant<A, Base> var_base_a    = example_A;

    EXPECT_EQ(var_base_base.index(), 1);
    EXPECT_EQ(var_base_a.index(),    0);
}

TEST(VARIANT, raw_c_text) {
    using namespace simpleapi;

    // "сырые" строки разной длины
    const char* str_1 = "first";
    const char* str_2 = "second";

    Variant<int, const char*> var_with_strings = str_1;
    EXPECT_EQ(var_with_strings.index(), 1);

    var_with_strings = str_2;
    EXPECT_EQ(var_with_strings.index(), 1);
}

// запрещено указать два и более одинаковых типов
// разные типы одного формата, например числа, должны распределяться согласно наилучшему варианту
TEST(VARIANT, duplications) {
    using namespace simpleapi;

    // NOTE: дублирование проверяется внутри Variant

    // перекрытие типов
    Variant<int, uint8_t, size_t> var_dup = (uint8_t)15;
    EXPECT_EQ(var_dup.index(), 1);

    var_dup = (size_t)16;
    EXPECT_EQ(var_dup.index(), 2);
}

// const int и int - не дубликаты, но будет выбран индекс первого типа из указанных
TEST(VARIANT, cv_modificators) {
    using namespace simpleapi;

    Variant<const int, std::string, int> var_cv_1 = 15;
    EXPECT_EQ(var_cv_1.index(), 0);

    const int ci = 15;
    Variant<const int, std::string, int> var_cv_2(ci);
    EXPECT_EQ(var_cv_2.index(), 0);

    Variant<const int, std::string, int> var_cv_3(std::move(ci));
    EXPECT_EQ(var_cv_3.index(), 0);

    int i = 15;
    Variant<const int, std::string, int> var_cv_4(i);
    EXPECT_EQ(var_cv_4.index(), 0);

    Variant<const int, std::string, int> var_cv_5(std::move(i));
    EXPECT_EQ(var_cv_5.index(), 0);

    uint8_t ui = 15;
    Variant<const int, std::string, uint8_t> var_cv_6(ui);
    EXPECT_EQ(var_cv_6.index(), 2);

    var_cv_6 = static_cast<uint8_t>(165);
    EXPECT_EQ(var_cv_6.index(), 2);
    //NOTE: смена значения через "= int" для типа "const int" запрещена на уровне компиляции
}

// const char[4] - сырые строки, переданные напрямую
TEST(VARIANT, constructors_with_raw_string) {
    using namespace simpleapi;

    Variant<bool, std::string> var_bool_string("asd");
    EXPECT_EQ(var_bool_string.index(), 1);

    using FindType = decltype("asd");
    EXPECT_TRUE((std::is_constructible<std::string, FindType>::value));

    EXPECT_TRUE((decltype(tools::compiler_type_resolver<0, FindType, std::string>::
                          template match<FindType>(std::declval<FindType>()))::value != static_cast<size_t>(-1)));
}

// проверка указателей
TEST(VARIANT, null_pointers) {
    using namespace simpleapi;

    Variant<int, std::string> var_ptr_val;
    EXPECT_EQ(var_ptr_val.get_if<std::string>(), nullptr);
    EXPECT_NE(var_ptr_val.get_if<int>(),         nullptr);
    EXPECT_NE(var_ptr_val.get_if<uint8_t>(),     nullptr);

    Variant<const int, const std::string> var_const_ptr_val("asd");
    EXPECT_NE(var_const_ptr_val.get_if<std::string>(), nullptr);
    EXPECT_EQ(var_const_ptr_val.get_if<int>(),         nullptr);
    EXPECT_EQ(var_const_ptr_val.get_if<uint8_t>(),     nullptr);

    Variant<std::nullptr_t, const int, std::string> var_const_ptr_val_;
    EXPECT_EQ(var_const_ptr_val_.get_if<std::nullptr_t>(), nullptr);
}

// проверка присвоения значения без выделения памяти (тип совпал)
TEST(VARIANT, apply_new_value_without_recreate_memory) {
    using namespace simpleapi;

    Variant<int, std::string> var(10);
    var = 15;
    EXPECT_EQ(var.get<int>(), 15);

    const int i = 16;
    var = i;
    EXPECT_EQ(var.get<int>(), 16);

    // защита от перезаписи const типов уже внутри Variant::operator=()
}
