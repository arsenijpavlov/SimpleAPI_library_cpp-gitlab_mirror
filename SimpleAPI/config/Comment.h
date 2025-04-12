#ifndef COMMENT_H
#define COMMENT_H

#include <string>

#include <bits/shared_ptr.h>


//#define DEFAULT_COMMENT_COLUMN_SIZE 50
struct CommentDesign {
    //NOTE: однострочные комментарии могут иметь два символа в начале
    std::array<char,2>  oneline_comment_symbols;
    /* NOTE: многострочные комментарии должны быть заданы в стиле <*> (<*comment_string*>)   *
     * третий символ может быть пустым, тогда для завершения будет использован первый символ */
    std::array<char,3>  multiline_comment_symbols;
    char                opt_multiline_vertical_border;
    uint8_t             opt_multiline_column_size;

    CommentDesign() :
        oneline_comment_symbols{'/', '/'},
        multiline_comment_symbols{'/', '*', 0}, // 0 - завершающий символ повторяет первый
        opt_multiline_vertical_border(0),       // применяется ТОЛЬКО для многострочных комментариев,
                                                //  по умолчанию не используется
        opt_multiline_column_size(0)            // многострочность комментария пользователь задаёт сам
                                                //  либо самостоятельно ставя '\n'
                                                //  либо задав максимальный размер
    {}
};

std::string ToComment(const std::string &comment, const CommentDesign& design,
                      const uint8_t tabulation_level) noexcept;
std::string FromComment(const std::string &comment_string, CommentDesign& design) noexcept;

class Comment {
private:
    std::string *m_prefix;
    std::string *m_suffix;

    //NOTE: при выводе в файл (+комментарии) будут учитываться только параметры корневого элемента
    //NOTE: если не назначено, то будет применён стиль C++: //однострочный,  /*многострочный*/, с шириной DEFAULT_COMMENT_COLUMN_SIZE знаков
    CommentDesign *m_comment_design;

public:
    Comment() noexcept;
    Comment(const Comment& other) noexcept;
    Comment(const Comment&& other) noexcept;
    Comment(const std::string& comment_before, const std::string& comment_after = "") noexcept;
    ~Comment() noexcept;

private:
    void init() {}
public:

    bool isEmpty() const noexcept;

    //NOTE: выделит память, если nullptr
    std::string&    prefix() noexcept;
    std::string     prefix() const noexcept;
    std::string&    suffix() noexcept;
    std::string     suffix() const noexcept;

    void set(const std::string& prefix_comment, const std::string& suffix_comment) noexcept;
    void set(const Comment& other) noexcept;
    void setPrefix(const std::string& comment) noexcept;
    void setSuffix(const std::string& comment) noexcept;

    void clear() noexcept;
    void clearPrefix() noexcept;
    void clearSuffix() noexcept;

    void del() noexcept;
    void delPrefix() noexcept;
    void delSuffix() noexcept;

    //Оформление комментариев при выводе в файл -----------------------------------
    //NOTE: выделит память, если nullptr
    CommentDesign&  commentDesign() noexcept;
    CommentDesign   commentDesign() const noexcept;

    void    setDesign(const CommentDesign &design) noexcept;
    void    clearDesign() noexcept; //он же освободит память
    //-----------------------------------------------------------------------------

    bool        operator==(const Comment& other) const noexcept;
    Comment&    operator=(const Comment& other) noexcept;
    Comment&    operator=(const Comment&& other) noexcept;
    Comment&    operator=(const std::string& prefix_comment) noexcept;
    Comment&    operator=(const std::string&& prefix_comment) noexcept;
};

#endif // COMMENT_H
