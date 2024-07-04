#ifndef LOGGER_H
#define LOGGER_H

#include <string>

namespace logs {

enum LEVEL {
    eERROR,
    eINFO,
    eDEBUG
};
std::string to_string(LEVEL level);

}

#endif // LOGGER_H
