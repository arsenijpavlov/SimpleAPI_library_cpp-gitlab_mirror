#include "utils.h"

#include <iostream>
#include <string>

namespace utils {

bool isNumber(const std::string &str, bool use_point) noexcept {
    for(int i = 0; i < str.size(); i++)
        if(!isNumber(str[i], use_point))
            return false;
    return true;
}

bool isNumber(const char ch, bool use_point) noexcept {
    return (std::isdigit(ch) || (use_point && (ch == '.')));
}

void RemoveComments(std::string& str, bool& startComment, char& quote,
                    char& start_comment_sym, char& stop_comment_sym) {
    std::string tempString;
    bool isOneLineComment = false;
    bool isMultiLineComment = startComment;
    for(size_t i = 0; i < str.length(); i++) {
        char previous = (i - 1 >= 0) ? str[i - 1] : 0;
        char current = str[i];
        char next = (str.length() > i + 1 ? str[i + 1] : 0);

        if(quote == 0) { //если не часть строкового значения
            //поиск комментариев ===========================================================
            if(!isOneLineComment && !isMultiLineComment) {
                //сперва искать многострочные комментарии!
                for(uint8_t j = 0; j < utils::cmt::SIZE_comment_multi_line; j++) {
                    if(current == utils::cmt::comment_multi_line[j][0] && next == utils::cmt::comment_multi_line[j][1]) {
                        start_comment_sym = current;
                        stop_comment_sym = next;
                        //изменение завершающего символа
                        if(current == '<') start_comment_sym = '>';
                        i++; //проскакиваем следующий символ при парсинге
                        isMultiLineComment = true;
                        break;
                    }
                }
                if(isMultiLineComment) continue;
                //поиск однострочных комментариев
                for(uint8_t j = 0; j < utils::cmt::SIZE_comment_one_line; j++) {
                    if(current == utils::cmt::comment_one_line[j][0]) {
                        if((utils::cmt::comment_one_line[j][1] != 0) && (next == utils::cmt::comment_one_line[j][1]))
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
            if(isMultiLineComment) {
                //нужен следующий символ, если нет - исключение
                if(str.length() <= i + 1)
                    throw std::invalid_argument("invalid length of input JSON string");

                if((current == stop_comment_sym) && (next == start_comment_sym)) {
                    isMultiLineComment = false;
                    i++; //многострочные комментарии всегда обособляются двумя символами
                }
                continue;
            }
            //==============================================================================
        }

        if(!isMultiLineComment && !isOneLineComment) {
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
    startComment = isMultiLineComment;
}

size_t CountSymInStr(const std::string &str, const char ch) noexcept {
    size_t counter = 0;
    for(char temp : str)
        if(temp == ch) counter++;

    return counter;
}

bool CharsInString(const char ch, std::string symbols) noexcept {
    for(char c : symbols)
        if(c == ch) return true;
    return false;
}

std::string toString(double d) noexcept {
    std::ostringstream str;
    str << d;
    return str.str();
}

bool isBool(std::string& str) noexcept {
    if(str == "true" || str == "false") return true;
    else                                return false;
}

bool toBool(std::string& str) noexcept {
    if(str == "true")   return true;
    else                return false;
}

std::string to_string(bool b) noexcept {
    if(b) return "true";
    else  return "false";
}

bool OnlySpaces(const std::string& str) noexcept {
    for(char c : str)
        if(!CharsInString(c, " \n\t"))
            return false;
    return true;
}

std::string PrintVector(const std::vector<std::string> &strings) noexcept {
    std::string ret;
    size_t all_size = 0;
    for(std::string s : strings)
        all_size += s.length() + 4; //4 на спецсимволы
    ret.reserve(all_size);

    for(std::string s : strings)
        ret += "[" + s + "], ";
    if(strings.size() > 0) {
        ret.pop_back();
        ret.pop_back();
    }

    return ret;
}

std::string to_hex_string(const std::vector<uint8_t>& data) noexcept {
    auto getHex = [&](uint8_t halfByte) -> char {
        halfByte = halfByte & 0xF;
        switch(halfByte) {
        case 0:     { return '0'; }
        case 1:     { return '1'; }
        case 2:     { return '2'; }
        case 3:     { return '3'; }
        case 4:     { return '4'; }
        case 5:     { return '5'; }
        case 6:     { return '6'; }
        case 7:     { return '7'; }
        case 8:     { return '8'; }
        case 9:     { return '9'; }
        case 0xA:   { return 'A'; }
        case 0xB:   { return 'B'; }
        case 0xC:   { return 'C'; }
        case 0xD:   { return 'D'; }
        case 0xE:   { return 'E'; }
        case 0xF:   { return 'F'; }
        default: throw "";
        }
    };

    std::string str;
    for(uint8_t d : data) {
        str += getHex(d >> 4);
        str += getHex(d & 0xF);
    }

    return str;
}

std::vector<uint8_t> from_hex_string(std::string str) noexcept {
    if(str.size() % 2 != 0) str.push_back('0');

    std::vector<uint8_t> vec;
    for(int i = 0; i < str.size(); i+=2) {
        std::string temp = str.substr(i, 2);
        vec.push_back(std::stoi((temp), 0 , 16));
    }

    return vec;
}


std::string RepeatSymToStr(const char ch, const uint16_t size) noexcept {
    std::string ret;
    for(uint16_t i = 0; i < size; i++)
        ret += ch;

    return ret;
}


//на вход подаётся массив данных, в начале которого 1 байт отвечает за CRC
bool checkCrc8(std::vector<uint8_t>& data) noexcept {
    bool needCheck = data[0] != 0;

    uint16_t sum = 0;
    //первый байт не влияет на итоговый результат
    for(uint8_t i = 1; i < data.size(); i++)
        sum += data[i];

    while(sum > 0xFF)
        sum = (sum & 0xFF) + (sum >> 8);
    sum = static_cast<uint8_t>(~sum);

    if(needCheck) {
        needCheck = sum == data[0];
        data[0] = (needCheck ? 0 : sum);
        return needCheck;
    } else {
        data[0] = sum;
        return true;
    }
}

//на вход подаётся массив данных, в начале которого 2 байта отвечают за CRC
bool checkCrc16(std::vector<uint8_t>& data) noexcept {
    bool needCheck = data[0] != 0 || data[1] != 0;

    uint32_t sum = 0;
    //первые два байта не влияют на итоговый результат
    for(uint8_t i = 2; i < data.size(); i+=2)
        sum += ((data[i] << 8) + (i < data.size() ? (data[i + 1]) : 0));

    while(sum > 0xFFFF)
        sum = (sum & 0xFFFF) + (sum >> 16);
    sum = static_cast<uint16_t>(~sum);

    uint8_t byte_0 = (sum >> 8) & 0xFF;
    uint8_t byte_1 = sum & 0xFF;

    if(needCheck) {
        needCheck = byte_0 == data[0] && byte_1 == data[1];
        data[0] = (needCheck ? 0 : byte_0);
        data[1] = (needCheck ? 0 : byte_1);
        return needCheck;
    } else {
        data[0] = byte_0;
        data[1] = byte_1;
        return true;
    }
}

//на вход подаётся массив данных, в начале которого 4 байта отвечают за CRC
bool checkCrc32(std::vector<uint8_t>& data) noexcept {
    bool needCheck = data[0] != 0 || data[1] != 0 || data[2] != 0 || data[3] != 0;

    uint64_t sum = 0;
    //первые четыре байта не влияют на итоговый результат
    for(uint8_t i = 4; i < data.size(); i+=4)
        sum += ((data[i] << 24)
                + (i + 1 < data.size() ? (data[i + 1] << 16) : 0)
                + (i + 2 < data.size() ? (data[i + 2] << 8) : 0)
                + (i + 3 < data.size() ? (data[i + 3]) : 0));

    while(sum > 0xFFFFFFFF)
        sum = (sum & 0xFFFFFFFF) + (sum >> 32);
    sum = static_cast<uint32_t>(~sum);

    uint8_t byte_0 = (sum >> 24) & 0xFF;
    uint8_t byte_1 = (sum >> 16) & 0xFF;
    uint8_t byte_2 = (sum >> 8) & 0xFF;
    uint8_t byte_3 = sum & 0xFF;

    if(needCheck) {
        needCheck = byte_0 == data[0] && byte_1 == data[1] && byte_2 == data[2] && byte_3 == data[3];
        data[0] = (needCheck ? 0 : byte_0);
        data[1] = (needCheck ? 0 : byte_1);
        data[2] = (needCheck ? 0 : byte_2);
        data[3] = (needCheck ? 0 : byte_3);
        return needCheck;
    } else {
        data[0] = byte_0;
        data[1] = byte_1;
        data[2] = byte_2;
        data[3] = byte_3;
        return true;
    }
}

//TODO: \u0000
char getEscChar(const char ch) noexcept {
    switch(ch) {
    case '"':   return '\"';
    case '\\':  return '\\';
    case 'b':   return '\b';
    case 'f':   return '\f';
    case 'n':   return '\n';
    case 'r':   return '\r';
    case 't':   return '\t';

    default:    return 0;
    }
}

char getFromEscChar(const char ch) noexcept {
    switch(ch) {
    case '"':   return '\"';
    case '\\':  return '\\';
    case '\b':   return 'b';
    case '\f':   return 'f';
    case '\n':   return 'n';
    case '\r':   return 'r';
    case '\t':   return 't';

    default:    return 0;
    }
}

void UpdEscSymbols(std::string& string) noexcept {
    std::string temp_string;
    temp_string.reserve(string.capacity());
    for(size_t i = 0; i < string.size(); i++) {
        char current = string[i];
        if(current == '\\' && string.length() > i + 1) {
            char e_ch = utils::getEscChar(string[i + 1]);
            if(e_ch != 0) {
                std::cout << "found escape symbols <"
                          << current << string[i+1] << ">" << std::endl;
                temp_string += '\\' + e_ch;
                i++;
            }
        } else temp_string += current;
    }

    string = temp_string;
}

size_t getStringSize(const std::string &str) noexcept {
    size_t size = 0;

    bool flag = false;
    for(char ch : str) {
        if(ch < 0) {
            if(flag)
                size++;
            flag = !flag;
        } else
            size++;
    }

    return size;
}

std::string SeparateString(std::string &str, const size_t length) noexcept {
    std::string res;

    size_t counter = 0;
    bool flag = false;
    size_t i = 0;
    for(; i < str.length() && counter < length; i++) {
        char ch = str[i];

        if(ch < 0) {
            if(flag)
                counter++;
            flag = !flag;
        } else
            counter++;
    }
    res = str.substr(0, i);
    str = str.substr(i, str.size() - 1);

    return res;
}

bool isMultiLine(const std::string &str, const size_t column_size) noexcept {
    if(column_size == 0)
        return str.find('\n') != -1;

    return false;
}

std::string to_string_with_esc(const std::string &str) noexcept {
    std::string ret;

    for(size_t i = 0; i < str.length(); i++) {
        char current    = str[i];

        char c = getFromEscChar(current);
        if(c != 0)
            ret += {'\\', c};
        else
            ret += current;
    }

    return ret;
}

CommentType CheckComment(char& first, const char second, size_t& iter_counter) noexcept {
    //сперва искать многострочные комментарии!
    for(uint8_t i = 0; i < utils::cmt::SIZE_comment_multi_line; i++) {
        if(first == utils::cmt::comment_multi_line[i][0] && second == utils::cmt::comment_multi_line[i][1]) {
            //изменение завершающего символа
            if(first == '<') first = '>';
            iter_counter++; //проскакиваем следующий символ при парсинге
            return CommentType::eMultiLineComment;
        }
    }
    //поиск однострочных комментариев
    for(uint8_t i = 0; i < utils::cmt::SIZE_comment_one_line; i++) {
        if(first == utils::cmt::comment_one_line[i][0]) {
            if((utils::cmt::comment_one_line[i][1] != 0)) {
                if(second == utils::cmt::comment_one_line[i][1]) {
                    iter_counter++;
                    return CommentType::eOneLineComment;
                }
            } else
                return CommentType::eOneLineComment;
        }
    }

    return CommentType::eNotComment;
}

CommentChecker CheckComments(const char current_sym, const char next_sym,
                   bool& is_one_line, bool& is_multi_line,
                   char& first_ml_sym, char& second_ml_sym,
                   const bool enable_comment, std::string& current_sym_comment_line,
                   size_t& iter_counter, const bool external_flag) {
    if(!external_flag) return CommentChecker::isNotComment;

    if(!is_one_line && !is_multi_line) {
        first_ml_sym    = current_sym;
        second_ml_sym   = next_sym;
//        std::cout << "\"" << first_ml_sym << second_ml_sym << "\"" << std::endl;
        switch(CheckComment(first_ml_sym, second_ml_sym, iter_counter)) {
        case CommentType::eOneLineComment: {
            is_one_line = true;
            if(!current_sym_comment_line.empty() && enable_comment)
                current_sym_comment_line += "\n";
            return CommentChecker::isComment;
        }
        case CommentType::eMultiLineComment: {
            is_multi_line = true;
            if(!current_sym_comment_line.empty() && enable_comment)
                current_sym_comment_line += "\n";
            return CommentChecker::isComment;
        }
        default: return CommentChecker::isNotComment;
        }
    }

    //обработка комментариев
    if(is_one_line) {
        current_sym_comment_line += current_sym;

        //если следующий символ должен обрабатываться другим кодом
        if((current_sym == '\n') || ((next_sym != 0) && (next_sym == '\n'))) {
            is_one_line = false;
            if(current_sym != '\n') current_sym_comment_line += '\n';
            return CommentChecker::isCommentEnd;
        }

        return CommentChecker::isComment;
    }
    if(is_multi_line) {
        //нужен следующий символ, если нет - исключение
        if(next_sym == 0)
            throw std::invalid_argument("invalid length of input string, multiline comment not closed");

        if((current_sym == second_ml_sym) && (next_sym == first_ml_sym)) {
            is_multi_line = false;
            iter_counter++; //многострочные комментарии всегда обособляются двумя символами
            return CommentChecker::isCommentEnd;
        }

        current_sym_comment_line += current_sym;
        return CommentChecker::isComment;
    }
    return CommentChecker::isNotComment;
}





}
