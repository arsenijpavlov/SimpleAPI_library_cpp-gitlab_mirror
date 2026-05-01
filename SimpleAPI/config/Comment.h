#ifndef COMMENT_H
#define COMMENT_H

#include "ConfigCommon.h"
#include <string>
#include <vector>
#include <bits/shared_ptr.h>


namespace simpleapi {

//NOTE: это лишь примеры, не использовать при записи и парсинге
//первый символ в случае многострочного комментария может быть другим
//constexpr uint8_t SIZE_comment_multi_line = 6;
//constexpr char comment_multi_line[SIZE_comment_multi_line][3] {
//    {'/', '*', 0},
//    {'/', '#', 0},
//    {'<', '#', '>'},
//    {'<', '-', '>'},
//    {'!', '.', 0},
//    {'?', '.', 0}
//    //нельзя использовать двойной символ из второй таблицы как границу многострочного комментария
//};
//constexpr uint8_t SIZE_comment_one_line = 6;
//constexpr char comment_one_line[SIZE_comment_one_line][2] {
//    {'/', '/'},
//    {'%', 0},
//    {'#', 0},
//    {'!', 0},
//    {';', 0},
//    {'?', 0}
//};

enum class CommentType : uint8_t {
    eNotComment,
    eOneLineComment,
    eMultiLineComment,
    eCommentEnd       //последний требует continue!
};
std::string ToString(const CommentType& type);
//#define DEFAULT_COMMENT_COLUMN_SIZE 50

//NOTE: пробел в качестве символа начала комментария использовать крайне не советуется, хотя и возможно, в теории
//NOTE: если пользователь не заполнил варианты обрамления комментариев,
//      то должны быть использованы пресеты "comment_multi_line" и "comment_one_line"
class CommentDesign {
public:
    // применяются ТОЛЬКО для многострочных комментариев, по умолчанию не используются
    char    opt_multiline_border;
    bool    opt_multiline_border_at_content_line; //конфликтует с opt_multiline_border(приоритет)
    uint8_t opt_multiline_column_size;

    //следующие два поля нужны только для парсинга
    CommentType         temp_type;
    std::array<char, 3> temp_schema;  //используется во время парсинга и в функции FromComment

    // многострочность комментария пользователь задаёт сам
    //  либо самостоятельно ставя '\n'
    //  либо задав максимальный размер
    //NOTE: однострочные комментарии могут иметь два символа в начале
    // (1) {x,0} - комментарий от символа x до конца строки
    // (2) {x,y} - комментарий от последовательностит символов 'x' и 'y' до конца строки
    std::vector<std::array<char, 2>> oneline_comment_variants;

    // NOTE: Варианты заполнения
    // (1) {x, 0, 0} - один символ открывает и завершает многострочный комментарий
    // (2) {x, 0, y} - один символ открывает многострочный комментарий, другой - завершает
    // (3) {x, y, 0}
    // (4) {x, y, z}
    // 1) # ... #
    // 2) # ... $
    // 3) /* ... */
    // 4) {* ... *}
    std::vector<std::array<char, 3>> multiline_comment_variants;

    //NOTE: чтение по всем вариантам вектора + дефолтный
    //NOTE: запись строго по первому элементу + дефолтный при необходимости

    bool with_comments;
    bool is_in_container;

    CommentDesign() noexcept :
        opt_multiline_border(0),
        opt_multiline_border_at_content_line(false),
        opt_multiline_column_size(0),
        temp_type(CommentType::eNotComment),
        temp_schema{},
        oneline_comment_variants{},
        multiline_comment_variants{},
        with_comments{false},
        is_in_container{false}
    {}

private:
    void copy_from(const CommentDesign& other) noexcept {
        if(this != &other) {
            opt_multiline_border                    = (uint8_t)other.opt_multiline_border;
            opt_multiline_border_at_content_line    = other.opt_multiline_border_at_content_line;
            opt_multiline_column_size               = other.opt_multiline_column_size;
            temp_type                               = other.temp_type;
            temp_schema                             = other.temp_schema;
            oneline_comment_variants                = other.oneline_comment_variants;
            multiline_comment_variants              = other.multiline_comment_variants;
            with_comments                           = other.with_comments;
            is_in_container                         = other.is_in_container;
        }
    }
    void move_from(CommentDesign&& other) noexcept {
        if(this != &other) {
            opt_multiline_border                    = std::move((uint8_t)other.opt_multiline_border);
            opt_multiline_border_at_content_line    = std::move(other.opt_multiline_border_at_content_line);
            opt_multiline_column_size               = std::move(other.opt_multiline_column_size);
            temp_type                               = std::move(other.temp_type);
            temp_schema                             = std::move(other.temp_schema);
            oneline_comment_variants                = std::move(other.oneline_comment_variants);
            multiline_comment_variants              = std::move(other.multiline_comment_variants);
            with_comments                           = std::move(other.with_comments);
            is_in_container                         = std::move(other.is_in_container);
        }
    }

public:
    CommentDesign(const CommentDesign& other) noexcept {
        copy_from(other);
    }

    CommentDesign(CommentDesign&& other) noexcept {
        move_from(std::move(other));
    }

    CommentDesign operator=(const CommentDesign& other) noexcept {
        copy_from(other);
        return *this;
    }

    CommentDesign operator=(CommentDesign&& other) noexcept {
        move_from(std::move(other));
        return *this;
    }

    bool operator==(const CommentDesign& other)             const   noexcept;
    bool operator!=(const CommentDesign& other)             const   noexcept { return !(*this == other); }

    // {#,0} - второй символ 0 -> один символ уже комментирует
    static std::array<char, 2> GetDefaultOnelineCommentVariant()    noexcept { return {'/', '/'}; }

    // 0 - завершающий символ повторяет первый
    static std::array<char, 3> GetDefaultMultilineCommentVariant()  noexcept { return {'/', '*', 0}; }
};

class Comment {
private:
    std::string *m_prefix;
    std::string *m_suffix;

    //NOTE: при выводе в файл (+комментарии) будут учитываться только параметры корневого элемента
    //NOTE: если не назначено, то будет применён стиль C++: //однострочный,  /*многострочный*/, с шириной DEFAULT_COMMENT_COLUMN_SIZE знаков
    CommentDesign *m_comment_design;

public:
    Comment()                                                                                   noexcept;
    Comment(const Comment& other)                                                               noexcept;
    Comment(const Comment&& other)                                                              noexcept;
    // WATRNING: explicit делать не стоит - сломается преобразование из const char*
    Comment(const std::string& comment_before, const std::string& comment_after = "")           noexcept;
    ~Comment()                                                                                  noexcept;

public:
    bool isEmpty()                                                                              const noexcept;

    //NOTE: выделит память, если nullptr
    std::string     prefix()                                                                    const noexcept;
    std::string     suffix()                                                                    const noexcept;

    //NOTE: доступы к внутренним значениям строго по set() и get() запросам
    //std::string&    prefix()                                                                    noexcept;
    //std::string&    suffix()                                                                    noexcept;

    void set(const std::string& prefix_comment, const std::string& suffix_comment = "")         noexcept;
    void set(const Comment& other)                                                              noexcept;
    void setPrefix(const std::string& comment)                                                  noexcept;
    void setSuffix(const std::string& comment)                                                  noexcept;

    void clear()                                                                                noexcept;
    void clearPrefix()                                                                          noexcept;
    void clearSuffix()                                                                          noexcept;

    void del()                                                                                  noexcept;
    void delPrefix()                                                                            noexcept;
    void delSuffix()                                                                            noexcept;
    void delCommentDesign()                                                                     noexcept;

    //Оформление комментариев при выводе в файл -----------------------------------
    //NOTE: выделит память, если nullptr
    CommentDesign&  commentDesign()                                                             noexcept;
    CommentDesign   commentDesign()                                                             const noexcept;

    void setDesign(const CommentDesign &design)                                                 noexcept;
    //он же освободит память
    void clearDesign()                                                                          noexcept;
    //-----------------------------------------------------------------------------

    bool        operator==(const Comment& other)                                                const noexcept;
    bool        operator!=(const Comment& other)                                                const noexcept  { return !(*this == other); }
    Comment&    operator=(const Comment& other)                                                 noexcept;
    Comment&    operator=(const Comment&& other)                                                noexcept;
    Comment&    operator=(const std::string& prefix_comment)                                    noexcept;
    Comment&    operator=(const std::string&& prefix_comment)                                   noexcept;
};

namespace tools {

std::string GetOnelineCommentStr(const CommentDesign &design)                                   noexcept;
std::string GetMultilineCommentStartStr(const CommentDesign &design)                            noexcept;
std::string GetMultilineCommentStopStr(const CommentDesign &design)                             noexcept;

struct SeparatedLines {
    VString lines;
    size_t max_length;
};
SeparatedLines SeparateWithoutColumned(const std::string& input_string)                         noexcept;
SeparatedLines SeparateToColumns(const std::string& input_string, const size_t column_size)     noexcept;
std::string VStringToString(const VString& input_vec, const bool need_quotes = false)           noexcept;
std::string ToComment(const std::string &comment, const CommentDesign &design,
                const int8_t tabulation_level = 0)                                              noexcept;
bool        DefineCommentSymbols(const char first_sym, const char second_sym,
                CommentDesign& cd)                                                              noexcept;
std::string FromComment(std::string comment_string, CommentDesign &design)                      noexcept;


void RemoveComments(std::string& input_string, CommentDesign design = {});

void CheckComments(const char ch_current, const char ch_next,
                   size_t &iter_counter, CommentDesign &design,
                   std::string &current_comment, const bool external_flag = true);

} // namespace tools
} // namespace simpleapi

#endif // COMMENT_H
