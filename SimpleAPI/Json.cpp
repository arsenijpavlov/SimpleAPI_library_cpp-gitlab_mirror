#include "Json.h"

#include <fstream>
#include <iostream>

#include "utils.h"

namespace json {

std::string ToString(const NextReadState state)
{
    switch(state) {
    case eJsonStart:    return "JsonStart";
    case eKey:          return "Key";
    case eValue:        return "Value";
    case eColon:        return "Colon";
    case eComma:        return "Comma";
    case eJsonEnd:      return "JsonEnd";
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
    std::cout << "state changed: "
              << ToString(state)
              << "->"
              << ToString(nextState)
              << std::endl;
    state = nextState;
}

// ArrayElement
std::string ToString(Array array, int16_t tabulation_level)
{
    return ""; // TODO: ToString(Array())
}

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
    return this->jsons.insert(std::pair<std::string, Json>(key, json)).second;
}

bool Json::put(std::string key, Array value)
{
    return this->arrays.insert(std::pair<std::string, Array>(key, value)).second;
}

bool Json::readFile(const std::string path)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cout << "File not found" << std::endl;
        return false;
    }

    std::string temp_string;
    bool nextStrStartFromComment = false;
    std::string json_str;
    while(getline(file, temp_string)) {
        utils::removeComments(temp_string, nextStrStartFromComment);
        json_str += temp_string + '\n';
    }
    file.close();

    //обработка JSON
    return ParseJson(json_str, this);
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
/// Json

//STATIC:
ValueType CheckValue(std::string& value)
{
//    //поиск ошибок синтаксиса
//    switch(json_str[i]) {
//    case '"':   isString = !isString;   break;
//    case '.':   pointCounter++;         break;
//    }
//    if(pointCounter > 1) {
//        std::cout << "point counter > 1" << std::endl;
//        return_code = false;
//        ch_err = json_str[i];
//        exit = true;
//        break;
//    }
    bool isValue = false;
    std::string _value;
    char _ch;
    ValueType vType = eNull;
    for(size_t i = 0; i < value.length(); i++) {
        if(!isValue
            && value[i] != ' '
            && value[i] != '\t'
            && value[i] != '\n')
            isValue = true;
        else {
            if(vType == ValueType::eNull) {
                if(utils::isNumber(value[i]))
                    vType = ValueType::eNumber;
                else if(value[i] == '"' || value[i] == '\'') {
                    vType = ValueType::eString;
                    _ch = value[i]; //граница слова
                } else if(value[i] == '{') {
                    vType = ValueType::eJson;
                } else if(value[i] == '[') {
                    vType = ValueType::eArray;
                }
            }
            if(vType != ValueType::eNumber)
                _value += value[i];

            switch(vType) {
            case eNumber:   {

                break;
            }
            case eString:   {
                break;
            }
            case eJson:     {
                break;
            }
            case eArray:    {
                break;
            }
            default: break;
            }
        }
    }

    switch(vType) {
    case ValueType::eNumber: {
        return CheckDouble(_value) ? vType : ValueType::eNull;
    case ValueType::eString: {
        return CheckString(_value) ? vType : ValueType::eNull;
    }
    }
    }

    return vType;
}

bool CheckDouble(std::string& value)
{

    return true;
}

bool CheckString(std::string& word)
{
    std::cout << "CutWord: " << word << std::endl;
    char ch = 0;
    std::string temp;
    bool done = false;
    for(size_t i = 0; i < word.length(); i++) {
        if(ch == 0 && (word[i] == '"' || word[i] == '\''))
            ch = word[i];
        else { //начинаем запись слова
            if(!done) {
                if(word[i] != ch) //пока не встретили конец слова
                    temp += word[i];
                else {
                    done = true;
                }
            } else { //замкнули слово, надо проверить оставшиеся символы
                if(word[i] != ' '
                    && word[i] != '\n'
                    && word[i] != '\t') {
                    std::cout << "Error with parse word in: " << word << std::endl;
                    return false;
                }
            }
        }
    }

    word = temp;
    return true;
}

bool ParseJson(const std::string& json_str, Json* json)
{
//    std::cout << "ParseJson(): " << json_str << std::endl;
    if(!json) return false;
    bool return_code = true;

    uint32_t strCounter = 1;
    uint32_t chCounter = 0;
    ValueType valueType = ValueType::eNull;
    char ch_err;

    bool exit = false;
    bool isKey = false;
    bool isValue = false;
    std::string key = "";
    std::string value = "";
    NextReadState state = NextReadState::eJsonStart;
    for(size_t i = 0; i < json_str.length() && !exit; i++) {
//        std::cout << "current [" << json_str[i] << "]: "
//                  << "key: [" << key << "], "
//                  << "value: [" << value << "], "
//                  << "k:" << (isKey ? "+" : "-") << " "
//                  << "v:" << (isValue ? "+" : "-")
//                  << std::endl;

        //счётчик строк и символов, для вывода ошибки
        if ((json_str[i] == '\n') || (i == 0)) {
            strCounter++;
            chCounter = 0;
        } else
            chCounter++;

        //чтение данных
        switch(state) {
        case eJsonStart: {
            if(json_str[i] == ' '
                || json_str[i] == '\n'
                || json_str[i] == '\t')
                continue;
            else if(json_str[i] == '{')
                ChangeNextState(state, NextReadState::eKey);
            else {
                ch_err = json_str[i];
                exit = true;
            }
            break;
        }
        case eJsonEnd: {
            if(json_str[i] == ' '
                || json_str[i] == '\n'
                || json_str[i] == '\t')
                continue;
            else if(json_str[i] == '}')
                ChangeNextState(state, NextReadState::eUnknown);
            else {
                ch_err = json_str[i];
                exit = true;
            }
            break;
        }
        case eKey: {
            if(key.empty() && !isKey) {   //запись ключа не начата
                if(json_str[i] == '"' || json_str[i] == '\'') //ищем начало строкового ключа
                    isKey = true;
            } else {            //запись ключа начата
                if(isKey) {
                    if(!key.empty() && (json_str[i] == '"' || json_str[i] == '\''))
                        isKey = false;
                    key += json_str[i];
                }
                if(!isKey) { //весь ключ записан
                    if(!CutWord(key)) { //вычленяем по маске "?.*"? или '?.*'?
                        std::cout << "Error with parse key: " << key << std::endl;
                        return false;
                    }
                    std::cout << "key: " << key << std::endl;
                    ChangeNextState(state, NextReadState::eColon);
                }
            }
            break;
        }
        case eValue: { //может быть числом, строкой, Json или Array
            if(value.empty())   isValue = true;

            value += json_str[i];
            if(i + 1 < value.length()) { //следующий символ существует
                if(value[i] == ',' || value[i] == '}')
                    isValue = false;
            } else
                isValue = false;

            if(!isValue) { //это конец значения?
                ValueType vType = CheckValue(value); //NOTE: по-хорошему, надо избавиться от избыточного выполнения кода
                switch(vType) {
                case eString:   { return_code = json->put(key, value); break; }
                case eNumber:   { return_code = json->put(key, value); break; }
                case eJson:     {
                    Json _innerJson;
                    if(!ParseJson(value, &_innerJson)) {
                        std::cout << "parse error in key:" << key
                                  << "valueType:" << ToString(valueType)
                                  << std::endl;
                        exit = true;
                    } else {
                        return_code = json->put(key, _innerJson);
                    }
                    break;
                }
                case eArray:    {
                    Array _innerArray;
                    return_code = ParseArray(value, &_innerArray);
                    if(!return_code) {
                        std::cout << "parse error in key:" << key
                                  << "valueType:" << ToString(valueType)
                                  << std::endl;
                        exit = true;
                    } else
                        return_code = json->put(key, _innerArray);
                    break;
                }
                case eNull:     { //значение ещё не прочитано!
                    continue;
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
            if(json_str[i] == ' '
                || json_str[i] == '\''
                || json_str[i] == '\n')
                continue;

            if(json_str[i] != ':') {
                std::cout << "exp: ':'" << std::endl;
                return_code = false;
                ch_err = json_str[i];
                exit = true;
            } else {
                isValue = true;
                ChangeNextState(state, NextReadState::eValue);
            }
            break;
        }
        case eComma: {
            if(json_str[i] == ' '
                || json_str[i] == '\''
                || json_str[i] == '\n')
                continue;

            if(json_str[i] != ',') {
                if(json_str[i] != '}') {
                    std::cout << "exp: ','" << std::endl;
                    return_code = false;
                }
                ch_err = json_str[i];
                exit = true;
            } else
                ChangeNextState(state, NextReadState::eKey);
            break;
        }
        case eUnknown: {
            ch_err = json_str[i];
            exit = true;
            break;
        }
        }
    }

    if(!return_code) {
        std::cout << "Syntax error, line:" << strCounter
                  << " ch:" << chCounter
                  << " unexpected symbol: '" << ch_err << "'"
                  << std::endl;
    }

    return return_code;
}

bool ParseArray(const std::string& array_str, Array* array)
{
    return true; //TODO: ParseArray()
}
///STATIC

} /// namespace json
