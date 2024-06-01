#include "utils.h"

#include <iostream>

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

bool utils::OnlySpaces(std::string str)
{
    for(char c : str)
        if(!CharsInString(c, " \n\t"))
            return false;
    return true;
}
