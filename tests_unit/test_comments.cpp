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
    using namespace simpleapi;

    EXPECT_EQ("//", GetOnelineCommentStr(CommentDesign()));
    EXPECT_EQ("/*", GetMultilineCommentStartStr(CommentDesign()));
    EXPECT_EQ("*/", GetMultilineCommentStopStr(CommentDesign()));

    CommentDesign design;
    design.multiline_comment_variants = {{'/', 0, 0}};
    EXPECT_EQ("/", GetMultilineCommentStartStr(design));
    EXPECT_EQ("/", GetMultilineCommentStopStr(design));
}

TEST(COMMENT, tabulation_level) {
    using namespace simpleapi;

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

    std::string res = ToComment("asd\n"
                                "aaa\n"
                                "asd", design_1, 0);
    EXPECT_EQ(expect_string_1, res);

    std::string expect_string_2 = "/***********\n"
                                  "* asd      *\n"
                                  "* aaa      *\n"
                                  "* asd      *\n"
                                  "***********/";
    CommentDesign design_2;
    design_2.multiline_comment_variants = {{'/', 0, 0}};
    design_2.opt_multiline_border = '*';
    design_2.opt_multiline_column_size = 8;
    res = ToComment("asd\n"
                    "aaa\n"
                    "asd", design_2, 0);
    EXPECT_EQ(expect_string_2, res);

    std::string expect_string_3 = "\t\t\t\t/*\n"
                                  "\t\t\t\tasd\n"
                                  "\t\t\t\taaa\n"
                                  "\t\t\t\tasd\n"
                                  "\t\t\t\t*/";
    res = ToComment("asd\n"
                    "aaa\n"
                    "asd", cd, 4);
    EXPECT_EQ(expect_string_3, res);
}

TEST(COMMENT, multiline_chopper) {
    using namespace simpleapi;

    CommentDesign cd;
    cd.opt_multiline_border = '#';
    cd.opt_multiline_column_size = 20;

    const SeparatedLines sl = SeparateToColumns("very large strings... String very long drive", cd.opt_multiline_column_size);
    EXPECT_EQ(sl.lines[0], "very large");
    EXPECT_EQ(sl.lines[1], "strings... String");
    EXPECT_EQ(sl.lines[2], "very long drive");

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

TEST(COMMENT, SeparateToColumn_1) {
    using namespace simpleapi;

    CommentDesign cd;
    cd.opt_multiline_column_size = 20;

    // между специальными знаками алгоритм всё равно выставит пробелы
    std::string input = "a, , , , , , , , , ,"
                        "a, , , , , , , , , ,"
                        "a, , , , , , , , , ,"
                        "a, , , , , , , , , ,"; // (без переносов строк)

    VString vs = SeparateToColumns(input, cd.opt_multiline_column_size).lines;

    std::string result = VStringToString(vs);
    input = "a, , , , , , , , , ,\n"
            "a, , , , , , , , , ,\n"
            "a, , , , , , , , , ,\n"
            "a, , , , , , , , , ,";

    EXPECT_EQ(input, result);
}

TEST(COMMENT, SeparateToColumn_2) {
    using namespace simpleapi;

    CommentDesign cd;
    cd.opt_multiline_column_size = 8;

    std::string input = "abc123, ab.cdef@hijk";
    VString vs = SeparateToColumns(input, cd.opt_multiline_column_size).lines;
    std::string result = VStringToString(vs);
    input = "abc123,\n"
            "ab.\n"
            "cdef@\n"
            "hijk";

    EXPECT_EQ(input, result);
}

TEST(COMMENT, SeparateToColumn_3) {
    using namespace simpleapi;

    CommentDesign cd;
    cd.opt_multiline_column_size = 5; // преобразуется по максимальной длине слова

    // NOTE: длинные числа не должны переноситься на другую строку по разделителю в дробной части!
    // пробелы до и после цифр должны быть сохранены в авторском стиле
    // цифры, разделённые точками должны считаться частью одного слова

    std::string input = "1234.5678 "
                        "1234,5678 "
                        "1234, 567 "
                        "192.168.0.1 "
                        "11.1.a.b.c "
                        "192.168:0.1";
    VString vs = SeparateToColumns(input, cd.opt_multiline_column_size).lines;
    std::string result = VStringToString(vs);
    input = "1234.5678\n"
            "1234, 5678\n"
            "1234, 567\n"
            "192.168.0.1\n"
            "11.1.a.b.c\n"
            "192.168:0.1";

    EXPECT_EQ(input, result);
}

// обработка дефисов и тире
TEST(COMMENT, SeparateToColumn_4) {
    using namespace simpleapi;

    CommentDesign cd;
    cd.opt_multiline_column_size = 4; // преобразуется по максимальной длине слова

    // NOTE: длинные числа не должны переноситься на другую строку по разделителю в дробной части!
    // пробелы до и после цифр должны быть сохранены в авторском стиле
    // цифры, разделённые точками должны считаться частью одного слова

    std::string input = "abc-cd "
                        "abc - cd";
    VString vs = SeparateToColumns(input, cd.opt_multiline_column_size).lines;
    std::string result = VStringToString(vs);
    input = "abc-\n"
            "cd\n"
            "abc\n"
            "- cd";

    EXPECT_EQ(input, result);
}

TEST(COMMENT, SeparateToColumn_5) {
    using namespace simpleapi;

    CommentDesign cd;
    cd.opt_multiline_column_size = 20;

    // NOTE: если длина строки не превышает лимит и при этом есть пользовательский перенос -
    //  два переноса строки подряд быть не должно

    std::string input = "json first element\n comment";
    VString vs = SeparateToColumns(input, cd.opt_multiline_column_size).lines;
    std::string result = VStringToString(vs);
    input = "json first element\n"
            "comment";

    EXPECT_EQ(input, result);
}

TEST(COMMENT, ToComment_FromComment_Oneline) {
    using namespace simpleapi;

    CommentDesign cd;
    std::string input = "1;losdihfg2;slopighsd3;pogihvd4;pfgvibhdfns5;ipnbedf6 7;voihnaern8 som9 word1...";

    std::string str_to = ToComment(input, cd);
    std::string str_from = FromComment(str_to, cd);

    EXPECT_EQ(input, str_from);
}

TEST(COMMENT, ToComment_FromComment_Multiline) {
    using namespace simpleapi;

    CommentDesign cd;
    cd.opt_multiline_column_size = 20;
    std::string input = "1;losdihfg2;slopighsd3;pogihvd4;pfgvibhdfns5;ipnbedf6 7;voihnaern8 som9 word1...";

    std::string str_to = ToComment(input, cd);
    std::string str_from = FromComment(str_to, cd);

           //12345678901234567890
    input = "1; losdihfg2;\n"
            "slopighsd3;\n"
            "pogihvd4;\n"
            "pfgvibhdfns5;\n"
            "ipnbedf6 7;\n"
            "voihnaern8 som9\n"
            "word1...";

    EXPECT_EQ(input, str_from);
}

TEST(COMMENT, CheckComments_Multiline) {
    using namespace simpleapi;

    std::string     input = "A/*comment1*/B/*comment2*/C/*comment3*/D";
    CommentDesign   cd;
    std::string     current_comment;
    VString         comments;
    std::string     another; // весь пример без комментариев будет лежать там
    for(size_t i = 0; i < input.size(); i++) {
        char current = input[i];
        char next    = i+1 < input.size() ? input[i+1] : 0;

        CheckComments(current, next, i, cd, current_comment);
        current_comment = current_comment;
        if(cd.temp_type == CommentType::eCommentEnd) {
            comments.push_back(FromComment(current_comment, cd));
            current_comment.clear();
            cd.temp_type = CommentType::eNotComment;
            continue;
        }
        if(cd.temp_type != CommentType::eNotComment)
            continue;
        another += current;
    }

    ASSERT_EQ(comments.size(), 3);
    EXPECT_EQ(comments[0], "comment1");
    EXPECT_EQ(comments[1], "comment2");
    EXPECT_EQ(comments[2], "comment3");
    EXPECT_EQ(another, "ABCD");
}

TEST(COMMENT, CheckComments_Oneline) {
    using namespace simpleapi;

    std::string     input = "A//comment\nB";
    CommentDesign   cd;
    std::string     current_comment;
    VString         comments;
    std::string     another; // весь пример без комментариев будет лежать там
    for(size_t i = 0; i < input.size(); i++) {
        char current = input[i];
        char next    = i+1 < input.size() ? input[i+1] : 0;

        CheckComments(current, next, i, cd, current_comment);
        current_comment = current_comment;
        if(cd.temp_type == CommentType::eCommentEnd) {
            comments.push_back(FromComment(current_comment, cd));
            current_comment.clear();
            cd.temp_type = CommentType::eNotComment;
            continue;
        }
        if(cd.temp_type != CommentType::eNotComment)
            continue;
        another += current;
    }

    ASSERT_EQ(comments.size(), 1);
    EXPECT_EQ(comments[0], "comment");
    //перенос строки после однострочного комментария является и завершением комментария, и частью исходного документа
    EXPECT_EQ(another, "A\nB");
}

TEST(COMMENT, FromComment_Extractor_FromTABs) {
    using namespace simpleapi;

    std::string input = "/*######################\n"
                        "\t# some                 #\n"
                        "\t# words...             #\n"
                        "\t######################*/";
    CommentDesign cd;
    std::string str_from = FromComment(input, cd);
    input = "some\nwords...";
    EXPECT_EQ(input, str_from);
}

TEST(COMMENT, Compare) {
    using namespace simpleapi;

    Comment cmt1;
    Comment cmt2 = cmt1;
    EXPECT_EQ(cmt1, cmt2);
}
