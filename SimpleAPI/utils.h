#ifndef UTILS_H
#define UTILS_H

#include <string>


namespace utils {

bool IsNumber(const std::string& str);
bool IsNumber(const char ch);
inline std::string Tab(uint8_t tabs_counter) {
    std::string ret = "";
    for(uint8_t i = 0; i < tabs_counter; i++)
        ret += "\t";
    return ret;
}
void RemoveComments(std::string& str, bool& startComment);
size_t CountSymInStr(const std::string& str, const char ch);
bool CharsInString(const char ch, std::string symbols);

}

#endif // UTILS_H
