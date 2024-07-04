#include "Logger.h"

namespace logs {


std::string to_string(LEVEL level) {
    switch(level) {
    case eERROR:    return "[ERROR]";
    case eINFO:     return "[INFO]";
    case eDEBUG:    return "[DEBUG]";
    default:        return "[UNKNOWN]";
    }
}


}
