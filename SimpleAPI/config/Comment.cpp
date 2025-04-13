#include "Comment.h"

#include "../utils/Utils.h"
#include "ConfigDefines.h"
#include "ConfigCommon.h"

//TODO: нужно вынести функцию в утилиты
#include "../logger/Logger.h"


std::string GetOnelineCommentStr(const CommentDesign& design) noexcept {
    return std::string(design.oneline_comment_symbols.data(),
                       (design.oneline_comment_symbols[1] == 0 ? 1 : 2));
}

std::string GetMultilineCommentStartStr(const CommentDesign& design) noexcept {
    return std::string(design.multiline_comment_symbols.data(), design.multiline_comment_symbols.size());
}

std::string GetMultilineCommentStopStr(const CommentDesign& design) noexcept {
    //TODO:
    return std::to_string((char)design.multiline_comment_symbols[1])
           + (design.multiline_comment_symbols[2] != 0 ? std::to_string((char)design.multiline_comment_symbols[2])
                                                       : std::to_string((char)design.multiline_comment_symbols[0]));
}

//TODO: не ставится знак /* */
std::string ToComment(const std::string &comment, const CommentDesign& design,
                      const uint8_t tabulation_level) noexcept
{
    if(comment.empty()) return "";

    using namespace utils;
    VString lines;
    std::string temp = "";

    //удалить пробелы в начале и конце строки
    std::string current_string = comment;
    RemoveIllegalSpaces(current_string);

    // наметить значение комментария ====================
    for(char c : current_string) {
        if(c == '\n') {
            if(!temp.empty()) {
                lines.push_back(temp);
                temp.clear();
            }
            continue;
        }
        // (м, если COLUMN_SIZE не 0) разделить комментарий на строки
        if(design.opt_multiline_column_size && !temp.empty()
//TODO:            && utils::GetStringCharCount(current_string) >= column_size)
//TODO:            && (utils::CharsInString(ch, __COMMENT_SEPARATOR_SYMBOLS__) || (i == comment_string.length() - 1))
            ) {
            if(GetStringCharCount(temp) >= design.opt_multiline_column_size) {
                lines.push_back(temp);
                temp.clear();
            }
        }
        temp += c;
    }
    // завершающий штрих
    if(!temp.empty()) {
        lines.push_back(temp);
        temp.clear();
    }
    // ==================================================

    switch(lines.size()) {
    case 0: return "";
    case 1: return RepeatSymToStr('\t', tabulation_level) + GetOnelineCommentStr(design) + lines[0];
    default: {
        // (м, если BORDER не 0) ============================
        if(design.opt_multiline_border) {
            // учесть: B_COMMENTSTRING_B
            size_t max = 0;
            for(std::string& s : lines)
                if(max < s.size()) max = s.size();

            // выставить знак вертикальной границы
            for(std::string& s : lines) {
                s = std::to_string(design.opt_multiline_border) + " "
                    + logs::columned(s, max)
                    + " " + std::to_string(design.opt_multiline_border);
            }

            // выставить знаки горизонтальных границ
            temp = RepeatSymToStr(design.opt_multiline_border, max + 4);
            lines.insert(lines.cbegin(), temp);
            lines.push_back(temp);
        } else {

        }
        // ==================================================

        //TODO: /* ... */
        break;
    }
    }

    // выставить табуляцию и завершить формирование =====
    std::string ret;
    temp = RepeatSymToStr('\t', tabulation_level);
    for(std::string& s : lines) {
        ret += temp + s + '\n';
    }
    if(ret.back() == '\n') ret.pop_back();
    // ==================================================

    return ret;
}

//NOTE: если символ в списке и в первой строке комментария повторяется минимум 5 раз - это граница, иначе - часть комментария
//NOTE: для всего файла конфига подменяется символ границы только если не задан (первый комментарий с границей)
std::string FromComment(const std::string &comment_string, CommentDesign& design) noexcept {
    std::string ret;

    bool isBorderExists = utils::CharsInString(comment_string[0], __BORDER_SYMBOLS__); //от 5 до 0xFF символов
    if(design.opt_multiline_border == 0 && isBorderExists)
        design.opt_multiline_border = comment_string[0];

    bool isFirstBorderLine = isBorderExists;
    uint8_t border_size = 0;

    std::string current_string;
    bool isBorderLine = isBorderExists;
    for(char ch : comment_string) {
        if(ch == '\n') {
            if(isFirstBorderLine) isFirstBorderLine = false;
            if(isBorderLine && border_size < 5) {
                isBorderExists = false;
                isBorderLine = false;
            }
            if(isBorderLine) {
                current_string = "";
                continue;
            }

            RemoveIllegalSpaces(current_string);
            if(isBorderExists) {
                if(((comment_string[0] == '-' || comment_string[0] == '=') && current_string[0] == '|')
                    || (current_string[0] == comment_string[0]))
                    current_string.erase(current_string.begin());
            }
            RemoveIllegalSpaces(current_string);

            if(!current_string.empty()) {
                if(!ret.empty())
                    ret += "\n";
                ret += current_string;
            }
            current_string = "";
            isBorderLine = isBorderExists;
        } else {
            //сработает только для первой строки
            if(isFirstBorderLine) {
                if(ch == comment_string[0])
                    border_size++;
                else { //встречен лишний символ, строка - часть комментария
                    isBorderExists = false;
                    border_size = 0;
                }
            }
            //является ли строка границей
            if(isBorderLine && ch != comment_string[0])
                isBorderLine = false;
            current_string += ch;
        }
    }

    RemoveIllegalSpaces(current_string);
    if(!isBorderLine && !current_string.empty()) {
        if(!ret.empty()) ret += '\n';
        ret += current_string;
    }

    if(isBorderExists
        && design.opt_multiline_column_size == 0
        && border_size != 0
        && design.opt_multiline_column_size < border_size
        )
        design.opt_multiline_column_size = border_size;

    return ret;
}

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

void Comment::clearDesign() noexcept {
    if(m_comment_design) delete m_comment_design;
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
