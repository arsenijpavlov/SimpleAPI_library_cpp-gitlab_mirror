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
std::string global_output;

std::string func_null_to_void_output = "std::nullptr_t";
void func_null_to_void(std::nullptr_t) {
    global_output = func_null_to_void_output;
}
std::string func_ref_string_to_void_output = "std::string(&)";
void func_string_to_void(std::string& str) {
    global_output = func_ref_string_to_void_output;
}
std::string func_const_string_to_void_output = "std::string(const)";
void func_const_string_to_void(const std::string& str) {
    global_output = func_const_string_to_void_output;
}
std::string func_ref_int_to_void_output = "int(&)";
void func_int_to_void(int& str) {
    global_output = func_ref_int_to_void_output;
}
std::string func_const_int_to_void_output = "int(const)";
void func_const_int_to_void(const int& str) {
    global_output = func_const_int_to_void_output;
}

//-----------------------------------------------

std::string func_null_to_string_output = "std::nullptr_t";
std::string func_null_to_string(std::nullptr_t) {
    global_output = func_null_to_string_output;
    return global_output;
}
std::string func_ref_string_to_string_output = "std::string(&)";
std::string func_string_to_string(std::string& str) {
    global_output = func_ref_string_to_string_output;
    return global_output;
}
std::string func_const_string_to_string_output = "std::string(const)";
std::string func_const_string_to_string(const std::string& str) {
    global_output = func_const_string_to_string_output;
    return global_output;
}
std::string func_ref_int_to_string_output = "int(&)";
std::string func_int_to_string(int& str) {
    global_output = func_ref_int_to_string_output;
    return global_output;
}
std::string func_const_int_to_string_output = "int(const)";
std::string func_const_int_to_string(const int& str) {
    global_output = func_const_int_to_string_output;
    return global_output;
}

//========================================================================================
using namespace simpleapi;

TEST(VISIT, primitive_placement_lambdas) {
    // примеры вызова для типа std::nullptr_t
    Variant<std::string, std::nullptr_t> var_null(nullptr);
    Visit(var_null
         , [](std::nullptr_t)         { global_output = func_null_to_void_output; }
         , [](const std::string& str) { global_output = func_const_string_to_void_output; }
         );
    EXPECT_EQ(global_output, func_null_to_void_output);

    global_output = ""; // обнуляем для отработки следующего теста
    Visit(var_null
         , [](std::nullptr_t)         { global_output = func_null_to_string_output; }
         , [](const std::string& str) { global_output = func_const_string_to_string_output; }
         );
    EXPECT_EQ(global_output, func_null_to_string_output);

    // примеры вызова для типа НЕ std::nullptr_t
    Variant<std::string, std::nullptr_t> var_string("my string");
    Visit(var_string
         , [](std::nullptr_t)         { global_output = func_null_to_void_output; }
         , [](const std::string& str) { global_output = func_const_string_to_void_output; }
         );
    EXPECT_EQ(global_output, func_const_string_to_void_output);

    global_output = ""; // обнуляем для отработки следующего теста
    Visit(var_string
         , [](std::nullptr_t)         { global_output = func_null_to_string_output; }
         , [](const std::string& str) { global_output = func_const_string_to_string_output; }
         );
    EXPECT_EQ(global_output, func_const_string_to_string_output);
}

TEST(VISIT, primitive_lambdas) {
    auto lambda_null   = [](std::nullptr_t)         { global_output = func_null_to_void_output; };
    auto lambda_string = [](const std::string& str) { global_output = func_const_string_to_void_output; };

    // примеры вызова для типа std::nullptr_t
    Variant<std::string, std::nullptr_t> var_null(nullptr);
    Visit(var_null
         , lambda_null
         , lambda_string
         );
    EXPECT_EQ(global_output, func_null_to_void_output);

    global_output = ""; // обнуляем для отработки следующего теста
    Visit(var_null
          , lambda_null
          , lambda_string
         );
    EXPECT_EQ(global_output, func_null_to_string_output);

    // примеры вызова для типа НЕ std::nullptr_t
    Variant<std::string, std::nullptr_t> var_string("my string");
    Visit(var_string
          , lambda_null
          , lambda_string
         );
    EXPECT_EQ(global_output, func_const_string_to_void_output);

    global_output = ""; // обнуляем для отработки следующего теста
    Visit(var_string
          , lambda_null
          , lambda_string
         );
    EXPECT_EQ(global_output, func_const_string_to_string_output);
}

TEST(VISIT, primitive_ptr_lambdas) {
    auto lambda_null   = [](std::nullptr_t)         { global_output = func_null_to_void_output; };
    auto lambda_string = [](const std::string& str) { global_output = func_const_string_to_void_output; };

    // примеры вызова для типа std::nullptr_t
    Variant<std::string, std::nullptr_t> var_null(nullptr);
    Visit(var_null
          , *lambda_null
          , *lambda_string
          );
    EXPECT_EQ(global_output, func_null_to_void_output);

    global_output = ""; // обнуляем для отработки следующего теста
    Visit(var_null
          , *lambda_null
          , *lambda_string
          );
    EXPECT_EQ(global_output, func_null_to_string_output);

    // примеры вызова для типа НЕ std::nullptr_t
    Variant<std::string, std::nullptr_t> var_string("my string");
    Visit(var_string
          , *lambda_null
          , *lambda_string
          );
    EXPECT_EQ(global_output, func_const_string_to_void_output);

    global_output = ""; // обнуляем для отработки следующего теста
    Visit(var_string
          , *lambda_null
          , *lambda_string
          );
    EXPECT_EQ(global_output, func_const_string_to_string_output);
}

TEST(VISIT, primitive_ptr_funcs) {
    // примеры вызова для типа std::nullptr_t
    Variant<std::string, std::nullptr_t> var_null(nullptr);
    Visit(var_null
         , func_const_string_to_void
         , func_null_to_void
         );
    EXPECT_EQ(global_output, func_null_to_void_output);

    global_output = ""; // обнуляем для отработки следующего теста
    Visit(var_null
         , func_const_string_to_string
         , func_null_to_string
         );
    EXPECT_EQ(global_output, func_null_to_string_output);

    // примеры вызова для типа НЕ std::nullptr_t
    Variant<std::string, std::nullptr_t> var_string("my string");
    Visit(var_string
         , func_const_string_to_void
         , func_null_to_void
         );
    EXPECT_EQ(global_output, func_const_string_to_void_output);

    global_output = ""; // обнуляем для отработки следующего теста
    Visit(var_string
         , func_const_string_to_string
         , func_null_to_string
         );
    EXPECT_EQ(global_output, func_const_string_to_string_output);
}
