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
