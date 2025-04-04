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

    std::string& prefix()                           { return *m_prefix; }
    std::string& suffix()                           { return *m_suffix; }

    void clear() noexcept;
    void clearPrefix() noexcept;
    void clearSuffix() noexcept;

    void del() noexcept;
    void delPrefix() noexcept;
    void delSuffix() noexcept;

    bool operator==(Comment other) const noexcept;
    void operator=(Comment other) noexcept;
};

#endif // COMMENT_H
