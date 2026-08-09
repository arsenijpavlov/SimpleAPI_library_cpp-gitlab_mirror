#pragma once

#include <string>
#include <vector>


namespace simpleapi {

// вектор строк
using VString = std::vector<std::string>;
// вектор векторов строк
using VVString = std::vector<std::vector<std::string>>;

//Format =======================================================================================
enum class ConfigFormat {
    eONLY_VALUE,
    eJSON,
    eINI,
    eYAML,
    eXML
};
//======================================================================================= Format

//Value type ===================================================================================
enum class ValueType {
    eNull,
    eBool,
    eNumber,
    eString,
    eArray,
    eJson,
    eYaml,
    eXml
};
 std::string ToString(const ValueType type)                                                                 noexcept;
//=================================================================================== Value type


//удалить пробелы в начале и конце строки
void RemoveFrontTabsIllegalSpaces(std::string& string, const int8_t tabulation_level)                       noexcept;
void RemoveFrontIllegalSpaces(std::string& string)                                                          noexcept;
void RemoveEndIllegalSpaces(std::string& string)                                                            noexcept;
void RemoveIllegalSpaces(std::string& string)                                                               noexcept;

void RemoveQuotes(std::string& string)                                                                      noexcept;

//READING -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
bool GetAllStringsFromFile(const std::string& path, std::string& dest_string,
                           std::string* error_log = nullptr)                                                noexcept;
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= READING

//WRITING -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
bool CreateEmptyFile(const std::string& file_path, const std::string& start_comment,
                     const std::string& finish_comment, std::string* error_log = nullptr)                   noexcept;
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= WRITING

} // namespace simpleapi
