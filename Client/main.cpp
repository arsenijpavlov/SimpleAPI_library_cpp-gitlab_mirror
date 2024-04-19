#include <SimpleAPI.h>

#include <iostream>

bool read = false;
std::string readFilePath;
bool write = false;
std::string writeFilePath;
bool test = false;

void ParseParameter(std::string parameter) {
    if(read) {
        readFilePath = parameter;
        read = false;
        return;
    }
    if(write) {
        writeFilePath = parameter;
        write = false;
        return;
    }

    if (parameter == "read" || parameter == "-r" || parameter == "--read") {
        read = true;
    } else if (parameter == "write" || parameter == "-w" || parameter == "--write") {
        write = true;
    } else if (parameter == "--test") {
        test = true;
    }
}

int main(int argc, char **argv) {
    using namespace json;

    //parse arguments
    for(int _argc = 1; _argc < argc && argc > 0; _argc++) {
//        std::cout << "argc[" << _argc << "]: " << argv[_argc] << std::endl;
        ParseParameter(argv[_argc]);
    }

    bool ret;
    if(!readFilePath.empty()) {
        Json json;
        ret = json.readFile(readFilePath);
        std::cout << "File is read: " << (ret ? "true" : "false") << std::endl;
        std::cout << std::endl;
        std::cout << "Json \"" << readFilePath << "\":" << std::endl;
        std::cout << json.to_string(-1) << std::endl;
    }

    if(test) {
        Json j;
        j.put("a", (double)156);
        j.put("b", true);

        Array ja;
        j.put("ja", ja);

        Array a;
        a.push_back(true);
        a.push_back(15.0);
        a.push_back(ja);
        std::cout << a.to_string(-1) << std::endl;

        std::cout << reinterpret_cast<BoolElement*>(a.getAt(0))->value << std::endl;

        Array aa;
//        aa.push_back(j);
        aa.push_back(a);

//        std::cout << aa.to_string(-1) << std::endl;
    }
//    Json json, json2;
//    json.put("TestString", "TestValue");
//    json.put("TestNum", "999");
//    json2.put("Json2_1", "1");
//    json2.put("Json2_2", "2");
//    json.put("input_json", json2);

//    ret = json.writeFile("./test.json");
//    std::cout << "File is written: " << (ret ? "true" : "false") << std::endl;

//    std::cout << std::endl;
//    std::cout << json.to_string() << std::endl;
//    std::cout << std::endl << std::endl;


    return 0;
}
