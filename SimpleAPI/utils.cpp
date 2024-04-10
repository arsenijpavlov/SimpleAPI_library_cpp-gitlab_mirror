#include "utils.h"

#include <iostream>

bool utils::IsNumber(const std::string &str)
{
    for(int i = 0; i < str.size(); i++) {
        if(!IsNumber(str[i])) return false;
    }
    return true;
}
bool utils::IsNumber(const char ch) {
    return (std::isdigit(ch) || (ch == '.'));
}

void utils::RemoveComments(std::string& str, bool& startComment, char& quote)
{
    std::string tempString;
    bool isOneComment = false;
    bool isFullComment = startComment;
    for(size_t i = 0; i < str.length(); i++) {
        if (quote == 0) { //если не часть строкового значения
            if(i <= str.length() + 1) { //проверка на границы строки
                if(!isFullComment) { //многострочные комментарии имеют приоритет
                    if(str[i] == '/' && str[i + 1] == '*') {
                        isFullComment = true;
                        continue;
                    }
                    if(!isOneComment) {
                        if(str[i] == '/' && str[i+1] == '/') {
                            isOneComment = true;
                            continue;
                        }
                    }
                } else if(str[i] == '*' && str[i + 1] == '/') {
                    isFullComment = false;
                    i++;
                    continue;
                }
            }
            if(!isFullComment && str[i] == '\n') //действует только до конца строки
                isOneComment = false;
        }

        if(str[i] == '"' || str[i] == '\'') {
            if(str[i] == quote) quote = 0;
            else                quote = str[i];
        }

        if(!isOneComment && !isFullComment)
            tempString += str[i];
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
