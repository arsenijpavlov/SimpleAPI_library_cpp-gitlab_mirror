#include <SimpleAPI.h>
#include <iomanip>
#include <iostream>
#include <fstream>


int main() {
    system("tabs 4");
    Json json("{asd:\n\n\n \n15}", ConfigFormat::eJSON);
    std::cout << "count: "
              << json.size()
              << std::endl;

    json["key"] = 123;
    std::cout << "key: "
              << json["key"].to_string()
              << std::endl;
    return 0;
}
