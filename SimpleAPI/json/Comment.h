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
    std::shared_ptr<std::string> prefix;
    std::shared_ptr<std::string> suffix;

    Comment() noexcept  {}
    Comment(const std::string& comment_before, const std::string& comment_after = "") noexcept;

    bool isEmpty() const noexcept                   { return prefix.empty() && suffix.empty(); }
    void clear() noexcept;
    void clearPrefix() noexcept;
    void clearSuffix() noexcept;

    bool operator==(Comment other) const noexcept   { return prefix == other.prefix && suffix == other.suffix; }
};

#endif // COMMENT_H
