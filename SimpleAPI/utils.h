#ifndef UTILS_H
#define UTILS_H

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

bool        isNumber(const std::string& str, bool use_point = true);
bool        isNumber(const char ch, bool use_point = true);
void        RemoveComments(std::string& str, bool& startComment, char& quote,
                    char& start_comment_sym, char& stop_comment_sym);
size_t      CountSymInStr(const std::string& str, const char ch);
bool        CharsInString(const char ch, std::string symbols);
std::string toString(double d); //исключить лишние знаки при выводе чисел
bool        isBool(std::string& str);
bool        toBool(std::string& str);
bool        OnlySpaces(const std::string& str);

std::string to_hex_string(const std::vector<uint8_t>& data);
std::vector<uint8_t> from_hex_string(std::string str);

std::string RepeatSymToStr(const char ch, const uint16_t size);

bool        checkCrc8(std::vector<uint8_t>& data);
bool        checkCrc16(std::vector<uint8_t>& data);
bool        checkCrc32(std::vector<uint8_t>& data);

}

#endif // UTILS_H
