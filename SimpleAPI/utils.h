#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sstream>

namespace utils {

bool IsNumber(const std::string& str, bool use_point = true);
bool IsNumber(const char ch, bool use_point = true);
inline std::string Tab(uint8_t tabs_counter) {
    std::string ret = "";
    for(uint8_t i = 0; i < tabs_counter; i++)
        ret += "\t";
    return ret;
}
void RemoveComments(std::string& str, bool& startComment, char& quote);
size_t CountSymInStr(const std::string& str, const char ch);
bool CharsInString(const char ch, std::string symbols);
//исключить лишние знаки при выводе чисел
inline std::string ToString(double d) {
    std::ostringstream str;
    str << d;
    return str.str();
}
inline bool isBool(std::string& str) {
    if(str == "true" || str == "false") return true;
    else                                return false;
}
inline bool ToBool(std::string& str) {
    if(str == "true")   return true;
    else                return false;
}
bool OnlySpaces(std::string str);

}

#endif // UTILS_H
