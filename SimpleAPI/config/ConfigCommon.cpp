#include "ConfigCommon.h"
#include "ConfigDefines.h"

#include "../utils/Utils.h"

#include <fstream>
#include <regex>


namespace simpleapi {

std::string ToString(const ValueType type) noexcept {
    switch(type) {
    case ValueType::eNumber:    return "Number";    break;
    case ValueType::eBool:      return "Bool";      break;
    case ValueType::eString:    return "String";    break;
    case ValueType::eJson:      return "Json";      break;
    case ValueType::eArray:     return "Array";     break;
    default:                    return "null";      break;
    }
}

void RemoveFrontTabsIllegalSpaces(std::string &string, const int8_t tabulation_level) noexcept {
    using namespace utils;
    if(string.empty()) return;

    for(uint8_t i = tabulation_level; i > 0 && CharInString(*string.begin(), "\t"); --i)
        string = string.erase(0, 1);
}

void RemoveFrontIllegalSpaces(std::string &string) noexcept {
    using namespace utils;
    if(string.empty()) return;

    while(CharInString(*string.begin(), __SPACES__))
        string = string.erase(0, 1);
}

void RemoveEndIllegalSpaces(std::string &string) noexcept {
    using namespace utils;
    if(string.empty()) return;

    while(CharInString(string.back(), __SPACES__))
        string.pop_back();
}

void RemoveIllegalSpaces(std::string &string) noexcept {
    RemoveFrontIllegalSpaces(string);
    RemoveEndIllegalSpaces(string);
}

void RemoveQuotes(std::string &string) noexcept {
    bool b_is_eq = !string.empty()
                   && (string.front() == '"' || string.front() == '\'')
                   && string.front() == string.back();
    if(b_is_eq) {
        string.erase(0, 1);
        string.pop_back();
    }
}

bool GetAllStringsFromFile(const std::string& path, std::string& dest_string,
                           std::string* error_log) noexcept
{
    try {
        std::ifstream file(path);
        if (!file.is_open()) {
            if(error_log) *error_log = "File not found";
            return false;
        }

        std::string temp_string;
        while(getline(file, temp_string))
            dest_string += temp_string + '\n';
        file.close();
    } catch (const std::exception& e) {
        if(error_log) *error_log = std::string("Unable to read file: ") + e.what();
        return false;
    }

    return true;
}

bool CreateEmptyFile(const std::string &file_path, const std::string &start_comment,
                     const std::string &finish_comment, std::string* error_log) noexcept
{
    try {
        std::ofstream file(file_path);
        if (!file.is_open()) {
            if(error_log) *error_log = "The file cannot be opened";
            return false;
        }

        file.write(start_comment.c_str(), start_comment.length());
        file.write("\n", 1);
        file.write("\n", 1);
        file.write(finish_comment.c_str(), finish_comment.length());
        file.write("\n", 1);
        file.close();
    } catch (const std::exception& e) {
        if(error_log) *error_log = std::string("Unable to write file: ") + e.what();
        return false;
    }

    return false;
}

} // namespace simpleapi

