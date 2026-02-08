#include <SimpleAPI.h>

#include <iostream>

bool read = false;
std::string readFilePath;
bool write = false;
std::string writeFilePath;
bool test = false;
bool isTabLvl = false;
int8_t tabLvl = -1;
bool isInputJsonString = false;
std::string inputJsonString;
bool isInputArrayString = false;
std::string inputArrayString;

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
    if(isInputJsonString) {
        isInputJsonString = false;
        inputJsonString = parameter;
        return;
    }
    if(isInputArrayString) {
        isInputArrayString = false;
        inputArrayString = parameter;
        return;
    }

    if (parameter == "read" || parameter == "-r" || parameter == "--read") {
        read = true;
    } else if (parameter == "write" || parameter == "-w" || parameter == "--write") {
        write = true;
    } else if (parameter == "--test") {
        test = true;
    } else if (parameter == "--tablvl" || parameter == "-tl") {
        isTabLvl = true;
    } else if (parameter == "--inputJson" || parameter == "--iJ") {
        isInputJsonString = true;
    } else if (parameter == "--inputArray" || parameter == "--iA") {
        isInputArrayString = true;
    }

    return;
}

int main(int argc, char **argv) {
//    using namespace json;
//    system("tabs 4");

    std::cout << "args: ";
    for(int i = 0; i < argc; i++)
        std::cout << argv[i] << " ";
    std::cout << std::endl;

    //parse arguments
    for(int _argc = 1; _argc < argc && argc > 0; _argc++) {
#ifdef __DEBUG__
        std::cout << "argc[" << _argc << "]: " << argv[_argc] << std::endl;
#endif
        ParseParameter(argv[_argc]);
    }

    bool ret;
    Config json;
    if(!readFilePath.empty()) {
        json.readFile(readFilePath, ConfigFormat::eJSON);
        std::cout << "File is read: " << (!json.isEmpty() ? "true" : "false") << std::endl;
        std::cout << std::endl;
        std::cout << "Json \"" << readFilePath << "\":" << std::endl;
        std::cout << json.toString(ConfigFormat::eJSON) << std::endl;
    }

    if(test) {
        Config j;
        j.push_back("a", (double)156);
        j.push_back("b", true);
        std::cout << j.toString(ConfigFormat::eJSON) << std::endl;
//        double* dd = j.value<double>(0);

        Config ja(ValueType::eArray);
        ja.push_back(j);
        j.push_back("ja", ja);

        Config a(ValueType::eArray);
        a.push_back(true);
        a.push_back(15.0);
        a.push_back(ja);
        std::cout << a.toString(ConfigFormat::eJSON) << std::endl;
        a[0] = 21;

        Config aa(ValueType::eArray);
        aa.push_back(j);
        aa.push_back(a);
        aa.push_front((double)15.0f);
        std::cout << aa.toString(ConfigFormat::eJSON) << std::endl;

        Config el = aa[1]; //обращение к Json "j"

        double d = el["a"].getNumber();
        std::cout << "aa.j.a: (double)" << d << std::endl;

        Config jj;
        jj.push_back("aa", aa);
        jj.push_back("jjInt", (double)42);
        jj.push_back("jjString", "asde");
        std::cout << jj.toString(ConfigFormat::eJSON) << std::endl;

        //тест для каскада вложенных значений с одним get'тером
        std::vector<std::string> index_1;
        index_1.push_back("aa");  //array
        index_1.push_back("0");   //json
        index_1.push_back("a");   //array
        Config ee;
        ee = jj[index_1];
        std::array<std::string, 3> index_2({"aa", "1", "a"});
//        ee = jj[index_2]; //TODO: исправить наличие метода
        double dddd = ee.getNumber();
        std::cout << std::endl << "d: " << dddd << std::endl;
    }

    if(!writeFilePath.empty()) {
        std::cout << "File for write: " << writeFilePath << std::endl;
        ret = json.writeFile(writeFilePath, ConfigFormat::eJSON);
        std::cout << "File is written: " << (ret ? "true" : "false") << std::endl;
    }

    return 0;
}
