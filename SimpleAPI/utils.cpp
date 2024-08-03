#include "utils.h"

#include <iostream>
#include <string>

namespace utils {

bool isNumber(const std::string &str, bool use_point)
{
    for(int i = 0; i < str.size(); i++)
        if(!isNumber(str[i], use_point))
            return false;
    return true;
}

bool isNumber(const char ch, bool use_point) {
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

std::string toString(double d) {
    std::ostringstream str;
    str << d;
    return str.str();
}

bool isBool(std::string& str) {
    if(str == "true" || str == "false") return true;
    else                                return false;
}

bool toBool(std::string& str) {
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

    std::string str;
    for(uint8_t d : data) {
        str += getHex(d >> 4);
        str += getHex(d & 0xF);
    }

    return str;
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

//на вход подаётся массив данных, в начале которого 1 байт отвечает за CRC
bool checkCrc8(std::vector<uint8_t>& data)
{
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
bool checkCrc16(std::vector<uint8_t>& data)
{
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
bool checkCrc32(std::vector<uint8_t>& data)
{
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


}
