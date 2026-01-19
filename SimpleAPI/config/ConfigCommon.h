#ifndef JSON_COMMON_H
#define JSON_COMMON_H

#include <string>
#include <vector>


using VString = std::vector<std::string>;

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
    eJson
//    eYaml,
//    eXml
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

/*static*/ ValueType    CheckValue(std::string& value, const ConfigFormat& format = ConfigFormat::eJSON)    noexcept;
/*static*/ bool         CheckNumber(const std::string& value)                                               noexcept;
/*static*/ bool         CheckBool(std::string& value)                                                       noexcept;
/*static*/ bool         CheckNull(std::string& value)                                                       noexcept;
/*static*/ bool         CheckString(std::string& value, const ConfigFormat& format = ConfigFormat::eJSON)   noexcept;
/*static*/ bool         CheckJson(std::string& value)                                                       noexcept;
/*static*/ bool         CheckArray(std::string& value, const ConfigFormat& format = ConfigFormat::eJSON)    noexcept;

void SymbolCounter(const char ch_current, size_t &line_counter,
                                size_t &symbol_counter)                                                     noexcept;

#endif // JSON_COMMON_H
