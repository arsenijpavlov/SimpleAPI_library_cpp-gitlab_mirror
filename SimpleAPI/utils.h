#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sstream>
#include <vector>

namespace utils {

bool IsNumber(const std::string& str, bool use_point = true);
bool IsNumber(const char ch, bool use_point = true);
std::string Tab(uint8_t tabs_counter);
void RemoveComments(std::string& str, bool& startComment, char& quote);
size_t CountSymInStr(const std::string& str, const char ch);
bool CharsInString(const char ch, std::string symbols);
//исключить лишние знаки при выводе чисел
std::string ToString(double d);
bool isBool(std::string& str);
bool ToBool(std::string& str);
bool OnlySpaces(const std::string& str);
std::string to_hex_string(const std::vector<uint8_t>& data);
std::vector<uint8_t> from_hex_string(std::string str);



}

#endif // UTILS_H
