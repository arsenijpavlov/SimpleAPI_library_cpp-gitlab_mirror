#pragma once

#include <chrono>
#include <string>
#include <vector>


namespace simpleapi {

//NOTE: https://stackoverflow.com/questions/4842424/list-of-ansi-color-escape-sequences
//      гайд по цветовому выводу

namespace logs {

enum class COLOR {
    //формат вывода
    eRESET_TEXT          = 0,
    eBOLD_TEXT           = 1,
    eLOW_LIGHT_TEXT      = 2,
    eITALIC_TEXT         = 3,
    eUNDERLINED_TEXT     = 4,

    //цвета
    //Foreground
    eBLACK_FG            = 30,
    eGRAY_FG             = 90,
    eWHITE_FG            = 97,

    eRED_FG              = 31,
    eGREEN_FG            = 32,
    eYELLOW_FG           = 33,
    eBLUE_FG             = 34,
    eMAGENTA_FG          = 35,
    eCYAN_FG             = 36,

    eBRIGHT_GRAY_FG      = 37,
    eBRIGHT_RED_FG       = 91,
    eBRIGHT_GREEN_FG     = 92,
    eBRIGHT_YELLOW_FG    = 93,
    eBRIGHT_BLUE_FG      = 94,
    eBRIGHT_MAGENTA_FG   = 95,
    eBRIGHT_CYAN_FG      = 96,

    //Background
    eBLACK_BG            = 40,
    eGRAY_BG             = 100,
    eWHITE_BG            = 107,

    eRED_BG              = 41,
    eGREEN_BG            = 42,
    eYELLOW_BG           = 43,
    eBLUE_BG             = 44,
    eMAGENTA_BG          = 45,
    eCYAN_BG             = 46,

    eBRIGHT_GRAY_BG      = 47,
    eBRIGHT_RED_BG       = 101,
    eBRIGHT_GREEN_BG     = 102,
    eBRIGHT_YELLOW_BG    = 103,
    eBRIGHT_BLUE_BG      = 104,
    eBRIGHT_MAGENTA_BG   = 105,
    eBRIGHT_CYAN_BG      = 106
};
const std::string to_string(const COLOR color) noexcept;

enum class ColorType {
    eForeground = 38,
    eBackground = 48
};
class Color {
    std::string m_color_string;
public:
    //NOTE: \033 то же самое, что и \e
    Color(const ColorType color_type, const uint8_t color_as_num) {
        m_color_string = std::string("\e[")
                         + std::to_string(static_cast<uint8_t>(color_type)) + ";5;"
                         + std::to_string(static_cast<uint8_t>(color_as_num))
                         + "m";
    }
    Color(const COLOR color_as_define) {
        m_color_string = std::string("\e[")
                         + std::to_string(static_cast<uint8_t>(color_as_define))
                         + "m";
    }
    Color(const ColorType color_type, const uint8_t pigment_R,
          const uint8_t pigment_G, const uint8_t pigment_B)
    {
        m_color_string = std::string("\e[")
                         + std::to_string(static_cast<uint8_t>(color_type)) + ";2;"
                         + std::to_string(static_cast<uint8_t>(pigment_R)) + ";"
                         + std::to_string(static_cast<uint8_t>(pigment_G)) + ";"
                         + std::to_string(static_cast<uint8_t>(pigment_B))
                         + "m";
    }

    std::string getColorString() const noexcept     { return m_color_string; }
};

enum class LEVEL {
    eERROR,
    eWARNING,
    eINFO,
    eDEBUG,
    eDEBUG2,
    eDEBUG3
};
const std::string to_string(const LEVEL level) noexcept;


#define __TYPE_NOT_STRING__(ARG) \
    template<typename ARG, typename std::enable_if<!std::is_convertible<ARG, std::string>::value>::type* = nullptr>
//-----------------------------------------------------------------------------------------
std::string to_color_string(const LEVEL level, const std::string& log_message) noexcept;
__TYPE_NOT_STRING__(T)
std::string to_color_string(const LEVEL level, const T& log_message) noexcept
                                { return to_color_string(level, std::to_string(log_message)); }
//-----------------------------------------------------------------------------------------
std::string to_color_string(const std::vector<Color>& colors, const std::string& log_message) noexcept;
__TYPE_NOT_STRING__(T)
std::string to_color_string(const std::vector<Color>& colors, const T& log_message) noexcept
                                { return to_color_string(colors, std::to_string(log_message)); }
//-----------------------------------------------------------------------------------------
std::string to_color_string(const Color color, const std::string& log_message) noexcept;
__TYPE_NOT_STRING__(T)
std::string to_color_string(const Color color, const T& log_message) noexcept
                                { return to_color_string(color, std::to_string(log_message)); }
//-----------------------------------------------------------------------------------------

std::string get_time_string() noexcept;
std::string get_time_string(const long millis) noexcept;
std::string get_time_string(const std::chrono::system_clock::time_point& tp_millis) noexcept;

//-----------------------------------------------------------------------------------------
std::string columned(const std::string& log_message, const int column_size,
                     const bool right_align = false) noexcept;
__TYPE_NOT_STRING__(T)
std::string columned(const T& log_message, const int column_size,
                     const bool right_align = false) noexcept
                                { return columned(std::to_string(log_message), column_size, right_align); }
//-----------------------------------------------------------------------------------------
std::string columned(const std::vector<Color> colors, const std::string& log_message,
                     const int column_size, const bool right_align = false) noexcept;
__TYPE_NOT_STRING__(T)
std::string columned(const std::vector<Color> colors, const T& log_message,
                     const int column_size, const bool right_align = false) noexcept
                                { return columned(colors, std::to_string(log_message), column_size, right_align); }
//-----------------------------------------------------------------------------------------
std::string columned(const COLOR color, const std::string& log_message,
                     const int column_size, const bool right_align = false) noexcept;
__TYPE_NOT_STRING__(T)
std::string columned(const COLOR color, const T& log_message,
                     const int column_size, const bool right_align = false) noexcept
                                { return columned(color, std::to_string(log_message), column_size, right_align); }
//-----------------------------------------------------------------------------------------
std::string columned(const LEVEL level, const std::string& log_message,
                     const int column_size, const bool right_align = false) noexcept;
__TYPE_NOT_STRING__(T)
std::string columned(const LEVEL level, const T& log_message,
                     const int column_size, const bool right_align = false) noexcept
                                { return columned(level, std::to_string(log_message), column_size, right_align); }
//-----------------------------------------------------------------------------------------

} // namespace logs

} // namespace simpleapi
