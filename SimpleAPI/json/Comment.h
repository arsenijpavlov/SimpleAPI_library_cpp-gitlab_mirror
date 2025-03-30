#ifndef COMMENT_H
#define COMMENT_H

#include <string>


enum class CommentType {
    eNotComment,
    eOneLineComment,
    eMultiLineComment
};

struct Comment {
    std::string prefix;
    std::string suffix;

    Comment() noexcept  {}
    Comment(const std::string& comment_before, const std::string& comment_after = "") noexcept :
        prefix(comment_before), suffix(comment_after) {}

    bool isEmpty() const noexcept                   { return prefix.empty() && suffix.empty(); }
    void clear() noexcept;
    void clearPrefix() noexcept;
    void clearSuffix() noexcept;

    bool operator==(Comment other) const noexcept   { return prefix == other.prefix && suffix == other.suffix; }
};

#endif // COMMENT_H
