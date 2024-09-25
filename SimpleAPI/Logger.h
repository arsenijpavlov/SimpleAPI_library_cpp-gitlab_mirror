#ifndef LOGGER_H
#define LOGGER_H

#include <chrono>
#include <string>
#include <vector>

namespace logs {

//NOTE: \033 то же самое, что и \e
#define RESET_TEXT          "\e[0m"
#define BOLD_TEXT           "\e[1m"
#define LOW_LIGHT_TEXT      "\e[2m"
#define ITALIC_TEXT         "\e[3m"
#define UNDERLINED_TEXT     "\e[4m"

//цвета
//Foreground
#define BLACK_FG            "\e[30m"
#define GRAY_FG             "\e[90m"
#define WHITE_FG            "\e[97m"

#define RED_FG              "\e[31m"
#define GREEN_FG            "\e[32m"
#define YELLOW_FG           "\e[33m"
#define BLUE_FG             "\e[34m"
#define MAGENTA_FG          "\e[35m"
#define CYAN_FG             "\e[36m"

#define BRIGHT_GRAY_FG      "\e[37m"
#define BRIGHT_RED_FG       "\e[91m"
#define BRIGHT_GREEN_FG     "\e[92m"
#define BRIGHT_YELLOW_FG    "\e[93m"
#define BRIGHT_BLUE_FG      "\e[94m"
#define BRIGHT_MAGENTA_FG   "\e[95m"
#define BRIGHT_CYAN_FG      "\e[96m"

//Background
#define BLACK_BG            "\e[40m"
#define GRAY_BG             "\e[100m"
#define WHITE_BG            "\e[107m"

#define RED_BG              "\e[41m"
#define GREEN_BG            "\e[42m"
#define YELLOW_BG           "\e[43m"
#define BLUE_BG             "\e[44m"
#define MAGENTA_BG          "\e[45m"
#define CYAN_BG             "\e[46m"

#define BRIGHT_GRAY_BG      "\e[47m"
#define BRIGHT_RED_BG       "\e[101m"
#define BRIGHT_GREEN_BG     "\e[102m"
#define BRIGHT_YELLOW_BG    "\e[103m"
#define BRIGHT_BLUE_BG      "\e[104m"
#define BRIGHT_MAGENTA_BG   "\e[105m"
#define BRIGHT_CYAN_BG      "\e[106m"

enum COLOR {
    eRESET_TEXT,
    eBOLD_TEXT,
    eLOW_LIGHT_TEXT,
    eITALIC_TEXT,
    eUNDERLINED_TEXT,

    //Foreground
    eBLACK_FG,
    eGRAY_FG,
    eWHITE_FG,

    eRED_FG,
    eGREEN_FG,
    eYELLOW_FG,
    eBLUE_FG,
    eMAGENTA_FG,
    eCYAN_FG,

    eBRIGHT_GRAY_FG,
    eBRIGHT_RED_FG,
    eBRIGHT_GREEN_FG,
    eBRIGHT_YELLOW_FG,
    eBRIGHT_BLUE_FG,
    eBRIGHT_MAGENTA_FG,
    eBRIGHT_CYAN_FG,

    //Background
    eBLACK_BG,
    eGRAY_BG,
    eWHITE_BG,

    eRED_BG,
    eGREEN_BG,
    eYELLOW_BG,
    eBLUE_BG,
    eMAGENTA_BG,
    eCYAN_BG,

    eBRIGHT_GRAY_BG,
    eBRIGHT_RED_BG,
    eBRIGHT_GREEN_BG,
    eBRIGHT_YELLOW_BG,
    eBRIGHT_BLUE_BG,
    eBRIGHT_MAGENTA_BG,
    eBRIGHT_CYAN_BG
};

enum LEVEL {
    eERROR,
    eWARNING,
    eINFO,
    eDEBUG,
    eDEBUG2,
    eDEBUG3
};

const std::string to_string(const LEVEL level) noexcept;
const std::string to_string(const COLOR color) noexcept;

std::string to_color_string(const LEVEL level, const std::string& log_message) noexcept;
std::string to_color_string(const std::vector<COLOR>& colors, const std::string& log_message) noexcept;
std::string to_color_string(const COLOR color, const std::string& log_message) noexcept;

std::string get_time_string() noexcept;
std::string get_time_string(const long millis) noexcept;
std::string get_time_string(const std::chrono::system_clock::time_point& tp_millis) noexcept;

std::string columned(const std::string& log_message, const int column_size,
                     const bool right_align = false) noexcept;
std::string columned(const std::vector<COLOR> colors, const std::string& log_message,
                     const int column_size, const bool right_align = false) noexcept;
std::string columned(const COLOR color, const std::string& log_message,
                     const int column_size, const bool right_align = false) noexcept;
std::string columned(const LEVEL level, const std::string& log_message,
                     const int column_size, const bool right_align = false) noexcept;

}

#endif // LOGGER_H
