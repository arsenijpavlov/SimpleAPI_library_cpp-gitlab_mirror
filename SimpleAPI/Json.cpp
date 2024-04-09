#include "Json.h"

#include <fstream>
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
    default:            return "unexpected state";
    }
}

std::string ToString(const ValueType type) {
    switch(type) {
    case eString:       return "String"; break;
    case eNumber:       return "Number"; break;
    case eJson:         return "Json"; break;
    case eArray:        return "Array"; break;
    default:            return "null"; break;
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

// ArrayElement
ArrayElement::~ArrayElement()
{
    delete string;
    delete number;
    delete json;
    delete array;
}

std::string ArrayElement::getString()
{
    return this->to_string();
}

double ArrayElement::getNumber()
{
    if(this->type != ValueType::eNumber || !this->number)
        return 0;
    else
        return *this->number;
}

Json ArrayElement::getJson()
{
    if(this->json)
        return *this->json;
    else
        return Json();
}

Array ArrayElement::getArray()
{
    if(this->array)
        return *this->array;
    else
        return Array();
}

std::string ArrayElement::to_string()
{
    //TODO: ArrayElement::to_string()
    return "";
}
/// ArrayElement

// Json
Json::Json()
{

}

bool Json::put(std::string key, std::string value)
{
    return this->strings.insert(std::pair<std::string, std::string>(key, value)).second;
}

bool Json::put(std::string key, double value)
{
    return this->numbers.insert(std::pair<std::string, double>(key, value)).second;
}

bool Json::put(std::string key, Json json)
{
    return this->containers.insert(std::pair<std::string, Json>(key, json)).second;
}

bool Json::put(std::string key, Array value)
{
    return this->arrays.insert(std::pair<std::string, Array>(key, value)).second;
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

std::string Json::to_string(int16_t tabulation_level)
{
    std::string ret;
    bool withoutSpaces = tabulation_level < 0;
    ret += "{"; //start of json
    if(!withoutSpaces) ret += "\n";

    if(!withoutSpaces) tabulation_level++;
    std::string tabs_str = !withoutSpaces ? utils::tab(tabulation_level) : "";
    //numbers
    std::map<std::string, double>::iterator it_num = this->numbers.begin();
    for(int i = 0; it_num != numbers.end(); i++, it_num++) {
        ret += tabs_str + "\"" + it_num->first + "\"";
        if(!withoutSpaces) ret += " ";
        ret += ":";
        if(!withoutSpaces) ret += " ";
        ret += std::to_string(it_num->second)
               + (((i < numbers.size() - 1)
                   || (strings.size() > 0)
                   || (jsons.size() > 0)
                   || (arrays.size() > 0)) ? "," : "");
        if(!withoutSpaces) ret += "\n";
    }
    //strings
    std::map<std::string, std::string>::iterator it_str = this->strings.begin();
    for(int i = 0; it_str != strings.end(); i++, it_str++) {
        ret += tabs_str + "\"" + it_str->first + "\"";
        if(!withoutSpaces) ret += " ";
        ret += ":";
        if(!withoutSpaces) ret += " ";
        ret += "\"" + it_str->second + "\""
               + (((i < strings.size() - 1)
                   || (jsons.size() > 0)
                   || (arrays.size() > 0)) ? "," : "");
        if(!withoutSpaces) ret += "\n";
    }
    //jsons
    std::map<std::string, Json>::iterator it_js = this->jsons.begin();
    for(int i = 0; it_js != jsons.end(); i++, it_js++) {
        ret += tabs_str + "\"" + it_js->first + "\" : "
               + it_js->second.to_string(tabulation_level + (!withoutSpaces ? 2 : 0))
               + (((i < jsons.size() - 1)
                   || (arrays.size() > 0)) ? "," : "");
        if(!withoutSpaces) ret += "\n";
    }
    //arrays
    std::map<std::string, Array>::iterator it_ar = this->arrays.begin();
    for(int i = 0; it_ar != arrays.end(); i++, it_ar++) {
        ret += tabs_str + "\"" + it_ar->first + "\" : "
               + ToString(it_ar->second, tabulation_level + (!withoutSpaces ? 2 : 0))
               + ((i < arrays.size() - 1) ? "," : "");
        if(!withoutSpaces) ret += "\n";
    }

    if(!withoutSpaces) tabulation_level--;
    if(!withoutSpaces) ret += utils::tab(tabulation_level);
    ret += "}"; //end of json
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
            if(!isKey && !isValue) { //
                switch(temp_string[i]) {
                case '\t':
                case ' ':
                case '\n':
                    continue;
                }
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
//                        std::cout << "key: " << key << std::endl;
                        isKey = false;
                        ChangeNextState(state, NextReadState::eColon);
                    }
                }
                break;
            }
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
                case eNumber:   {
                    if(!utils::isNumber(temp_string[i])) {
//                        std::cout << "value(num): " << value << std::endl;
                        isValue = false;

                        i--;
//                        continue;
                    }
                    if(isValue) value += temp_string[i];
                    break;
                }
                case eString:   {
                    if(isString) {
                        if(isValue) value += temp_string[i];
                    } else {
                        if(isValue) {
                            //удаленение лишней кавычки
                            value.erase(0, 1);
//                            std::cout << "value(str): " << value << std::endl;
                            isValue = false;
                        }
                    }
                    break;
                }
                case eJson:     {
                    if(isValue) value += temp_string[i];
                    if(temp_string[i] == '}') {
//                        std::cout << "inner Json: " << value << std::endl;
                        isValue = false;
                    }
                    break;
                }
                case eArray:    {
                    if(isValue) value += temp_string[i];
                    if(temp_string[i] == ']') {
//                        std::cout << "array: " << value << std::endl;
                        isValue = false;
                    }
                    break;
                }
                default: break;
                }

                if(!isValue) {
                    switch(valueType) {
                    case eString:   { return_code = json->put(key, value); break; }
                    case eNumber:   { return_code = json->put(key, value); break; }
                    case eJson:     {
                        Json _innerJson;
                        std::vector<std::string> _tempVector;
                        _tempVector.push_back(value);
                        return_code = ParseJson(_tempVector, &_innerJson);
                        if(!return_code) {
                            std::cout << "parse error in key:" << key
                                      << "valueType:" << ToString(valueType)
                                      << std::endl;
                            exit = true;
                        } else
                            return_code = json->put(key, _innerJson);
                        break;
                    }
                    case eArray:    {

                        break;
                    }
                    default: break;
                    }


                    //обнуление временных переменных, переход к следующему элементу
                    key = "";
                    value = "";
                    valueType = eNull;
                    ChangeNextState(state, NextReadState::eComma);
                }
                break;
            }
            case eColon: {
                if(temp_string[i] != ':') {
                    std::cout << "exp: ':'" << std::endl;
                    return_code = false;
                    ch_err = temp_string[i];
                    exit = true;
                } else {
                    isValue = true;
                    ChangeNextState(state, NextReadState::eValue);
                }
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
/// Json

} /// namespace json
