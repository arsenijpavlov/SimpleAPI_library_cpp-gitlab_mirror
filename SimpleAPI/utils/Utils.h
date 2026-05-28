#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sstream>
#include <vector>

#ifdef __DEBUG__
    #include <iostream>
    #define DEBUG_LOG(...) { std::cout << __VA_ARGS__ << std::endl; };
#else
    #define DEBUG_LOG(...)
#endif


namespace simpleapi {

namespace utils {

bool        IsNumber(const std::string& str, bool use_point = true)                     noexcept;
bool        IsNumber(const char ch, bool use_point = true)                              noexcept;

size_t      CountSymInStr(const std::string& str, const char ch)                        noexcept;
bool        CharInString(const char ch, std::string symbols)                            noexcept;
char        GetLastNotSpaceChar(const std::string& str)                                 noexcept;

std::string ToString(const long double& d)                                              noexcept; //исключить лишние знаки при выводе чисел
bool        IsBool(const std::string& str)                                              noexcept;
bool        ToBool(const std::string& str)                                              noexcept;
std::string ToString(const bool b)                                                      noexcept;
bool        OnlySpaces(const std::string& str)                                          noexcept;
std::string PrintVector(const std::vector<std::string>& strings)                        noexcept;

std::string ToHexString(const std::vector<uint8_t>& data)                               noexcept;
std::vector<uint8_t> FromHexString(std::string str)                                     noexcept;

std::string RepeatSymToStr(const char ch, const int16_t size)                           noexcept;

bool        CheckCrc8(std::vector<uint8_t>& data)                                       noexcept;
bool        CheckCrc16(std::vector<uint8_t>& data)                                      noexcept;
bool        CheckCrc32(std::vector<uint8_t>& data)                                      noexcept;

//NOTE: нет сценариев, когда это необходимо
//std::string GetEscChar(const std::string& str)                                          noexcept;
//std::string GetEscChar2(const std::string& str)                                         noexcept;
//char        GetFromEscChar(const char ch)                                               noexcept;
//std::string GetFromEscChar2(const std::string& str)                                     noexcept;
//void        UpdEscSymbols(std::string& string)                                          noexcept;
//std::string ToStringWithEsc(const std::string& str, const bool use_backslash = false)   noexcept;

size_t      GetStringCharCount(const std::string &str, bool only_visible = false);
bool        IsMultiLine(const std::string& str, const size_t column_size = 0)           noexcept;
bool        IsStringOfFloatNumber(const std::string& str, long double& result)          noexcept;
bool        IsStringOfIntNumber(const std::string& str, long int& result)               noexcept;
bool        IsStringOfUIntNumber(const std::string& str, uint64_t& result)              noexcept;

std::string AddQuotes(const std::string& str)                                           noexcept;
std::string RemoveStartTabulations(const std::string& str)                              noexcept;
void        AddTabsForFromLine(std::string& str, const size_t from_line_number,
                const int8_t tabulation_size)                                           noexcept;
void        AddStringForFromLine(std::string& str, const size_t from_line_number,
                          const std::string& added_line)                                noexcept;
void        SetStringAsOnlySpaces(std::string& str)                                     noexcept;
void        SetVisibleColumn(std::string& input_str, const size_t& column_size)         noexcept;
uint8_t     LineOfOneSymbol(std::string& input_str)                                     noexcept;

std::vector<uint8_t> FromHexStringToHex(const std::string& str)                         noexcept;

std::string GetNormalizeString(const std::string& input)                                noexcept;
bool        IsNormalizeEqual(const std::string& str_1, const std::string& str_2)        noexcept;

} // namespace utils
} // namespace simpleapi

#endif // UTILS_H
