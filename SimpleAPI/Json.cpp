#include "Json.h"

#import <fstream>
#include <iostream>

#include "utils.h"

Json::Json()
{

}

void Json::put(std::string key, Json json)
{
    this->containers.insert(std::pair<std::string, Json>(key, json));
}

void Json::put(std::string key, std::string value)
{
    this->values.insert(std::pair<std::string, std::string>(key, value));
}

bool Json::readFile(const std::string path)
{
    std::ifstream file(path);
    if (!file.is_open())
        return false;

    uint32_t stringCounter = 1;
    uint32_t symbolCounter = 0;

    bool return_code = true;
    std::string json_str;
    char c;
    bool isString = false;
    uint8_t innerLvl = 0;
    uint8_t pointCounter = 0; //если в числе больше 1 точки
//    while(file.get(c)) {
//        //счётчик строк и символов, для вывода ошибки
//        if (c == '\n') {
//            stringCounter++;
//            symbolCounter = 0;
//        } else
//            symbolCounter++;
//        //обработка символов
//        if(c != '\t'
//            && c != ' '
//            ) {
//            //проверка на соотвествие синтаксису:
//            switch(c) {
//            case '{':   innerLvl++;             break;
//            case '}':   innerLvl--;             break;
//            case '"':   isString = !isString;   break;
//            case '.':   pointCounter++;         break;
//            }
//            if(innerLvl < 0
//                || pointCounter > 1
//                ) {
//                return_code = false;
//                break;
//            }

//            if (c != '\n')
//                json_str += c;
//        }
//    }
    std::string temp_string;
    bool nextStrStartFromComment = false;
    while(getline(file, temp_string)) {
        std::cout << "string: " << stringCounter++ << " = ";
        utils::removeComments(temp_string, nextStrStartFromComment);
        std::cout << std::endl;
        json_str += temp_string;
    }
    std::cout << json_str << std::endl;

    file.close();
    if(!return_code) {
        std::cout << "Syntax error, line:" << stringCounter
                  << " ch:" << symbolCounter
                  << " unexpected symbol: '" << c << "'"
                  << std::endl;
    }
    return return_code;
}

bool Json::writeFile(const std::string path)
{
    std::ofstream file(path);
    if (!file.is_open())
        return false;

    file << this->to_string() << std::endl;

    file.flush();
    file.close();
    return true;
}

std::string Json::to_string(uint8_t tabulation_level)
{
    std::string ret;
    ret += "{\n"; //start of json

    tabulation_level++;
    std::string tabs_str = utils::tab(tabulation_level);
    //strings/numbers
    std::map<std::string, std::string>::iterator it = this->values.begin();
    for(int i = 0; it != values.end(); i++, it++) {
        ret += tabs_str + "\"" + it->first + "\" : "
               + (utils::isNumber(it->second) ? "" : "\"")
               + it->second
               + (utils::isNumber(it->second) ? "" : "\"")
               + ((i < values.size() - 1) || (containers.size() > 0) ? "," : "")
               + "\n";
    }
    //jsons
    std::map<std::string, Json>::iterator it_js = this->containers.begin();
    for(int i = 0; it_js != containers.end(); i++, it_js++) {
        ret += tabs_str + "\"" + it_js->first + "\" : "
               + it_js->second.to_string(tabulation_level + 2)
               + ((i < containers.size() - 1) ? "," : "")
               + "\n";
    }

    tabulation_level--;
    ret += utils::tab(tabulation_level) + "}"; //end of json
    return ret;
}
