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

public:
    Comment() noexcept                              {}
    Comment(const std::string& comment_before, const std::string& comment_after = "") noexcept :
        m_prefix(new std::string(comment_before)),
        m_suffix(new std::string(comment_after))    {}
    ~Comment() noexcept;

    bool isEmpty() const noexcept;

    //NOTE: выделит память, если nullptr
    std::string& prefix() noexcept;
    std::string& suffix() noexcept;

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

    bool operator==(const Comment& other) const noexcept;
    void operator=(const Comment& other) noexcept;
    void operator=(const std::string& prefix_comment) noexcept;
};

#endif // COMMENT_H
