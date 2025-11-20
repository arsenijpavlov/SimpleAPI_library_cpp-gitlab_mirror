#include "Comment.h"

#include "../utils/Utils.h"
#include "../utils/Logger.h"
#include "ConfigDefines.h"
#include <algorithm>



std::string to_string(const CommentType &type) {
    switch(type) {
    case CommentType::eNotComment:          return "not_a_comment";
    case CommentType::eOneLineComment:      return "oneline_comment";
    case CommentType::eMultiLineComment:    return "multiline_comment";
    case CommentType::eCommentEnd:          return "end_of_comment";
    default:                                return "unknown";
    }
}

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

/* Обрезать строку на подстроки с заданной шириной
 * - если хотя бы одна строка неделима и превышеает предел,
 *  то остальные строки должны быть выровнены по новому пределу
 * - многоточие считается частью слова, не переносится на другую строку
*/
//INFO: можно оптимизировать
SeparatedLines SeparateToColumns(const std::string& input_string, const size_t column_size) noexcept {
    VString words;
    std::string temp;

    // разбиение на самостоятельные слова/объекты
    //    uint8_t save_next_symbols = 0;
    bool need_add = false;
    for(size_t i = 0; i < input_string.size(); i++) {
        if(need_add || input_string[i] == '\n') {
            if(!temp.empty()) {
                words.push_back(temp);
                temp.clear();
            }
            need_add = false;
        }

        // пользовательские переносы строк сохраняются
        temp += input_string[i];

        if(utils::CharInString(input_string[i], __COMMENT_SEPARATOR_SYMBOLS__ " ")) {
            need_add = true;

            // два пробела подряд должны быть заменены на один
            if(input_string[i] == ' ') {
                temp.pop_back();

                while(i + 1 < input_string.size() && input_string[i+1] == ' ') {
                    ++i;
                }
            }
            // пропуск многоточия как единого знака
            else if(input_string[i] == '.'
                     && i + 2 < input_string.size()
                     && input_string[i+1] == '.'
                     && input_string[i+2] == '.')
            {
                i += 2;
                temp += "..";
            }
        }
    }
    // завершающее присвоение
    if(!temp.empty()) {
        words.push_back(temp);
    }

    // упоаковка по столбцам (если не влезает, то переработать по минимальной)
    size_t max_len = column_size;
    for(const auto& word : words) {
        if(max_len < word.size())
            max_len = word.size();
    }
    VString res;
    temp.clear();
    size_t current_line_size = 0;
    for(const auto& word : words) {
        if(current_line_size + utils::GetStringCharCount(word, true) + /*space*/1 > max_len
            || word == "\n")
        {
            if(!temp.empty()) {
                res.push_back(temp);
                temp.clear();
            }
            current_line_size = 0;
            if (word == "\n") {
                res.push_back("");
                continue;
            }
        } else {
            temp += temp.empty() ? "" : " ";
            current_line_size += /*space*/1;
        }
        temp += word;
        current_line_size += utils::GetStringCharCount(word, true);
    }
    // завершающее присвоение
    if(!temp.empty()) {
        res.push_back(temp);
    }

    return {res, max_len};
}

std::string VStringToString(const VString& input_vec) noexcept {
    std::string res;

    for(const auto& s : input_vec) {
        res += s + "\n";
    }

    return res;
}

//TODO: при парсинге многострочных комментариев с окантовкой надо учитывать совпадение по закрывающей части
//TODO: должен ли быть пробел между знаками начала/конца м.комментария и окантовкой?

// @TEST(COMMENT, tabulation_level)
std::string ToComment(const std::string &comment, const CommentDesign& design,
                      const int8_t tabulation_level) noexcept
{
    if(comment.empty()) return "";

    using namespace utils;
//    VString             result_lines;
    std::string         temp = "";
    std::vector<size_t> separators;

    //удалить пробелы в начале и конце строки
    std::string current_string = comment;
    RemoveIllegalSpaces(current_string);

    //разделить на строки необходимой длины
    SeparatedLines sl = SeparateToColumns(comment, design.opt_multiline_column_size);
    VString& result_lines = sl.lines;

    switch(result_lines.size()) {
    case 0: return "";
    case 1 /*oneline comments*/: {
        return RepeatSymToStr('\t', tabulation_level)
               + GetOnelineCommentStr(design) + " " + result_lines[0];
    }
    default /*multiline comments*/: {
        if(design.opt_multiline_border) {
            // если нужно обрамление, то строки нужно выровнять по одной длине (дополнить пробелами)
            for(auto& line : result_lines) {
                line = " " + line;
                SetVisibleColumn(line, sl.max_length + /*left space*/1);

                //добавить знаки обрамления
                line = design.opt_multiline_border + line + design.opt_multiline_border;
            }

            //TODO: начало и конец комментария
            //добавить обрамление сверху
            result_lines.insert(result_lines.begin(), RepeatSymToStr(design.opt_multiline_border, sl.max_length + 2));
            //добавить обрамление снизу
            result_lines.push_back(RepeatSymToStr(design.opt_multiline_border, sl.max_length + 2));
        }

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

bool DefineCommentSymbols(const char first_sym, const char second_sym,
                          CommentDesign& cd) noexcept
{
    //поиск многострочных комментариев
    for(const auto& format : cd.multiline_comment_variants) {
        bool b2 = format[1] == 0;
        if(first_sym == format[0] && (b2 || second_sym == format[1])) {
            cd.temp_schema = format;
            cd.temp_type = CommentType::eMultiLineComment;
            return true;
        }

    }
    //поиск однострочных комментариев
    for(const auto& format : cd.oneline_comment_variants) {
        bool b2 = format[1] == 0;
        if(first_sym == format[0] && (b2 || second_sym == format[1])) {
            cd.temp_schema = std::array<char, 3>{format[0], format[1], 0};
            cd.temp_type = CommentType::eOneLineComment;
            return true;
        }
    }
    return false;
};

//NOTE: для всего файла конфига подменяется символ границы только если не задан (первый комментарий с границей)
std::string FromComment(std::string comment_string, CommentDesign& design,
                        const int8_t tabulation_level) noexcept
{
    using namespace utils;

    VString     lines;
    // определить синтаксические знаки комментария и удалить из входной строки
    {
        char first_ch   = comment_string.empty() ? 0 : comment_string[0];
        char second_ch  = comment_string.size() > 2 ? comment_string[1] : 0;
        DefineCommentSymbols(first_ch, second_ch, design);

        RemoveIllegalSpaces(comment_string); //удалить незначащие пробелы
        //удалить начальные символы
        comment_string.erase(0, design.temp_schema[1] == 0 ? 1 : 2);
        //удалить конечные символы в зависимости от типа комментария - однострочный/многострочный
        if(design.temp_type == CommentType::eOneLineComment) {
            if(comment_string.back() == '\n') comment_string.pop_back();
        } else {
            comment_string.pop_back();
            if(design.temp_schema[1] != 0)
                comment_string.pop_back(); // второй замыкающий символ для М
        }
    }

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

    std::for_each(lines.begin(), lines.end(), [&ret, &lines](const std::string& s) { ret += s + (s != lines.back() ? "\n" : ""); });
    //убрать перенос строки в последней строке
//    ret.pop_back();
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
        bool b2 = design.temp_schema[1] == 0;
        bool b3 = design.temp_schema[2] == 0;
        char finish_ch = design.temp_schema[(b3 ? 0 : 2)];
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
            if(current_sym == design.temp_schema[1] && next_sym == finish_ch) {
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
                design.temp_schema = format;
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
