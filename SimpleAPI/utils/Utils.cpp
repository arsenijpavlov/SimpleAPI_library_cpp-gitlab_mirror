#include "Utils.h"

#include <iostream>
#include <regex>
#include <string>

namespace utils {

bool IsNumber(const std::string &str, bool use_point) noexcept {
    for(int i = 0; i < str.size(); i++)
        if(!IsNumber(str[i], use_point))
            return false;
    return true;
}

bool IsNumber(const char ch, bool use_point) noexcept {
    return (std::isdigit(ch) || (use_point && (ch == '.')));
}

size_t CountSymInStr(const std::string &str, const char ch) noexcept {
    size_t counter = 0;
    for(char temp : str)
        if(temp == ch) counter++;

    return counter;
}

bool CharInString(const char ch, std::string symbols) noexcept {
    for(char c : symbols)
        if(c == ch) return true;
    return false;
}

std::string ToString(const long double& d) noexcept {
    std::ostringstream str;
    str << d;
    return str.str();
}

bool IsBool(std::string& str) noexcept {
    if(str == "true" || str == "false") return true;
    else                                return false;
}

bool ToBool(const std::string& str) noexcept {
    if(str == "true")   return true;
    else                return false;
}

std::string ToString(const bool b) noexcept {
    if(b) return "true";
    else  return "false";
}

bool OnlySpaces(const std::string& str) noexcept {
    for(char c : str)
        if(!CharInString(c, " \n\t"))
            return false;
    return true;
}

std::string PrintVector(const std::vector<std::string> &strings) noexcept {
    std::string ret;
    size_t all_size = 0;
    for(auto& s : strings)
        all_size += s.length() + 4; //4 на спецсимволы
    ret.reserve(all_size);

    for(const std::string& s : strings)
        ret += "[" + s + "], ";
    if(strings.size() > 0) {
        ret.pop_back();
        ret.pop_back();
    }

    return ret;
}

std::string ToHexString(const std::vector<uint8_t>& data) noexcept {
    auto getHex = [&](uint8_t halfByte) -> char {
        halfByte = halfByte & 0xF;
        switch(halfByte) {
        case 0:     return '0';
        case 1:     return '1';
        case 2:     return '2';
        case 3:     return '3';
        case 4:     return '4';
        case 5:     return '5';
        case 6:     return '6';
        case 7:     return '7';
        case 8:     return '8';
        case 9:     return '9';
        case 0xA:   return 'A';
        case 0xB:   return 'B';
        case 0xC:   return 'C';
        case 0xD:   return 'D';
        case 0xE:   return 'E';
        case 0xF:   return 'F';
        }
        return '\0';
    };

    std::string str;
    for(uint8_t d : data) {
        str += getHex(d >> 4);
        str += getHex(d & 0xF);
    }

    return str;
}

std::vector<uint8_t> FromHexString(std::string str) noexcept {
    if(str.size() % 2 != 0) str.push_back('0');

    std::vector<uint8_t> vec;
    for(int i = 0; i < str.size(); i+=2) {
        std::string temp = str.substr(i, 2);
        vec.push_back(std::stoi((temp), 0 , 16));
    }

    return vec;
}


std::string RepeatSymToStr(const char ch, const int16_t size) noexcept {
    if(size < 0) return "";

    std::string ret;
    for(uint16_t i = 0; i < size; i++)
        ret += ch;

    return ret;
}


//на вход подаётся массив данных, в начале которого 1 байт отвечает за CRC
bool CheckCrc8(std::vector<uint8_t>& data) noexcept {
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
bool CheckCrc16(std::vector<uint8_t>& data) noexcept {
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
bool CheckCrc32(std::vector<uint8_t>& data) noexcept {
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

//TODO: на замену
std::string GetEscChar(const std::string &str) noexcept {
    switch(str[0]) {
    case '"':   return "\"";    //символ "
    case '\\':  return "\\";    //символ '\'
    case 'b':   return "\b";    //возврат на один символ
    case 'f':   return "\f";    //прогон страницы
    case 'n':   return "\n";    //перенос строки
    case 'r':   return "\r";    //возврат каретки
    case 't':   return "\t";    //табуляция

    default:    return {};
    }
}

std::string GetEscChar2(const std::string &str) noexcept {
    if(str[0] == '\\') {
        if(str.size() == 1) return "\\";
        switch(str[1]) {
        case '"':   return "\""; //символ "
        case 'b':   return "\b"; //возврат на один символ
        case 'f':   return "\f"; //прогон страницы
        case 'n':   return "\n"; //перенос строки
        case 'r':   return "\r"; //возврат каретки
        case 't':   return "\t"; //табуляция
        }
        //NOTE: UNICODE-символы не допускается экранировать, так как кодировка UTF-8 и так позволяет их вставлять в текст
    }
    return str;
}

//TODO: на замену
char GetFromEscChar(const char ch) noexcept {
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

std::string GetFromEscChar2(const std::string& str) noexcept {
    switch(str[0]) {
    case '\"':   return "\\\""; //символ "
    case '\b':   return "\\\b"; //возврат на один символ
    case '\f':   return "\\\f"; //прогон страницы
    case '\n':   return "\\\n"; //перенос строки
    case '\r':   return "\\\r"; //возврат каретки
    case '\t':   return "\\\t"; //табуляция
    }
    //NOTE: UNICODE-символы не допускается экранировать, так как кодировка UTF-8 и так позволяет их вставлять в текст
    return str;
}

//TODO: ???
void UpdEscSymbols(std::string& string) noexcept {
    std::string temp_string;
    temp_string.reserve(string.capacity());
    for(size_t i = 0; i < string.size(); i++) {
        char current = string[i];
        if(current == '\\' && string.length() > i + 1) {
            std::string e_ch = utils::GetEscChar2(string.substr(i, 2));
//            if(!e_ch.empty()) {
//                std::cout << "found escape symbols <"
//                          << current << string[i+1] << ">" << std::endl;
                temp_string += e_ch;
                i++;
//            }
        } else temp_string += current;
    }

    string = temp_string;
}

size_t GetStringCharCount(const std::string &str, bool only_visible) {
    //1 => 0xxx xxxx
    //2 => 110x xxxx 10xx xxxx
    //3 => 1110 xxxx 10xx xxxx 10xx xxxx
    //4 => 1111 0xxx 10xx xxxx 10xx xxxx 10xx xxxx
    size_t size = 0;
    for(size_t i = 0; i < str.size(); ) {
        if((uint8_t)str[i] < 0b10000000) {
                if(only_visible) {
                    if(str[i] == '\033') {
                        ++i;
                        while(i < str.size() && (str[i] != 'm' && str[i] != 'M'))
                            i++;
                        i++;
                        continue;
                    }
                    if(CharInString(str[i], "\n\r\0")) {
                        i++;
                        continue;
                    }
                }

                size++;
                i++;
        } else if((uint8_t)str[i] < 0b11000000) {
                throw std::invalid_argument("incorrect symbol header");
        } else if((uint8_t)str[i] < 0b11100000) {
                size++;
                i += 2;
        } else if((uint8_t)str[i] < 0b11110000) {
                size++;
                i += 3;
        } else {
                size++;
                i += 4;
        }
    }

    return size;
}

//TODO: написать тест для проверки
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

bool IsMultiLine(const std::string &str, const size_t column_size) noexcept {
    if(column_size == 0)
        return str.find('\n') != -1;

    return false;
}

std::string ToStringWithEsc(const std::string &str, const bool use_backslash) noexcept {
    std::string ret;

    for(size_t i = 0; i < str.length(); i++) {
        char current    = str[i];

        char c = GetFromEscChar(current);
        if(c != 0) {
            if(use_backslash && current == '\n')
                ret += " \\\n\t";
            else
                ret += {'\\', c};
        } else
            ret += current;
    }

    return ret;
}

bool IsStringOfFloatNumber(const std::string& str, long double& result) noexcept {
    std::regex reg("^[+-]?[0-9]*[.]?[0-9]*[eE]?[+-]?[0-9]*[fF]?$");
    bool matched = std::regex_match(str, reg);
    bool e_is_last = str[str.length() - 1] == 'e' || str[str.length() - 1] == 'E';
    bool f_is_last = str[str.length() - 1] == 'f' || str[str.length() - 1] == 'F';

    if(f_is_last && str.length() > 1)
        e_is_last = str[str.length() - 2] == 'e' || str[str.length() - 2] == 'E';

    if(matched && !e_is_last) {
        try {
            result = std::stod(str);
            return true;
        } catch (...) {}
    }

    return false;
}

bool IsStringOfIntNumber(const std::string& str, long int& result) noexcept {
    std::regex reg("^[+-]?[0-9]+$");
    bool matched = std::regex_match(str, reg);

    if(matched) {
        try {
            result = std::stoi(str);
            return true;
        } catch (...) {}
    }

    return false;
}

bool IsStringOfUIntNumber(const std::string& str, uint64_t& result) noexcept {
    std::regex reg("^[0-9]+$");
    bool matched = std::regex_match(str, reg);

    if(matched) {
        try {
            result = std::stoi(str);
            return true;
        } catch (...) {}
    }

    return false;
}

std::string AddQuotes(const std::string &str) noexcept
{
    return "\"" + str + "\"";
}

std::string RemoveStartTabulations(const std::string &str) noexcept
{
    std::string ret = str;
    while(ret.front() == '\t')
        ret.erase(ret.begin(), ret.begin() + 1);

    return ret;
}

void AddTabsForFromLine(std::string &str, const size_t from_line_number,
                        const int8_t tabulation_size) noexcept
{
    std::string output_string = "";
    size_t line_counter = 0;
    for(size_t i = 0; i < str.size(); i++) {
        output_string += str[i];
        if(str[i] == '\n') {
            ++line_counter;
            if(line_counter + 1 >= from_line_number) {
                output_string += RepeatSymToStr('\t', tabulation_size);
            }
        }
    }
    str = output_string;
}

void AddStringForFromLine(std::string &str, const size_t from_line_number,
                          const std::string &added_line) noexcept
{
    std::string output_string = "";
    size_t line_counter = 0;
    for(size_t i = 0; i < str.size(); i++) {
        output_string += str[i];
        if(str[i] == '\n') {
            ++line_counter;
            if(line_counter + 1 >= from_line_number) {
                output_string += added_line;
            }
        }
    }
    str = output_string;
}

void SetStringAsOnlySpaces(std::string& str) noexcept {
    for(size_t i = 0; i < str.size(); i++) {
        if(!CharInString(str[i], "\n\t ")) {
            str[i] = ' ';
        }
    }
}

void SetVisibleColumn(std::string &input_str, const size_t &column_size) noexcept
{
    //вычислить видимую длину
    const size_t visible_len = GetStringCharCount(input_str, true);
    //определить необходимое количество пробелов
    const size_t need_spaces = visible_len < column_size ? column_size - visible_len: 0;

    //выдать результатs
    input_str += RepeatSymToStr(' ', need_spaces);
//    std::cout << "v.size: " << visible_len
//              << " c.size: " << column_size
//              << " n.spaces: " << need_spaces
    //              << " \"" << input_str << "\"" << std::endl;
}

/*если строка пустая или не является строгим повторением одного и того же символа, вернёт 0
 * в остальных случаях вернёт символ, который повторяется
 */
uint8_t LineOfOneSymbol(std::string &input_str) noexcept
{
    if(input_str.empty()) return 0;

    char ex = input_str.front();
    for(auto c : input_str) {
        if(ex != c) return 0;
    }
    return ex;
}

std::vector<uint8_t> FromHexStringToHex(const std::string &str) noexcept {
    auto func = [](char ch) {
        switch(ch) {
        case '0':   return 0x00;
        case '1':   return 0x01;
        case '2':   return 0x02;
        case '3':   return 0x03;
        case '4':   return 0x04;
        case '5':   return 0x05;
        case '6':   return 0x06;
        case '7':   return 0x07;
        case '8':   return 0x08;
        case '9':   return 0x09;
        case 'a':
        case 'A':   return 0x0A;
        case 'b':
        case 'B':   return 0x0B;
        case 'c':
        case 'C':   return 0x0C;
        case 'd':
        case 'D':   return 0x0D;
        case 'e':
        case 'E':   return 0x0E;
        case 'f':
        case 'F':   return 0x0F;
        default:    return 0x00;
        }
    };

    std::vector<uint8_t> vec;

    for(size_t i = 0; i < str.size(); i += 2) {
        uint8_t a = func(str[0]);
        uint8_t b = (i+1 < str.size()) ? func(str[1]) : 0; // проверка на чётное количество символов в строке
        vec.push_back((a << 4) + b);
    }
    return vec;
}


}
