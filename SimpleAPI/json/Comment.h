#ifndef COMMENT_H
#define COMMENT_H

#include <string>

#include <bits/shared_ptr.h>


enum class CommentType {
    eNotComment,
    eOneLineComment,
    eMultiLineComment
};

struct Comment {
    std::string *prefix;
    std::string *suffix;

    Comment() noexcept                              {}
    Comment(const std::string& comment_before, const std::string& comment_after = "") noexcept :
        prefix(new std::string(comment_before)),
        suffix(new std::string(comment_after))      {}
    ~Comment() noexcept;

    bool isEmpty() const noexcept;
    void clear() noexcept;
    void clearPrefix() noexcept;
    void clearSuffix() noexcept;
    void del() noexcept;
    void delPrefix() noexcept;
    void delSuffix() noexcept;

    bool operator==(Comment other) const noexcept   { return prefix == other.prefix && suffix == other.suffix; }
};

#endif // COMMENT_H
