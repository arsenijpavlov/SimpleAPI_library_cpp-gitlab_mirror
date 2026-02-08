#include <iomanip>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <SimpleAPI.h>


int main() {
    using namespace logs;
    std::cout << to_color_string({COLOR::eBRIGHT_GREEN_FG, COLOR::eBOLD_TEXT}, "Hello world! Usage SimpleAPI as CMake::add_subdirectory() usage!") << std::endl;
    return 0;
}
