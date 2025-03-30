#include "Comment.h"



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
