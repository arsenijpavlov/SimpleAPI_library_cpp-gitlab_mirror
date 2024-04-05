#ifndef UTILS_H
#define UTILS_H

#include <string>


namespace utils {

bool isNumber(const std::string& str);
inline std::string tab(uint8_t tabs_counter) {
    std::string ret = "";
    for(uint8_t i = 0; i < tabs_counter; i++)
        ret += "\t";
    return ret;
}

}

#endif // UTILS_H
