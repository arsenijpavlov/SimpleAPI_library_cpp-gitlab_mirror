#include "utils.h"

#include <iostream>

bool utils::isNumber(const std::string &str)
{
    for(int i = 0; i < str.size(); i++) {
        if(!std::isdigit(str.data()[i]) && (str.data()[i] != '.'))
            return false;
    }
    return true;
}

void utils::removeComments(std::string& str, bool& startComment)
{
    std::string tempString;
    uint32_t quotesCounter = 0;
    bool isComment = startComment;
    for(size_t i = 0; i < str.length(); i++) {
        if (quotesCounter % 2 == 0) {
            if(str[i] == '/') {
                if(i <= str.length() + 1) { //проверка на границы строки
                    if(str[i + 1] == '/' && !isComment) {
                        std::cout << "comment(full):" << i << ". ";
                        break; //дальше только комментарии
                    } else if(str[i + 1] == '*'
                               && !isComment) { //дальше возможен конец комментария
                        std::cout << "comment(+):" << i << ". ";
                        isComment = true;
                        continue;
                    }
                }
            } else if(str[i] == '*' && isComment) {
                if(i <= str.length() + 1) { //проверка на границы строки
                    if(str[i + 1] == '/') {
                        std::cout << "comment(-):" << (i + 1) << ". ";
                        isComment = false;
                        //пропускаем конец комментария
                        i++;
                        continue; //чтобы не записать символ лишний и не выйти за границы
                    }
                }
            }
        }
        if((str[i] == '"' || '\'') //TODO: обработать кавычки отдельно!
            && !isComment)
            quotesCounter++;
        if(!isComment)
            tempString += str[i];
    }

    str = tempString;
    startComment = isComment;
}
