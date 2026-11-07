#include "string_utils.h"
#include "utils.h"
#include "../config/ConfigDefines.h"


namespace simpleapi {
namespace utils {

void RemoveFrontTabsIllegalSpaces(std::string &string, const int8_t tabulation_level) noexcept
{
    if(string.empty()) return;

    for(uint8_t i = tabulation_level; i > 0 && CharInString(*string.begin(), "\t"); --i)
        string = string.erase(0, 1);
}

void RemoveFrontIllegalSpaces(std::string &string) noexcept
{
    if(string.empty()) return;

    while(CharInString(*string.begin(), __SPACES__))
        string = string.erase(0, 1);
}

void RemoveEndIllegalSpaces(std::string &string) noexcept
{
    if(string.empty()) return;

    while(CharInString(string.back(), __SPACES__))
        string.pop_back();
}

void RemoveIllegalSpaces(std::string &string) noexcept
{
    RemoveFrontIllegalSpaces(string);
    RemoveEndIllegalSpaces(string);
}

void RemoveQuotes(std::string &string) noexcept
{
    bool b_is_eq = !string.empty()
                   && (string.front() == '"' || string.front() == '\'')
                   && string.front() == string.back();
    if(b_is_eq) {
        string.erase(0, 1);
        string.pop_back();
    }
}

// Обрежет входную строку на список строк. Учитываются только пользовательские переносы строк
SplittedLines SplitWithoutColumned(const std::string& input_string) noexcept
{
    SplittedLines sl;
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
        if(sl.max_length < GetStringCharCount(s, true))
            sl.max_length = GetStringCharCount(s, true);

    return sl;
}

/* Обрезать строку на подстроки с заданной шириной
 * - если хотя бы одна строка неделима и превышает предел,
 *   то остальные строки должны быть выровнены по новому пределу
 * - многоточие считается частью слова, не переносится на другую строку
 * - пользовательские переносы строк должны быть сохранены
*/
//INFO: можно оптимизировать
SplittedLines SplitToColumns(const std::string& input_string, const size_t column_size) noexcept
{
    VString words;
    std::string temp;

    const size_t input_visible_len = GetStringCharCount(input_string, true);
    bool is_oneline = input_string.find('\n') == std::string::npos;

    if(is_oneline && (column_size == 0 || input_visible_len <= column_size))
    {
        return SplittedLines{{input_string}, input_visible_len};
    }

    // разбиение на самостоятельные слова/объекты
    bool need_add = false;
    for(size_t i = 0; i < input_string.size(); i++) {
        if(need_add) {
            if(!temp.empty()) {
                // пробелы выставляются только в этом блоке
                if(CharInString(temp.back(), __COMMENT_SEPARATOR_SYMBOLS__))
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
        if(CharInString(input_string[i], "0123456789")) {
            // добавляем числа, буквы, точки, дефисы и двоеточия пока не встретится любой другой
            while(i + 1 < input_string.size()
                   && !CharInString(input_string[i+1], __SPACES__ __COMMENT_SEPARATOR_SYMBOLS_FOR_NUMBER__))
            {
                ++i;
                temp += input_string[i];
            }
        }

        if(CharInString(input_string[i], __COMMENT_SEPARATOR_SYMBOLS__
                                                     __COMMENT_OTHERS_SPEC_SYMBOLS__
                                                         __SPACES__))
        {
            need_add = true;

            // если следующий символ пробел - сохранить его в этом же слове
            if(i + 1 < input_string.size() && CharInString(input_string[i+1], " \t")) {
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
        if(max_len < GetStringCharCount(word, true))
            max_len = GetStringCharCount(word, true);
    }
    VString res;
    temp.clear();
    size_t current_line_size = 0;
    for(/*const*/ auto& word : words) {
        const size_t append_word_size = word.empty() ? 0
                                                     : GetStringCharCount(word, true);
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
std::string VStringToString(const VString& input_vec, const bool need_quotes) noexcept
{
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

SplittedLines SplitString(const std::string input_string, char split_char,
                          bool with_empty_strings) noexcept
{
    SplittedLines sl;
    sl.max_length = 0;

    size_t last_pos = 0;
    size_t next_pos = 0;
    do {
        next_pos = input_string.find(split_char, last_pos);

        size_t new_len = next_pos - last_pos;
        if(with_empty_strings || new_len > 0)
        {
            sl.lines.push_back(input_string.substr(last_pos, new_len));
            if(sl.max_length < new_len) {
                sl.max_length = new_len;
            }
        }

        last_pos = next_pos + /*знак-разделитель*/1;
    } while (next_pos != std::string::npos);

    return sl;
}

// @TEST(UTILS, string_utf8_to_wstring)
std::wstring StrUtf8ToWStr(const std::string &str) {
    // Linux:   wchar_t = 1 символ UTF-32
    // TODO(cross): Windows: wchar_t = 1 символ UTF-16

    // Логика формата UTF-8 (маска наложения от старших байтов к младшим)
    // 1 байт  на символ => 0xxx xxxx
    // 2 байта на символ => 110x xxxx 10xx xxxx
    // 3 байта на символ => 1110 xxxx 10xx xxxx 10xx xxxx
    // 4 байта на символ => 1111 0xxx 10xx xxxx 10xx xxxx 10xx xxxx

    // обрабатываем не по байтам, а по диапазонам с учётом масок UTF-8
    // 1b -> {7}          -> 7  бит на символ
    // 2b -> {5}{6}       -> 11 бит на символ
    // 3b -> {4}{6}{6}    -> 16 бит на символ
    // 4b -> {3}{6}{6}{6} -> 21 бит на символ

    std::wstring ret;
    ret.reserve(simpleapi::utils::GetStringCharCount(str));

    for(size_t i = 0; i < str.size(); ) {
        if((uint8_t)str[i] < 0b10000000) {
            ret += str[i]; // {7}
            i++;
        } else if((uint8_t)str[i] < 0b11000000) {
            throw std::invalid_argument("SimpleAPI: incorrect UTF-8 symbol header");
        } else if((uint8_t)str[i] < 0b11100000) {
            // {5}{6} -> {3}{8}
            wchar_t wc = ((static_cast<uint8_t>(str[i]) >> 2) & 0x07) << 8; // 3 бита первого байта (с учётом маски)
            wc        |= (static_cast<uint8_t>(str[i]) & 0x03) << 6;        // 2 левых бита второго байта (с учётом маски)
            wc        |= static_cast<uint8_t>(str[i + 1]) & 0x3F;           // 6 правых битов второго байта (с учётом маски)
            ret       += wc;
            i         += 2;
        } else if((uint8_t)str[i] < 0b11110000) {
            // {4}{6}{6} -> {8}{8}
            wchar_t wc = (static_cast<uint8_t>(str[i]) & 0x0F) << 12;           // 4 правых бита первого байта
            wc        |= ((static_cast<uint8_t>(str[i + 1]) >> 2) & 0x0F) << 8; // 4 левых бита второго байта (с учётом маски)
            wc        |= (static_cast<uint8_t>(str[i + 1]) & 0x03) << 6;        // 2 правых бита второго байта (с учётом маски)
            wc        |= static_cast<uint8_t>(str[i + 2]) & 0x3F;               // 6 правых битов второго байта (с учётом маски)
            ret       += wc;
            i += 3;
        } else {
            // {3}{6}{6}{6} -> {5}{8}{8}
            wchar_t wc = (static_cast<uint8_t>(str[i]) & 0x07) << 20;            // 3 правых бита первого байта
            wc        |= ((static_cast<uint8_t>(str[i + 1]) >> 4) & 0x03) << 16; // 2 левых бита второго байта (с учётом маски)
            wc        |= (static_cast<uint8_t>(str[i + 1]) & 0x0F) << 12;        // 4 правых бита второго байта (с учётом маски)
            wc        |= ((static_cast<uint8_t>(str[i + 2]) >> 2) & 0x0F) << 8;  // 4 левых бита третьего байта (с учётом маски)
            wc        |= (static_cast<uint8_t>(str[i + 2]) & 0x03) << 6;         // 2 правых бита третьего байта (с учётом маски)
            wc        |= static_cast<uint8_t>(str[i + 3]) & 0x3F;                // 6 правых битов четвёртого байта (с учётом маски)
            ret       += wc;
            i += 4;
        }
    }

    return ret;
}

// @TEST(UTILS, wstring_to_string_utf8)
std::string WStrToStrUtf8(const std::wstring &wstr) noexcept {
    // Linux:   wchar_t = 1 символ UTF-32
    // TODO(cross): Windows: wchar_t = 1 символ UTF-16

    // Логика формата UTF-8 (маска наложения от старших байтов к младшим)
    // 1 байт  на символ => 0xxx xxxx
    // 2 байта на символ => 110x xxxx 10xx xxxx
    // 3 байта на символ => 1110 xxxx 10xx xxxx 10xx xxxx
    // 4 байта на символ => 1111 0xxx 10xx xxxx 10xx xxxx 10xx xxxx

    std::string ret;
    ret.reserve(wstr.size()); // предполагаем, что wstr содержит только однобайтовые символы (частый вариант)

    for(wchar_t sym : wstr) {
        // срезаем не по байтам, а по диапазонам с учётом масок UTF-8
        // 1b -> {7}          -> 7  бит на символ
        // 2b -> {5}{6}       -> 11 бит на символ
        // 3b -> {4}{6}{6}    -> 16 бит на символ
        // 4b -> {3}{6}{6}{6} -> 21 бит на символ

        uint8_t symbol_size = 4;
        if((sym >> 7) == 0)  --symbol_size;
        if((sym >> 11) == 0) --symbol_size;
        if((sym >> 16) == 0) --symbol_size;

        switch(symbol_size) {
        case 1: {
            // std::cout << "1b" << std::endl; // лог для дебага
            ret += sym & 0x7F;
            break;
        }
        case 2: {
            // std::cout << "2b" << std::endl; // лог для дебага
            ret.push_back(static_cast<char>(0xC0 | ((sym >> 6) & 0x1F)));
            ret.push_back(static_cast<char>(0x80 | (sym & 0x3F)));
            break;
        }
        case 3: {
            // std::cout << "3b" << std::endl; // лог для дебага
            ret.push_back(static_cast<char>(0xE0 | ((sym >> 12) & 0x0F)));
            ret.push_back(static_cast<char>(0x80 | ((sym >> 6) & 0x3F)));
            ret.push_back(static_cast<char>(0x80 | (sym & 0x3F)));
            break;
        }
        case 4: {
            // std::cout << "4b" << std::endl; // лог для дебага
            ret.push_back(static_cast<char>(0xF0 | ((sym >> 18) & 0x07)));
            ret.push_back(static_cast<char>(0x80 | ((sym >> 12) & 0x3F)));
            ret.push_back(static_cast<char>(0x80 | ((sym >> 6) & 0x3F)));
            ret.push_back(static_cast<char>(0x80 | (sym & 0x3F)));
            break;
        }
        default: break;
        }
    }

    return ret;
}


} // namespace utils
} // namespace simpleapi
