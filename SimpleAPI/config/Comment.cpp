#include "Comment.h"

#include "../utils/Utils.h"
#include "ConfigDefines.h"
#include "ConfigCommon.h"

//TODO: нужно вынести функцию в утилиты
#include "../utils/Logger.h"


// @TEST(COMMENT, default_wrappers)
std::string GetOnelineCommentStr(const CommentDesign& design) noexcept {
    return std::string(design.oneline_comment_symbols.data(),
                       (design.oneline_comment_symbols[1] == 0 ? 1 : 2));
}

// @TEST(COMMENT, default_wrappers)
std::string GetMultilineCommentStartStr(const CommentDesign& design) noexcept {
    return std::string(design.multiline_comment_symbols.data(), 2);
}

// @TEST(COMMENT, default_wrappers)
std::string GetMultilineCommentStopStr(const CommentDesign& design) noexcept {
    std::stringstream ss;
    ss << design.multiline_comment_symbols[1];
    if(design.multiline_comment_symbols[2])
        ss << design.multiline_comment_symbols[2];
    else
        ss << design.multiline_comment_symbols[0];

    return ss.str();
}

std::string ToComment(const std::string &comment, const CommentDesign& design,
                      const uint8_t tabulation_level) noexcept
{
    if(comment.empty()) return "";

    using namespace utils;
    VString             result_lines;
    std::string         temp = "";
    std::vector<size_t> separators;

    //удалить пробелы в начале и конце строки
    std::string current_string = comment;
    RemoveIllegalSpaces(current_string);

    // наметить значение комментария ====================
    for(size_t i = 0; i < current_string.length(); i++) {
        char ch = current_string[i];
        //если встретили разделитель
        if(utils::CharsInString(ch, __COMMENT_SEPARATOR_SYMBOLS__))
            separators.push_back(temp.length());

        if(ch == '\n') {
            //вывести если не пустое
            if(!temp.empty()) {
                //удалить пробелы в начале и конце строки
                RemoveIllegalSpaces(temp);

                result_lines.push_back(temp);
                temp.clear();
                separators.clear();
            }
            continue;
        }

        // (м, если COLUMN_SIZE не 0) разделить комментарий на строки
        if(design.opt_multiline_column_size && !temp.empty()) {
            if(GetStringCharCount(temp) >= design.opt_multiline_column_size
                && (CharsInString(ch, __COMMENT_SEPARATOR_SYMBOLS__) || (i == current_string.length() - 1)) )
            {
                //если превышен максимальный размер строки
                if(GetStringCharCount(temp) > design.opt_multiline_column_size && !separators.empty()) {
                    uint8_t separate_size;
                    switch(separators.size()) {
                    case 0:
                    case 1:
                        separate_size = 0;
                        break;
                    default:
                        separate_size = separators.size() - 2;
                    }
                    if(temp.size() > separators[separate_size])
                        separate_size = separators[separate_size] + 1;
                    else
                        separate_size = separators[separate_size];

                    //найти границу, левая часть которой даст строку внутри колонки
                    std::string left = SeparateString(temp, separate_size);
                    RemoveIllegalSpaces(left);

                    if(!CharsInString(temp.back(), __COMMENT_SEPARATOR_SYMBOLS__))
                        temp += ' ';
                    result_lines.push_back(left);

                    //снова найти индексы разделителей
                    separators.clear();
                    for(size_t j = 0; j < temp.length(); j++) {
                        if(CharsInString(temp[j], __COMMENT_SEPARATOR_SYMBOLS__))
                            separators.push_back(j);
                    }
                } else {
                    //удалить пробелы в начале и конце строки
                    RemoveIllegalSpaces(temp);

                    result_lines.push_back(temp);
                    temp.clear();
                    separators.clear();
                }
            }
        }
        temp += ch;
    }
    // завершающий штрих
    if(!temp.empty()) {
        result_lines.push_back(temp);
        temp.clear();
    }
    // ==================================================

    switch(result_lines.size()) {
    case 0: return "";
    case 1: return RepeatSymToStr('\t', tabulation_level) + GetOnelineCommentStr(design) + " " + result_lines[0];
    default: {
        size_t max = 0;
        for(std::string& s : result_lines) {
            size_t size = GetStringCharCount(s);
            if(max < size) max = size;
        }

        // (м, если BORDER не 0) ============================
        if(design.opt_multiline_border) {
            // выставить знаки вертикальной границы
            for(std::string& s : result_lines) {
                std::stringstream ss;
                // учесть: B_COMMENTSTRING_B
                ss << design.opt_multiline_border << " "
                   << logs::columned(s, max)
                   << " " << design.opt_multiline_border;
                s = ss.str();
            }

            // выставить знаки горизонтальных границ
            temp = RepeatSymToStr(design.opt_multiline_border, max + 4);
            result_lines.insert(result_lines.cbegin(), temp);
            result_lines.front()[0] = GetMultilineCommentStartStr(design)[0];
            result_lines.front()[1] = GetMultilineCommentStartStr(design)[1];

            result_lines.push_back(temp);
            result_lines.back()[result_lines.back().size() - 2] = GetMultilineCommentStopStr(design)[0];
            result_lines.back()[result_lines.back().size() - 1] = GetMultilineCommentStopStr(design)[1];
        } else {
            for(std::string& s : result_lines)
                s = logs::columned(s, max);
            result_lines.front()    = GetMultilineCommentStartStr(design) + " " + result_lines.front();
            for(size_t i = 1; i < result_lines.size(); i++)
                result_lines[i] = " " + result_lines[i];
            result_lines.back()     = result_lines.back() + " " + GetMultilineCommentStopStr(design);
        }
        // ==================================================

        break;
    }
    }

    // выставить табуляцию и завершить формирование =====
    std::string ret;
    temp = RepeatSymToStr('\t', tabulation_level);
    for(std::string& s : result_lines) {
        ret += temp + s + '\n';
    }
    if(ret.back() == '\n') ret.pop_back();
    // ==================================================

    return ret;
}

//NOTE: если символ в списке и в первой строке комментария повторяется минимум 5 раз - это граница, иначе - часть комментария
//NOTE: для всего файла конфига подменяется символ границы только если не задан (первый комментарий с границей)
std::string FromComment(const std::string &comment_string, CommentDesign& design) noexcept {
    VString     lines;
    std::string temp_string = "";
    for(size_t i = 0; i < comment_string.size(); i++) {
        if(comment_string[i] == '\n') {
            lines.push_back(temp_string); //пустые строки - тоже часть комментария
            temp_string.clear();
            continue;
        }
        temp_string += comment_string[i];
    }
    if(!temp_string.empty())
        lines.push_back(temp_string); //пустая строка в конце игнорируется

    if(lines.empty()) return "";

    bool is_multiline   = lines.size() > 1;
    bool temp_bool      = true;
    if(is_multiline) {
        //первые два символа считаются открывающими комментарий (/*)
        if(lines.front().size() > 3) {
            for(size_t i = 3; i < lines.front().size(); i++) {
                if(lines.front()[i] != lines.front()[2]) {
                    temp_bool = false;
                    break;
                }
            }
        } else
            temp_bool = false;
    }
    bool is_border_exists       = is_multiline && temp_bool;
    design.opt_multiline_border = is_border_exists ? lines.front()[2] : 0;

    if(is_multiline) {
        if(lines.front().size() > 2) {
            design.multiline_comment_symbols[0] = lines.front()[0];
            design.multiline_comment_symbols[1] = lines.front()[1];
            lines.front().erase(0, 2);
        }
        if(lines.back().size() > 2) {
            if(design.multiline_comment_symbols[0] == lines.back().back())
                design.multiline_comment_symbols[2] = 0;
            else
                design.multiline_comment_symbols[2] = lines.back().back();

            lines.back().pop_back(); //удалить символы комментария
            lines.back().pop_back(); //удалить символы комментария
        }
    } else {
        design.oneline_comment_symbols[0] = lines.front()[0];
        design.oneline_comment_symbols[1] = lines.front()[1] == ' ' ? 0 : lines.front()[1];
        lines.front().erase(0, 2); //удалить символы комментария
    }

    if(is_border_exists) {
        lines.erase(lines.cbegin());
        lines.pop_back();
        for(std::string& s : lines) {
            RemoveIllegalSpaces(s); //удалить табуляции в начале строк
            if(s.front() == design.opt_multiline_border)
                s.erase(s.begin());         //удалить границу
            if(s.back() == design.opt_multiline_border)
                s.pop_back();       //удалить границу
            RemoveIllegalSpaces(s); //удалить лишние пробелы в начале и конце строки
        }
    } else {
        for(std::string& s : lines)
            RemoveIllegalSpaces(s); //удалить лишние пробелы в начале и конце строки
    }

    std::string ret;

//    TEST
//    ret = "is_multiline:" + utils::to_string(is_multiline)
//           + " is_border_exists:" + utils::to_string(is_border_exists)
//           + (is_border_exists ? std::string(" border_sym:") + design.opt_multiline_border : "")
//           + "\n";

    for(auto& s : lines)
        ret += s + "\n";

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
