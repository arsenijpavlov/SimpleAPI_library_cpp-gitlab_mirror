#include "utils.h"

#include <iostream>
#include <string>

bool utils::IsNumber(const std::string &str, bool use_point)
{
    for(int i = 0; i < str.size(); i++) {
        if(!IsNumber(str[i], use_point)) return false;
    }
    return true;
}
bool utils::IsNumber(const char ch, bool use_point) {
    return (std::isdigit(ch) || (use_point && (ch == '.')));
}

void utils::RemoveComments(std::string& str, bool& startComment, char& quote)
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

size_t utils::CountSymInStr(const std::string &str, const char ch)
{
    size_t counter = 0;
    for(char temp : str)
        if(temp == ch) counter++;

    return counter;
}

bool utils::CharsInString(const char ch, std::string symbols)
{
    for(char c : symbols)
        if(c == ch) return true;
    return false;
}

bool utils::OnlySpaces(const std::string& str)
{
    for(char c : str)
        if(!CharsInString(c, " \n\t"))
            return false;
    return true;
}

std::string utils::to_hex_string(const std::vector<uint8_t>& data)
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

std::vector<uint8_t> utils::from_hex_string(std::string str)
{
    if(str.size() % 2 != 0) str.push_back('0');

    std::vector<uint8_t> vec;
    for(int i = 0; i < str.size(); i+=2) {
        std::string temp = str.substr(i, 2);
//        std::cout << "temp: [" << temp << "]" << std::endl;
        vec.push_back(std::stoi((temp), 0 , 16));
//        std::cout << "vec[i]: " << std::hex << (int)vec.back() << std::endl;
    }

    return vec;
}
