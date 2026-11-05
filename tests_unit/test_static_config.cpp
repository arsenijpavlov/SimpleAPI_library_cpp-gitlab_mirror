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


// NOTE: следующая группа тестов не разбита на составляющие, т.к. любая ошибка повлияет на ВСЮ компиляцию файла

#define ENUM1_VARIANTS(X) \
    X(e1) X(e2) X(e3)

SAPI_REGISTER_ENUM(CustomEnum_1, ENUM1_VARIANTS)
//-------------------------------------------------------------------------

// пример с заданными внутренними индексами
#define ENUM2_VARIANTS(X) \
    X(e1, 10)             \
    X(e2, 20)             \
    X(e3, 30)

// пример также учитывает наследование enum class от uint8_t
SAPI_REGISTER_ENUM(CustomEnum_2, ENUM2_VARIANTS, uint8_t)
//-------------------------------------------------------------------------

// частный случай со значениями бесконечности: в Config оно распарсится как число, даже если подразумевается строка
#define ENUM_INF_VARIANTS(X) \
    X(inf, 10)               \
    X(infinity, 20)          \
    X(other, 30)

SAPI_REGISTER_ENUM(CustomEnum_Inf, ENUM_INF_VARIANTS, uint8_t)

#define INF_FIELDS(X, Y) \
    X(CustomEnum_Inf, var, CustomEnum_Inf::infinity)
SAPI_REGISTER_CONFIG(InfConf, INF_FIELDS)
//-------------------------------------------------------------------------

using PQueuePlaceholder = std::priority_queue<int, std::vector<int>, std::less<int>>;
std::vector<int> vec_int = {1, 2, 3, 4, 5};
using ArrayPlaceholder  = std::array<uint8_t, 5>;
using MapPlaceholder    = std::map<int, float>;
using MMapPlaceholder   = std::multimap<int, float>;
using UMapPlaceholder   = std::unordered_map<int, float>;
using UMMapPlaceholder  = std::unordered_multimap<int, float>;
using PairPlaceholder   = std::pair<std::string, int>;

#define STRUCT_FIELDS(X, Y)                                                                                      \
    X(bool,                           val_b,       true                                                        ) \
    X(int,                            val_i,       1                                                           ) \
    X(uint8_t,                        val_u8,      1                                                           ) \
    X(uint16_t,                       val_u16,     1                                                           ) \
    X(uint32_t,                       val_u32,     1                                                           ) \
    X(uint64_t,                       val_u64,     1                                                           ) \
    X(int8_t,                         val_i8,      1                                                           ) \
    X(int16_t,                        val_i16,     1                                                           ) \
    X(int32_t,                        val_i32,     1                                                           ) \
    X(int64_t,                        val_i64,     1                                                           ) \
    X(float,                          val_f,       1                                                           ) \
    X(double,                         val_d,       1                                                           ) \
    X(long double,                    val_ld,      1                                                           ) \
    X(std::string,                    val_s,       "str"                                                       ) \
    X(CustomEnum_1,                   my_enum_1,   CustomEnum_1::e1                                            ) \
    X(CustomEnum_2,                   my_enum_2,   CustomEnum_2::e1                                            ) \
    X(CustomEnum_Inf,                 my_enum_3,   CustomEnum_Inf::infinity                                    ) \
    X(std::vector<std::string>,       val_vs,      (std::vector<std::string>({"s1", "s2"}))                    ) \
    X(std::list<std::string>,         val_ls,      (std::list<std::string>({"s1", "s2"}))                      ) \
    X(std::forward_list<std::string>, val_fls,     (std::forward_list<std::string>({"s1", "s2"}))              ) \
    X(std::deque<std::string>,        val_ds,      (std::deque<std::string>({"s1", "s2"}))                     ) \
    X(std::set<int>,                  val_si,      (std::set<int>({1, 2}))                                     ) \
    X(std::multiset<int>,             val_msi,     (std::multiset<int>({1, 1, 2, 3}))                          ) \
    X(std::unordered_set<int>,        val_usi,     (std::unordered_set<int>({1, 2}))                           ) \
    X(std::unordered_multiset<int>,   val_umsi,    (std::unordered_multiset<int>({1, 1, 2, 3}))                ) \
    X(std::queue<int>,                val_qi,      (std::queue<int>({1, 1, 2, 3}))                             ) \
    X(PQueuePlaceholder,              val_pqi,     (PQueuePlaceholder(vec_int.begin(), vec_int.end()))         ) \
    X(std::stack<int>,                val_stack_i, (std::stack<int>({1, 1, 2, 3}))                             ) \
    X(ArrayPlaceholder,               val_ar,      (ArrayPlaceholder({1, 2, 3, 4, 5}))                         ) \
    X(std::bitset<5>,                 val_bs,      (std::bitset<5>(0b01010))                                   ) \
    X(MapPlaceholder,                 map,         (MapPlaceholder({{1, 2.3f}, {2, 2.4f}}))                    ) \
    X(MMapPlaceholder,                mmap,        (MMapPlaceholder({{1, 2.3f}, {2, 2.4f}}))                   ) \
    X(UMapPlaceholder,                umap,        (UMapPlaceholder({{1, 2.3f}, {2, 2.4f}}))                   ) \
    X(UMMapPlaceholder,               ummap,       (UMMapPlaceholder({{1, 2.3f}, {2, 2.4f}}))                  ) \
    X(PairPlaceholder,                val_pair,    (PairPlaceholder("key", 2.3))                               ) \
    X(simpleapi::Config,              dyn_cfg,     (simpleapi::Config(simpleapi::ValueType::eJson, "key", 2.3))) \

SAPI_REGISTER_CONFIG(CustomStruct, STRUCT_FIELDS)
//-------------------------------------------------------------------------

// варианты допустимых ключей для ассоциативных контейнеров
using Map_WithKey_String = std::map<std::string, int>;
using Map_WithKey_Bool   = std::map<bool, int>;
using Map_WithKey_Number = std::map<int, int>;

#define MAP_FIELDS(X, Y)                                           \
    X(Map_WithKey_String, m1, (Map_WithKey_String({{"asd", 15}}))) \
    X(Map_WithKey_Bool,   m2, (Map_WithKey_Bool({{false, 15}})))   \
    X(Map_WithKey_Number, m3, (Map_WithKey_Number({{1, 15}})))

SAPI_REGISTER_CONFIG(MapConfig, MAP_FIELDS)
//-------------------------------------------------------------------------

// разные варианты использования лямбд и комментариев
auto lambda_1 = [](const int& val) -> bool {
    /* ... */
    return true;
};
auto lambda_2 = [](const int& val, const std::string& key) -> bool {
    /* ... */
    return true;
};

#define WITH_LAMBDA_FIELDS(X, Y)                                                       \
    X(int, i_1, 0)                                                                     \
    X(int, i_2, 0, [](const int& val) -> bool { return true; } )                       \
    X(int, i_3, 0, [](const int& val) -> bool { return true; }, "prefix comment" )     \
    X(int, i_4, 0, [](const int& val) -> bool { return true; }, "", "suffix comment" ) \
    X(int, i_5, 0, nullptr, "comment", "comment" )                                     \
    X(int, i_6, 0, lambda_1, "comment", "comment" )                                    \
    X(int, i_7, 0, lambda_2, "comment", "comment" )

SAPI_REGISTER_CONFIG(StructWithLambdas, WITH_LAMBDA_FIELDS)
//-------------------------------------------------------------------------

// Опциональные и вариативные значения
#define OPT_VAR_FIELDS(X, Y) \
    X(simpleapi::Optional<int>, opt_i, simpleapi::Optional<int>(15, false))

SAPI_REGISTER_CONFIG(OptVarConfig, OPT_VAR_FIELDS)
//-------------------------------------------------------------------------

// вариант рекурсивных вложенностей зарегистрированных структур
#define STRUCT2_FIELDS(X, Y)                       \
    X(int,               i,  15)                   \
    X(CustomStruct,      cs, CustomStruct())       \
    X(MapConfig,         mc, MapConfig())          \
    X(StructWithLambdas, swl, StructWithLambdas()) \
    X(OptVarConfig,      ovc, OptVarConfig())

SAPI_REGISTER_CONFIG(CustomStruct2, STRUCT2_FIELDS)
//-------------------------------------------------------------------------

TEST(STATIC, main) {
    using namespace simpleapi;

    CustomStruct cs;
    CustomStruct2 cs2;
    CustomStruct2 cs2_copy;

    cs2.cs = cs;
    cs2.i  = 30;
    cs2.cs.my_enum_3 = CustomEnum_Inf::inf;

    // далее идёт сохранение дефолтных настроек в указанный формат конфига и чтение структуры из него же
    Config cfg = cs2.saveConfig();
    cfg.writeFile("static.json", simpleapi::ConfigFormat::eJSON);
    EXPECT_TRUE(cs2_copy.loadConfig(cfg));
    EXPECT_EQ(static_config_error_str, "");

    EXPECT_EQ(cs2.cs,  cs2_copy.cs);
    EXPECT_EQ(cs2.i,   cs2_copy.i);
    EXPECT_EQ(cs2.mc,  cs2_copy.mc);
    EXPECT_EQ(cs2.swl, cs2_copy.swl);
    EXPECT_EQ(cs2.ovc, cs2_copy.ovc);

    EXPECT_EQ(cs2, cs2_copy);
}

TEST(STATIC, correct_reader_bool) {
    using namespace simpleapi;

    CustomStruct cs1;
    Config cfg1;
    bool new_cfg_bool;

    cfg1          = cs1.saveConfig();
    new_cfg_bool = !cfg1["val_b"].getBool();
    cfg1["val_b"] = new_cfg_bool;
    EXPECT_TRUE(cs1.loadConfig(cfg1));
    EXPECT_EQ(cs1.val_b, new_cfg_bool);

    // дубль с инвертированным значением
    cfg1          = cs1.saveConfig();
    new_cfg_bool = !cfg1["val_b"].getBool();
    cfg1["val_b"] = new_cfg_bool;
    EXPECT_TRUE(cs1.loadConfig(cfg1));
    EXPECT_EQ(cs1.val_b, new_cfg_bool);

    CustomStruct2 cs2;
    Config cfg2;

    cfg2                = cs2.saveConfig();
    new_cfg_bool        = !cfg2["cs"]["val_b"].getBool();
    cfg2["cs"]["val_b"] = new_cfg_bool;
    EXPECT_TRUE(cs2.loadConfig(cfg2));
    EXPECT_EQ(cs2.cs.val_b, new_cfg_bool);

    // дубль с инвертированным значением
    cfg2                = cs2.saveConfig();
    new_cfg_bool        = !cfg2["cs"]["val_b"].getBool();
    cfg2["cs"]["val_b"] = new_cfg_bool;
    EXPECT_TRUE(cs2.loadConfig(cfg2));
    EXPECT_EQ(cs2.cs.val_b, new_cfg_bool);
}

#define TEST_FIELDS(X, Y) \
    X(float, fl, 2.4f)
SAPI_REGISTER_CONFIG(FloatConf, TEST_FIELDS)

TEST(STATIC, float_writer) {
    using namespace simpleapi;

    float ff = 2.4f;
    long double ld = static_cast<long double>(ff);
    EXPECT_EQ(ld, 2.4f);

    FloatConf fc;
    Config cfg = fc.saveConfig();
    cfg.parseJson(cfg.toString());
    fc.loadConfig(cfg);

    EXPECT_EQ(fc.fl, 2.4f);
}

using MyVariant = simpleapi::Variant<int, std::nullptr_t, std::string>;
std::string my_variant_test_string;

//-------------------------
#define VARIANT_FIELDS_SIMPLE(X, Y) \
    X(MyVariant, variant, MyVariant(0))
SAPI_REGISTER_CONFIG(VariantSimple, VARIANT_FIELDS_SIMPLE)
//-------------------------
TEST(STATIC, variant_simple) {
    using namespace simpleapi;

    Config cfg;
    VariantSimple vs;

    int i           = 100;
    std::string str = "asd";

    std::string result_str_int     = "int: " + std::to_string(i);
    std::string result_str_string  = "string: " + str;
    std::string result_str_nullptr = "std::nullptr_t";

    bool result = false;
    auto run = [&](std::string&& input) {
        cfg.parseJson(input);
        result = vs.loadConfig(cfg);
        Visit(vs.variant
            , [&](const int& i)           { my_variant_test_string = result_str_int;     }
            , [&](const std::string& str) { my_variant_test_string = result_str_string;  }
            , [&](std::nullptr_t)         { my_variant_test_string = result_str_nullptr; }
            );
    };

    my_variant_test_string.clear();
    run(std::string("{variant:\"") + str + "\"}");
    EXPECT_EQ(my_variant_test_string, result_str_string);
    EXPECT_TRUE(result);

    my_variant_test_string.clear();
    run(std::string("{variant:") + std::to_string(i) + "}");
    EXPECT_EQ(my_variant_test_string, result_str_int);
    EXPECT_TRUE(result);

    my_variant_test_string.clear();
    run("{variant: null}");
    EXPECT_EQ(my_variant_test_string, result_str_nullptr);
    EXPECT_TRUE(result);
}

//-------------------------
#define VARIANT_FIELDS_LAMBDA_OBJ(X, Y) \
    X(MyVariant, variant, MyVariant(0), [](const MyVariant& var) -> bool {                           \
        return Visit(var                                                                             \
            , [](const int& i)           -> bool { return i == 0; }                                  \
            , [](const std::string& str) -> bool { return !str.empty(); }                            \
            , [](std::nullptr_t)         -> bool { return true; /* здесь нет смысла в валидации */ } \
        );                                                                                           \
    })
SAPI_REGISTER_CONFIG(VariantLambdaObj, VARIANT_FIELDS_LAMBDA_OBJ)
//-------------------------
TEST(STATIC, variant_lambda_obj) {
    using namespace simpleapi;

    Config cfg;
    VariantLambdaObj vlo;

    int i           = 0;
    std::string str = "asd";

    std::string result_str_int     = "int: " + std::to_string(i);
    std::string result_str_string  = "string: " + str;
    std::string result_str_nullptr = "std::nullptr_t";

    bool result = false;
    auto run = [&](std::string&& input) {
        cfg.parseJson(input);
        result = vlo.loadConfig(cfg);
        Visit(vlo.variant
            , [&](const int& i)           { my_variant_test_string = result_str_int;     }
            , [&](const std::string& str) { my_variant_test_string = result_str_string;  }
            , [&](std::nullptr_t)         { my_variant_test_string = result_str_nullptr; }
            );
    };

    my_variant_test_string.clear();
    run(std::string("{variant:\"") + str + "\"}");
    EXPECT_EQ(my_variant_test_string, result_str_string);
    EXPECT_TRUE(result);

    my_variant_test_string.clear();
    run(std::string("{variant:") + std::to_string(i) + "}");
    EXPECT_EQ(my_variant_test_string, result_str_int);
    EXPECT_TRUE(result);

    my_variant_test_string.clear();
    run("{variant: null}");
    EXPECT_EQ(my_variant_test_string, result_str_nullptr);
    EXPECT_TRUE(result);

    // ошибки валидации
    run("{variant:\"\"}");
    EXPECT_FALSE(result);

    run(std::string("{variant:") + std::to_string(100) + "}");
    EXPECT_FALSE(result);
}

//-------------------------
std::string temp_key_string;
#define VARIANT_FIELDS_LAMBDA_OBJ_KEY(X, Y) \
    X(MyVariant, variant, MyVariant(0), [](const MyVariant& var, const std::string& key_name) -> bool { \
        return Visit(var                                                                                \
        , [&key_name](const int& i)           -> bool { temp_key_string = key_name; return true; }      \
        , [&key_name](const std::string& str) -> bool { temp_key_string = key_name; return true; }      \
        , [&key_name](std::nullptr_t)         -> bool { temp_key_string = key_name; return true; }      \
        );                                                                                              \
    })
SAPI_REGISTER_CONFIG(VariantLambdaObjKey, VARIANT_FIELDS_LAMBDA_OBJ_KEY)
//-------------------------
TEST(STATIC, variant_lambda_obj_key) {
    using namespace simpleapi;

    Config cfg;
    VariantLambdaObjKey vlok;

    int i           = 100;
    std::string str = "asd";

    std::string result_str_int     = "int: " + std::to_string(i);
    std::string result_str_string  = "string: " + str;
    std::string result_str_nullptr = "std::nullptr_t";

    bool result = false;
    auto run = [&](std::string&& input) {
        cfg.parseJson(input);
        result = vlok.loadConfig(cfg);
        Visit(vlok.variant
            , [&](const int& i)           { my_variant_test_string = result_str_int;     }
            , [&](const std::string& str) { my_variant_test_string = result_str_string;  }
            , [&](std::nullptr_t)         { my_variant_test_string = result_str_nullptr; }
            );
    };

    my_variant_test_string.clear();
    temp_key_string.clear();
    run(std::string("{variant:\"") + str + "\"}");
    EXPECT_EQ(temp_key_string, "variant");
    EXPECT_EQ(my_variant_test_string, result_str_string);
    EXPECT_TRUE(result);

    my_variant_test_string.clear();
    temp_key_string.clear();
    run(std::string("{variant:") + std::to_string(i) + "}");
    EXPECT_EQ(temp_key_string, "variant");
    EXPECT_EQ(my_variant_test_string, result_str_int);
    EXPECT_TRUE(result);

    my_variant_test_string.clear();
    temp_key_string.clear();
    run("{variant: null}");
    EXPECT_EQ(temp_key_string, "variant");
    EXPECT_EQ(my_variant_test_string, result_str_nullptr);
    EXPECT_TRUE(result);
}
