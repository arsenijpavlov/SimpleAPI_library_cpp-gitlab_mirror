#include "Comment.h"


Comment::~Comment() noexcept {
    if(prefix) delete prefix;
    if(suffix) delete suffix;
}

bool Comment::isEmpty() const noexcept {
    if(prefix && !prefix->empty()) return false;
    if(suffix && !suffix->empty()) return false;
    return true;
}

//NOTE: можно обнулять я указатель, но зачем? Чаще всего нужна именно перезапись?
void Comment::clear() noexcept {
    prefix->clear();
    suffix->clear();
}

void Comment::clearPrefix() noexcept {
    prefix->clear();
}

void Comment::clearSuffix() noexcept {
    suffix->clear();
}

void Comment::del() noexcept {
    if(prefix) delete prefix;
    if(suffix) delete suffix;
}

void Comment::delPrefix() noexcept {
    if(prefix) delete prefix;
}

void Comment::delSuffix() noexcept {
    if(suffix) delete suffix;
}
