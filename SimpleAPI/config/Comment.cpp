#include "Comment.h"

#include "../utils/Utils.h"
#include "ConfigDefines.h"
#include "ConfigCommon.h"


std::string ToComment(const std::string &comment, const CommentDesign& design,
                      const uint8_t tabulation_level) noexcept {
    using namespace utils;

    std::string result;
    std::string current_string = "";
    std::string prefix = RepeatSymToStr('\t', tabulation_level);
    if(border_symbol != 0) {
        char border = border_symbol;
        if(border == '=' || border == '-')
            border = '|';
        prefix += border + std::string(" ");
    }
    char last_symbol = ' ';
    std::vector<size_t> separators;
    separators.reserve(10);

    for(size_t i = 0; i < comment_string.length(); i++) {
        char ch = comment_string[i];

        //игнор "двойного" пробела
        if(last_symbol == ' ' && ch == ' ')
            continue;

        //если встретили разделитель
        if(utils::CharsInString(ch, __COMMENT_SEPARATOR_SYMBOLS__))
            separators.push_back(current_string.length());

        current_string += ch;
        last_symbol = ch;

        if(ch == '\n') {
            //удалить пробелы в начале и конце строки
            RemoveIllegalSpaces(current_string);

            //вывести если не пустое
            if(!current_string.empty()) {
                result += prefix + current_string;
            }

            current_string = "";
            separators.clear();
        }

        if((column_size != 0
             && utils::GetStringSize(current_string) >= column_size)
            && (utils::CharsInString(ch, __COMMENT_SEPARATOR_SYMBOLS__) || (i == comment_string.length() - 1))
            ) {
            //удалить пробелы в начале и конце строки
            RemoveIllegalSpaces(current_string);

            //вывести если не пустое
            if(!current_string.empty()) {
                result += prefix;

                //если превышен максимальный размер строки
                if(utils::GetStringSize(current_string) > column_size && separators.size() > 0) {
                    uint8_t separate_size;
                    switch(separators.size()) {
                    case 0:     separate_size = 0;              break;
                    case 1:     separate_size = 0;              break;
                    default:    separate_size = separators.size() - 2;
                    }
                    if(current_string.size() > separators[separate_size])
                        separate_size = separators[separate_size] + 1;
                    else
                        separate_size = separators[separate_size];


                    std::string left = utils::SeparateString(current_string, separate_size);
                    RemoveIllegalSpaces(left);

                    if(!utils::CharsInString(current_string.back(), __COMMENT_SEPARATOR_SYMBOLS__))
                        current_string += ' ';
                    result += left + "\n";

                    //снова найти индексы разделителей
                    separators.clear();
                    for(size_t j = 0; j < current_string.length(); j++) {
                        if(utils::CharsInString(current_string[j], __COMMENT_SEPARATOR_SYMBOLS__))
                            separators.push_back(j);
                    }
                } else {
                    result += current_string + "\n";
                    current_string = "";
                    separators.clear();
                }
            }
        }
    }

    if(!current_string.empty()) {
        RemoveIllegalSpaces(current_string);
        result += prefix + current_string;
    }

    std::string ret;
    bool isMulti = result.find('\n') != -1;
    if(isMulti) {
        ret = utils::RepeatSymToStr('\t', tabulation_level) + "/*"
              + (border_symbol != 0 ? utils::RepeatSymToStr(border_symbol, column_size) : "")
              + "\n";
    } else {
        ret += utils::RepeatSymToStr('\t', tabulation_level)
               + "// ";
        RemoveIllegalSpaces(result);
        if(result[0] == border_symbol)
            result.erase(0, 1);
        RemoveIllegalSpaces(result);
    }
    ret += result;
    if(isMulti) {
        ret += (ret.back() == '\n' ? "" : "\n")
               + (border_symbol != 0 ? utils::RepeatSymToStr(border_symbol, column_size) : "")
               + utils::RepeatSymToStr('\t', tabulation_level) + "*/";
    }

    return ret;
}

//NOTE: если символ в списке и в первой строке комментария повторяется минимум 5 раз - это граница, иначе - часть комментария
//NOTE: для всего файла конфига подменяется символ границы только если не задан (первый комментарий с границей)
std::string FromComment(const std::string &comment_string, CommentDesign& design) noexcept {
    std::string ret;

    bool isBorderExists = utils::CharsInString(comment_string[0], __BORDER_SYMBOLS__); //от 5 до 0xFF символов
    if(border_symbol == 0 && isBorderExists) border_symbol = comment_string[0];
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

    if(isBorderExists && column_size == 0 && border_size != 0)
        if(column_size < border_size)
            column_size = border_size;

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
