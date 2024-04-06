#include "Json.h"

#import <fstream>
#include <iostream>

#include "utils.h"

namespace json {

std::string ToString(const NextReadState state)
{
    switch(state) {
    case eKey:          return "Key";
    case eValue:        return "Value";
    case eColon:        return "Colon";
    case eComma:        return "Comma";
    case eJsonStart:    return "Json Start";
    case eJsonEnd:      return "Json End";
    default:            return "unexpected state";
    }
}

void ChangeNextState(NextReadState &state, const NextReadState nextState)
{
    if(state == nextState) return;
//    std::cout << "state changed: "
//              << ToString(state)
//              << "->"
//              << ToString(nextState)
//              << std::endl;
    state = nextState;
}

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

    std::string temp_string;
    bool nextStrStartFromComment = false;
    std::vector<std::string> strings;
    while(getline(file, temp_string)) {
        utils::removeComments(temp_string, nextStrStartFromComment);
        strings.push_back(temp_string);
    }
    file.close();

    //обработка JSON
    return ParseJson(strings, this);
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

//PRIVATE FUNCTIONS:
bool Json::ParseJson(const std::vector<std::string>& str, Json* json)
{
    bool return_code = true;

    std::string temp_string;
    uint32_t stringCounter = 1;
    uint32_t symbolCounter = 0;
    bool isString = false;
    ValueType valueType = ValueType::eNull;
    uint8_t pointCounter = 0; //если в числе больше 1 точки
    char ch_err;

    bool exit = false;
    bool isKey = false;
    bool isValue = false;
    std::string key = "";
    std::string value = "";
    NextReadState state = NextReadState::eKey;
    for(size_t j = 0; j < str.size() && !exit; j++) {
        temp_string = str[j];
        for(size_t i = 0; i < temp_string.length() && !exit; i++) {
            //счётчик строк и символов, для вывода ошибки
            if ((temp_string[i] == '\n') || (i == 0)) {
                stringCounter++;
                symbolCounter = 0;
            } else
                symbolCounter++;
            //пропуск избыточных символов
            switch(temp_string[i]) {
            case '\t':
            case ' ':
            case '\n':
                continue;
            }

            //поиск ошибок синтаксиса
            switch(temp_string[i]) {
            case '"':   isString = !isString;   break;
            case '.':   pointCounter++;         break;
            }
            if(pointCounter > 1) {
                std::cout << "point counter > 1" << std::endl;
                return_code = false;
                ch_err = temp_string[i];
                exit = true;
                break;
            }

            //чтение данных
            switch(state) {
            case eKey: {
                if(isString) {
                    if(key.empty()) isKey = true;
                    if(isKey)       key += temp_string[i];
                } else {
                    if(isKey) {
                        //удаленение лишней кавычки
                        key.erase(0, 1);
                        std::cout << "key: " << key << std::endl;
                        key = "";
                        isKey = false;
                        ChangeNextState(state, NextReadState::eColon);
                    }
                }
                break;
            }
            case eJsonStart:
            case eJsonEnd:
            case eValue: { //может быть строкой, числом или Json
                if(value.empty())   isValue = true;

                //определяем тип переменной
                if(valueType == eNull) {
                    if(utils::isNumber(temp_string[i]))
                        valueType = ValueType::eNumber;
                    else if(temp_string[i] == '"' || temp_string[i] == '\'')
                        valueType = ValueType::eString;
                    else if(temp_string[i] == '{')
                        valueType = ValueType::eJson;
                    else if(temp_string[i] == '[')
                        valueType = ValueType::eArray;
                }

                //считываем значение
                switch(valueType) {
                case eString: {
                    if(isString) {
                        if(key.empty()) isValue = true;
                        if(isValue) value += temp_string[i];
                    } else {
                        if(isValue) {
                            //удаленение лишней кавычки
                            value.erase(0, 1);
                            std::cout << "value(str): " << value << std::endl;
                            isValue = false;
                            value = "";
                            valueType = eNull;
                            ChangeNextState(state, NextReadState::eComma); //запятая
                        }
                    }
                    break;
                }
                case eNumber: {
                    if(!utils::isNumber(temp_string[i])) {
                        std::cout << "value(num): " << value << std::endl;
                        isValue = false;
                        value = "";
                        valueType = eNull;
                        ChangeNextState(state, NextReadState::eComma); //запятая

                        i--;
                        continue;
                    }
                    if(isValue) value += temp_string[i];
                    break;
                }
                case eJson: {
                    if(isValue) value += temp_string[i];
                    if(temp_string[i] == '}') {
                        std::cout << "inner Json: " << value << std::endl;
                        Json _innerJson;
                        std::vector<std::string> _tempVector;
                        _tempVector.push_back(value);
                        return_code = ParseJson(_tempVector, &_innerJson);
                        if(!return_code) {
                            std::cout << "parse error in Json::key:" << key << std::endl;
                            exit = true;
                        }
                        value = "";
                        isValue = false;
                        valueType = eNull;
                        ChangeNextState(state, NextReadState::eComma);

                        //TODO: обработать как объект и положить в список mapJsons
                    }
                    break;
                }
                case eArray:
                default: break;
                }
                break;
            }
            case eColon: {
                if(temp_string[i] != ':') {
                    std::cout << "exp: ':'" << std::endl;
                    return_code = false;
                    ch_err = temp_string[i];
                    exit = true;
                } else
                    ChangeNextState(state, NextReadState::eValue);
                break;
            }
            case eComma: {
                if(temp_string[i] != ',') {
                    if(temp_string[i] != '}') {
                        std::cout << "exp: ','" << std::endl;
                        return_code = false;
                    }
                    ch_err = temp_string[i];
                    exit = true;
                } else
                    ChangeNextState(state, NextReadState::eKey);
                break;
            }
            }
        }
    }

    if(!return_code) {
        std::cout << "Syntax error, line:" << stringCounter
                  << " ch:" << symbolCounter
                  << " unexpected symbol: '" << ch_err << "'"
                  << std::endl;
    }

    return return_code;
}

} /// namespace json
