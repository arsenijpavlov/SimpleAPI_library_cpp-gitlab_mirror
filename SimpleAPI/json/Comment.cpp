#include "Comment.h"


Comment::~Comment() noexcept {
    if(m_prefix) delete m_prefix;
    if(m_suffix) delete m_suffix;
}

bool Comment::isEmpty() const noexcept {
    if(m_prefix && !m_prefix->empty()) return false;
    if(m_suffix && !m_suffix->empty()) return false;
    return true;
}

std::string& Comment::prefix() noexcept {
    if(!m_prefix) m_prefix = new std::string();
    return *m_prefix;
}
std::string& Comment::suffix() noexcept {
    if(!m_suffix) m_suffix = new std::string();
    return *m_suffix;
}

void Comment::set(const std::string &prefix_comment, const std::string &suffix_comment) noexcept {
    setPrefix(prefix_comment);
    setSuffix(suffix_comment);
}

void Comment::set(const Comment &other) noexcept {
    set(*other.m_prefix, *other.m_suffix);
}

void Comment::setPrefix(const std::string &comment) noexcept {
    if(m_prefix == nullptr) m_prefix = new std::string();
    *m_prefix = comment;
}

void Comment::setSuffix(const std::string &comment) noexcept {
    if(m_suffix == nullptr) m_suffix = new std::string();
    *m_suffix = comment;
}

//NOTE: можно обнулять я указатель, но зачем? Чаще всего нужна именно перезапись?
void Comment::clear() noexcept {
    m_prefix->clear();
    m_suffix->clear();
}

void Comment::clearPrefix() noexcept {
    m_prefix->clear();
}

void Comment::clearSuffix() noexcept {
    m_suffix->clear();
}

void Comment::del() noexcept {
    if(m_prefix) delete m_prefix;
    if(m_suffix) delete m_suffix;
}

void Comment::delPrefix() noexcept {
    if(m_prefix) delete m_prefix;
}

void Comment::delSuffix() noexcept {
    if(m_suffix) delete m_suffix;
}

bool Comment::operator==(const Comment& other) const noexcept {
    bool b1 = m_prefix == nullptr && other.m_prefix == nullptr;
    bool b2 = m_prefix != nullptr && other.m_prefix != nullptr;
    bool b3 = true;
    bool b4 = true;
    bool b5 = true;
    if(b2) {
        b3 = m_prefix->empty() && other.m_prefix->empty();
        b4 = m_prefix->empty() && other.m_prefix->empty();
        b5 = *m_prefix == *other.m_prefix;
    }

    return b1 && b2 && b3 && b4 && b5;
}

void Comment::operator=(const Comment& other) noexcept {
    set(other);
}
