#include <SimpleAPI.h>
#include <iomanip>
#include <iostream>
#include <fstream>


int main() {
    system("tabs 4");
    Json json("asd", nullptr);

    std::cout << "count: "
              << json.size()
              << std::endl;

    return 0;
}
