#ifndef LOGGER_H
#define LOGGER_H

#include <string>

namespace logs {

#define RESET_TEXT "\033[0m"
#define GREEN_TEXT "\033[32m"
#define YELLOW_TEXT "\033[33m"
#define RED_TEXT "\033[31m"
#define GREY_TEXT "\033[90m"
#define BOLD_TEXT "\033[1m"

enum LEVEL {
    eERROR,
    eWARNING,
    eINFO,
    eDEBUG
};
std::string to_string(LEVEL level);
std::string to_color_string(LEVEL level, std::string name);

std::string get_time_string();

}

#endif // LOGGER_H
