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

std::string to_color_string(LEVEL level) {
    switch(level) {
    case eERROR:    return std::string(RED_TEXT) + "[ERROR]" + RESET_TEXT;
    case eWARNING:  return std::string(YELLOW_TEXT) + "[WARNING]" + RESET_TEXT;
    case eINFO:     return std::string(GREEN_TEXT) + "[INFO]" + RESET_TEXT;
    case eDEBUG:    return std::string(GREY_TEXT) + "[DEBUG]" + RESET_TEXT;
    default:        return std::string(RED_TEXT) + "[UNKNOWN]" + RESET_TEXT;
    }
}


}
