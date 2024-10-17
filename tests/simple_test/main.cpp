#include <SimpleAPI.h>
#include <iomanip>
#include <iostream>
#include <fstream>


int main() {
    system("tabs 4");
    Json json("{asd:\n\n\n \n15}");

    std::cout << "count: "
              << json.size()
              << std::endl;

    return 0;
}
