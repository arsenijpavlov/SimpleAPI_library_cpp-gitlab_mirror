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

std::string func_null_output = "std::nullptr_t";
void func_null(std::nullptr_t) {
    global_output = func_null_output;
}

std::string func_ref_string_output = "std::string(&)";
void func_string(std::string& str) {
    global_output = func_ref_string_output;
}
std::string func_const_string_output = "std::string(const)";
void func_const_string(const std::string& str) {
    global_output = func_const_string_output;
}

std::string func_ref_int_output = "int(&)";
void func_int(int& str) {
    global_output = func_ref_int_output;
}
std::string func_const_int_output = "int(const)";
void func_const_int(const int& str) {
    global_output = func_const_int_output;
}

//========================================================================================
using namespace simpleapi;

//TEST(VISIT, primitive_null) {
//    Variant<std::string, std::nullptr_t> var = "asd";
//    Visit(var
//          , func_const_string_output
//          , func_null
//          );
//}

//TEST(VISIT, primitive_placement_lambdas) {

//}

//TEST(VISIT, primitive_lambdas) {

//}

//TEST(VISIT, primitive_ptr_lambdas) {

//}

//TEST(VISIT, primitive_ptr_funcs) {

//}
