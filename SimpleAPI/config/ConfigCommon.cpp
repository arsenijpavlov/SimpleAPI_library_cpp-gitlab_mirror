#include "ConfigCommon.h"
#include "ConfigDefines.h"

#include "../utils/Utils.h"

#include <fstream>


std::string to_string(const ValueType type) noexcept {
    switch(type) {
    case eNumber:   return "Number";    break;
    case eBool:     return "Bool";      break;
    case eString:   return "String";    break;
    case eJson:     return "Json";      break;
    case eArray:    return "Array";     break;
    default:        return "null";      break;
    }
}

void RemoveIllegalSpaces(std::string &string) noexcept {
    if(!string.empty()) {
        while(utils::CharsInString(string.back(), __SPACES_WITHOUT_SEPARATORS__))
            string.pop_back();

        while(utils::CharsInString(*string.begin(), __SPACES_WITHOUT_SEPARATORS__))
            string = string.erase(0, 1);
    }
}

bool GetAllStringsFromFile(const std::string& path, std::string& dest_string,
                           std::string* error_log) noexcept {
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
    } catch (std::exception e) {
        if(error_log) *error_log = std::string("Unable to read file: ") + e.what();
        return false;
    }

    return true;
}

bool CreateEmptyFile(const std::string &file_path, const std::string &start_comment,
                     const std::string &finish_comment, std::string* error_log) noexcept {
    try{
        std::ofstream file(file_path);
        if (!file.is_open()) {
            if(error_log) *error_log = "File not found";
            return false;
        }

        file.write(start_comment.c_str(), start_comment.length());
        file.write("\n", 1);
        file.write("\n", 1);
        file.write(finish_comment.c_str(), finish_comment.length());
        file.write("\n", 1);
        file.close();
    } catch (std::exception e) {
        if(error_log) *error_log = std::string("Unable to write file: ") + e.what();
        return false;
    }

    return false;
}
