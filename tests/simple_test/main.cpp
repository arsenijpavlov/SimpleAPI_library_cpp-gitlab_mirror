#include <SimpleAPI.h>
#include <iomanip>
#include <iostream>
#include <fstream>


int main() {
    system("tabs 4");
    std::string str = "{\njson:\"12345\ns67890\"}";
    Json json;
    json.parseJSON(str);

    return 0;
}
