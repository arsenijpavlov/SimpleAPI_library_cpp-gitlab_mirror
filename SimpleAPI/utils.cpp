#include "utils.h"

#include <iostream>
#include <string>

namespace utils {

bool IsNumber(const std::string &str, bool use_point)
{
    for(int i = 0; i < str.size(); i++)
        if(!IsNumber(str[i], use_point))
            return false;
    return true;
}

bool IsNumber(const char ch, bool use_point) {
    return (std::isdigit(ch) || (use_point && (ch == '.')));
}

std::string Tab(uint8_t tabs_counter) {
    std::string ret = "";
    for(uint8_t i = 0; i < tabs_counter; i++)
        ret += "\t";
    return ret;
}

void RemoveComments(std::string& str, bool& startComment, char& quote)
{
    std::string tempString;
    bool isOneComment = false;
    bool isFullComment = startComment;
    for(size_t i = 0; i < str.length(); i++) {
        if(i <= str.length() + 1) { //проверка на границы строки
            if(quote == 0) { //если не часть строкового значения
                if(!isFullComment) { //многострочные комментарии имеют приоритет
                    if(str[i] == '/' && str[i + 1] == '*' && !isOneComment) {
                        isFullComment = true;
                        i++;
                        continue;
                    }
                    if(!isOneComment) {
                        if(str[i] == '/' && str[i+1] == '/') {
                            isOneComment = true;
                            i++;
                            continue;
                        }
                    }
                } else if(str[i] == '*' && str[i + 1] == '/') {
                    isFullComment = false;
                    i++;
                    continue;
                }
            }
        }
        if(!isFullComment && str[i] == '\n' && quote == 0) //действует только до конца строки
            isOneComment = false;

        if(!isFullComment && !isOneComment) {
            if(str[i] == '"') { //пропускать \"
                bool isIgnore = false;
                if( i - 1 >= 0) {
                    if(str[i - 1] == '\\')
                        isIgnore = true;
                }

                if(!isIgnore) {
                    if(str[i] == quote) quote = 0;
                    else                quote = str[i];
                }
            }
            tempString += str[i];
        }
    }

    str = tempString;
    startComment = isFullComment;
}

size_t CountSymInStr(const std::string &str, const char ch)
{
    size_t counter = 0;
    for(char temp : str)
        if(temp == ch) counter++;

    return counter;
}

bool CharsInString(const char ch, std::string symbols)
{
    for(char c : symbols)
        if(c == ch) return true;
    return false;
}

std::string ToString(double d) {
    std::ostringstream str;
    str << d;
    return str.str();
}

bool isBool(std::string& str) {
    if(str == "true" || str == "false") return true;
    else                                return false;
}

bool ToBool(std::string& str) {
    if(str == "true")   return true;
    else                return false;
}

bool OnlySpaces(const std::string& str)
{
    for(char c : str)
        if(!CharsInString(c, " \n\t"))
            return false;
    return true;
}

std::string to_hex_string(const std::vector<uint8_t>& data)
{
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

    std::stringstream ss;
    for(uint8_t d : data) {
        ss << getHex(d >> 4);
        ss << getHex(d & 0xF);
    }

    return ss.str();
}

std::vector<uint8_t> from_hex_string(std::string str)
{
    if(str.size() % 2 != 0) str.push_back('0');

    std::vector<uint8_t> vec;
    for(int i = 0; i < str.size(); i+=2) {
        std::string temp = str.substr(i, 2);
        vec.push_back(std::stoi((temp), 0 , 16));
    }

    return vec;
}

bool checkCrc8(std::vector<uint8_t>& data)
{
    bool needCheck = data[0] != 0;

    uint16_t sum = 0;
    for(uint8_t d : data)
        sum += d;
    while(sum > 0xFF)
        sum = (sum & 0xFF) + (sum >> 8);
    sum = !sum;
    data[0] = sum & 0xFF;

    if(needCheck && data[0] != 0)
        return false;
    return true;
}

bool checkCrc16(std::vector<uint8_t>& data)
{
    union u8_16_32 {
        uint8_t     u8[4];
        uint16_t    u16[2];
        uint32_t    u32;
    };

    bool needCheck = (data[0] != 0) && (data[1] != 0);

    u8_16_32 sum;
    sum.u32 = 0;
    for(size_t i = 0; i < data.size(); i += 2) {
        u8_16_32 d8;
        d8.u8[0] = data[i];
        d8.u8[1] = (i + 1 < data.size()) ? data[i + 1] : 0;
        sum.u32 += d8.u16[0];
    }
    while(sum.u32 > 0xFFFFFFFF)
        sum.u32 = sum.u16[0] + sum.u16[1];
    sum.u32 = !sum.u32;
    data[0] = sum.u8[0];
    data[1] = sum.u8[1];

    if(needCheck && ((data[0] != 0) || (data[1] != 0)))
        return false;
    return true;
}

bool checkCrc32(std::vector<uint8_t>& data)
{
    union u8_32_64 {
        uint8_t     u8[8];
        uint32_t    u32[2];
        uint64_t    u64;
    };

    bool needCheck = (data[0] != 0) && (data[1] != 0)
                     && (data[1] != 0) && (data[2] != 0);

    u8_32_64 sum;
    sum.u64 = 0;
    for(size_t i = 0; i < data.size(); i += 4) {
        u8_32_64 d8;
        d8.u8[0] = data[i];
        d8.u8[1] = (i + 1 < data.size()) ? data[i + 1] : 0;
        d8.u8[2] = (i + 2 < data.size()) ? data[i + 2] : 0;
        d8.u8[3] = (i + 3 < data.size()) ? data[i + 3] : 0;
        sum.u64 += d8.u32[0];
    }
    while(sum.u64 > 0xFFFFFFFFFFFFFFFF)
        sum.u64 = sum.u32[0] + sum.u32[1];
    sum.u64 = !sum.u64;
    data[0] = sum.u8[0];
    data[1] = sum.u8[1];
    data[2] = sum.u8[2];
    data[3] = sum.u8[3];

    if(needCheck && ((data[0] != 0) || (data[1] != 0)
                      || (data[2] != 0) || (data[3] != 0)))
        return false;
    return true;
}


}
