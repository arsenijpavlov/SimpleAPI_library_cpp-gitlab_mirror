#include "Logger.h"

#include <chrono>
#include <iomanip>
#include <cstdlib>


namespace simpleapi {
namespace logs {

const std::string to_string(const LEVEL level) noexcept {
    switch(level) {
    case LEVEL::eERROR:     return "[ERROR]";
    case LEVEL::eWARNING:   return "[WARNING]";
    case LEVEL::eINFO:      return "[INFO]";
    case LEVEL::eDEBUG:     return "[DEBUG]";
    case LEVEL::eDEBUG2:    return "[DEBUG2]";
    case LEVEL::eDEBUG3:    return "[DEBUG3]";
    default:                return "[UNKNOWN]";
    }
}

const std::string to_string(const COLOR color) noexcept {
    switch(color) {
    case COLOR::eRESET_TEXT:        return "[RESET_TEXT]";
    case COLOR::eBOLD_TEXT:         return "[BOLD_TEXT]";
    case COLOR::eLOW_LIGHT_TEXT:    return "[LOW_LIGHT_TEXT]";
    case COLOR::eITALIC_TEXT:       return "[ITALIC_TEXT]";
    case COLOR::eUNDERLINED_TEXT:   return "[UNDERLINED_TEXT]";
    case COLOR::eBLACK_FG:          return "[BLACK_FG]";
    case COLOR::eGRAY_FG:           return "[GRAY_FG]";
    case COLOR::eWHITE_FG:          return "[WHITE_FG]";
    case COLOR::eRED_FG:            return "[RED_FG]";
    case COLOR::eGREEN_FG:          return "[GREEN_FG]";
    case COLOR::eYELLOW_FG:         return "[YELLOW_FG]";
    case COLOR::eBLUE_FG:           return "[BLUE_FG]";
    case COLOR::eMAGENTA_FG:        return "[MAGENTA_FG]";
    case COLOR::eCYAN_FG:           return "[CYAN_FG]";
    case COLOR::eBRIGHT_GRAY_FG:    return "[BRIGHT_GRAY_FG]";
    case COLOR::eBRIGHT_RED_FG:     return "[BRIGHT_RED_FG]";
    case COLOR::eBRIGHT_GREEN_FG:   return "[BRIGHT_GREEN_FG]";
    case COLOR::eBRIGHT_YELLOW_FG:  return "[BRIGHT_YELLOW_FG]";
    case COLOR::eBRIGHT_BLUE_FG:    return "[BRIGHT_BLUE_FG]";
    case COLOR::eBRIGHT_MAGENTA_FG: return "[BRIGHT_MAGENTA_FG]";
    case COLOR::eBRIGHT_CYAN_FG:    return "[BRIGHT_CYAN_FG]";
    case COLOR::eBLACK_BG:          return "[BLACK_BG]";
    case COLOR::eGRAY_BG:           return "[GRAY_BG]";
    case COLOR::eWHITE_BG:          return "[WHITE_BG]";
    case COLOR::eRED_BG:            return "[RED_BG]";
    case COLOR::eGREEN_BG:          return "[GREEN_BG]";
    case COLOR::eYELLOW_BG:         return "[YELLOW_BG]";
    case COLOR::eBLUE_BG:           return "[BLUE_BG]";
    case COLOR::eMAGENTA_BG:        return "[MAGENTA_BG]";
    case COLOR::eCYAN_BG:           return "[CYAN_BG]";
    case COLOR::eBRIGHT_GRAY_BG:    return "[BRIGHT_GRAY_BG]";
    case COLOR::eBRIGHT_RED_BG:     return "[BRIGHT_RED_BG]";
    case COLOR::eBRIGHT_GREEN_BG:   return "[BRIGHT_GREEN_BG]";
    case COLOR::eBRIGHT_YELLOW_BG:  return "[BRIGHT_YELLOW_BG]";
    case COLOR::eBRIGHT_BLUE_BG:    return "[BRIGHT_BLUE_BG]";
    case COLOR::eBRIGHT_MAGENTA_BG: return "[BRIGHT_MAGENTA_BG]";
    case COLOR::eBRIGHT_CYAN_BG:    return "[BRIGHT_CYAN_BG]";
    default:                        return std::string("\e[") + std::to_string(static_cast<uint8_t>(color)) + "m";
    }
}

std::string to_color_string(const LEVEL level, const std::string& log_message) noexcept {
    COLOR color;
    switch(level) {
    case LEVEL::eERROR:     color = COLOR::eRED_FG;     break;
    case LEVEL::eWARNING:   color = COLOR::eYELLOW_FG;  break;
    case LEVEL::eINFO:      color = COLOR::eGREEN_FG;   break;
    case LEVEL::eDEBUG:     color = COLOR::eGRAY_FG;    break;
    case LEVEL::eDEBUG2:    color = COLOR::eGRAY_FG;    break;
    case LEVEL::eDEBUG3:    color = COLOR::eGRAY_FG;    break;
    default:                color = COLOR::eRED_FG;     break;
    }

    return to_color_string(Color(color), log_message);
}

std::string to_color_string(const Color color, const std::string &log_message) noexcept
{
    return to_color_string(std::vector<Color>{color}, log_message);
}

std::string to_color_string(const std::vector<Color>& colors, const std::string& log_message) noexcept {
    std::string result_string = "";
    for(const Color& clr : colors) {
        result_string += clr.getColorString();
    }
    result_string += log_message + Color(COLOR::eRESET_TEXT).getColorString();
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

std::string columned(const std::vector<Color> colors, const std::string &log_message,
                     const int column_size, const bool right_align) noexcept
{ return to_color_string(colors, columned(log_message, column_size, right_align)); }

std::string columned(const Color color, const std::string &log_message,
                     const int column_size, const bool right_align) noexcept
{ return columned(std::vector<Color>{color}, log_message, column_size, right_align); }

std::string columned(const LEVEL level, const std::string &log_message,
                     const int column_size, const bool right_align) noexcept
{ return to_color_string(level, columned(log_message, column_size, right_align)); }

} // namespace logs
} // namespace simpleapi


