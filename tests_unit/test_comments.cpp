#include <SimpleAPI.h>
#include "../SimpleAPI/config/Comment.h"
#include <iostream>
#include <fstream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);

    return RUN_ALL_TESTS();
}

//========================================================================================

TEST(COMMENT, default_wrappers) {
    EXPECT_EQ("//", GetOnelineCommentStr(CommentDesign()));
    EXPECT_EQ("/*", GetMultilineCommentStartStr(CommentDesign()));
    EXPECT_EQ("*/", GetMultilineCommentStopStr(CommentDesign()));

    CommentDesign design;
    design.multiline_comment_variants = {{'/', 0, 0}};
    EXPECT_EQ("/", GetMultilineCommentStartStr(design));
    EXPECT_EQ("/", GetMultilineCommentStopStr(design));
}

TEST(COMMENT, tabulation_level) {
    //oneline
    CommentDesign cd;
    EXPECT_EQ("\t// asd", ToComment("asd", cd, 1));
    EXPECT_EQ("\t\t\t\t// asd", ToComment("asd", cd, 4));

    //multiline
    std::string expect_string_1 = "/***********\n"
                                  "* asd      *\n"
                                  "* aaa      *\n"
                                  "* asd      *\n"
                                  "***********/";
    CommentDesign design_1;
    design_1.opt_multiline_border = '*';
    design_1.opt_multiline_column_size = 8; // пробелы по бокам от строки комментария не учитываются в размере "колонки"
    EXPECT_EQ(expect_string_1, ToComment("asd\n"
                                         "aaa\n"
                                         "asd", design_1, 0));

    std::string expect_string_2 = "/***********\n"
                                  "* asd      *\n"
                                  "* aaa      *\n"
                                  "* asd      *\n"
                                  "***********/";
    CommentDesign design_2;
    design_2.multiline_comment_variants = {{'/', 0, 0}};
    design_2.opt_multiline_border = '*';
    design_2.opt_multiline_column_size = 8;
    EXPECT_EQ(expect_string_2, ToComment("asd\n"
                                         "aaa\n"
                                         "asd", design_2, 0));

    std::string expect_string_3 = "\t\t\t\t/*\n"
                                  "\t\t\t\tasd\n"
                                  "\t\t\t\taaa\n"
                                  "\t\t\t\tasd\n"
                                  "\t\t\t\t*/";
    EXPECT_EQ(expect_string_3, ToComment("asd\n"
                                         "aaa\n"
                                         "asd", cd, 4));
}

TEST(COMMENT, multiline_chopper) {
    CommentDesign cd;
    cd.opt_multiline_border = '#';
    cd.opt_multiline_column_size = 20;

    std::string res = ToComment("very large strings... String very long drive", cd, 0);
    std::string correct_result = "/*######################\n"
                                 "# very large           #\n"
                                 "# strings... String    #\n"
                                 "# very long drive      #\n"
                                 "######################*/";
    EXPECT_EQ(res, correct_result);

//#define NEED_PRINT_TO_FILE
#ifdef NEED_PRINT_TO_FILE
    std::ofstream file("./test_chopper.txt");
    if (!file.is_open())
        FAIL();
    file << res
         << std::endl;
#endif

    cd.opt_multiline_border = 0;
    res = ToComment("very large strings... String very long drive", cd, 0);
    correct_result = "/*\n"
                     "very large\n"
                     "strings... String\n"
                     "very long drive\n"
                     "*/";
    EXPECT_EQ(res, correct_result);
#ifdef NEED_PRINT_TO_FILE
    file << std::endl
         << res
         << std::endl;
#endif

#ifdef NEED_PRINT_TO_FILE
    file.close();
#endif
#undef NEED_PRINT_TO_FILE
}

//TODO: нужно исправить на проверку ожидаемого
TEST(COMMENT, multiline_chopper_reader) {
    CommentDesign cd;

    cd.temp_schema = std::array<char, 3>{'/', '*', 0};
    cd.temp_type = CommentType::eMultiLineComment;
    std::string res = FromComment("/*######################\n"
                                  "# very large strings.. #\n"
                                  "# . String very        #\n"
                                  "# long driv e          #\n"
                                  "######################*/", cd);
    std::string correct_result = "very large strings..\n"
                                 ". String very\n"
                                 "long driv e";
    EXPECT_EQ(res, correct_result);

//#define NEED_PRINT_TO_FILE
#ifdef NEED_PRINT_TO_FILE
    std::ofstream file("./test_chopper_reader.txt");
    if (!file.is_open())
        FAIL();
    file << res
         << std::endl;
#endif

    cd.temp_schema = std::array<char, 3>{'/', '*', 0};
    cd.temp_type = CommentType::eMultiLineComment;
    res = FromComment("/* very large strings..\n"
                      " . String very         \n"
                      " long driv e         */", cd);
    correct_result = "very large strings..\n"
                     ". String very\n"
                     "long driv e";
    EXPECT_EQ(res, correct_result);
#ifdef NEED_PRINT_TO_FILE
    file << std::endl
         << res
         << std::endl;
#endif

    cd.temp_type = CommentType::eOneLineComment;
    res = FromComment("// small comment string", cd);
    correct_result = "small comment string";
    EXPECT_EQ(res, correct_result);
#ifdef NEED_PRINT_TO_FILE
    file << std::endl
         << res
         << std::endl;
#endif

    cd.temp_type = CommentType::eOneLineComment;
    res = FromComment("//small comment string", cd);
    correct_result = "small comment string";
    EXPECT_EQ(res, correct_result);
#ifdef NEED_PRINT_TO_FILE
    file << std::endl
         << res
         << std::endl;
#endif

#ifdef NEED_PRINT_TO_FILE
    file.close();
#endif
#undef NEED_PRINT_TO_FILE
}

TEST(COMMENT, SeparateToColumn_1) {
    CommentDesign cd;
    cd.opt_multiline_column_size = 20;
    std::string input = ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,";

    VString vs = SeparateToColumns(input, cd.opt_multiline_column_size).lines;

    std::string result = VStringToString(vs);
    input = ",,,,,,,,,,,,,,,,,,,,\n"
            ",,,,,,,,,,,,,,,,,,,,\n"
            ",,,,,,,,,,,,,,,,,,,,\n"
            ",,,,,,,,,,,,,,,,,,,,";

    EXPECT_EQ(input, result);
}

TEST(COMMENT, SeparateToColumn_2) {
    //TODO: переносы строк для разных пользовательских вводах
}

TEST(COMMENT, ToComment_FromComment_Oneline) {
    CommentDesign cd;
    std::string input = "1;losdihfg2;slopighsd3;pogihvd4;pfgvibhdfns5;ipnbedf6 7;voihnaern8 som9 word1...";

    std::string str_to = ToComment(input, cd);
    std::string str_from = FromComment(str_to, cd);

    EXPECT_EQ(input, str_from);
}

TEST(COMMENT, ToComment_FromComment_Multiline) {
    CommentDesign cd;
    cd.opt_multiline_column_size = 20;
    std::string input = ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,";
//    std::string input = "1;losdihfg2;slopighsd3;pogihvd4;pfgvibhdfns5;ipnbedf6 7;voihnaern8 som9 word1...";

    std::string str_to = ToComment(input, cd);
    std::string str_from = FromComment(str_to, cd);

           //12345678901234567890
//    input = "1;losdihfg2;\n"
//            "slopighsd3;pogihvd4;\n"
//            "pfgvibhdfns5;\n"
//            "ipnbedf6 7;\n"
//            "voihnaern8 som9 \n"
//            "word1...";
           //12345678901234567890
    input = ",,,,,,,,,,,,,,,,,,,,\n"
            ",,,,,,,,,,,,,,,,,,,,\n"
            ",,,,,,,,,,,,,,,,,,,,\n"
            ",,,,,,,,,,,,,,,,,,,,";

    EXPECT_EQ(input, str_from);
}
