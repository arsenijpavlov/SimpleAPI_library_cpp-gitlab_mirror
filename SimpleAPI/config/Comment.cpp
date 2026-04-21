#include "Comment.h"

#include "../utils/Utils.h"
#include "../utils/Logger.h" //для cout (debug)
#include "ConfigDefines.h"
#include <algorithm>


namespace simpleapi {

std::string ToString(const CommentType &type) {
    switch(type) {
    case CommentType::eNotComment:          return "not_a_comment";
    case CommentType::eOneLineComment:      return "oneline_comment";
    case CommentType::eMultiLineComment:    return "multiline_comment";
    case CommentType::eCommentEnd:          return "end_of_comment";
    default:                                return "unknown";
    }
}

bool CommentDesign::operator==(const CommentDesign &other) const noexcept {
    if(this != &other) {
        bool ret = opt_multiline_border                 == other.opt_multiline_border;
        if(ret && opt_multiline_border == 0)
            ret = opt_multiline_border_at_content_line  == other.opt_multiline_border_at_content_line;

        //NOTE: column_size сравнивать не имеет смысла; описано в заметках - кратко: запись N, чтение N-K
        //if(ret) ret = opt_multiline_column_size         == other.opt_multiline_column_size;

        if(ret) ret = oneline_comment_variants          == other.oneline_comment_variants;
        if(ret) ret = multiline_comment_variants        == other.multiline_comment_variants;
        if(ret) ret = with_comments                     == other.with_comments;
        return ret;
    }

    return true;
}

Comment::Comment() noexcept
    : m_prefix(nullptr), m_suffix(nullptr), m_comment_design(nullptr)
{}

Comment::Comment(const Comment &other) noexcept
    : m_prefix(nullptr), m_suffix(nullptr), m_comment_design(nullptr)
{
    if(this != &other) {
        if(!other.m_prefix) delPrefix();
        else                setPrefix(other.prefix());

        if(!other.m_suffix) delSuffix();
        else                setSuffix(other.suffix());

        if(!other.m_comment_design) delCommentDesign();
        else                setDesign(other.commentDesign());
    }
}

Comment::Comment(const Comment &&other) noexcept
    : m_prefix(nullptr), m_suffix(nullptr), m_comment_design(nullptr)
{
    if(this != &other) {
        if(!other.m_prefix) delPrefix();
        else                setPrefix(other.prefix());

        if(!other.m_suffix) delSuffix();
        else                setSuffix(other.suffix());

        if(!other.m_comment_design) delCommentDesign();
        else                setDesign(other.commentDesign());
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

std::string Comment::prefix() const noexcept {
    if(!m_prefix) return "";
    return *m_prefix;
}

std::string Comment::suffix() const noexcept {
    if(!m_suffix) return "";
    return *m_suffix;
}

//std::string& Comment::prefix() noexcept {
//    if(!m_prefix) m_prefix = new std::string();
//    return *m_prefix;
//}

//std::string& Comment::suffix() noexcept {
//    if(!m_suffix) m_suffix = new std::string();
//    return *m_suffix;
//}

void Comment::set(const std::string &prefix_comment, const std::string &suffix_comment) noexcept {
    setPrefix(prefix_comment);
    setSuffix(suffix_comment);
}

void Comment::set(const Comment &other) noexcept {
    if(!other.m_prefix) delPrefix();
    else                setPrefix(other.prefix());

    if(!other.m_suffix) delSuffix();
    else                setSuffix(other.suffix());
}

void Comment::setPrefix(const std::string &comment) noexcept {
    if(comment.empty() && m_prefix) {
        delPrefix();
        return;
    }

    std::string new_comment = comment;
    RemoveIllegalSpaces(new_comment);

    if(!m_prefix)
        m_prefix = new std::string(new_comment);
    else
        *m_prefix = new_comment;
}

void Comment::setSuffix(const std::string &comment) noexcept {
    if(comment.empty() && m_suffix) {
        delSuffix();
        return;
    }

    std::string new_comment = comment;
    RemoveIllegalSpaces(new_comment);

    if(m_suffix == nullptr)
        m_suffix = new std::string(new_comment);
    else
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
    } else
        *m_comment_design = design;
}

void Comment::clearDesign() noexcept {
    delCommentDesign();
}

//NOTE: строки будут проверены как нормализованные (без пробелов)
//NOTE: ширина колонки не будет проверена
bool Comment::operator==(const Comment& other) const noexcept {
    if(this == &other)
        return true;

    if(m_prefix == other.m_prefix)
        return true; /*один и тот же адрес, хотя и невозможно для разных Comment; оба nullptr*/
    else { /*адреса неодинаковые*/
        if(m_prefix == nullptr) {
            if(other.m_prefix != nullptr)
                return false;
        } else { /*m_prefix != nullptr*/
            if(other.m_prefix == nullptr)
                return false;
            else if(!utils::IsNormalizeEqual(*m_prefix, *other.m_prefix))
                return false;
        }
    }

    if(m_suffix == other.m_suffix)
        return true; /*один и тот же адрес, хотя и невозможно для разных Comment; оба nullptr*/
    else { /*адреса неодинаковые*/
        if(m_suffix == nullptr) {
            if(other.m_suffix != nullptr)
                return false;
        } else { /*m_suffix != nullptr*/
            if(other.m_suffix == nullptr)
                return false;
            else if(!utils::IsNormalizeEqual(*m_suffix, *other.m_suffix))
                return false;
        }
    }

    if(m_comment_design == other.m_comment_design)
        return true; /*один и тот же адрес, хотя и невозможно для разных Comment; оба nullptr*/
    else { /*адреса неодинаковые*/
        if(m_comment_design == nullptr) {
            if(other.m_comment_design != nullptr)
                return false;
        } else { /*m_comment_design != nullptr*/
            if(other.m_comment_design == nullptr)
                return false;
            else if(*m_comment_design != *other.m_comment_design)
                return false;
        }
    }

    return true;
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

namespace tools {

//только для to_string(design)
// @TEST(COMMENT, default_wrappers)
std::string GetOnelineCommentStr(const CommentDesign &design) noexcept {
    const auto variant = design.oneline_comment_variants.empty() ? CommentDesign::GetDefaultOnelineCommentVariant()
                                                                 : design.oneline_comment_variants[0];
    return std::string(&variant[0], variant[1] == 0 ? 1 : 2);
}

//только для to_string(design)
// @TEST(COMMENT, default_wrappers)
std::string GetMultilineCommentStartStr(const CommentDesign &design) noexcept {
    const auto variant = design.multiline_comment_variants.empty() ? CommentDesign::GetDefaultMultilineCommentVariant()
                                                                   : design.multiline_comment_variants[0];

    return std::string(&variant[0], variant[1] == 0 ? 1 : 2);
}

//только для to_string(design)
// @TEST(COMMENT, default_wrappers)
std::string GetMultilineCommentStopStr(const CommentDesign &design) noexcept {
    const auto variant = design.multiline_comment_variants.empty() ? CommentDesign::GetDefaultMultilineCommentVariant()
                                                                   : design.multiline_comment_variants[0];

    if(variant[1] == 0)
        return std::string(&variant[variant[2] == 0 ? 0 : 2], 1);

    uint8_t index = variant[2] == 0 ? 0 : 2;
    std::stringstream ss;
    ss << (char)variant[1];
    ss << (char)variant[index];

    return ss.str();
}

// Обрежет входную строку на список строк. Учитываются только пользовательские переносы строк
SeparatedLines SeparateWithoutColumned(const std::string& input_string) noexcept {
    SeparatedLines sl;
    std::string temp;
    for(char c : input_string) {
        if(c == '\n') {
            sl.lines.push_back(temp);
            temp.clear();
            continue;
        }
        temp.push_back(c);
    }
    if(!temp.empty())
        sl.lines.push_back(temp);

    for(auto& s : sl.lines)
        RemoveIllegalSpaces(s);

    //определить максимальную длину строки (будет иметь влияние только при border!=0)
    sl.max_length = 0;
    for(auto& s : sl.lines)
        if(sl.max_length < s.size())
            sl.max_length = s.size();

    return sl;
}

/* Обрезать строку на подстроки с заданной шириной
 * - если хотя бы одна строка неделима и превышеает предел,
 *   то остальные строки должны быть выровнены по новому пределу
 * - многоточие считается частью слова, не переносится на другую строку
 * - пользовательские переносы строк должны быть сохранены
*/
//INFO: можно оптимизировать
SeparatedLines SeparateToColumns(const std::string& input_string, const size_t column_size) noexcept {
    VString words;
    std::string temp;

    const size_t input_visible_len = utils::GetStringCharCount(input_string, true);
    bool is_oneline = input_string.find('\n') == std::string::npos;

    if(is_oneline && (column_size == 0 || input_visible_len <= column_size))
    {
        return SeparatedLines{{input_string}, input_visible_len};
    }

    // разбиение на самостоятельные слова/объекты
    bool need_add = false;
    for(size_t i = 0; i < input_string.size(); i++) {
        if(need_add) {
            if(!temp.empty()) {
                // пробелы выставляются только в этом блоке
                if(utils::CharInString(temp.back(), __COMMENT_SEPARATOR_SYMBOLS__))
                    temp += " ";
                // если последний знак тире, и предпоследний знак пробел, то пробел в конце нужен
                if(temp.size() > 2 && temp[temp.size()-1] == '-' && temp[temp.size()-2] == ' ')
                    temp += " ";

                words.push_back(temp);
                temp.clear();
            }
            need_add = false;
        }

        // пользовательские переносы строк сохраняются
        temp += input_string[i];
        if(temp == " ") temp.clear();

        // работа с числами и точками; значения вида "1.2.a" тоже попадают в эту категорию
        if(utils::CharInString(input_string[i], "0123456789")) {
            // добавляем числа, буквы, точки, дефисы и двоеточия пока не встретится любой другой
            while(i + 1 < input_string.size()
                   && !utils::CharInString(input_string[i+1], __SPACES__ __COMMENT_SEPARATOR_SYMBOLS_FOR_NUMBER__))
            {
                ++i;
                temp += input_string[i];
            }
        }

        if(utils::CharInString(input_string[i], __COMMENT_SEPARATOR_SYMBOLS__
                                                __COMMENT_OTHERS_SPEC_SYMBOLS__
                                                __SPACES__))
        {
            need_add = true;

            // если следующий символ пробел - сохранить его в этом же слове
            if(i + 1 < input_string.size() && utils::CharInString(input_string[i+1], " \t")) {
                temp += input_string[i+1];
                i++;
            }
            // два пробела подряд должны быть заменены на один (табуляции не учитываются)
            if(input_string[i] == ' ' && temp.back() == ' ') {
                while(i + 1 < input_string.size() && input_string[i+1] == ' ') {
                    ++i;
                }
            }

            // пропуск многоточий (... !!! ??? ?!) как единого знака
            if(input_string[i] == '.'
                     && i + 2 < input_string.size()
                     && input_string[i+1] == '.'
                     && input_string[i+2] == '.')
            {
                i += 2;
                temp += "..";
            }
            else if(input_string[i] == '!'
                     && i + 2 < input_string.size()
                     && input_string[i+1] == '!'
                     && input_string[i+2] == '!')
            {
                i += 2;
                temp += "!!";
            }
            else if(input_string[i] == '?'
                     && i + 2 < input_string.size()
                     && input_string[i+1] == '?'
                     && input_string[i+2] == '?')
            {
                i += 2;
                temp += "??";
            }
            else if(input_string[i] == '?'
                       && i + 1 < input_string.size()
                       && input_string[i+1] == '!')
            {
                ++i;
                temp += "!";
            }
        }
    }
    // завершающее присвоение
    if(!temp.empty()) {
        words.push_back(temp);
    }

    // упаковка по столбцам (если не влезает, то переработать по минимальной)
    size_t max_len = column_size;
    for(const auto& word : words) {
        if(max_len < utils::GetStringCharCount(word, true))
            max_len = utils::GetStringCharCount(word, true);
    }
    VString res;
    temp.clear();
    size_t current_line_size = 0;
    for(/*const*/ auto& word : words) {
        const size_t append_word_size = word.empty() ? 0
                                                     : utils::GetStringCharCount(word, true);
        // пробел в конце не должен учитываться в длине добавляемого слова
        bool space_at_back_of_word = word.back() == ' ';

        // отсечь строку, если добавление следующего слова превысит максимальную длину
        //  первая строка списка не может быть пустой!
        if( (!res.empty() || !temp.empty())
            && ( (!temp.empty() && temp.back() == '\n')
                || current_line_size + (append_word_size - space_at_back_of_word) > max_len
                || word == "\n") )
        {
            RemoveIllegalSpaces(temp); // пробел в конце здесь уже ничего не значит
            if(temp.back() == '\n')
                temp.pop_back();
            res.push_back(temp);
            temp.clear();
            current_line_size = 0;
        }

        if(word != "\n") {
            // если пробел был нужен (пользователь указал в тексте комментария),
            // то он уже есть вслед за словом
            temp += word;
        }
        current_line_size += append_word_size;
    }
    // завершающее присвоение
    if(!temp.empty()) {
        res.push_back(temp);
    }

    // по завершении, все лишние пробелы в конце каждой строки удаляются
    for(auto &s : res)
        RemoveIllegalSpaces(s);

    return {res, max_len};
}

// Вспомогательная функция для вывода массива строк в лог
std::string VStringToString(const VString& input_vec, const bool need_quotes) noexcept {
    if(input_vec.empty())
        return "";

    std::string res;

    for(const auto& s : input_vec) {
        if(need_quotes)
            res += "\"";
        res += s;
        if(need_quotes)
            res += "\"";
        res += "\n";
    }
    //последний перенос лишний
    if(!res.empty())
        res.pop_back();

    return res;
}

//TODO (проверить): при парсинге многострочных комментариев с окантовкой надо учитывать совпадение по закрывающей части
/*TODO (проверить): должен ли быть пробел между знаками начала/конца м.комментария и окантовкой?
 *      - при записи пробел не ставится
 *      - при чтении всегда пытается считать два символа начала комментария (в зависимости от заполненности вариантов)
 *      - если второй символ начала комментария пробел - многострочный комментарий начинается с одного символа
*/

// @TEST(COMMENT, tabulation_level)
std::string ToComment(const std::string &comment, const CommentDesign &design,
                      const int8_t tabulation_level) noexcept
{
    using namespace utils;

    if(comment.empty()) return "";

    std::string         temp = "";
    std::vector<size_t> separators;

    //удалить пробелы в начале и конце строки
    std::string current_string = comment;
    RemoveIllegalSpaces(current_string);

    //FIXME: если не нужна рамка и все строки меньше указанной ширины - считать по максимальной длине
    SeparatedLines sl = design.opt_multiline_column_size == 0 ? SeparateWithoutColumned(current_string)
                                                              : SeparateToColumns(current_string, design.opt_multiline_column_size);
    VString& result_lines = sl.lines;
//    std::cout << "sl.max_length: " << sl.max_length << std::endl;
//    std::cout << "sl.lines:" << std::endl << VStringToString(sl.lines, true) << std::endl;

    switch(result_lines.size()) {
    case 0: return "";
    case 1 /*oneline comments*/: {
        return RepeatSymToStr('\t', tabulation_level) + GetOnelineCommentStr(design) + " " + result_lines[0];
    }
    default /*multiline comments*/: {
        // если нужно обрамление, то строки нужно выровнять по одной длине (дополнить пробелами)
        if(design.opt_multiline_border != 0 || design.opt_multiline_border_at_content_line)
            for(auto& line : result_lines) {
                SetVisibleColumn(line, sl.max_length);
            }

        if(design.opt_multiline_border != 0) {
            for(auto& line : result_lines) {
                line = " " + line;
                SetVisibleColumn(line, sl.max_length + /*left space*/2);

                //добавить знаки обрамления
                line = design.opt_multiline_border + line + design.opt_multiline_border;
            }
        }

        //обозначить комментарии
        std::string start_comment_symbols = GetMultilineCommentStartStr(design);
        std::string stop_comment_symbols = GetMultilineCommentStopStr(design);
        //(взаимоисключающие параметры)
        if(design.opt_multiline_border != 0 || !design.opt_multiline_border_at_content_line)
        {
            result_lines.insert(result_lines.begin(), start_comment_symbols);
            result_lines.push_back(stop_comment_symbols);

            //дополнить рамку при необходимости
            if(design.opt_multiline_border != 0) {
                // заполняется в стиле (начало комментария "/*"):       /*#######
                //  альтернативный вариант (начало комментария "/"):    /########

                // длина актуальна для обеих строк
                size_t needed_spaces = sl.max_length + 4 - result_lines.front().size();
                result_lines.front() += RepeatSymToStr(design.opt_multiline_border, needed_spaces);
                result_lines.back() = RepeatSymToStr(design.opt_multiline_border, needed_spaces) + result_lines.back();
            }
        } else if(design.opt_multiline_border_at_content_line) {
            // в первую строку добавить знаки начала комментария
            // во все остальные строки дополнить пробелами по длине начала комментария "/*" -> 2 пробела
            bool is_first = true;
            for(auto& line : result_lines) {
                if(is_first) {
                    line = start_comment_symbols + (start_comment_symbols.back() == ' ' ? "" : " ") + line;
                    is_first = false;
                } else {
                    line = RepeatSymToStr(' ', start_comment_symbols.size() + (start_comment_symbols.back() == ' ' ? 0 : 1))
                           + line;
                }
            }
            // в последнюю строку добавить знаки конца комментария
            result_lines.back() += (stop_comment_symbols.front() == ' ' ? "" : " ") + stop_comment_symbols;
        }

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

// Определит тип комментария на основе начальных двух символов
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

    //поиск многострочных комментариев (знаки по умолчанию)
    auto default_m = CommentDesign::GetDefaultMultilineCommentVariant();
    if (first_sym == default_m[0] && second_sym == default_m[1]) {
        cd.temp_schema = default_m;
        cd.temp_type = CommentType::eMultiLineComment;
        return true;
    }

    //поиск однострочных комментариев (знаки по умолчанию)
    auto default_o = CommentDesign::GetDefaultOnelineCommentVariant();
    if (first_sym == default_o[0] && second_sym == default_o[1]) {
        cd.temp_schema = std::array<char, 3>{default_o[0], default_o[1], 0};
        cd.temp_type = CommentType::eOneLineComment;
        return true;
    }

    return false;
};

// NOTE: для всего файла конфига подменяется символ границы только если не задан (первый комментарий с границей)
// Предполагается, что пользователь сам укажет символы для поиска комментариев, при парсинге будет обновлён CommentDesign
// FIXME: строку стоит передать через && ?
std::string FromComment(std::string comment_string, CommentDesign &design) noexcept
{
    using namespace utils;

    if(comment_string.empty())
        return "";

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

            //определить флаг opt_multiline_border_at_content_line
            if(comment_string[0] != '\n')
                //потом если встретится opt_multiline_border, то флаг перезапишет (приоритетность)
                design.opt_multiline_border_at_content_line = true; //включится, если есть хотя бы один такой комментарий

            // удалить лишние переносы строк в начале и конце многострочного комментария
            while(!comment_string.empty() && comment_string.front() == '\n')
                comment_string.erase(0, 1);
            while(!comment_string.empty() && comment_string.back() == '\n')
                comment_string.pop_back();
        }
    }

    VString lines;
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
        RemoveIllegalSpaces(s);
    }

    bool is_border_exists = false;
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
    // NOTE: знак границы определяется по первому комментарию с границей
    if(is_border_exists && design.opt_multiline_border == 0)
        design.opt_multiline_border = lines.front()[0];

    if(is_border_exists) {
        lines.erase(lines.cbegin());
        lines.pop_back();
        for(std::string& s : lines) {
            if(s.front() == design.opt_multiline_border)
                s.erase(s.begin()); //удалить границу
            if(s.back() == design.opt_multiline_border)
                s.pop_back();       //удалить границу
        }

        for(std::string& s : lines) {
            // если вся строка состоит из одного символа-рамки, то обнулить строку
            if(design.opt_multiline_border == utils::LineOfOneSymbol(s))
                s.clear();
        }
    }

    for(std::string& s : lines) {
        RemoveIllegalSpaces(s); //удалить лишние пробелы в начале и конце строки
        // определить максимальную ширину колонки комментариев
        if(design.opt_multiline_column_size < s.size())
            design.opt_multiline_column_size = s.size();
    }

    //если пустых строк подряд больше одной, то дубликаты надо удалить
    {
        size_t counter = 0;
        for(auto it = lines.begin(); it != lines.end(); it++) {
            if(it->empty())
                counter++;
            else
                counter = 0;

            if(counter > 1) {
                counter--;
                it = lines.erase(it);

                //обязательная проверка при использовании it=erase(it)
                if(it == lines.end()) break;
            }
        }
    }
    //удалить пустые строки в начале и конце комментария
    {
        if(!lines.empty() && lines.front().empty())
            lines.erase(lines.begin());
        if(!lines.empty() && lines.back().empty())
            lines.erase(lines.end());
    }

    std::string ret;

    std::for_each(lines.begin(), lines.end(), [&ret, &lines](const std::string& s) {
        ret += s + (!s.empty() && s != lines.back() ? "\n" : "");
    });

    return ret;
}

void RemoveComments(std::string &input_string, CommentDesign design)
{
    std::string temp;
    bool is_quotes = false;
    std::string comment_string;
    for(size_t i = 0; i < input_string.size(); i++) {
        char ch_current     = input_string[i];
        char ch_next        = i < input_string.size() ? input_string[i + 1] : 0;

        //поиск комментариев ===================================================
        const bool ext_flag = !is_quotes;
        // вернёт комментарий без обрамления
        CheckComments(ch_current, ch_next, i, design, comment_string, ext_flag);
        if(!design.with_comments)
            comment_string.clear();
        //сюда зайдёт, если внутри комментария либо если встречен конец комментария
        if(design.temp_type != CommentType::eNotComment)
            continue;
        //=================================================== поиск комментариев

        temp += ch_current;
    }

    input_string = temp;
}



//предполагается использовать только для парсинга
void CheckComments(const char ch_current, const char ch_next,
                   size_t &iter_counter, CommentDesign &design,
                   std::string &current_comment, const bool external_flag)
{
    if(!external_flag) { //например, значения в кавычках не могут влиять на комментирование
        design.temp_type = CommentType::eNotComment;
        return;
    }

    switch(design.temp_type) {
    case CommentType::eOneLineComment: {
        //считывается строго до переноса строки
        if(ch_current == '\n' || ch_next == 0) {
            design.temp_type = CommentType::eCommentEnd;
            if(ch_current == '\n')
                --iter_counter;

            if(ch_next == 0) {
                current_comment += ch_current;
                current_comment += "\n"; //т.к. дальше конец входной строки/файла
            }
            return;
        }

        current_comment += ch_current;
        return;
    }
    case CommentType::eMultiLineComment: {
        bool b2 = design.temp_schema[1] == 0;
        bool b3 = design.temp_schema[2] == 0;
        char finish_ch = design.temp_schema[(b3 ? 0 : 2)];
        if(b2) { //второй символ не участвует
            //коммент закрывает последовательность [finish_ch]
            if(ch_current == finish_ch) {
                current_comment += ch_current;
                design.temp_type = CommentType::eCommentEnd;
                return;
            }
        } else { //второй символ участвует
            if(ch_next == 0)
                throw std::invalid_argument("invalid length of input string, multiline comment not closed");

            //коммент закрывает последовательность [1][finish_ch]
            if(ch_current == design.temp_schema[1] && ch_next == finish_ch) {
                current_comment += ch_current;
                current_comment += ch_next;
                ++iter_counter;
                design.temp_type = CommentType::eCommentEnd;
                return;
            }
        }

        current_comment += ch_current;
        return;
    }
    case CommentType::eCommentEnd:
    case CommentType::eNotComment: {
        auto SetMultiline = [&](bool b2, std::array<char, 3> format){
            design.temp_schema = format;
            design.temp_type = CommentType::eMultiLineComment;
            current_comment += format[0];
            if(!b2) {
                ++iter_counter;
                current_comment += format[1];
            }
        };
        auto SetOneline = [&](bool b2, std::array<char, 2> format){
            design.temp_type = CommentType::eOneLineComment;
            current_comment += format[0];
            if(!b2) {
                ++iter_counter;
                current_comment += format[1];
            }
        };

        //поиск многострочных комментариев
        for(const auto& format : design.multiline_comment_variants) {
            //v1: {X,0} - #...
            //v2: {X,Y} - #!...
            bool b2 = format[1] == 0;
            if(ch_current == format[0] && (b2 || ch_next == format[1])) {
                SetMultiline(b2, format);
                return;
            }
        }
        //поиск однострочных комментариев
        for(const auto& format : design.oneline_comment_variants) {
            //v1: {X,0} - #...
            //v2: {X,Y} - #!...
            bool b2 = format[1] == 0;
            if(ch_current == format[0] && (b2 || ch_next == format[1])) {
                SetOneline(b2, format);
                return;
            }
        }

        // поиск многострочных комментариев, если не заполнены варианты
        const auto temp_v_m = CommentDesign::GetDefaultMultilineCommentVariant();
        bool b2 = temp_v_m[1] == 0;
        if(temp_v_m[0] == ch_current && (b2 || temp_v_m[1] == ch_next)) {
            SetMultiline(b2, temp_v_m);
            return;
        }
        // поиск одноострочных комментариев, если не заполнены варианты
        const auto temp_v_o = CommentDesign::GetDefaultOnelineCommentVariant();
        b2 = temp_v_o[1] == 0;
        if(temp_v_o[0] == ch_current && (b2 || temp_v_o[1] == ch_next)) {
            SetOneline(b2, temp_v_o);
            return;
        }
        break;
    }
    default: break;
    }

    design.temp_type = CommentType::eNotComment;
    return;
}

} // namespace tools
} // namespace simpleapi
