#ifndef COMMENT_H
#define COMMENT_H

#include <string>


enum class CommentType {
    eNotComment,
    eOneLineComment,
    eMultiLineComment
};

struct Comment {
    std::string before;
    std::string after;

    Comment() noexcept  {}
    Comment(const std::string& comment_before, const std::string& comment_after = "") noexcept :
        before(comment_before), after(comment_after) {}

    bool isEmpty()      { return before.empty() && after.empty(); }
    bool operator==(Comment other) const noexcept {
        return before == other.before && after == other.after;
    }
};

#endif // COMMENT_H
