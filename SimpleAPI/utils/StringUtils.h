#pragma once

#include <string>
#include <vector>

namespace simpleapi {
namespace utils {

// вектор строк
using VString = std::vector<std::string>;
// вектор векторов строк
using VVString = std::vector<std::vector<std::string>>;

//удалить пробелы в начале и конце строки
void RemoveFrontTabsIllegalSpaces(std::string& string, const int8_t tabulation_level)   noexcept;
void RemoveFrontIllegalSpaces(std::string& string)                                      noexcept;
void RemoveEndIllegalSpaces(std::string& string)                                        noexcept;
void RemoveIllegalSpaces(std::string& string)                                           noexcept;

void RemoveQuotes(std::string& string)                                                  noexcept;

struct SplittedLines {
    VString lines;
    size_t max_length;
};
SplittedLines SplitWithoutColumned(const std::string& input_string)                     noexcept;
SplittedLines SplitToColumns(const std::string& input_string, const size_t column_size) noexcept;
std::string VStringToString(const VString& input_vec, const bool need_quotes = false)   noexcept;

SplittedLines SplitString(const std::string input_string, char split_char,
                          bool with_empty_strings = true)                               noexcept;

} // namespace utils
} // namespace simpleapi
