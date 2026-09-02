#include "number_utils.h"
#include <iostream>


namespace simpleapi {
namespace utils {

bool CreateBoolFromString(const std::string &input)
{
    std::string temp;
    if(input.size() == 4 || input.size() == 5) {
        for(auto ch : input)
            temp += std::tolower(ch);
        if(temp == "true")  return true;
        if(temp == "false") return false;
        temp.clear();
    }
    if(input.size() == 1) {
        switch(std::tolower(input[0])) {
        //case 't': // NOTE: может запуать пользователя, убрал
        case '+':   return true;
        //case 'f': // NOTE: может запуать пользователя, убрал
        case '-':   return false;
        default: break;
        }
    }

    throw std::invalid_argument("input param is not a bool as string");
    // return false;
}

long long CreateLLongFromString(const std::string &input)
{
    try {
        long double ll = 0;
        std::size_t pos_ll = 0;
        ll = std::stoll(input, &pos_ll);
        if(pos_ll == input.size())
            return ll;
    } catch (std::exception& e) {
        std::cerr << "SimpleAPI: is not a number: " << e.what() << std::endl;
    }

    return 0;
}

long double CreateLDoubleFromString(const std::string &input)
{
    try {
        long double ld = 0;
        std::size_t pos_ld = 0;
        ld = std::stold(input, &pos_ld);
        if(pos_ld == input.size())
            return ld;
    } catch (std::exception& e) {
        std::cerr << "SimpleAPI: is not a number: " << e.what() << std::endl;
    }

    return 0.f;
}

} // namespace utils
} // namespace simpleapi
