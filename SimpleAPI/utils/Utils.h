#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sstream>
#include <vector>

#ifdef __DEBUG__
    #include <iostream>
    #define DEBUG_LOG(...) { std::cout << __VA_ARGS__ << std::endl; };
#endif


namespace utils {

bool        IsNumber(const std::string& str, bool use_point = true)                     noexcept;
bool        IsNumber(const char ch, bool use_point = true)                              noexcept;

size_t      CountSymInStr(const std::string& str, const char ch)                        noexcept;
bool        CharInString(const char ch, std::string symbols)                            noexcept;
std::string ToString(const long double& d)                                                          noexcept; //исключить лишние знаки при выводе чисел
bool        IsBool(const std::string& str)                                                    noexcept;
bool        ToBool(const std::string& str)                                                    noexcept;
std::string ToString(const bool b)                                                            noexcept;
bool        OnlySpaces(const std::string& str)                                          noexcept;
std::string PrintVector(const std::vector<std::string>& strings)                        noexcept;

std::string ToHexString(const std::vector<uint8_t>& data)                               noexcept;
std::vector<uint8_t> FromHexString(std::string str)                                     noexcept;

std::string RepeatSymToStr(const char ch, const uint16_t size)                          noexcept;

bool        CheckCrc8(std::vector<uint8_t>& data)                                       noexcept;
bool        CheckCrc16(std::vector<uint8_t>& data)                                      noexcept;
bool        CheckCrc32(std::vector<uint8_t>& data)                                      noexcept;

std::string GetEscChar(const std::string& str)                                          noexcept;
std::string GetEscChar2(const std::string& str)                                         noexcept;
char        GetFromEscChar(const char ch)                                               noexcept;
std::string GetFromEscChar2(const std::string& str)                                     noexcept;
void        UpdEscSymbols(std::string& string)                                          noexcept;
size_t      GetStringCharCount(const std::string& str);
std::string SeparateString(std::string& str, const size_t length)                       noexcept;
bool        IsMultiLine(const std::string& str, const size_t column_size = 0)           noexcept;
std::string ToStringWithEsc(const std::string& str, const bool use_backslash = false)   noexcept;
bool        IsStringOfFloatNumber(const std::string& str, long double& result)          noexcept;
bool        IsStringOfIntNumber(const std::string& str, long int& result)               noexcept;
bool        IsStringOfUIntNumber(const std::string& str, uint64_t& result)              noexcept;

std::string AddQuotes(const std::string& str) noexcept;

}

#endif // UTILS_H
