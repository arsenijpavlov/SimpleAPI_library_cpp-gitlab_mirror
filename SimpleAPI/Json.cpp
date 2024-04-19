#include "Json.h"

#include <fstream>
#include <iostream>

#include "utils.h"

namespace json {
#define SPACES " \n\t"

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

//ARRAY

Array::Array()
{
}

Array::Array(Array& array)
{
    for(ArrayElements el : array.values) {
        switch(el.first) {
        case eNumber:
            this->values.push_back(ArrayElements(
                el.first,
                reinterpret_cast<BaseElement*>(
                    new DoubleElement(*reinterpret_cast<DoubleElement*>(el.second)))));
            break;
        case eBool:
            this->values.push_back(ArrayElements(
                el.first,
                reinterpret_cast<BaseElement*>(
                    new BoolElement(*reinterpret_cast<BoolElement*>(el.second)))));
            break;
        case eString:
        case eJson:
        case eArray:
        case eNull:     break;
        }
    }
}

Array::~Array()
{
    for(ArrayElements element : this->values) {
        std::cout << "free(" << ToString(element.first) << "), addr:" << element.second << std::endl;
        switch(element.first) {
        case eNumber:
            delete reinterpret_cast<DoubleElement*>(element.second);
            break;
        case eBool:
            delete reinterpret_cast<BoolElement*>(element.second);
            break;
        case eString:
            delete reinterpret_cast<StringElement*>(element.second);
            break;
        case eJson:
            delete reinterpret_cast<JsonElement*>(element.second);
            break;
        case eArray:
            delete reinterpret_cast<ArrayElement*>(element.second);
            break;
        default: break;
        }
    }
}


void Array::push_back(double d)
{
    this->values.push_back(ArrayElements(
        ValueType::eNumber, reinterpret_cast<BaseElement*>(new DoubleElement(d))));
//    size_t index = this->values
//                       .insert(std::pair<size_t, ValueType>(this->values.size() + 1,
//                                                            ValueType::eNumber))
//                       .first->first;
//    this->numbers.insert(std::pair<size_t, double>(index, d));
}

void Array::push_back(bool b)
{
    this->values.push_back(ArrayElements(
        ValueType::eBool, reinterpret_cast<BaseElement*>(new BoolElement(b))));
//    size_t index = this->values
//                       .insert(std::pair<size_t, ValueType>(this->values.size() + 1,
//                                                            ValueType::eBool))
//                       .first->first;
//    this->bools.insert(std::pair<size_t, bool>(index, b));
}

void Array::push_back(std::string string)
{
    this->values.push_back(ArrayElements(
        ValueType::eString, reinterpret_cast<BaseElement*>(new StringElement(string))));
//    size_t index = this->values
//                       .insert(std::pair<size_t, ValueType>(this->values.size() + 1,
//                                                            ValueType::eString))
//                       .first->first;
//    this->strings.insert(std::pair<size_t, std::string>(index, string));
}

void Array::push_back(Json json)
{
    this->values.push_back(ArrayElements(
        ValueType::eJson, reinterpret_cast<BaseElement*>(new JsonElement(json))));
//    size_t index = this->values
//                       .insert(std::pair<size_t, ValueType>(this->values.size() + 1,
//                                                            ValueType::eJson))
//                       .first->first;
//    this->jsons.insert(std::pair<size_t, Json>(index, json));
}

void Array::push_back(Array& array)
{
    //TODO: скопировать(!) элементы массива из параметров функции и заново выделить память для них
    this->values.push_back(ArrayElements(
        ValueType::eArray, reinterpret_cast<BaseElement*>(new ArrayElement(array))));
//    size_t index = this->values
//                       .insert(std::pair<size_t, ValueType>(this->values.size() + 1,
//                                                            ValueType::eArray))
//                       .first->first;
//    this->arrays.insert(std::pair<size_t, Array>(index, array));
}

/*
void Array::push_front(const double d)
{
    this->values.push_back(ArrayElements(ValueType::eNumber, reinterpret_cast<BaseElement*>(new DoubleElement(d))));
}

void Array::push_front(const bool b)
{
    this->values.push_back(ArrayElements(ValueType::eBool, reinterpret_cast<BaseElement*>(new BoolElement(b))));
}

void Array::push_front(const std::string string)
{
    this->values.push_back(ArrayElements(ValueType::eString, reinterpret_cast<BaseElement*>(new StringElement(string))));
}

void Array::push_front(const Json json)
{
    Json *pJ = new Json(json);
    this->values.push_back(ArrayElements(ValueType::eJson, reinterpret_cast<BaseElement*>(pJ)));
}

void Array::push_front(const Array array)
{
    Array *pA = new Array(array);
    this->values.push_back(ArrayElements(ValueType::eArray, reinterpret_cast<BaseElement*>(pA)));
}
*/

ValueType Array::getType(size_t index)
{
    return this->values[index].first;
}

void* Array::getAt(size_t index)
{
    return this->values[index].second;
}

void* Array::operator[](size_t index)
{
    return this->values[index].second;
}

std::string Array::to_string(int16_t tabulation_level)
{
    std::string ret;
    ret += "[";

    //TODO: Array::to_string()

    ret += "]";
    return ret;
}

///ARRAY

// Json
Json::~Json()
{
    this->numbers.clear();
    this->bools.clear();
    this->strings.clear();
    this->jsons.clear();
    this->arrays.clear();
}

bool Json::put(const std::string key, const std::string value)
{
    return this->strings.insert(std::pair<std::string, std::string>(key, value)).second;
}

bool Json::put(const std::string key, const double value)
{
    return this->numbers.insert(std::pair<std::string, double>(key, value)).second;
}

bool Json::put(const std::string key, const bool value)
{
    return this->bools.insert(std::pair<std::string, bool>(key, value)).second;
}

bool Json::put(const std::string key, const Json json)
{
    return this->jsons.insert(std::pair<std::string, Json>(key, json)).second;
}

bool Json::put(const std::string key, const Array value)
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
    std::string tabs_str = !withoutSpaces ? utils::Tab(tabulation_level) : "";
    //numbers
    std::map<std::string, double>::iterator it_num = this->numbers.begin();
    for(int i = 0; it_num != numbers.end(); i++, it_num++) {
        ret += tabs_str + "\"" + it_num->first + "\"";
        if(!withoutSpaces) ret += " ";
        ret += ":";
        if(!withoutSpaces) ret += " ";
        ret += utils::ToString(it_num->second)
               + (((i < numbers.size() - 1)
                   || (bools.size() > 0)
                   || (strings.size() > 0)
                   || (jsons.size() > 0)
                   || (arrays.size() > 0)) ? "," : "");
        if(!withoutSpaces) ret += "\n";
    }
    //bools
    std::map<std::string, bool>::iterator it_b = this->bools.begin();
    for(int i = 0; it_b != bools.end(); i++, it_b++) {
        ret += tabs_str + "\"" + it_b->first + "\"";
        if(!withoutSpaces) ret += " ";
        ret += ":";
        if(!withoutSpaces) ret += " ";
        ret += utils::ToString(it_b->second)
               + (((i < bools.size() - 1)
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
               + it_ar->second.to_string(tabulation_level + (!withoutSpaces ? 2 : 0))
               + ((i < arrays.size() - 1) ? "," : "");
        if(!withoutSpaces) ret += "\n";
    }

    if(!withoutSpaces) tabulation_level--;
    if(!withoutSpaces) ret += utils::Tab(tabulation_level);
    ret += "}"; //end of json
    return ret;
}
/// Json

//STATIC:
ValueType CheckValue(std::string& value)
{
//    std::cout << "CheckValue(): \"" << value << "\"" << std::endl;
    bool isValue = false;
    std::string _value;
    char _ch;
    ValueType vType = eNull;
    for(size_t i = 0; i < value.length(); i++) {
        if(!isValue && !utils::CharsInString(value[i], SPACES))
            isValue = true;

        if(isValue) {
            if(vType == ValueType::eNull) {
                if(utils::IsNumber(value[i]))   vType = ValueType::eNumber;
                else if(value[i] == '"' || value[i] == '\'') {
                    vType = ValueType::eString;
                    _ch = value[i]; //граница слова
                } else if(value[i] == '{') {
                    vType = ValueType::eJson;
                    _ch = '}';      //граница слова
                } else if(value[i] == '[') {
                    vType = ValueType::eArray;
                    _ch = ']';      //граница слова
                } else if(!utils::CharsInString(value[i], SPACES)) {
                    vType = ValueType::eBool;
                }
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
//    std::cout << "CheckValue(): \"" << value << "\"" << std::endl;
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

//    std::cout << "-CheckValue(): \"" << temp << "\"" << std::endl;
    if(temp != "true" && temp != "false") return false;

//    std::cout << "~CheckValue(): \"" << temp << "\"" << std::endl;
    value = temp;
    return true;
}

bool CheckString(std::string& value)
{
//    std::cout << "CheckString(): " << word << std::endl;
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
//        std::cout << "CheckJson(): current:[" << value[i] << "]"
//                  << " done:" << (done ? "+" : "-")
//                  << " [" << "F:" << innerLvlFBrace << "]"
//                  << " [" << "Q:" << innerLvlQBrace << "]"
//                  << " [" << "W:" << innerWord << "]"
//                  << std::endl;
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

//    std::cout << "~CheckJson()[" << (done ? "+" : "-") << "]:"
//              << " [" << "F:" << innerLvlFBrace << "]"
//              << " [" << "Q:" << innerLvlQBrace << "]"
//              << " [" << "W:" << innerWord << "]"
//              << " \"" << temp << "\""
//              << std::endl;
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
//        std::cout << "CheckJson(): current:[" << value[i] << "]"
//                  << " done:" << (done ? "+" : "-")
//                  << " [" << "F:" << innerLvlFBrace << "]"
//                  << " [" << "Q:" << innerLvlQBrace << "]"
//                  << " [" << "W:" << innerWord << "]"
//                  << std::endl;
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

//    std::cout << "~CheckArray()[" << (done ? "+" : "-") << "]:"
//              << " [" << "F:" << innerLvlFBrace << "]"
//              << " [" << "Q:" << innerLvlQBrace << "]"
//              << " [" << "W:" << innerWord << "]"
//              << " \"" << temp << "\""
//              << std::endl;
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
            if(utils::CharsInString(json_str[i], SPACES)) continue;
            else if(json_str[i] == '{')
                ChangeNextState(state, NextReadState::eKey);
            else {
                ch_err = json_str[i];
                exit = true;
            }
            break;
        }
        case eJsonEnd: {
            if(utils::CharsInString(json_str[i], SPACES)) continue;
            else if(json_str[i] == '}')
                ChangeNextState(state, NextReadState::eUnknown);
            else {
                ch_err = json_str[i];
                exit = true;
            }
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
                else {
//                    std::cout << "KEY: \"" << key << "\"" << std::endl;
                    ChangeNextState(state, NextReadState::eColon);
                }
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
                if(valueType != ValueType::eNull)
                    std::cout << "JSON KEY:\"" << key << "\", VALUE(" << ToString(valueType) << "):\"" << value << "\"" << std::endl;
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
                ch_err = json_str[i];
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
                ch_err = json_str[i];
                exit = true;
            } else
                ChangeNextState(state, NextReadState::eKey);
            break;
        }
        default:
        case eUnknown: {
            ch_err = json_str[i];
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
    std::cout << "ParseArray(): " << array_str << std::endl;
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
    char ch_err;

    bool exit = false;
    bool isValue = false;
    std::string value = "";
    NextReadState state = NextReadState::eArrayStart;
    for(size_t i = 0; i < array_str.length() && !exit; i++) {
//        std::cout << "current [" << array_str[i] << "]: "
//                  << "key: [" << key << "], "
//                  << "value: [" << value << "], "
//                  << "k:" << (isKey ? "+" : "-") << " "
//                  << "v:" << (isValue ? "+" : "-")
//                  << std::endl;

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
            else {
                ch_err = array_str[i];
                exit = true;
            }
            break;
        }
        case eArrayEnd: {
            if(utils::CharsInString(array_str[i], SPACES)) continue;
            else if(array_str[i] == ']')
                ChangeNextState(state, NextReadState::eUnknown);
            else {
                ch_err = array_str[i];
                exit = true;
            }
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
                if(valueType != ValueType::eNull)
                    std::cout << "ARRAY VALUE(" << ToString(valueType) << "):\"" << value << "\"" << std::endl;
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
                ch_err = array_str[i];
                exit = true;
            } else ChangeNextState(state, NextReadState::eValue);
            break;
        }
        default:
        case eUnknown: {
            ch_err = array_str[i];
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
