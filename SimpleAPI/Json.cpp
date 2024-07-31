#include "Json.h"

#include <fstream>
#include <iostream>

//namespace json {
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

std::string to_string(const NextReadState state)
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
//              << to_string(state)
//              << "->"
//              << to_string(nextState)
//              << std::endl;
    state = nextState;
}

std::string to_string(const ValueType type) {
    switch(type) {
    case eNumber:       return "Number"; break;
    case eBool:         return "Bool"; break;
    case eString:       return "String"; break;
    case eJson:         return "Json"; break;
    case eArray:        return "Array"; break;
    default:            return "null"; break;
    }
}


// struct Element
Element::Element(const double value) : first(ValueType::eNumber) {
    second = reinterpret_cast<BaseElement*>(new DoubleElement(value));
}

Element::Element(const bool value) : first(ValueType::eBool) {
    second = reinterpret_cast<BaseElement*>(new BoolElement(value));
}

Element::Element(const std::string value) : first(ValueType::eString) {
    second = reinterpret_cast<BaseElement*>(new StringElement(value));
}

Element::Element(const char *value) : first(ValueType::eString) {
    second = reinterpret_cast<BaseElement*>(new StringElement(std::string(value)));
}

Element::Element(const Json& value) : first(ValueType::eJson) {
    second = reinterpret_cast<BaseElement*>(new JsonElement(value));
}

Element::Element(const Array& value) : first(ValueType::eArray) {
    second = reinterpret_cast<BaseElement*>(new ArrayElement(value));
}

bool Element::operator==(const Element &other) const
{
    if(this->first != other.first)
        return false;

    switch(this->first) {
    case eNull:     return true;
    case eNumber:
        if(reinterpret_cast<DoubleElement*>(this->second)->m_value
            == reinterpret_cast<DoubleElement*>(other.second)->m_value)
            return true;
        break;
    case eBool:
        if(reinterpret_cast<BoolElement*>(this->second)->m_value
            == reinterpret_cast<BoolElement*>(other.second)->m_value)
            return true;
        break;
    case eString:
        if(reinterpret_cast<StringElement*>(this->second)->m_value
            == reinterpret_cast<StringElement*>(other.second)->m_value)
            return true;
        break;
    case eJson:
        if(reinterpret_cast<JsonElement*>(this->second)->m_value
            == reinterpret_cast<JsonElement*>(other.second)->m_value)
            return true;
        break;
    case eArray:
        if(reinterpret_cast<ArrayElement*>(this->second)->m_value
            == reinterpret_cast<ArrayElement*>(other.second)->m_value)
            return true;
        break;
    }

    return false;
}

bool Element::operator!=(const Element &other) const {
    return !(*this == other);
}

double Element::getNum()
{
    if(first == ValueType::eNumber)
        return reinterpret_cast<DoubleElement*>(this->second)->m_value;
    else return 0; //TODO: вызвать exception
}

bool Element::getBool()
{
    if(first == ValueType::eBool)
        return reinterpret_cast<BoolElement*>(this->second)->m_value;
    else return false; //TODO: вызвать exception
}

std::string Element::getString()
{
    if(first == ValueType::eString)
        return reinterpret_cast<StringElement*>(this->second)->m_value;
    else return ""; //TODO: вызвать exception
}

Json Element::getJson()
{
    if(first == ValueType::eJson)
        return reinterpret_cast<JsonElement*>(this->second)->m_value;
    else return {}; //TODO: вызвать exception
}

Array Element::getArray()
{
    if(first == ValueType::eArray)
        return reinterpret_cast<ArrayElement*>(this->second)->m_value;
    else return {}; //TODO: вызвать exception
}

Element Element::getInnerValue(const std::string& key)
{
    if(this->first == ValueType::eJson)
        return reinterpret_cast<JsonElement*>(this->second)->m_value[key];
    else
        return {}; //TODO: вызвать exception
}

Element Element::getInnerValue(const size_t index)
{
    if(this->first == ValueType::eJson)
        return reinterpret_cast<JsonElement*>(this->second)->m_value[index];
    else if(this->first == ValueType::eArray)
        return reinterpret_cast<ArrayElement*>(this->second)->m_value[index];
    else
        return {}; //TODO: вызвать exception
}
/// struct Element


//ARRAY
bool Array::checkIndexes(const size_t index) {
    if(index + 1 > m_values.size()) {
        //TODO: std::outofrange
        throw "Going beyond Array boundaries";
        return false;
    }
    return true;
}

Array::Array(const Array& array)
{
    for(Element el : array.m_values) {
        switch(el.first) {
        case eNumber:
            m_values.push_back(Element(
                el.first, reinterpret_cast<BaseElement*>(
                    new DoubleElement(*reinterpret_cast<DoubleElement*>(el.second)))));
            break;
        case eBool:
            m_values.push_back(Element(
                el.first, reinterpret_cast<BaseElement*>(
                    new BoolElement(*reinterpret_cast<BoolElement*>(el.second)))));
            break;
        case eString:
            m_values.push_back(Element(
                el.first, reinterpret_cast<BaseElement*>(
                    new StringElement(*reinterpret_cast<StringElement*>(el.second)))));
            break;
        case eJson:
            m_values.push_back(Element(
                el.first, reinterpret_cast<BaseElement*>(
                    new JsonElement(*reinterpret_cast<JsonElement*>(el.second)))));
            break;
        case eArray:
            m_values.push_back(Element(
                el.first, reinterpret_cast<BaseElement*>(
                    new ArrayElement(*reinterpret_cast<ArrayElement*>(el.second)))));
            break;
        case eNull:     break;
        }
    }
}

Array::~Array() {
    for(Element& el : m_values)
        delete el.second;
}

std::string Array::to_string(int16_t tabulation_level)
{
    if(m_values.empty()) return "[]";

    std::string ret;
    bool withoutSpaces = tabulation_level < 0;
    ret += "[";

    if(m_values.size() == 1
        && m_values[0].first != ValueType::eJson
        && m_values[0].first != ValueType::eArray) {
        if(!withoutSpaces) ret += " ";
        ret += m_values[0].second->to_string(tabulation_level);
        if(!withoutSpaces) ret += " ";
    } else {
        if(!withoutSpaces) ret += "\n";
        std::string tabs_str = !withoutSpaces ? utils::Tab(++tabulation_level) : "";

        for(size_t i = 0; i < m_values.size(); i++) {
            if(!withoutSpaces) ret += tabs_str;
            ret += m_values[i].second->to_string(tabulation_level);
            if(i < m_values.size() - 1) ret += ",";
            if(!withoutSpaces) ret += "\n";
        }

        if(!withoutSpaces) ret += utils::Tab(--tabulation_level);
    }

    ret += "]";

    return ret;
}

bool Array::operator==(const Array &other) const
{
    if(this->size() != other.size())
        return false;

    for(auto it1 = this->m_values.begin(), it2 = other.m_values.begin();
         it1 != this->m_values.end() && it2 != other.m_values.end();
         it1++, it2++
         ) {
        if(*it1 != *it2)
            return false;
    }

    return true;
}

Array& Array::erase(const size_t index)
{
    if(index <= m_values.size() - 1)
        m_values.erase(m_values.cbegin() + index);

    return *this;
}

Element Array::operator[](const std::vector<std::string> &complex_name)
{
    if(m_values.empty()) return {};

    std::vector<std::string>::const_iterator it = complex_name.begin();
    if(!utils::isNumber(*it++, false))
        return {};
    Element el = (*this)[stoi(*it)]; //находим первый элемент списка
    for (; el.first != ValueType::eNull && it != complex_name.end(); it++) {
        bool isNumber = utils::isNumber(*it, false);
        switch(el.first) {
        case eJson:
            el = el.getInnerValue(*it);
            if(el.first == ValueType::eNull) {
                if(isNumber)    el = el.getInnerValue(stoi(*it));
                else            el = {};
            }
            break;
        case eArray:
            //для массива возможно обращение только по числовому индексу!
            if(isNumber)    el = el.getInnerValue(stoi(*it));
            else            el = {};
            break;
        default: return {}; //продолжать поиск можно только по двум структурам!
        }
    }

    return el;
}

Element Array::operator[](const size_t index)
{
    if(m_values.empty()) return {};
    if(!checkIndexes(index)) return {};

    return Element(m_values[index].first, m_values[index].second);
}
/// class Array

// Json
bool Json::checkIndexes(const size_t index) {
    if(index + 1 > m_values.size()) {
        throw "Going beyond Json boundaries";
        return false;
    }
    return true;
}

Json::Json(const Json& json)
{
    for(const JPair &el : json.m_values) {
        switch(el.second.first) {
        case eNumber: {
            double value = reinterpret_cast<DoubleElement*>(el.second.second)->m_value;
            m_values.push_back(std::make_pair(
                el.first,
                Element(el.second.first, reinterpret_cast<BaseElement*>(new DoubleElement(value)))));
            break;
        }
        case eBool: {
            bool value = reinterpret_cast<BoolElement*>(el.second.second)->m_value;
            m_values.push_back(std::make_pair(
                el.first,
                Element(el.second.first, reinterpret_cast<BaseElement*>(new BoolElement(value)))));
            break;
        }
        case eString: {
            std::string value = reinterpret_cast<StringElement*>(el.second.second)->m_value;
            m_values.push_back(std::make_pair(
                el.first,
                Element(el.second.first, reinterpret_cast<BaseElement*>(new StringElement(value)))));
            break;
        }
        case eJson: {
            Json value = reinterpret_cast<JsonElement*>(el.second.second)->m_value;
            m_values.push_back(std::make_pair(
                el.first,
                Element(el.second.first, reinterpret_cast<BaseElement*>(new JsonElement(value)))));
            break;
        }
        case eArray: {
            Array value = reinterpret_cast<ArrayElement*>(el.second.second)->m_value;
            m_values.push_back(std::make_pair(
                el.first,
                Element(el.second.first, reinterpret_cast<BaseElement*>(new ArrayElement(value)))));
            break;
        }
        case eNull: break;
        }
    }
}

Json::~Json() {
    for(JPair& el : m_values)
        delete el.second.second;
}

Json &Json::operator=(const Json &other) {
    for(const JPair &el : other.m_values) {
        switch(el.second.first) {
        case eNumber: {
            double value = reinterpret_cast<DoubleElement*>(el.second.second)->m_value;
            m_values.push_back(std::make_pair(
                el.first,
                Element(el.second.first, reinterpret_cast<BaseElement*>(new DoubleElement(value)))));
            break;
        }
        case eBool: {
            bool value = reinterpret_cast<BoolElement*>(el.second.second)->m_value;
            m_values.push_back(std::make_pair(
                el.first,
                Element(el.second.first, reinterpret_cast<BaseElement*>(new BoolElement(value)))));
            break;
        }
        case eString: {
            std::string value = reinterpret_cast<StringElement*>(el.second.second)->m_value;
            m_values.push_back(std::make_pair(
                el.first,
                Element(el.second.first, reinterpret_cast<BaseElement*>(new StringElement(value)))));
            break;
        }
        case eJson: {
            Json value = reinterpret_cast<JsonElement*>(el.second.second)->m_value;
            m_values.push_back(std::make_pair(
                el.first,
                Element(el.second.first, reinterpret_cast<BaseElement*>(new JsonElement(value)))));
            break;
        }
        case eArray: {
            Array value = reinterpret_cast<ArrayElement*>(el.second.second)->m_value;
            m_values.push_back(std::make_pair(
                el.first,
                Element(el.second.first, reinterpret_cast<BaseElement*>(new ArrayElement(value)))));
            break;
        }
        case eNull: break;
        }
    }

    return *this;
}

Json& Json::put(const Json &other, const bool rewrite)
{
    /*TODO: bool для обновления дублей*/
    for(const JPair &el : other.m_values) {
        if(contains(el.first)) continue; //NOTE: дубли пропустятся

        Element new_element;

        switch(el.second.first) {
        case eNumber: {
            double value = reinterpret_cast<DoubleElement*>(el.second.second)->m_value;
            new_element = Element(el.second.first, reinterpret_cast<BaseElement*>(new DoubleElement(value)));
            break;
        }
        case eBool: {
            bool value = reinterpret_cast<BoolElement*>(el.second.second)->m_value;
            new_element = Element(el.second.first, reinterpret_cast<BaseElement*>(new BoolElement(value)));
            break;
        }
        case eString: {
            std::string value = reinterpret_cast<StringElement*>(el.second.second)->m_value;
            new_element = Element(el.second.first, reinterpret_cast<BaseElement*>(new StringElement(value)));
            break;
        }
        case eJson: {
            Json value = reinterpret_cast<JsonElement*>(el.second.second)->m_value;
            new_element = Element(el.second.first, reinterpret_cast<BaseElement*>(new JsonElement(value)));
            break;
        }
        case eArray: {
            Array value = reinterpret_cast<ArrayElement*>(el.second.second)->m_value;
            new_element = Element(el.second.first, reinterpret_cast<BaseElement*>(new ArrayElement(value)));
            break;
        }
        case eNull: break;
        }

        if(el.second.first != eNull)
            m_values.push_back(std::make_pair(el.first, new_element));
    }

    return *this;
}

bool Json::readFile(const std::string& path)
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
        std::cout << "prepare(" << (nextStrStartFromComment ? "true" : "false") << "): " << temp_string << std::endl;
        utils::RemoveComments(temp_string, nextStrStartFromComment, quote);
        if(!temp_string.empty() && !utils::OnlySpaces(temp_string)) {
            json_str += temp_string + '\n';
            std::cout << "temp: " << temp_string << std::endl;
        }
    }
    file.close();

    //обработка JSON
    return this->parseJson(json_str);
}

bool Json::writeFile(const std::string& path, int16_t tabulation_level)
{
    std::ofstream file(path);
    if (!file.is_open())
        return false;

    file << this->to_string(tabulation_level) << std::endl;

    file.flush();
    file.close();
    return true;
}

std::string Json::to_string(int16_t tabulation_level) const
{
    std::string ret;
    bool withoutSpaces = tabulation_level < 0;
    ret += "{"; //start of json

    if(m_values.size() == 1
        && m_values[0].second.first != ValueType::eJson
        && m_values[0].second.first != ValueType::eArray) {
        if(!withoutSpaces) ret += " ";
        ret += "\"" + m_values[0].first + "\"";
        if(!withoutSpaces) ret += " ";
        ret += ":";
        if(!withoutSpaces) ret += " ";
        ret += m_values[0].second.second->to_string(tabulation_level);
        if(!withoutSpaces) ret += " ";
    } else {
        if(!withoutSpaces) ret += "\n";

        std::string tabs_str = !withoutSpaces ? utils::Tab(++tabulation_level) : "";

        size_t i = 0;
        for(const JPair& el : m_values) {
            ret += tabs_str + "\"" + el.first + "\"";
            if(!withoutSpaces) ret += " ";
            ret += ":";
            if(!withoutSpaces) ret += " ";
            ret += el.second.second->to_string(tabulation_level);
            if(i < m_values.size() - 1) ret += ",";
            if(!withoutSpaces) ret += "\n";
            i++;
        }

        if(!withoutSpaces) ret += utils::Tab(--tabulation_level);
    }

    ret += "}"; //end of json
    return ret;
}

bool Json::contains(const std::string &key) {
    for(auto& el : m_values) {
        if(el.first == key)
            return true;
    }

    return false;
}

bool Json::operator==(const Json &other) const
{
    if(this->size() != other.size())
        return false;

    for(auto it1 = this->m_values.begin(), it2 = other.m_values.begin();
         it1 != this->m_values.end() && it2 != other.m_values.end();
         it1++, it2++
         ) {
        if(it1->second != it2->second)
            return false;
    }

    return true;
}

Json& Json::erase(const size_t index)
{
    if(index <= m_values.size() - 1)
        m_values.erase(m_values.cbegin() + index);

    return *this;
}

Json& Json::erase(const std::string &key)
{
    bool flag = false;
    size_t index;
    for(index = 0; index < this->size(); index++) {
        if(m_values[index].first == key) {
            flag = true;
            break;
        }
    }

    if(flag) m_values.erase(m_values.cbegin() + index);

    return *this;
}

Json& Json::erase(const std::vector<std::string> &keys)
{
    for(const std::string &key : keys)
        this->erase(key);

    return *this;
}

Element Json::operator[](const std::vector<std::string> &complex_name)
{
    if(m_values.empty()) return {};

    Element el = (*this)[complex_name[0]]; //находим первый элемент списка
    std::vector<std::string>::const_iterator it = complex_name.begin() + 1; //первый элемент пропускаем
    for (; el.first != ValueType::eNull && it != complex_name.end(); it++) {
        bool isNumber = utils::isNumber(*it, false);
        switch(el.first) {
        case eJson:
            el = el.getInnerValue(*it);
            if(el.first == ValueType::eNull) {
                if(isNumber)    el = el.getInnerValue(stoi(*it));
                else            el = {};
            }
            break;
        case eArray:
            //для массива возможно обращение только по числовому индексу!
            if(isNumber)    el = el.getInnerValue(stoi(*it));
            else            el = {};
            break;
        default: return {}; //продолжать поиск можно только по двум структурам!
        }
    }

    return el;
}

Element Json::operator[](const std::string &name)
{
    if(m_values.empty()) return {};

    for(size_t i = 0; i < m_values.size(); i++)
        if(m_values[i].first == name)
            return Element(m_values[i].second.first,
                           m_values[i].second.second);
    return {};
}

Element Json::operator[](const size_t index)
{
    if(m_values.empty())        return {};
    if(!checkIndexes(index))    return {};

    return Element(m_values[index].second.first, m_values[index].second.second);
}
/// class Json

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
                if(utils::isNumber(value[i]))   vType = ValueType::eNumber;
                else if(value[i] == '"')        vType = ValueType::eString;
                else if(value[i] == '{')        vType = ValueType::eJson;
                else if(value[i] == '[')        vType = ValueType::eArray;
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
        if(!utils::isNumber(ch) || pCounter > 1) {
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
//    std::cout << "CheckString(): \"" << value << "\"" << std::endl;
    char ch = 0;
    std::string temp;
    bool done = false;
    for(size_t i = 0; i < value.length(); i++) {
        if(ch != 0) { //начинаем запись слова
            if(!done) {
                //экранирование?
                if(i <= value.size()) { //следующий символ существует?
                    if(value[i] == '\\' && value[i+1] == ch) {
                        temp += value[i];
                        temp += value[i + 1];
                        i++;
                        continue;
                    }
                }
                if(value[i] == '"')
                    done = true;
                else
                    temp += value[i];
            } else { //замкнули слово, надо проверить оставшиеся символы
                if(!utils::CharsInString(value[i], SPACES)) {
                    std::cout << "Error with parse String in: " << value << std::endl;
                    return false;
                }
            }
        } else if(value[i] == '"') {
            ch = value[i];
        }
    }

    value = temp;
    return done;
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
            if(value[i] == '"') {
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
            if(value[i] == '"') {
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

bool Json::parseJson(const std::string& json_str)
{
//    std::cout << "ParseJson(): " << json_str << std::endl;
    bool return_code = true;

    //ищем границы Json конструкции
    size_t startIndex = json_str.find('{');
    size_t endIndex = json_str.find_last_of('}');
    if((startIndex == -1) || (endIndex == -1)) {
//        std::cout << "JSON not found in: " << json_str << std::endl;
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
            if(i + 1 < json_str.length()) { //следующий символ существует
                if(json_str[i + 1] == ',' || (json_str[i + 1] == '}' && ((i + 1) == endIndex)))
                    isValue = false;
            }

            if(!isValue) { //это конец значения?
                valueType = CheckValue(value);

                switch(valueType) {
                case eNumber:   { return_code = this->put(key, std::stod(value)); break; }
                case eBool:     {
                    return_code = utils::isBool(value);
                    if(return_code)
                        return_code = this->put(key, utils::toBool(value));
                    break;
                }
                case eString:   { return_code = this->put(key, value); break; }
                case eJson:     {
                    Json _innerJson;
                    if(!_innerJson.parseJson(value)) {
                        std::cout << "parse error in key:" << key
                                  << "valueType:" << to_string(valueType)
                                  << std::endl;
                        exit = true;
                    } else {
                        return_code = this->put(key, _innerJson);
                    }
                    break;
                }
                case eArray:    {
                    Array _innerArray;
                    return_code = _innerArray.parseArray(value);
                    if(!return_code) {
                        std::cout << "parse error in key:" << key
                                  << "valueType:" << to_string(valueType)
                                  << std::endl;
                        exit = true;
                    } else
                        return_code = this->put(key, _innerArray);
                    break;
                }
                case eNull:     { //значение ещё не прочитано!
                    return_code = false;
                    isValue = true;
                    continue;
                }
                default: break;
                }

//                std::cout << "Json key: \"" << key << "\""
//                          << ", value: \"" << value << "\"" << std::endl;

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

bool Array::parseArray(const std::string& array_str)
{
//    std::cout << "ParseArray(): " << array_str << std::endl;
    bool return_code = true;

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
                case eNumber:   { this->push_back(std::stod(value));       break; }
                case eBool:     {
                    return_code = utils::isBool(value);
                    if(return_code) this->push_back(utils::toBool(value));
                    break;
                }
                case eString:   { this->push_back(value);                  break; }
                case eJson:     {
                    Json _innerJson;
                    return_code = _innerJson.parseJson(value);
                    if(return_code)
                        this->push_back(_innerJson);
                    else {
                        std::cout << "parse error valueType:" << to_string(valueType) << std::endl;
                        exit = true;
                    }
                    break;
                }
                case eArray:    {
                    Array _innerArray;
                    return_code = _innerArray.parseArray(value);
                    if(return_code)
                        this->push_back(_innerArray);
                    else {
                        std::cout << "parse error valueType:" << to_string(valueType) << std::endl;
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

//                std::cout << "Array value: \"" << value << "\"" << std::endl;

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

    if(!return_code)
        std::cout << "Syntax error, parse error value \"" << value << "\"" << std::endl;

    return return_code;
}
///STATIC

//} /// namespace json










