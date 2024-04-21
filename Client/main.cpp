#include <SimpleAPI.h>

#include <iostream>

bool read = false;
std::string readFilePath;
bool write = false;
std::string writeFilePath;
bool test = false;
bool isTabLvl = false;
int8_t tabLvl = -1;

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
    if(isTabLvl) {
        isTabLvl = false;
        tabLvl = std::stoi(parameter);
        return;
    }

    if (parameter == "read" || parameter == "-r" || parameter == "--read") {
        read = true;
    } else if (parameter == "write" || parameter == "-w" || parameter == "--write") {
        write = true;
    } else if (parameter == "--test") {
        test = true;
    } else if (parameter == "--tablvl" || parameter == "-tl")
        isTabLvl = true;
}

int main(int argc, char **argv) {
    using namespace json;
    system("tabs 4");

    //parse arguments
    for(int _argc = 1; _argc < argc && argc > 0; _argc++) {
#ifdef __DEBUG__
        std::cout << "argc[" << _argc << "]: " << argv[_argc] << std::endl;
#endif
        ParseParameter(argv[_argc]);
    }

    bool ret;
    Json json;
    if(!readFilePath.empty()) {
        ret = json.readFile(readFilePath);
        std::cout << "File is read: " << (ret ? "true" : "false") << std::endl;
        std::cout << std::endl;
        std::cout << "Json \"" << readFilePath << "\":" << std::endl;
        std::cout << json.to_string(tabLvl) << std::endl;
    }

    if(test) {
        Json j;
        j.put("a", (double)156);
        j.put("b", true);
//        std::cout << j.to_string(tabLvl) << std::endl;

        Array ja;
        ja.push_back(j);
        j.put("ja", ja);

        Array a;
        a.push_back(true);
        a.push_back(15.0);
        a.push_back(ja);
//        std::cout << a.to_string(tabLvl) << std::endl;

//        std::cout << reinterpret_cast<BoolElement*>(a.getAt(0))->value << std::endl;

        Array aa;
        aa.push_back(j);
        aa.push_back(a);

//        std::cout << aa.to_string(tabLvl) << std::endl;
        Json jj;
        jj.put("aa", aa);
        jj.put("jjInt", (double)42);
        std::cout << jj.to_string(tabLvl) << std::endl;
    }
//    Json json, json2;
//    json.put("TestString", "TestValue");
//    json.put("TestNum", "999");
//    json2.put("Json2_1", "1");
//    json2.put("Json2_2", "2");
//    json.put("input_json", json2);

    if(!writeFilePath.empty()) {
        std::cout << "File for write: " << writeFilePath << std::endl;
        ret = json.writeFile(writeFilePath);
        std::cout << "File is written: " << (ret ? "true" : "false") << std::endl;

//    std::cout << std::endl;
//    std::cout << json.to_string() << std::endl;
//    std::cout << std::endl << std::endl;
    }

    return 0;
}
