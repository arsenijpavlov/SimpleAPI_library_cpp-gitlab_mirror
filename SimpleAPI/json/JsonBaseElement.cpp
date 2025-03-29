#include "JsonBaseElement.h"

#include "JsonDefines.h"
#include "../utils/Utils.h"

#include "JsonElementNull.h"
#include "JsonElementBool.h"
#include "JsonElementString.h"
#include "JsonElementNumber.h"
#include "JsonElementJson.h"
#include "JsonElementArray.h"

#include <fstream>


bool Element::writeFile(const std::string &file_path, const ConfigFormat format,
                        const bool with_comments) noexcept {
    return CreateEmptyFile(file_path,
                           with_comments ? m_preview_comment : "",
                           with_comments ? m_suffix_comment : "");
}

bool Element::writeFileJson(const std::string &file_path, const bool with_comments) noexcept {
    return CreateEmptyFile(file_path,
                           with_comments ? m_preview_comment : "",
                           with_comments ? m_suffix_comment : "");
}

bool Element::writeFileIni(const std::string &file_path, const bool with_comments) noexcept {
    return CreateEmptyFile(file_path,
                           with_comments ? m_preview_comment : "",
                           with_comments ? m_suffix_comment : "");
}


//----------------------------------------------------------------------------------------------------------------------


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

Element ReadFile(const std::string &file_path, const ConfigFormat format,
                  const bool with_comments, std::string* error_log) noexcept {
    std::string dest;
    GetAllStringsFromFile(file_path, dest, error_log);
    return Parse(dest, format, with_comments, error_log);
}

Element ReadFileJson(const std::string &file_path, const bool with_comments,
                     std::string* error_log) noexcept {
    std::string dest;
    GetAllStringsFromFile(file_path, dest, error_log);
    return ParseJson(dest, with_comments, error_log);
}

Element ReadFileIni(const std::string &file_path, const bool with_comments,
                    std::string* error_log) noexcept {
    std::string dest;
    GetAllStringsFromFile(file_path, dest, error_log);
    return ParseIni(dest, with_comments, error_log);
}

Element Parse(const std::string &content, const ConfigFormat format,
              const bool with_comments, std::string* error_log) noexcept {
    switch(format) {
    case ConfigFormat::eJSON:   return ParseJson(content, with_comments, error_log);
    case ConfigFormat::eINI:    return ParseIni(content, with_comments, error_log);
//    case ConfigFormat::eYAML:   return ParseYaml();
//    case ConfigFormat::eXML:    return ParseXml();
    default:                    return ElementNull();
    }
}

Element ParseJson(const std::string &file_path, const bool with_comments,
                  std::string* error_log) noexcept {
    //TODO: Element &ParseJson()
    return ElementNull();
}

Element ParseIni(const std::string &file_path, const bool with_comments,
                 std::string* error_log) noexcept {
    //TODO: Element &ParseIni()
    return ElementNull();
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

