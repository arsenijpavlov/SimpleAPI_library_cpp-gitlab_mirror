#include "Json.h"

#include <fstream>
#include <iostream>

namespace json {
#define SPACES " \n\t"

enum NextReadState {
    eUnknown,

    eKey,
    eValue,
    eColon,
    eComma,
    eJsonStart,
    eJsonEnd,
    eArrayStart,
    eArrayEnd
};

std::string ToString(const NextReadState state)
{
    switch(state) {
    case eKey:          return "Key";
    case eValue:        return "Value";
    case eColon:        return "Colon";
    case eComma:        return "Comma";
    case eJsonStart:    return "JsonStart";
    case eJsonEnd:      return "JsonEnd";
    case eArrayStart:   return "ArrayStart";
    case eArrayEnd:     return "ArrayEnd";
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

std::string ToString(const ValueType type) {
    switch(type) {
    case eNumber:       return "Number"; break;
    case eBool:         return "Bool"; break;
    case eString:       return "String"; break;
    case eJson:         return "Json"; break;
    case eArray:        return "Array"; break;
    default:            return "null"; break;
    }
}


//ELEMENT
double* Element::getNum()
{
    if(first == ValueType::eNumber)
        return &reinterpret_cast<DoubleElement*>(this->second)->value;
    else return nullptr;
}

bool* Element::getBool()
{
    if(first == ValueType::eBool)
        return &reinterpret_cast<BoolElement*>(this->second)->value;
    else return nullptr;
}

std::string* Element::getString()
{
    if(first == ValueType::eString)
        return &reinterpret_cast<StringElement*>(this->second)->value;
    else return nullptr;
}

Json* Element::getJson()
{
    if(first == ValueType::eJson)
        return &reinterpret_cast<JsonElement*>(this->second)->value;
    else return nullptr;
}

Array* Element::getArray()
{
    if(first == ValueType::eArray)
        return &reinterpret_cast<ArrayElement*>(this->second)->value;
    else return nullptr;
}
///ELEMENT


//ARRAY
Array::Array(const Array& array)
{
    for(Element el : array.values) {
        switch(el.first) {
        case eNumber:
            this->values.push_back(Element(
                el.first, reinterpret_cast<BaseElement*>(
                    new DoubleElement(*reinterpret_cast<DoubleElement*>(el.second)))));
            break;
        case eBool:
            this->values.push_back(Element(
                el.first, reinterpret_cast<BaseElement*>(
                    new BoolElement(*reinterpret_cast<BoolElement*>(el.second)))));
            break;
        case eString:
            this->values.push_back(Element(
                el.first, reinterpret_cast<BaseElement*>(
                    new StringElement(*reinterpret_cast<StringElement*>(el.second)))));
            break;
        case eJson:
            this->values.push_back(Element(
                el.first, reinterpret_cast<BaseElement*>(
                    new JsonElement(*reinterpret_cast<JsonElement*>(el.second)))));
            break;
        case eArray:
            this->values.push_back(Element(
                el.first, reinterpret_cast<BaseElement*>(
                    new ArrayElement(*reinterpret_cast<ArrayElement*>(el.second)))));
            break;
        case eNull:     break;
        }
    }
}

Array::~Array()
{
    for(Element element : this->values) {
        switch(element.first) {
        case eNumber:   delete reinterpret_cast<DoubleElement*>(element.second);    break;
        case eBool:     delete reinterpret_cast<BoolElement*>(element.second);      break;
        case eString:   delete reinterpret_cast<StringElement*>(element.second);    break;
        case eJson:     delete reinterpret_cast<JsonElement*>(element.second);      break;
        case eArray:    delete reinterpret_cast<ArrayElement*>(element.second);     break;
        default: break;
        }
    }
}


void Array::push_back(double d)
{
    this->values.push_back(Element(
        ValueType::eNumber, reinterpret_cast<BaseElement*>(new DoubleElement(d))));
}

void Array::push_back(bool b)
{
    this->values.push_back(Element(
        ValueType::eBool, reinterpret_cast<BaseElement*>(new BoolElement(b))));
}

void Array::push_back(std::string string)
{
    this->values.push_back(Element(
        ValueType::eString, reinterpret_cast<BaseElement*>(new StringElement(string))));
}

void Array::push_back(Json& json)
{
    this->values.push_back(Element(
        ValueType::eJson, reinterpret_cast<BaseElement*>(new JsonElement(json))));
}

void Array::push_back(Array& array)
{
    this->values.push_back(Element(
        ValueType::eArray, reinterpret_cast<BaseElement*>(new ArrayElement(array))));
}

//void Array::push_front(double d)
//{
//    this->values.push_back(Element(
//        ValueType::eNumber, reinterpret_cast<BaseElement*>(new DoubleElement(d))));
//}

//void Array::push_front(bool b)
//{
//    this->values.push_back(Element(
//        ValueType::eBool, reinterpret_cast<BaseElement*>(new BoolElement(b))));
//}

//void Array::push_front(std::string string)
//{
//    this->values.push_back(Element(
//        ValueType::eString, reinterpret_cast<BaseElement*>(new StringElement(string))));
//}

//void Array::push_front(Json& json)
//{
//    this->values.push_back(Element(
//        ValueType::eJson, reinterpret_cast<BaseElement*>(new JsonElement(json))));
//}

//void Array::push_front(Array& array)
//{
//    this->values.push_back(Element(
//        ValueType::eArray, reinterpret_cast<BaseElement*>(new ArrayElement(array))));
//}

std::string Array::to_string(int16_t tabulation_level)
{
    if(this->values.empty()) return "[]";

    std::string ret;
    bool withoutSpaces = tabulation_level < 0;
    ret += "[";

    if(this->values.size() == 1) {
        if(!withoutSpaces) ret += " ";
        ret += this->values[0].second->to_string(tabulation_level);
        if(!withoutSpaces) ret += " ";
    } else {
        if(!withoutSpaces) ret += "\n";
        std::string tabs_str = !withoutSpaces ? utils::Tab(++tabulation_level) : "";

        for(size_t i = 0; i < this->values.size(); i++) {
            if(!withoutSpaces) ret += tabs_str;
            ret += this->values[i].second->to_string(tabulation_level);
            if(i < this->values.size() - 1) ret += ",";
            if(!withoutSpaces) ret += "\n";
        }

        if(!withoutSpaces) ret += utils::Tab(--tabulation_level);
    }

    ret += "]";

    return ret;
}

Element Array::operator[](std::vector<std::string> complex_name) {
    if(this->values.empty()) return {};

    std::vector<std::string>::iterator it = complex_name.begin();
    if(!utils::IsNumber(*it++, false))
        return {};
    Element el = (*this)[stoi(*it)]; //находим первый элемент списка
    for (; el.first != ValueType::eNull && it != complex_name.end(); it++) {
        bool isNumber = utils::IsNumber(*it, false);
        switch(el.first) {
        case eJson:
            el = (reinterpret_cast<JsonElement*>(el.second))->value[*it];
            if(el.first == ValueType::eNull) {
                if(isNumber)
                    el = (reinterpret_cast<JsonElement*>(el.second))->value[stoi(*it)];
                else
                    el = {};
            }
            break;
        case eArray:
            //для массива возможно обращение только по числовому индексу!
            if(isNumber)
                el = (reinterpret_cast<ArrayElement*>(el.second))->value[stoi(*it)];
            else
                el = {};
            break;
        default: return {}; //продолжать поиск можно только по двум структурам!
        }
    }

    return el;
}
///ARRAY

// Json
Json::Json(const Json& json)
{
    for(const std::pair<std::string, Element> &el : json.values) {
        switch(el.second.first) {
        case eNumber: {
            double value = reinterpret_cast<DoubleElement*>(el.second.second)->value;
            this->values.push_back(std::make_pair(
                el.first,
                Element(el.second.first, reinterpret_cast<BaseElement*>(new DoubleElement(value)))));
            break;
        }
        case eBool: {
            bool value = reinterpret_cast<BoolElement*>(el.second.second)->value;
            this->values.push_back(std::make_pair(
                el.first,
                Element(el.second.first, reinterpret_cast<BaseElement*>(new BoolElement(value)))));
            break;
        }
        case eString: {
            std::string value = reinterpret_cast<StringElement*>(el.second.second)->value;
            this->values.push_back(std::make_pair(
                el.first,
                Element(el.second.first, reinterpret_cast<BaseElement*>(new StringElement(value)))));
            break;
        }
        case eJson: {
            Json value = reinterpret_cast<JsonElement*>(el.second.second)->value;
            this->values.push_back(std::make_pair(
                el.first,
                Element(el.second.first, reinterpret_cast<BaseElement*>(new JsonElement(value)))));
            break;
        }
        case eArray: {
            Array value = reinterpret_cast<ArrayElement*>(el.second.second)->value;
            this->values.push_back(std::make_pair(
                el.first,
                Element(el.second.first, reinterpret_cast<BaseElement*>(new ArrayElement(value)))));
            break;
        }
        case eNull: break;
        }
    }
}

Json::~Json()
{
    for(std::pair<std::string, Element>& el : this->values) {
        switch(el.second.first) {
        case eNumber:   delete reinterpret_cast<DoubleElement*> (el.second.second);    break;
        case eBool:     delete reinterpret_cast<BoolElement*>   (el.second.second);    break;
        case eString:   delete reinterpret_cast<StringElement*> (el.second.second);    break;
        case eJson:     delete reinterpret_cast<JsonElement*>   (el.second.second);    break;
        case eArray:    delete reinterpret_cast<ArrayElement*>  (el.second.second);    break;
        default: break;
        }
    }
}

bool Json::put(const std::string key, const double value)    
{
    bool ret = true;
    this->values.push_back(std::pair<std::string, Element>(
        key,
        Element(ValueType::eNumber,
                reinterpret_cast<BaseElement*>(new DoubleElement(value)))));
    return ret;
}

bool Json::put(const std::string key, const bool value)
{
    bool ret = true;
    this->values.push_back(std::pair<std::string, Element>(
        key,
        Element(ValueType::eBool,
                reinterpret_cast<BaseElement*>(new BoolElement(value)))));
    return ret;
}

bool Json::put(const std::string key, const std::string value)
{
    this->values.push_back(std::pair<std::string, Element>(
        key,
        Element(ValueType::eString,
                reinterpret_cast<BaseElement*>(new StringElement(value)))));
    return true;
}

bool Json::put(const std::string key, const Json& value)
{
    bool ret = true;
    this->values.push_back(std::pair<std::string, Element>(
        key,
        Element(ValueType::eJson,
                reinterpret_cast<BaseElement*>(new JsonElement(value)))));
    return ret;
}

bool Json::put(const std::string key, const Array& value)
{
    //TODO: экранирование спецсимволов, как минимум кавычек
    bool ret = true;
    this->values.push_back(std::pair<std::string, Element>(
        key,
        Element(ValueType::eArray,
                reinterpret_cast<BaseElement*>(new ArrayElement(value)))));
    return ret;
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
    char quote = 0;
    std::string json_str;
    while(getline(file, temp_string)) {
        utils::RemoveComments(temp_string, nextStrStartFromComment, quote);
        if(!temp_string.empty())
            json_str += temp_string + '\n';
    }
    file.close();

    //обработка JSON
    return ParseJson(json_str, this);
}

bool Json::writeFile(const std::string path, int16_t tabulation_level)
{
    std::ofstream file(path);
    if (!file.is_open())
        return false;

    file << this->to_string(tabulation_level) << std::endl;

    file.flush();
    file.close();
    return true;
}

std::string Json::to_string(int16_t tabulation_level)
{
    std::string ret;
    bool withoutSpaces = tabulation_level < 0;
    ret += "{"; //start of json

    if(this->values.size() == 1) {
        if(!withoutSpaces) ret += " ";
        ret += "\"" + this->values[0].first + "\"";
        if(!withoutSpaces) ret += " ";
        ret += ":";
        if(!withoutSpaces) ret += " ";
        ret += this->values[0].second.second->to_string(tabulation_level);
        if(!withoutSpaces) ret += " ";
    } else {
        if(!withoutSpaces) ret += "\n";

        std::string tabs_str = !withoutSpaces ? utils::Tab(++tabulation_level) : "";

        size_t i = 0;
        for(std::pair<std::string, Element>& el : this->values) {
            ret += tabs_str + "\"" + el.first + "\"";
            if(!withoutSpaces) ret += " ";
            ret += ":";
            if(!withoutSpaces) ret += " ";
            ret += el.second.second->to_string(tabulation_level);
            if(i < this->values.size() - 1) ret += ",";
            if(!withoutSpaces) ret += "\n";
            i++;
        }

        if(!withoutSpaces) ret += utils::Tab(--tabulation_level);
    }

    ret += "}"; //end of json
    return ret;
}

Element Json::operator[](std::vector<std::string> complex_name) {
    if(this->values.empty()) return {};

    Element el = (*this)[complex_name[0]]; //находим первый элемент списка
    std::vector<std::string>::iterator it = complex_name.begin() + 1; //первый элемент пропускаем
    for (; el.first != ValueType::eNull && it != complex_name.end(); it++) {
        bool isNumber = utils::IsNumber(*it, false);
        switch(el.first) {
        case eJson:
            el = (reinterpret_cast<JsonElement*>(el.second))->value[*it];
            if(el.first == ValueType::eNull) {
                if(isNumber)
                    el = (reinterpret_cast<JsonElement*>(el.second))->value[stoi(*it)];
                else
                    el = {};
            }
            break;
        case eArray:
            //для массива возможно обращение только по числовому индексу!
            if(isNumber)
                el = (reinterpret_cast<ArrayElement*>(el.second))->value[stoi(*it)];
            else
                el = {};
            break;
        default: return {}; //продолжать поиск можно только по двум структурам!
        }
    }

    return el;
}
/// Json

//STATIC:
ValueType CheckValue(std::string& value)
{
//    std::cout << "CheckValue(): \"" << value << "\"" << std::endl;
    bool isValue = false;
    std::string _value;
    ValueType vType = eNull;
    for(size_t i = 0; i < value.length(); i++) {
        if(!isValue && !utils::CharsInString(value[i], SPACES))
            isValue = true;

        if(isValue) {
            if(vType == ValueType::eNull) {
                if(utils::IsNumber(value[i]))
                    vType = ValueType::eNumber;
                else if(value[i] == '"' || value[i] == '\'')
                    vType = ValueType::eString;
                else if(value[i] == '{')
                    vType = ValueType::eJson;
                else if(value[i] == '[')
                    vType = ValueType::eArray;
                else if(!utils::CharsInString(value[i], SPACES))
                    vType = ValueType::eBool;
            }
            _value += value[i];
        }
    }

    switch(vType) {
    case ValueType::eNumber:    { isValue = CheckDouble(_value);    break; }
    case ValueType::eBool:      { isValue = CheckBool(_value);      break; }
    case ValueType::eString:    { isValue = CheckString(_value);    break; }
    case ValueType::eJson:      { isValue = CheckJson(_value);      break; }
    case ValueType::eArray:     { isValue = CheckArray(_value);     break; }
    default:                    return ValueType::eNull;
    }

    if(isValue) {
        value = _value;
        return vType;
    } else
        return ValueType::eNull;
}

bool CheckDouble(std::string& value)
{
//    std::cout << "CheckDouble(): \"" << value << "\"" << std::endl;
    uint32_t pCounter = 0;
    //remove spaces
    std::string temp;
    for(char ch : value)
        if(!utils::CharsInString(ch, SPACES)) temp += ch;

    for(char ch : temp) {
        if(ch == '.') pCounter++;
        if(!utils::IsNumber(ch) || pCounter > 1) {
            std::cout << "Error with parse Number in: " << value << std::endl;
            return false;
        }
    }

    value = temp;
    return true;
}

bool CheckBool(std::string& value)
{
//    std::cout << "CheckBool(): \"" << value << "\"" << std::endl;
    std::string temp;
    bool flag = false;
    for(char c : value) {
        if(utils::CharsInString(c, SPACES)) {
            if(flag) break;
        } else {
            if(!flag)   flag = true;
            if(flag)    temp += std::tolower(c);
        }
    }

    if(temp != "true" && temp != "false") return false;
    value = temp;
    return true;
}

bool CheckString(std::string& value)
{
    //TODO: проверить строки с обрамлением одинарными кавычками
    /* TODO: экранирование строк при распаковке
     *  skip \" \\ \'
     */
//    std::cout << "CheckString(): \"" << value << "\"" << std::endl;
    char ch = 0;
    std::string temp;
    bool done = false;
    for(size_t i = 0; i < value.length(); i++) {
        if(ch != 0) { //начинаем запись слова
            if(!done) {
                if(value[i] == ch) //пока не встретили конец слова
                    done = true;
                else
                    temp += value[i];
            } else { //замкнули слово, надо проверить оставшиеся символы
                if(!utils::CharsInString(value[i], SPACES)) {
                    std::cout << "Error with parse String in: " << value << std::endl;
                    return false;
                }
            }
        } else if(utils::CharsInString(value[i], "\"'"))
            ch = value[i];
    }

    value = temp;
    return true;
}

bool CheckJson(std::string& value)
{
//    std::cout << "CheckJson(): \"" << value << "\"" << std::endl;
    char ch = 0;
    std::string temp;
    bool done = false;
    char innerWord = 0;
    uint32_t innerLvlFBrace = 0;
    uint32_t innerLvlQBrace = 0;
    for(size_t i = 0; i < value.length(); i++) {
        if(ch != 0) { //начинаем запись слова
            if(utils::CharsInString(value[i], "\"'")) {
                if(innerWord == 0)              innerWord = value[i];
                else if(value[i] == innerWord)  innerWord = 0;
            }

            if(!done) {
                if(innerWord == 0) {
                    if(value[i] == '{')  innerLvlFBrace++;
                    if(value[i] == '}')  innerLvlFBrace--;
                    if(value[i] == '[')  innerLvlQBrace++;
                    if(value[i] == ']')  innerLvlQBrace--;
                }
                if(value[i] == ch
                    && innerLvlFBrace == 0
                    && innerLvlQBrace == 0
                    && innerWord == 0) { //пока не встретили конец слова
                    done = true;
                }
                temp += value[i];
            } else { //замкнули слово, надо проверить оставшиеся символы
                if(!utils::CharsInString(value[i], SPACES)) {
                    std::cout << "Error with parse Json in: [" << value[i] << "]" << std::endl;
                    return false;
                }
            }
        } else if(!utils::CharsInString(value[i], SPACES)) {
            if (value[i] == '{') {
                temp += value[i];
                ch = '}';
                innerLvlFBrace++;
            } else
                return false;
        }
    }

    if(!done) return false;
    value = temp;
    return true;
}

bool CheckArray(std::string& value)
{
//    std::cout << "CheckArray(): \"" << value << "\"" << std::endl;
    char ch = 0;
    std::string temp;
    bool done = false;
    char innerWord = 0;
    uint32_t innerLvlFBrace = 0;
    uint32_t innerLvlQBrace = 0;
    for(size_t i = 0; i < value.length(); i++) {
        if(ch != 0) { //начинаем запись слова
            if(utils::CharsInString(value[i], "\"'")) {
                if(innerWord == 0)              innerWord = value[i];
                else if(value[i] == innerWord)  innerWord = 0;
            }

            if(!done) {
                if(innerWord == 0) {
                    if(value[i] == '{')  innerLvlFBrace++;
                    if(value[i] == '}')  innerLvlFBrace--;
                    if(value[i] == '[')  innerLvlQBrace++;
                    if(value[i] == ']')  innerLvlQBrace--;
                }
                if(value[i] == ch
                    && innerLvlFBrace == 0
                    && innerLvlQBrace == 0
                    && innerWord == 0) { //пока не встретили конец слова
                    done = true;
                }
                temp += value[i];
            } else { //замкнули слово, надо проверить оставшиеся символы
                if(!utils::CharsInString(value[i], SPACES)) {
                    std::cout << "Error with parse Array in: [" << value[i] << "]" << std::endl;
                    return false;
                }
            }
        } else if(!utils::CharsInString(value[i], SPACES)) {
            if (value[i] == '[') {
                temp += value[i];
                ch = ']';
                innerLvlQBrace++;
            } else
                return false;
        }
    }

    if(!done) return false;
    value = temp;
    return true;
}

bool ParseJson(const std::string& json_str, Json* json)
{
//    std::cout << "ParseJson(): " << json_str << std::endl;
    bool return_code = true;
    if(!json) return false;

    //ищем границы Json конструкции
    size_t startIndex = json_str.find('{');
    size_t endIndex = json_str.find_last_of('}');
    if((startIndex == -1) || (endIndex == -1)) {
        std::cout << "JSON not found in: " << json_str << std::endl;
        return false;
    }

    uint32_t    strCounter = 1;
    uint32_t    chCounter = 0;
    ValueType   valueType = ValueType::eNull;

    bool exit = false;
    bool isKey = false;
    bool isValue = false;
    std::string key = "";
    std::string value = "";
    NextReadState state = NextReadState::eJsonStart;
    for(size_t i = 0; i < json_str.length() && !exit; i++) {
        //счётчик строк и символов, для вывода ошибки
        if ((json_str[i] == '\n') || (i == 0)) {
            strCounter++;
            chCounter = 0;
        } else
            chCounter++;

        //чтение данных
        switch(state) {
        case eJsonStart: {
            if(utils::CharsInString(json_str[i], SPACES)) continue;
            else if(json_str[i] == '{')
                ChangeNextState(state, NextReadState::eKey);
            else
                exit = true;
            break;
        }
        case eJsonEnd: {
            if(utils::CharsInString(json_str[i], SPACES)) continue;
            else if(json_str[i] == '}')
                ChangeNextState(state, NextReadState::eUnknown);
            else
                exit = true;
            break;
        }
        case eKey: {
            if(key.empty()) isKey = true;
            if(isKey)       key += json_str[i];

            //значение считано полностью?
            if(i + 1 < json_str.length()) { //следующий символ существует
                if(json_str[i + 1] == ':' || (json_str[i + 1] == '}' && (i + 1 == endIndex)))
                    isKey = false;
            }

            if(!isKey) {
                if(!CheckString(key))
                    isKey = true;
                else
                    ChangeNextState(state, NextReadState::eColon);
            }
            break;
        }
        case eValue: { //может быть числом, строкой, Json или Array
            if(value.empty())   isValue = true;
            if(isValue)         value += json_str[i];

            //значение считано полностью?
            if(i + 1 < json_str.length()) //следующий символ существует
                if(json_str[i + 1] == ',' || (json_str[i + 1] == '}' && ((i + 1) == endIndex)))
                    isValue = false;

            if(!isValue) { //это конец значения?
                valueType = CheckValue(value);

                switch(valueType) {
                case eNumber:   { return_code = json->put(key, std::stod(value)); break; }
                case eBool:     {
                    return_code = utils::isBool(value);
                    if(return_code)
                        return_code = json->put(key, utils::ToBool(value));
                    break;
                }
                case eString:   { return_code = json->put(key, value); break; }
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
                    return_code = false;
                    isValue = true;
                    continue;
                }
                default: break;
                }

                //обнуление временных переменных, переход к следующему элементу
//                std::cout << "Json key: \"" << key << "\""
//                          << ", value: \"" << value << "\"" << std::endl;
                key = "";
                value = "";
                ChangeNextState(state, NextReadState::eComma);
            }
            break;
        }
        case eColon: {
            if(utils::CharsInString(json_str[i], SPACES))
                continue;

            if(json_str[i] != ':') {
                std::cout << "exp: ':'" << std::endl;
                return_code = false;
                exit = true;
            } else {
                ChangeNextState(state, NextReadState::eValue);
            }
            break;
        }
        case eComma: {
            if(utils::CharsInString(json_str[i], SPACES))
                continue;

            if(json_str[i] != ',') {
                if(json_str[i] != '}') {
                    std::cout << "exp: ','" << std::endl;
                    return_code = false;
                }
                exit = true;
            } else
                ChangeNextState(state, NextReadState::eKey);
            break;
        }
        default:
        case eUnknown: {
            exit = true;
            break;
        }
        }
    }

    if(!return_code)
        std::cout << "Syntax error, parse error value for key \"" << key << "\""
                  << " readed VALUE:\"" << value << "\""
                  << std::endl;

    return return_code;
}

bool ParseArray(const std::string& array_str, Array* array)
{
//    std::cout << "ParseArray(): " << array_str << std::endl;
    bool return_code = true;
    if(!array) return false;

    //ищем границы Array конструкции
    size_t startIndex = array_str.find('[');
    size_t endIndex = array_str.find_last_of(']');
    if((startIndex == -1) || (endIndex == -1)) {
        std::cout << "Array not found in: " << array_str << std::endl;
        return false;
    }

    uint32_t    strCounter = 1;
    uint32_t    chCounter = 0;
    ValueType   valueType = ValueType::eNull;

    bool exit = false;
    bool isValue = false;
    std::string value = "";
    NextReadState state = NextReadState::eArrayStart;
    for(size_t i = 0; i < array_str.length() && !exit; i++) {
        //счётчик строк и символов, для вывода ошибки
        if ((array_str[i] == '\n') || (i == 0)) {
            strCounter++;
            chCounter = 0;
        } else
            chCounter++;

        //чтение данных
        switch(state) {
        case eArrayStart: {
            if(utils::CharsInString(array_str[i], SPACES)) continue;
            else if(array_str[i] == '[')
                ChangeNextState(state, NextReadState::eValue);
            else
                exit = true;
            break;
        }
        case eArrayEnd: {
            if(utils::CharsInString(array_str[i], SPACES)) continue;
            else if(array_str[i] == ']')
                ChangeNextState(state, NextReadState::eUnknown);
            else
                exit = true;
            break;
        }
        case eValue: {
            if(value.empty())   isValue = true;
            if(isValue)         value += array_str[i];

            //значение считано полностью?
            if(i + 1 < array_str.length()) //следующий символ существует
                if(array_str[i + 1] == ',' || (array_str[i + 1] == ']' && ((i + 1) == endIndex)))
                    isValue = false;

            if(!isValue) { //это конец значения?
                valueType = CheckValue(value);
                return_code = valueType != ValueType::eNull;

                switch(valueType) {
                case eNumber:   { array->push_back(std::stod(value));       break; }
                case eBool:     {
                    return_code = utils::isBool(value);
                    if(return_code) array->push_back(utils::ToBool(value));
                    break;
                }
                case eString:   { array->push_back(value);                  break; }
                case eJson:     {
                    Json _innerJson;
                    return_code = ParseJson(value, &_innerJson);
                    if(return_code)
                        array->push_back(_innerJson);
                    else {
                        std::cout << "parse error valueType:" << ToString(valueType) << std::endl;
                        exit = true;
                    }
                    break;
                }
                case eArray:    {
                    Array _innerArray;
                    return_code = ParseArray(value, &_innerArray);
                    if(return_code)
                        array->push_back(_innerArray);
                    else {
                        std::cout << "parse error valueType:" << ToString(valueType) << std::endl;
                        exit = true;
                    }
                    break;
                }
                case eNull:     { //значение ещё не прочитано!
                    return_code = false;
                    isValue = true;
                    continue;
                }
                default: break;
                }

                //обнуление временных переменных, переход к следующему элементу
//                std::cout << "Array value: \"" << value << "\"" << std::endl;
                value = "";
                ChangeNextState(state, NextReadState::eComma);
            }
            break;
        }
        case eComma: {
            if(utils::CharsInString(array_str[i], SPACES)) continue;

            if(array_str[i] != ',') {
                if(array_str[i] != ']') {
                    std::cout << "exp: ','" << std::endl;
                    return_code = false;
                }
                exit = true;
            } else ChangeNextState(state, NextReadState::eValue);
            break;
        }
        default:
        case eUnknown: {
            exit = true;
            break;
        }
        }
    }

    if(!return_code) {
        std::cout << "Syntax error, parse error value \"" << value << "\""
                  << std::endl;
    }

    return return_code;
}
///STATIC

} /// namespace json
