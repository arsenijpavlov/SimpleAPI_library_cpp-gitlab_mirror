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
std::string ToString(double d); //исключить лишние знаки при выводе чисел
bool isBool(std::string& str);
bool ToBool(std::string& str);
bool OnlySpaces(const std::string& str);

std::string to_hex_string(const std::vector<uint8_t>& data);
std::vector<uint8_t> from_hex_string(std::string str);

bool checkCrc8(const std::vector<uint8_t>& data, std::vector<uint8_t>& crcData);
bool checkCrc16(const std::vector<uint8_t>& data, std::vector<uint8_t>& crcData);
bool checkCrc32(const std::vector<uint8_t>& data, std::vector<uint8_t>& crcData);

}

#endif // UTILS_H
