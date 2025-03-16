#ifndef UTILS_H
#define UTILS_H

#include "../json/Comment.h"
#include <string>
#include <sstream>
#include <vector>


namespace utils {

namespace cmt {
    //первый символ в случае многострочного комментария может быть другим
    constexpr uint8_t SIZE_comment_multi_line = 6;
    constexpr char comment_multi_line[SIZE_comment_multi_line][2] {
        {'/', '*'},
        {'/', '#'},
        {'<', '#'},
        {'<', '-'},
        {'!', '.'},
        {'?', '.'}
        //нельзя использовать двойной символ из второй таблицы как границу многострочного комментария
    };
    constexpr uint8_t SIZE_comment_one_line = 6;
    constexpr char comment_one_line[SIZE_comment_one_line][2] {
        {'%', 0},
        {'#', 0},
        {'!', 0},
        {';', 0},
        {'?', 0},
        {'/', '/'}
    };
}

bool        isNumber(const std::string& str, bool use_point = true) noexcept;
bool        isNumber(const char ch, bool use_point = true) noexcept;
void        RemoveComments(std::string& str, bool& startComment, char& quote,
                    char& start_comment_sym, char& stop_comment_sym);
size_t      CountSymInStr(const std::string& str, const char ch) noexcept;
bool        CharsInString(const char ch, std::string symbols) noexcept;
std::string toString(double d) noexcept; //исключить лишние знаки при выводе чисел
bool        isBool(std::string& str) noexcept;
bool        toBool(std::string& str) noexcept;
std::string to_string(bool b) noexcept;
bool        OnlySpaces(const std::string& str) noexcept;
std::string PrintVector(const std::vector<std::string>& strings) noexcept;

std::string to_hex_string(const std::vector<uint8_t>& data) noexcept;
std::vector<uint8_t> from_hex_string(std::string str) noexcept;

std::string RepeatSymToStr(const char ch, const uint16_t size) noexcept;

bool        checkCrc8(std::vector<uint8_t>& data) noexcept;
bool        checkCrc16(std::vector<uint8_t>& data) noexcept;
bool        checkCrc32(std::vector<uint8_t>& data) noexcept;

std::string getEscChar(const std::string& str) noexcept;
std::string getEscChar2(const std::string& str) noexcept;
char        getFromEscChar(const char ch) noexcept;
std::string getFromEscChar2(const std::string& str) noexcept;
void        UpdEscSymbols(std::string& string) noexcept;
size_t      GetStringSize(const std::string& str);
std::string SeparateString(std::string& str, const size_t length) noexcept;
bool        isMultiLine(const std::string& str, const size_t column_size = 0) noexcept;
std::string to_string_with_esc(const std::string& str, const bool use_backslash = false) noexcept;

CommentType CheckComment(char& first, const char second, size_t& iter_counter) noexcept;

enum class CommentChecker : uint8_t {
    isComment, isNotComment, isCommentEnd //последний требует continue!
};

CommentChecker CheckComments(const char current_sym, const char next_sym,
                             bool& is_one_line, bool& is_multi_line,
                             char& first_ml_sym, char& second_ml_sym,
                             const bool enable_comment, std::string& current_sym_comment_line,
                             size_t& iter_counter, const bool external_flag = true);

}

#endif // UTILS_H
