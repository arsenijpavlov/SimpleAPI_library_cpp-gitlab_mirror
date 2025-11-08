#include "Comment.h"

#include "../utils/Utils.h"
#include "../utils/Logger.h"
#include "ConfigDefines.h"
#include "ConfigCommon.h"
#include <algorithm>




Comment::Comment() noexcept
    : m_prefix(nullptr), m_suffix(nullptr), m_comment_design(nullptr)
{}

Comment::Comment(const Comment &other) noexcept
    : m_prefix(nullptr), m_suffix(nullptr), m_comment_design(nullptr)
{
    if(this != &other) {
        set(other.prefix(), other.suffix());
        setDesign(other.commentDesign());
    }
}

Comment::Comment(const Comment &&other) noexcept
    : m_prefix(nullptr), m_suffix(nullptr), m_comment_design(nullptr)
{
    if(this != &other) {
        set(other.prefix(), other.suffix());
        setDesign(other.commentDesign());
    }
}

Comment::Comment(const std::string &comment_before, const std::string &comment_after) noexcept
    : m_prefix(nullptr), m_suffix(nullptr), m_comment_design(nullptr)
{
    m_prefix = (comment_before.empty()) ? nullptr : new std::string(comment_before);
    m_suffix = (comment_after.empty()) ? nullptr : new std::string(comment_after);

    m_comment_design = nullptr;
}

Comment::~Comment() noexcept {
    del();
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
    std::string new_comment = comment;
    RemoveIllegalSpaces(new_comment);

    if(!m_prefix) {
        m_prefix = new std::string(new_comment);
    }
    if(!comment.empty())
        *m_prefix = new_comment;
}

void Comment::setSuffix(const std::string &comment) noexcept {
    std::string new_comment = comment;
    RemoveIllegalSpaces(new_comment);

    if(m_suffix == nullptr)
        m_suffix = new std::string(new_comment);
    if(!comment.empty())
        *m_suffix = new_comment;
}

void Comment::clear() noexcept {
    clearPrefix();
    clearSuffix();
    clearDesign();
}

void Comment::clearPrefix() noexcept {
    if(m_prefix)
        m_prefix->clear();
}

void Comment::clearSuffix() noexcept {
    if(m_suffix)
        m_suffix->clear();
}

void Comment::del() noexcept {
    delPrefix();
    delSuffix();
    delCommentDesign();
}

void Comment::delPrefix() noexcept {
    if(m_prefix) {
        delete m_prefix;
        m_prefix = nullptr;
    }
}

void Comment::delSuffix() noexcept {
    if(m_suffix) {
        delete m_suffix;
        m_suffix = nullptr;
    }
}

void Comment::delCommentDesign() noexcept {
    if(m_comment_design) {
        delete m_comment_design;
        m_comment_design = nullptr;
    }
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
    delCommentDesign();
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

//только для to_string(design)
// @TEST(COMMENT, default_wrappers)
std::string GetOnelineCommentStr(const CommentDesign& design) noexcept {
    if(design.oneline_comment_variants.empty())
        return "";
    if(design.oneline_comment_variants.front()[1] == 0)
        return std::string(design.oneline_comment_variants.front().cbegin(),
                           design.oneline_comment_variants.front().cbegin() + 1);
    else
        return std::string(design.oneline_comment_variants.front().cbegin(),
                           design.oneline_comment_variants.front().cend());
}

//только для to_string(design)
// @TEST(COMMENT, default_wrappers)
std::string GetMultilineCommentStartStr(const CommentDesign& design) noexcept {
    if(design.multiline_comment_variants.empty())
        return "";
    if(design.multiline_comment_variants.front()[1] == 0) {
        return std::string(design.multiline_comment_variants.front().cbegin(),
                           design.multiline_comment_variants.front().cbegin() + 1);
    }
    return std::string(design.multiline_comment_variants.front().cbegin(),
                       design.multiline_comment_variants.front().cbegin() + 2);
}

//только для to_string(design)
// @TEST(COMMENT, default_wrappers)
std::string GetMultilineCommentStopStr(const CommentDesign& design) noexcept {
    if(design.multiline_comment_variants.front()[1] == 0) {
        uint8_t index = design.multiline_comment_variants.front()[2] == 0 ? 0 : 2;
        return std::string((char*)&design.multiline_comment_variants.front().at(index));
    }

    uint8_t index = design.multiline_comment_variants.front()[2] == 0 ? 0 : 2;
    std::stringstream ss;
    ss << (char)design.multiline_comment_variants.front()[1];
    ss << (char)design.multiline_comment_variants.front()[index];

    return ss.str();
}

// @TEST(COMMENT, tabulation_level)
std::string ToComment(const std::string &comment, const CommentDesign& design,
                      const int8_t tabulation_level) noexcept
{
    if(comment.empty()) return "";

    if(comment == "<string> prefix comment") {
        //FIXME: некорректно работает разбивка на строки
        std::cout << "Hello world!" << std::endl;
    }

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
        if(CharInString(ch, __COMMENT_SEPARATOR_SYMBOLS__))
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
                && (CharInString(ch, __COMMENT_SEPARATOR_SYMBOLS__) || (i == current_string.length() - 1)) )
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

                    if(!CharInString(temp.back(), __COMMENT_SEPARATOR_SYMBOLS__))
                        temp += ' ';
                    result_lines.push_back(left);

                    //снова найти индексы разделителей
                    separators.clear();
                    for(size_t j = 0; j < temp.length(); j++) {
                        if(CharInString(temp[j], __COMMENT_SEPARATOR_SYMBOLS__))
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
    case 1: {
        return RepeatSymToStr('\t', tabulation_level)
               + GetOnelineCommentStr(design) + " " + result_lines[0];
    }
    default: {
        size_t max = 0;
        for(std::string& s : result_lines) {
            size_t size = GetStringCharCount(s);
            if(max < size) max = size;
        }
        max = design.opt_multiline_column_size > max ? design.opt_multiline_column_size : max;

        // (м, если BORDER не 0) ============================
        if(design.opt_multiline_border) {
            // выставить знаки вертикальной границы
            for(std::string& s : result_lines) {
                std::stringstream ss;
                if(tabulation_level != -1) {
                    // учесть: B_COMMENTSTRING_B
                    ss << design.opt_multiline_border
                       << " "
                       << logs::columned(s, max +1 /*- 1*/) //-1 засчёт пробела в начале
                       << design.opt_multiline_border;
                } else {
                    ss << " " << logs::columned(s, max +1 /*- 1*/); //-1 засчёт пробела в начале
                }
                s = ss.str();
            }

            // выставить знаки горизонтальных границ
            std::string multiline_comment_symbols = GetMultilineCommentStartStr(design);
            temp = multiline_comment_symbols;
            if(tabulation_level != -1)
                temp += RepeatSymToStr(design.opt_multiline_border,
                                       max + /*пробелы*/2 + (multiline_comment_symbols.size() == 2 ? 0 : 1));
            result_lines.insert(result_lines.cbegin(), temp);

            multiline_comment_symbols = GetMultilineCommentStopStr(design);
            temp = "";
            if(tabulation_level != -1)
                temp += RepeatSymToStr(design.opt_multiline_border,
                                  max + /*пробелы*/2 + (multiline_comment_symbols.size() == 2 ? 0 : 1));
            temp += multiline_comment_symbols;
            result_lines.push_back(temp);
        } else {
            std::transform(result_lines.begin(), result_lines.end(),
                           result_lines.begin(), [&max](const std::string& s)
                           { return logs::columned(s, max); });
            result_lines.front() = GetMultilineCommentStartStr(design) + " " + result_lines.front();
            for(size_t i = 1; i < result_lines.size(); i++)
                result_lines[i] = " " + result_lines[i];
            RemoveIllegalSpaces(result_lines.back());
            result_lines.back() = " " + result_lines.back() + " " + GetMultilineCommentStopStr(design);
        }
        // ==================================================

        break;
    }
    }

    // выставить табуляцию и завершить формирование =====
    std::string ret;
    temp = RepeatSymToStr('\t', tabulation_level);
    std::for_each(result_lines.begin(), result_lines.end(), [&ret, &temp](const std::string& s){
        ret += temp + s + '\n';
    });
    if(ret.back() == '\n') ret.pop_back();
    // ==================================================

    return ret;
}

//NOTE: для всего файла конфига подменяется символ границы только если не задан (первый комментарий с границей)
std::string FromComment(const std::string &comment_string, CommentDesign& design,
                        const int8_t tabulation_level) noexcept
{
    using namespace utils;
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

    //NOTE: в начале каждой строки удаляются табуляции согласно аргументу функции
    for(std::string& s : lines) {
        RemoveFrontTabsIllegalSpaces(s, tabulation_level);
        RemoveEndIllegalSpaces(s); // пробелы в конце ничего не значат
    }

    bool is_border_exists   = false;
    if(lines.size() > 1) {
        if(!lines.front().empty()) {
            //NOTE: если символ в списке и в первой строке комментария повторяется минимум 3 раза - это граница, иначе - часть комментария
            bool found = true;
            for(uint8_t i = 1; i < 3 && i < lines.front().size(); i++) {
                if(lines.front()[i] != lines.front()[0]) {
                    found = false;
                    break;
                }
            }
            is_border_exists = found;
        }
    }
    design.opt_multiline_border = is_border_exists ? lines.front()[0] : 0;

    if(is_border_exists) {
        lines.erase(lines.cbegin());
        lines.pop_back();
        for(std::string& s : lines) {
            if(s.front() == design.opt_multiline_border)
                s.erase(s.begin()); //удалить границу
            if(s.back() == design.opt_multiline_border)
                s.pop_back();       //удалить границу
        }
    } else {
        for(std::string& s : lines)
            RemoveIllegalSpaces(s); //удалить лишние пробелы в начале и конце строки
    }

    std::string ret;
    //TEST
    //    ret = "is_multiline:" + utils::to_string(is_multiline)
    //          + " is_border_exists:" + utils::to_string(is_border_exists)
    //          + (is_border_exists ? std::string(" border_sym:") + design.opt_multiline_border
    //                              : "")
    //          + (design.opt_multiline_column_size ? " column_size:" + std::to_string(design.opt_multiline_column_size)
    //                                              : "")
    //          + "\n---------------------------------------"
    //          + "\n";

    std::for_each(lines.begin(), lines.end(), [&ret](const std::string& s) { ret += s + "\n"; });
    //убрать перенос строки в последней строке
    ret.pop_back();
    return ret;
}

//TODO: исправить на использование CheckComments()
void RemoveComments(std::string &str, bool &startComment,
                    char &quote, char &start_comment_sym,
                    char &stop_comment_sym)
{
    std::string tempString;
    bool isOneLineComment = false;
    bool IsMultiLineComment = startComment;
    for(size_t i = 0; i < str.length(); i++) {
        char previous = (i > 0) ? str[i - 1] : 0;
        char current = str[i];
        char next = (str.length() > i + 1 ? str[i + 1] : 0);

        if(quote == 0) { //если не часть строкового значения
            //поиск комментариев ===========================================================
            if(!isOneLineComment && !IsMultiLineComment) {
                //сперва искать многострочные комментарии!
                for(uint8_t j = 0; j < SIZE_comment_multi_line; j++) {
                    if(current == comment_multi_line[j][0] && next == comment_multi_line[j][1]) {
                        start_comment_sym = current;
                        stop_comment_sym = next;
                        //изменение завершающего символа
                        if(current == '<') start_comment_sym = '>';
                        i++; //проскакиваем следующий символ при парсинге
                        IsMultiLineComment = true;
                        break;
                    }
                }
                if(IsMultiLineComment) continue;
                //поиск однострочных комментариев
                for(uint8_t j = 0; j < SIZE_comment_one_line; j++) {
                    if(current == comment_one_line[j][0]) {
                        if((comment_one_line[j][1] != 0) && (next == comment_one_line[j][1]))
                            i++;
                        isOneLineComment = true;
                        break;
                    }
                }
                if(isOneLineComment) continue;
            }
            //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
            //обработка комментариев
            if(isOneLineComment) {
                //если следующий символ должен обрабатываться другим кодом
                if((current == '\n') || ((str.length() > i + 1) && (str[i + 1] == '\n'))) {
                    isOneLineComment = false;
                    i++;
                }
                continue;
            }
            if(IsMultiLineComment) {
                //нужен следующий символ, если нет - исключение
                if(str.length() <= i + 1)
                    throw std::invalid_argument("invalid length of input JSON string");

                if((current == stop_comment_sym) && (next == start_comment_sym)) {
                    IsMultiLineComment = false;
                    i++; //многострочные комментарии всегда обособляются двумя символами
                }
                continue;
            }
            //==============================================================================
        }

        if(!IsMultiLineComment && !isOneLineComment) {
            //пропускать \"
            if(current == '"' && previous != '\\'){
                if(current == quote)
                    quote = 0;
                else
                    quote = current;
            }
            tempString += current;
        }
    }

    str = tempString;
    startComment = IsMultiLineComment;
}

//предполагается использовать только для парсинга
void CheckComments(const char current_sym, const char next_sym,
                   size_t &iter_counter, CommentDesign& design,
                   std::string &current_comment, const bool external_flag)
{
    if(!external_flag) { //например, значения в кавычках не могут влиять на комментирование
        design.temp_type = CommentType::eNotComment;
        return;
    }

    switch(design.temp_type) {
    case CommentType::eOneLineComment: {
        //считывается строго до переноса строки
        if(next_sym == '\n') {
            design.temp_type = CommentType::eCommentEnd;
            return;
        }

        current_comment += current_sym;
        return;
    }
    case CommentType::eMultiLineComment: {
        bool b2 = design.temp_multiline_schema[1] == 0;
        bool b3 = design.temp_multiline_schema[2] == 0;
        char finish_ch = design.temp_multiline_schema[(b3 ? 0 : 2)];
        if(b2) {
            //второй символ не участвует
            //коммент закрывает последовательность [finish_ch]
            if(current_sym == finish_ch) {
                design.temp_type = CommentType::eCommentEnd;
                return;
            }
        } else {
            //второй символ участвует
            if(next_sym == 0)
                throw std::invalid_argument("invalid length of input string, multiline comment not closed");

            //коммент закрывает последовательность [1][finish_ch]
            if(current_sym == design.temp_multiline_schema[1] && next_sym == finish_ch) {
                iter_counter++;
                design.temp_type = CommentType::eCommentEnd;
                return;
            }
        }

        current_comment += current_sym;
        return;
    }
    case CommentType::eNotComment: {
        //поиск многострочных комментариев
        for(const auto& format : design.multiline_comment_variants) {
            //v1: {X,0} - #...
            //v2: {X,Y} - #!...
            bool b2 = format[1] == 0;
            if(current_sym == format[0] && (b2 || next_sym == format[1])) {
                design.temp_multiline_schema = format;
                design.temp_type = CommentType::eMultiLineComment;
                if(!b2) iter_counter++;
                return;
            }

        }
        //поиск однострочных комментариев
        for(const auto& format : design.oneline_comment_variants) {
            //v1: {X,0} - #...
            //v2: {X,Y} - #!...
            bool b2 = format[1] == 0;
            if(current_sym == format[0] && (b2 || next_sym == format[1])) {
                design.temp_type = CommentType::eOneLineComment;
                if(!b2) iter_counter++;
                return;
            }
        }
        break;
    }
    default: break;
    }

    design.temp_type = CommentType::eNotComment;
    return;
}
