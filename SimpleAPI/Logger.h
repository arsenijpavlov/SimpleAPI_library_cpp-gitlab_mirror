#ifndef LOGGER_H
#define LOGGER_H

#include <chrono>
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
std::string get_time_string(long millis);
std::string get_time_string(std::chrono::system_clock::time_point tp_millis);

}

#endif // LOGGER_H
