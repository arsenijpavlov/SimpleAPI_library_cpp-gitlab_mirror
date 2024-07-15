#include "Logger.h"

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


}
