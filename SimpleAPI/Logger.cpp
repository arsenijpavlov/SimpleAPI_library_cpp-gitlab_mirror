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

std::string get_time_string(std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<long, std::ratio<1, 1000000000> > > tp_millis)
{
    std::time_t tt = std::chrono::system_clock::to_time_t(tp_millis);
    struct tm* tmInfo = std::localtime(&tt);

    char hours[3];
    sprintf(hours, "%02d", tmInfo->tm_hour);
    char minutes[3];
    sprintf(minutes, "%02d", tmInfo->tm_min);
    char seconds[3];
    sprintf(seconds, "%02d", tmInfo->tm_sec);
    char millis[4];
    sprintf(millis, "%03ld", tp_millis.time_since_epoch().count() % 1000);

    return std::string("[")
           + hours + ":"
           + minutes + ":"
           + seconds + "."
           + millis
           + "]";
}


}
