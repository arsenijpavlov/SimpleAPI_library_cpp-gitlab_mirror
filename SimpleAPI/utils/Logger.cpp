#include "Logger.h"

#include <chrono>
#include <iomanip>

namespace logs {

const std::string to_string(const LEVEL level) noexcept {
    switch(level) {
    case eERROR:    return "[ERROR]";
    case eWARNING:  return "[WARNING]";
    case eINFO:     return "[INFO]";
    case eDEBUG:    return "[DEBUG]";
    case eDEBUG2:   return "[DEBUG2]";
    case eDEBUG3:   return "[DEBUG3]";
    default:        return "[UNKNOWN]";
    }
}

const std::string to_string(const COLOR color) noexcept {
    switch(color) {
    case eRESET_TEXT:           return RESET_TEXT;
    case eBOLD_TEXT:            return BOLD_TEXT;
    case eLOW_LIGHT_TEXT:       return LOW_LIGHT_TEXT;
    case eITALIC_TEXT:          return ITALIC_TEXT;
    case eUNDERLINED_TEXT:      return UNDERLINED_TEXT;
    case eBLACK_FG:             return BLACK_FG;
    case eGRAY_FG:              return GRAY_FG;
    case eWHITE_FG:             return WHITE_FG;
    case eRED_FG:               return RED_FG;
    case eGREEN_FG:             return GREEN_FG;
    case eYELLOW_FG:            return YELLOW_FG;
    case eBLUE_FG:              return BLUE_FG;
    case eMAGENTA_FG:           return MAGENTA_FG;
    case eCYAN_FG:              return CYAN_FG;
    case eBRIGHT_GRAY_FG:       return BRIGHT_GRAY_FG;
    case eBRIGHT_RED_FG:        return BRIGHT_RED_FG;
    case eBRIGHT_GREEN_FG:      return BRIGHT_GREEN_FG;
    case eBRIGHT_YELLOW_FG:     return BRIGHT_YELLOW_FG;
    case eBRIGHT_BLUE_FG:       return BRIGHT_BLUE_FG;
    case eBRIGHT_MAGENTA_FG:    return BRIGHT_MAGENTA_FG;
    case eBRIGHT_CYAN_FG:       return BRIGHT_CYAN_FG;
    case eBLACK_BG:             return BLACK_BG;
    case eGRAY_BG:              return GRAY_BG;
    case eWHITE_BG:             return WHITE_BG;
    case eRED_BG:               return RED_BG;
    case eGREEN_BG:             return GREEN_BG;
    case eYELLOW_BG:            return YELLOW_BG;
    case eBLUE_BG:              return BLUE_BG;
    case eMAGENTA_BG:           return MAGENTA_BG;
    case eCYAN_BG:              return CYAN_BG;
    case eBRIGHT_GRAY_BG:       return BRIGHT_GRAY_BG;
    case eBRIGHT_RED_BG:        return BRIGHT_RED_BG;
    case eBRIGHT_GREEN_BG:      return BRIGHT_GREEN_BG;
    case eBRIGHT_YELLOW_BG:     return BRIGHT_YELLOW_BG;
    case eBRIGHT_BLUE_BG:       return BRIGHT_BLUE_BG;
    case eBRIGHT_MAGENTA_BG:    return BRIGHT_MAGENTA_BG;
    case eBRIGHT_CYAN_BG:       return BRIGHT_CYAN_BG;
    default:                    return "";
    }
}

std::string to_color_string(const LEVEL level, const std::string& log_message) noexcept {
    COLOR color;
    switch(level) {
    case eERROR:    color = eRED_FG;     break;
    case eWARNING:  color = eYELLOW_FG;  break;
    case eINFO:     color = eGREEN_FG;   break;
    case eDEBUG:    color = eGRAY_FG;    break;
    case eDEBUG2:   color = eGRAY_FG;    break;
    case eDEBUG3:   color = eGRAY_FG;    break;
    default:        color = eRED_FG;     break;
    }

    return to_color_string(color, log_message);
}

std::string to_color_string(const COLOR color, const std::string &log_message) noexcept
{ return to_color_string(std::vector<COLOR>{color}, log_message); }

std::string to_color_string(const std::vector<COLOR>& colors, const std::string& log_message) noexcept {
    std::string result_string = "";
    for(COLOR clr : colors)
        result_string += to_string(clr);
    result_string += log_message + to_string(eRESET_TEXT);
    return result_string;
}

std::string get_time_string() noexcept {
    auto _now = std::chrono::system_clock::now();
    return get_time_string(_now);
}

std::string get_time_string(const long millis) noexcept {
    return get_time_string(std::chrono::system_clock::time_point{std::chrono::milliseconds{millis}});
}

std::string get_time_string(const std::chrono::system_clock::time_point& tp_millis) noexcept {
    std::time_t tt = std::chrono::system_clock::to_time_t(tp_millis);
    struct tm* tmInfo = std::localtime(&tt);

    char time_buf[20];
    std::strftime(time_buf, 20, "%0H:%0M:%0S", tmInfo);
    char millis[5];
    sprintf(millis, "%03ld", (tp_millis.time_since_epoch() / std::chrono::milliseconds(1)) % 1000);

    return std::string("[") + time_buf + "." + millis + "]";
}

std::string columned(const std::string &log_message, const int column_size,
                     const bool right_align) noexcept {
    if(column_size == -1)                   return log_message;
    if(column_size <= log_message.size())   return log_message;

    std::string _result = "";
    int _spaceSize = abs(static_cast<int>(log_message.size()) - column_size);

    for(int i = 0; i < _spaceSize; i++)
        _result += " ";

    if(right_align) return _result + log_message;
    else            return log_message + _result;
}

std::string columned(const std::vector<COLOR> colors, const std::string &log_message,
                     const int column_size, const bool right_align) noexcept
{ return to_color_string(colors, columned(log_message, column_size, right_align)); }

std::string columned(const COLOR color, const std::string &log_message,
                     const int column_size, const bool right_align) noexcept
{ return columned(std::vector<COLOR>{color}, log_message, column_size, right_align); }

std::string columned(const LEVEL level, const std::string &log_message,
                     const int column_size, const bool right_align) noexcept
{ return to_color_string(level, columned(log_message, column_size, right_align)); }



}
