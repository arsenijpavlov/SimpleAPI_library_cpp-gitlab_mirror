#include "Logger.h"

#include <chrono>
#include <iomanip>

namespace logs {


std::string to_string(LEVEL level) {
    switch(level) {
    case eERROR:    return "[ERROR]";
    case eWARNING:  return "[WARNING]";
    case eINFO:     return "[INFO]";
    case eDEBUG:    return "[DEBUG]";
    default:        return "[UNKNOWN]";
    }
}

std::string to_color_string(LEVEL level, std::string name) {
    switch(level) {
    case eERROR:    return std::string(RED_TEXT)    + "[" + name + ".e]" + RESET_TEXT;
    case eWARNING:  return std::string(YELLOW_TEXT) + "[" + name + ".w]" + RESET_TEXT;
    case eINFO:     return std::string(GREEN_TEXT)  + "[" + name + ".i]" + RESET_TEXT;
    case eDEBUG:    return std::string(GREY_TEXT)   + "[" + name + ".d]" + RESET_TEXT;
    default:        return std::string(RED_TEXT)    + "[" + name + ".unknown]" + RESET_TEXT;
    }
}

std::string get_time_string()
{
    auto _now = std::chrono::system_clock::now();
    return get_time_string(_now);
}

std::string get_time_string(long millis)
{
    return get_time_string(std::chrono::system_clock::time_point{std::chrono::milliseconds{millis}});
}

std::string get_time_string(std::chrono::system_clock::time_point tp_millis)
{
    std::time_t tt = std::chrono::system_clock::to_time_t(tp_millis);
    struct tm* tmInfo = std::localtime(&tt);

    char time_buf[20];
    std::strftime(time_buf, 20, "%0H:%0M:%0S", tmInfo);
    char millis[4];
    sprintf(millis, "%03ld", (tp_millis.time_since_epoch() / std::chrono::milliseconds(1)) % 1000);

    return std::string("[") + time_buf + "." + millis + "]";
}


}
