#ifndef JSON_COMMON_H
#define JSON_COMMON_H

#include <string>


//Format =======================================================================================
enum class ConfigFormat {
    eJSON,
    eYAML,
    eINI,
    eXML
};
//======================================================================================= Format


//Value type ===================================================================================
enum ValueType {
    eNull,
    eNumber,
    eBool,
    eString,
    eJson,
    eArray
};
static std::string to_string(const ValueType type) noexcept;
//=================================================================================== Value type

#endif // JSON_COMMON_H
