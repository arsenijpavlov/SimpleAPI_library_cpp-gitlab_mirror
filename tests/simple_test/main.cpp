#include <SimpleAPI.h>
#include <iomanip>
#include <iostream>
#include <fstream>


int main() {
    system("tabs 4");
    std::string str = "json= {json:[a, b, c]}";
    std::vector<std::string> keys = parseIniKeys(str);

    for(std::string key : keys)
        std::cout << "[" + key + "]" << std::endl;

    return 0;
}
