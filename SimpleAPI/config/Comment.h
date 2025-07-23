#ifndef COMMENT_H
#define COMMENT_H

#include <string>
#include <vector>
#include <bits/shared_ptr.h>


//первый символ в случае многострочного комментария может быть другим
constexpr uint8_t SIZE_comment_multi_line = 6;
constexpr char comment_multi_line[SIZE_comment_multi_line][3] {
    {'/', '*', 0},
    {'/', '#', 0},
    {'<', '#', '>'},
    {'<', '-', '>'},
    {'!', '.', 0},
    {'?', '.', 0}
    //нельзя использовать двойной символ из второй таблицы как границу многострочного комментария
};
constexpr uint8_t SIZE_comment_one_line = 6;
constexpr char comment_one_line[SIZE_comment_one_line][2] {
    {'%', 0},
    {'#', 0},
    {'!', 0},
    {';', 0},
    {'?', 0},
    {'/', '/'}
};

enum class CommentType : uint8_t {
    eNotComment,
    eOneLineComment,
    eMultiLineComment,
    eCommentEnd       //последний требует continue!
};
//#define DEFAULT_COMMENT_COLUMN_SIZE 50

struct CommentDesign {
    // применяется ТОЛЬКО для многострочных комментариев, по умолчанию не используются
    char    opt_multiline_border;
    uint8_t opt_multiline_column_size;

    //следующие два поля нужны только для парсинга
    CommentType         temp_type;
    std::array<char, 3> temp_multiline_schema; //используется только во время парсинга

    // многострочность комментария пользователь задаёт сам
    //  либо самостоятельно ставя '\n'
    //  либо задав максимальный размер
    //NOTE: однострочные комментарии могут иметь два символа в начале
    // (1) {x,0} - комментарий от символа x до конца строки
    // (2) {x,y} - комментарий от последовательностит символов 'x' и 'y' до конца строки
    std::vector<std::array<char, 2>> oneline_comment_variants;

    // (1) {x, 0, 0} - один символ открывает и завершает многострочный комментарий
    // (2) {x, 0, y} - один символ открывает многострочный комментарий, другой - завершает
    // (3) {x, y, 0}
    // (4) {x, y, z}
    // 1) # ... #
    // 2) # ... $
    // 3) /* ... */
    // 4) {* ... *}
    std::vector<std::array<char, 3>> multiline_comment_variants;

    //NOTE: попытка чтения по всем вариантам вектора
    //NOTE: запись строго по первому элементу

    CommentDesign() :
        opt_multiline_border(0),
        opt_multiline_column_size(0),
        temp_type(CommentType::eNotComment),
        temp_multiline_schema{}
    {
        oneline_comment_variants.push_back({'/', '/'});         // {#,0} - второй символ 0 -> один символ уже комментирует
        multiline_comment_variants.push_back({'/', '*', 0});    // 0 - завершающий символ повторяет первый
    }
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
    explicit Comment(const std::string& comment_before, const std::string& comment_after = "")  noexcept;
    ~Comment()                                                                                  noexcept;

public:
    bool isEmpty()                                                                              const noexcept;

    //NOTE: выделит память, если nullptr
    std::string&    prefix()                                                                    noexcept;
    std::string     prefix()                                                                    const noexcept;
    std::string&    suffix()                                                                    noexcept;
    std::string     suffix()                                                                    const noexcept;

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


std::string GetOnelineCommentStr(const CommentDesign& design)                                   noexcept;
std::string GetMultilineCommentStartStr(const CommentDesign& design)                            noexcept;
std::string GetMultilineCommentStopStr(const CommentDesign& design)                             noexcept;

std::string ToComment(const std::string &comment, const CommentDesign& design,
                      const uint8_t tabulation_level = 0)                                       noexcept;
std::string FromComment(const std::string &comment_string, CommentDesign& design)               noexcept;


void RemoveComments(std::string& str, bool& startComment, char& quote,
                    char& start_comment_sym, char& stop_comment_sym);

void CheckComments(const char current_sym, const char next_sym,
                   size_t &iter_counter, CommentDesign& design,
                   std::string &current_comment, const bool external_flag = true);

#endif // COMMENT_H
