#ifndef COMMENT_H
#define COMMENT_H

#include <string>

#include <bits/shared_ptr.h>


enum class CommentType {
    eNotComment,
    eOneLineComment,
    eMultiLineComment
};

class Comment {
private:
    std::string *m_prefix;
    std::string *m_suffix;

    //NOTE: при выводе в файл (+комментарии) будут учитываться только параметры корневого элемента
    //TODO: как задавать многострочные комментарии?
    uint8_t comment_column_size;
    char oneline_comment_symbol;
    char multiline_comment_symbol;

public:
    Comment() noexcept;
    Comment(const Comment& other) noexcept;
    Comment(const Comment&& other) noexcept;
    Comment(const std::string& comment_before, const std::string& comment_after = "") noexcept;
    ~Comment() noexcept;

private:
    void init();
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

    bool        operator==(const Comment& other) const noexcept;
    Comment&    operator=(const Comment& other) noexcept;
    Comment&    operator=(const Comment&& other) noexcept;
    Comment&    operator=(const std::string& prefix_comment) noexcept;
    Comment&    operator=(const std::string&& prefix_comment) noexcept;
};

#endif // COMMENT_H
