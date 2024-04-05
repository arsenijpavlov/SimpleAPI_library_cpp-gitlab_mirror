#include <SimpleAPI.h>

#include <iostream>

int main(int argc, char **argv) {
//    Json json, json2;
//    json.put("TestString", "TestValue");
//    json.put("TestNum", "999");
//    json2.put("Json2_1", "1");
//    json2.put("Json2_2", "2");
//    json.put("input_json", json2);

    bool ret;
//    ret = json.writeFile("./test.json");
//    std::cout << "File is written: " << (ret ? "true" : "false") << std::endl;

//    std::cout << std::endl;
//    std::cout << json.to_string() << std::endl;
//    std::cout << std::endl << std::endl;

    Json json3;
    ret = json3.readFile("./test.json");
    std::cout << "File is read: " << (ret ? "true" : "false") << std::endl;
    std::cout << std::endl;
    std::cout << "Json3:" << std::endl;
    std::cout << json3.to_string() << std::endl;

    return 0;
}
