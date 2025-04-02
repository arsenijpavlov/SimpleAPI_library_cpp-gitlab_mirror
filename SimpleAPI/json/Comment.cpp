#include "Comment.h"



Comment::Comment(const std::string &comment_before, const std::string &comment_after) noexcept :
    prefix(comment_before), suffix(comment_after) {
    prefix. = new std::string(comment_before);
}

void Comment::clear() noexcept {
    prefix.clear();
    suffix.clear();
}

void Comment::clearPrefix() noexcept {
    prefix.clear();
}

void Comment::clearSuffix() noexcept {
    suffix.clear();
}
