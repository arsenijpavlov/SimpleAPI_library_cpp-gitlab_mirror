#include "Comment.h"


Comment::Comment() noexcept {
    m_prefix = nullptr;
    m_suffix = nullptr;

    m_comment_design = nullptr;
}

Comment::Comment(const Comment &other) noexcept {
    if(this != &other) {
        set(other.prefix(), other.suffix());
        setDesign(other.commentDesign());
    }
}

Comment::Comment(const Comment &&other) noexcept {
    if(this != &other) {
        set(other.prefix(), other.suffix());
        setDesign(other.commentDesign());
    }
}

Comment::Comment(const std::string &comment_before, const std::string &comment_after) noexcept {
    m_prefix = (comment_before.empty()) ? nullptr : new std::string(comment_before);
    m_suffix = (comment_after.empty()) ? nullptr : new std::string(comment_after);

    m_comment_design = nullptr;
}

Comment::~Comment() noexcept {
    if(m_prefix) delete m_prefix;
    if(m_suffix) delete m_suffix;

    if(m_comment_design) delete m_comment_design;
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

std::string Comment::prefix() const noexcept {
    if(!m_prefix) return "";
    return *m_prefix;
}

std::string& Comment::suffix() noexcept {
    if(!m_suffix) m_suffix = new std::string();
    return *m_suffix;
}

std::string Comment::suffix() const noexcept {
    if(!m_suffix) return "";
    return *m_suffix;
}

void Comment::set(const std::string &prefix_comment, const std::string &suffix_comment) noexcept {
    setPrefix(prefix_comment);
    setSuffix(suffix_comment);
}

void Comment::set(const Comment &other) noexcept {
    setPrefix(other.prefix());
    setSuffix(other.suffix());
}

void Comment::setPrefix(const std::string &comment) noexcept {
    if(m_prefix == nullptr)
        m_prefix = new std::string();
    if(!comment.empty())
        *m_prefix = comment;
}

void Comment::setSuffix(const std::string &comment) noexcept {
    if(m_suffix == nullptr)
        m_suffix = new std::string();
    if(!comment.empty())
        *m_suffix = comment;
}

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
    if(m_comment_design) delete m_comment_design;
}

void Comment::delPrefix() noexcept {
    if(m_prefix) delete m_prefix;
}

void Comment::delSuffix() noexcept {
    if(m_suffix) delete m_suffix;
}

CommentDesign &Comment::commentDesign() noexcept {
    if(!m_comment_design) m_comment_design = new CommentDesign();
    return *m_comment_design;
}

CommentDesign Comment::commentDesign() const noexcept {
    if(!m_comment_design) return {};
    return *m_comment_design;
}

void Comment::setDesign(const CommentDesign &design) noexcept {
    if(!m_comment_design) {
        m_comment_design = new CommentDesign(design);
        return;
    }
    *m_comment_design = design;
}

void Comment::setDesign(const std::array<char,2> oneline_sym, const std::array<char,3> multiline_sym,
                        const uint8_t column_size) noexcept {
    if(!m_comment_design)
        m_comment_design = new CommentDesign();

    m_comment_design->oneline_comment_symbols   = oneline_sym;
    m_comment_design->multiline_comment_symbols = multiline_sym;
    m_comment_design->comment_column_size       = column_size;
}

void Comment::setOnelineDesign(const std::array<char,2> oneline_sym) noexcept {
    if(!m_comment_design)
        m_comment_design = new CommentDesign();

    m_comment_design->oneline_comment_symbols   = oneline_sym;
}

void Comment::setMultilineDesign(const std::array<char,3> multiline_sym, const uint8_t column_size) noexcept {
    if(!m_comment_design)
        m_comment_design = new CommentDesign();

    m_comment_design->multiline_comment_symbols = multiline_sym;
    m_comment_design->comment_column_size       = column_size;
}

void Comment::clearDesign() noexcept {
    if(m_comment_design) delete m_comment_design;
}

void Comment::clearOnelineDesign() noexcept {
    if(!m_comment_design) {
        m_comment_design = new CommentDesign;
        return;
    }
    m_comment_design->oneline_comment_symbols = {'/', '/'};
}

void Comment::clearMultilineDesign() noexcept {
    if(!m_comment_design) {
        m_comment_design = new CommentDesign;
        return;
    }
    m_comment_design->multiline_comment_symbols = {'/', '*'};
    m_comment_design->comment_column_size = DEFAULT_COMMENT_COLUMN_SIZE;
}

//TODO: исправить. Не работает.
bool Comment::operator==(const Comment& other) const noexcept {
    if(this == &other)
        return true;
//    if(m_prefix == nullptr && other.m_prefix == nullptr)
//        return true;
//    if(m_prefix != nullptr && other.m_prefix != nullptr) {
//        if(m_prefix->empty() && other.m_prefix->empty())
//            return true;
//        return *m_prefix == *other.m_prefix;
//    }

    return false;
}

Comment& Comment::operator=(const Comment& other) noexcept {
    if(this != &other) {
        set(other);
        setDesign(other.commentDesign());
    }
    return *this;
}

Comment& Comment::operator=(const Comment&& other) noexcept {
    if(this != &other) {
        set(other);
        setDesign(other.commentDesign());
    }
    return *this;
}

Comment& Comment::operator=(const std::string& prefix_comment) noexcept {
    if(this->m_prefix != &prefix_comment)
        set(prefix_comment);
    return *this;
}

Comment& Comment::operator=(const std::string&& prefix_comment) noexcept {
    if(this->m_prefix != &prefix_comment)
        set(prefix_comment);
    return *this;
}
