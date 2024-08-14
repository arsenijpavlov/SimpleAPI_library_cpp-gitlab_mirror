#include "Json.h"

#include <fstream>
#include <iostream>
#include <set>

//#define DEBUG_OUTPUT TODO: когда-нибудь потом...

#define __SPACES__                      " \n\t"
#define __SEPARATORS__                  ",\n"
#define __SPACES_WITHOUT_SEPARATORS__   " \t"
#define __POSIBLE_COLON__               ":="


// NextReadState ===============================================================================
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

std::string to_string(const NextReadState state) {
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

void ChangeNextState(NextReadState &state, const NextReadState nextState) {
    if(state == nextState) return;
//    std::cout << "state changed: "
//              << to_string(state)
//              << "->"
//              << to_string(nextState)
//              << std::endl;
    state = nextState;
}
// =============================================================================== NextReadState
// *
// *
// Element =====================================================================================
std::string to_string(const ValueType type) {
    switch(type) {
    case eNumber:   return "Number";    break;
    case eBool:     return "Bool";      break;
    case eString:   return "String";    break;
    case eJson:     return "Json";      break;
    case eArray:    return "Array";     break;
    default:        return "null";      break;
    }
}

Element::Element(const Json& value) : first(ValueType::eJson) {
    second = reinterpret_cast<BaseElement*>(new JsonElement(value));
}

Element::Element(const JArray& value) : first(ValueType::eArray) {
    second = reinterpret_cast<BaseElement*>(new JArrayElement(value));
}

Element::Element(const Element &other) {
    first = other.first;
    switch(first) {
    case eNumber:
        second = reinterpret_cast<BaseElement*>(new DoubleElement(other.getNum()));
        break;
    case eBool:
        second = reinterpret_cast<BaseElement*>(new BoolElement(other.getBool()));
        break;
    case eString:
        second = reinterpret_cast<BaseElement*>(new StringElement(other.getString()));
        break;
    case eJson:
        second = reinterpret_cast<BaseElement*>(new JsonElement(other.getJson()));
        break;
    case eArray:
        second = reinterpret_cast<BaseElement*>(new JArrayElement(other.getArray()));
        break;
    case eNull:
        second = nullptr;
        break;
    }
}

bool Element::operator==(const Element &other) const {
    if(this->first != other.first) return false;

    switch(this->first) {
    case eNull:                                 return true;
    case eNumber:
        if(getNum() == other.getNum())          return true;
        break;
    case eBool:
        if(getBool() == other.getBool())        return true;
        break;
    case eString:
        if(getString() == other.getString())    return true;
        break;
    case eJson:
        if(getJson() == other.getJson())        return true;
        break;
    case eArray:
        if(getArray() == other.getArray())      return true;
        break;
    }

    return false;
}

Element &Element::operator=(const Element &other) {
    if(this == &other) return *this;

    delete second;
    first = other.first;
    switch(first) {
    case eNumber:
        second = reinterpret_cast<BaseElement*>(new DoubleElement(other.getNum()));
        break;
    case eBool:
        second = reinterpret_cast<BaseElement*>(new BoolElement(other.getBool()));
        break;
    case eString:
        second = reinterpret_cast<BaseElement*>(new StringElement(other.getString()));
        break;
    case eJson:
        second = reinterpret_cast<BaseElement*>(new JsonElement(other.getJson()));
        break;
    case eArray:
        second = reinterpret_cast<BaseElement*>(new JArrayElement(other.getArray()));
        break;
    case eNull:
        second = nullptr;
        break;
    }

    return *this;
}

double &Element::getNum() const {
    if(first != ValueType::eNumber)
        throw std::invalid_argument("This element is not a 'Number' type: " + to_string(first));
    return reinterpret_cast<DoubleElement*>(second)->m_value;
}

bool &Element::getBool() const {
    if(first != ValueType::eBool)
        throw std::invalid_argument("This element is not a 'Bool' type: " + to_string(first));
    return reinterpret_cast<BoolElement*>(second)->m_value;
}

std::string &Element::getString() const {
    if(first != ValueType::eString)
        throw std::invalid_argument("This element is not a 'String' type: " + to_string(first));
    return reinterpret_cast<StringElement*>(second)->m_value;
}

Json &Element::getJson() const {
    if(first != ValueType::eJson)
        throw std::invalid_argument("This element is not a 'Json' type: " + to_string(first));
    return reinterpret_cast<JsonElement*>(second)->m_value;
}

JArray &Element::getArray() const {
    if(first != ValueType::eArray)
        throw std::invalid_argument("This element is not a 'JArray' type: " + to_string(first));
    return reinterpret_cast<JArrayElement*>(second)->m_value;
}

Element &Element::getInnerValue(const std::string& key) const {
    if(first != ValueType::eJson)
        throw std::invalid_argument("This element is not a 'Json' type: " + to_string(first));
    return reinterpret_cast<JsonElement*>(second)->m_value[key];
}

Element Element::getInnerValue(const size_t index) const {
    switch(first) {
    case eJson:
        return reinterpret_cast<JsonElement*>(second)->m_value[index];
    case eArray:
        return reinterpret_cast<JArrayElement*>(second)->m_value[index];
    default:
        throw std::invalid_argument("This element is not a 'Json' ot 'JArray' type: " + to_string(first));
    }
}
// ===================================================================================== Element
// *
// *
// JArray ======================================================================================
JArray::JArray(const JArray& other) {
    for(AVector::const_iterator it = other.m_values.cbegin();
         it != other.m_values.cend(); it++) {
        m_values.push_back(Element(*it));
    }
}

bool JArray::parseArray(const std::string& array_str) {
    //    std::cout << "ParseArray(): " << array_str << std::endl;
    bool return_code = true;

    //ищем границы JArray конструкции
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
            if(utils::CharsInString(array_str[i], __SPACES__)) continue;
            else if(array_str[i] == '[')
                ChangeNextState(state, NextReadState::eValue);
            else
                exit = true;
            break;
        }
        case eArrayEnd: {
            if(utils::CharsInString(array_str[i], __SPACES__)) continue;
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
                if(utils::CharsInString(array_str[i + 1], ",\n")
                    || (array_str[i + 1] == ']' && ((i + 1) == endIndex)))
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
                    JArray _innerArray;
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
            if(utils::CharsInString(array_str[i], __SPACES_WITHOUT_SEPARATORS__)) continue;

            if(!utils::CharsInString(array_str[i], __SEPARATORS__)) {
                if(array_str[i] != ']') {
                    std::cout << "exp: ',' or '\\n'" << std::endl;
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

JArray &JArray::append(const JArray &array) {
    for(const Element& el : array.m_values) {
        switch(el.first) {
        case eNumber:   push_back(el.getNum());     break;
        case eBool:     push_back(el.getBool());    break;
        case eString:   push_back(el.getString());  break;
        case eJson:     push_back(el.getJson());    break;
        case eArray:    push_back(el.getArray());   break;
        case eNull:     break;
        }
    }

    return *this;
}

std::string JArray::to_string(int16_t tabulation_level, const PrintType print_type, const uint8_t column_size) const {
    if(m_values.empty()) return "[]";

    std::string ret;
    bool withoutSpaces = tabulation_level < 0 && print_type == PrintType::eWithoutComment;

    if(print_type == PrintType::eWithComment
        && m_preview_comment.type != CommentType::eAfterValueOneLine
        && !m_preview_comment.value.empty()
        ) {
        ret += "\n";
        switch(m_preview_comment.type) {
        case CommentType::eBeforeValueMultiLine: {
            ret += utils::RepeatSymToStr('\t', tabulation_level)
                   + utils::RepeatSymToStr('#', m_comment_column_size + 2) + "\n";
            ret += ToComment(m_preview_comment.value, tabulation_level, m_comment_column_size) + "\n";
            ret += utils::RepeatSymToStr('\t', tabulation_level)
                   + utils::RepeatSymToStr('#', m_comment_column_size + 2) + "\n";
            break;
        }
        case CommentType::eBeforeValue: {
            ret += ToComment(m_preview_comment.value, tabulation_level) + "\n";
            break;
        }
        default: break;
        }
    }

    ret += "["; //start of array

    if(m_values.size() == 1
        && m_values[0].first != ValueType::eJson
        && m_values[0].first != ValueType::eArray
        ) {

        //===========================================================================
        auto comment_it = m_comments.find(0);
        if(comment_it != m_comments.end()
            && print_type == PrintType::eWithComment
            && comment_it->second.type != CommentType::eAfterValueOneLine
            ) {
            ret += "\n";
            switch(comment_it->second.type) {
            case CommentType::eBeforeValueMultiLine: {
                ret += utils::RepeatSymToStr('\t', tabulation_level + 1)
                       + utils::RepeatSymToStr('#', m_comment_column_size + 2) + "\n";
                ret += ToComment(comment_it->second.value, tabulation_level + 1, m_comment_column_size) + "\n";
                ret += utils::RepeatSymToStr('\t', tabulation_level + 1)
                       + utils::RepeatSymToStr('#', m_comment_column_size + 2) + "\n";
                break;
            }
            case CommentType::eBeforeValue: {
                ret += utils::RepeatSymToStr('\t', tabulation_level + 1)
                       + ToComment(comment_it->second.value, tabulation_level + 1) + "\n";
                break;
            }
            default: break;
            }
            ret += utils::RepeatSymToStr('\t', tabulation_level + 1);
        }
        //===========================================================================
        else if(!withoutSpaces)
            ret += " ";

        ret += m_values[0].second->to_string(tabulation_level, print_type);
        if(!withoutSpaces) ret += " ";

        //===========================================================================
        if(comment_it != m_comments.end()
            && print_type == PrintType::eWithComment
            && comment_it->second.type == CommentType::eAfterValueOneLine
            ) {
            ret += ToComment(comment_it->second.value)
                   + "\n" + utils::RepeatSymToStr('\t', tabulation_level);
        }
        //===========================================================================
    } else {
        if(!withoutSpaces) ret += "\n";
        std::string tabs_str = !withoutSpaces ? utils::Tab(++tabulation_level) : "";

        for(size_t i = 0; i < m_values.size(); i++) {
            //===========================================================================
            auto comment_it = m_comments.find(i);
            if(comment_it != m_comments.end()
                && print_type == PrintType::eWithComment
                && comment_it->second.type != CommentType::eAfterValueOneLine
                ) {
                ret += "\n";
                switch(comment_it->second.type) {
                case CommentType::eBeforeValueMultiLine: {
                    ret += utils::RepeatSymToStr('\t', tabulation_level)
                           + utils::RepeatSymToStr('#', m_comment_column_size + 2) + "\n";
                    ret += ToComment(comment_it->second.value, tabulation_level, m_comment_column_size) + "\n";
                    ret += utils::RepeatSymToStr('\t', tabulation_level)
                           + utils::RepeatSymToStr('#', m_comment_column_size + 2) + "\n";
                    break;
                }
                case CommentType::eBeforeValue: {
                    ret += ToComment(comment_it->second.value, tabulation_level, 0) + "\n";
                    break;
                }
                default: break;
                }
            }
            //===========================================================================

            if(!withoutSpaces) ret += tabs_str;
            if(column_size != 0) {
                if (m_values[i].first == ValueType::eJson)
                    m_values[i].getJson().setCommentColumnSize(column_size);
                else if(m_values[i].first == ValueType::eArray)
                    m_values[i].getArray().setCommentColumnSize(column_size);
            }
            ret += m_values[i].second->to_string(tabulation_level, print_type);
            if(i < m_values.size() - 1) ret += ",";

            //===========================================================================
            if(comment_it != m_comments.end()
                && print_type == PrintType::eWithComment
                && comment_it->second.type == CommentType::eAfterValueOneLine
                ) {
                ret += " " + ToComment(comment_it->second.value);
            }
            //===========================================================================

            if(!withoutSpaces) ret += "\n";
        }

        if(!withoutSpaces) ret += utils::Tab(--tabulation_level);
    }

    ret += "]"; //end of array

    if(print_type == PrintType::eWithComment
        && m_preview_comment.type == CommentType::eAfterValueOneLine
        && !m_preview_comment.value.empty()
        ) {
        ret += " " + ToComment(m_preview_comment.value);
    }

    return ret;
}

bool JArray::operator==(const JArray &other) const {
    if(this->size() != other.size()) return false;

    for(auto it1 = this->m_values.begin(), it2 = other.m_values.begin();
         it1 != this->m_values.end() && it2 != other.m_values.end();
         it1++, it2++) {
        if(*it1 != *it2) return false;
    }

    return true;
}

Element &JArray::operator[](const size_t index) {
    if(m_values.empty())
        __ARRAY_EMPTY_EXCEPTION__
    __CHECK_INDEX_BOUND__(this, index)

    return m_values[index];
}

Element &JArray::operator[](const std::vector<std::string> &complex_name) {
    if(m_values.empty())
        __ARRAY_EMPTY_EXCEPTION__

    std::vector<std::string>::const_iterator it = complex_name.begin();
    if(!utils::isNumber(*it, false))
        __ARRAY_INCORRECT_INDEX_EXCEPTION__
    Element& el = (*this)[stoi(*it++)]; //находим первый элемент списка
    for (; el.first != ValueType::eNull && it != complex_name.end(); it++) {
        bool isNumber = utils::isNumber(*it, false);
        switch(el.first) {
        case eJson:
            el = el.getInnerValue(*it);
            if(el.first == ValueType::eNull) {
                if(isNumber)
                    el = el.getInnerValue(stoi(*it));
                else
                    __JSON_KEY_NOT_FOUND_EXCEPTION__
            }
            break;
        case eArray:
            //для массива возможно обращение только по числовому индексу!
            if(isNumber)
                el = el.getInnerValue(stoi(*it));
            else
                __ARRAY_INCORRECT_INDEX_EXCEPTION__
            break;
        default:
            //продолжать поиск можно только по двум структурам!
            __INCORRECT_TYPE_ELEMENT_FOR_INDEX_EXCEPTION__
        }
    }

    return el;
}

JArray &JArray::erase(const size_t index) {
    if(index <= m_values.size() - 1)
        m_values.erase(m_values.cbegin() + index);

    return *this;
}

Comment &JArray::getComment(const size_t index) {
    __CHECK_INDEX_BOUND2__(m_values, index);

    auto it = m_comments.find(index);
    if(it == m_comments.end())
        throw std::invalid_argument("comment for index '" + std::to_string(index) + "' not found");
    return it->second;
}


// ====================================================================================== JArray
// *
// *
// Json ========================================================================================
Json::Json(const Json& other) {
    for(const JPair &el : other.m_values)
        m_values.push_back(std::make_pair(el.first, Element(el.second)));
}

Json::Json(const JVector &vec) {
    for(JVector::const_iterator j_it = vec.begin(); j_it != vec.end(); j_it++)
        put(j_it->first, j_it->second);
}

Json &Json::operator=(const Json &other) {
    this->clear();
    for(const JPair &el : other.m_values)
        m_values.push_back(std::make_pair(el.first, Element(el.second)));

    return *this;
}

Json &Json::put(const std::string &key, const Element &element, const bool rewrite) {
    if(!contains(key))
        m_values.push_back(JPair(key, element));
    else if(rewrite)
        updateValue(key, element);
    return *this;
}

Json &Json::put(const Json &json, const bool rewrite) {
    for(const JPair &pair : json.m_values) {
        if(contains(pair.first)) {
            if(!rewrite)    continue;
            else            updateValue(pair.first, pair.second);
        } else {
            put(pair.first, pair.second);
        }
    }

    return *this;
}

bool Json::parseJson(const std::string& json_str) {
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
            if(utils::CharsInString(json_str[i], __SPACES__)) continue;
            else if(json_str[i] == '{')
                ChangeNextState(state, NextReadState::eKey);
            else
                exit = true;
            break;
        }
        case eJsonEnd: {
            if(utils::CharsInString(json_str[i], __SPACES__)) continue;
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
                if(utils::CharsInString(json_str[i + 1], __POSIBLE_COLON__) || (json_str[i + 1] == '}' && (i + 1 == endIndex)))
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
        case eValue: { //может быть числом, строкой, Json или JArray
            if(value.empty())   isValue = true;
            if(isValue)         value += json_str[i];

            //значение считано полностью?
            if(i + 1 < json_str.length()) { //следующий символ существует
                if(utils::CharsInString(json_str[i + 1], ",\n")
                    || (json_str[i + 1] == '}' && ((i + 1) == endIndex)))
                    isValue = false;
            }

            if(!isValue) { //это конец значения?
                valueType = CheckValue(value);

                switch(valueType) {
                case eNumber:   {
                    return_code = true;
                    double num;
                    try {
                        num = std::stod(value);
                    } catch (...) {
                        return_code = false;
                    }

                    if(return_code)
                        this->put(key, std::stod(value));

                    break;
                }
                case eBool:     {
                    return_code = utils::isBool(value);
                    if(return_code)
                        this->put(key, utils::toBool(value));

                    break;
                }
                case eString:   {
                    return_code = true; //NOTE: выше уже проверили синтаксис

                    if(return_code)
                        this->put(key, value);
                    break;
                }
                case eJson:     {
                    Json _innerJson;
                    if(!_innerJson.parseJson(value)) {
                        std::cout << "parse error in key:" << key
                                  << "valueType:" << to_string(valueType)
                                  << std::endl;
                        exit = true;
                    } else {
                        return_code = true;
                        this->put(key, _innerJson);
                    }
                    break;
                }
                case eArray:    {
                    JArray _innerArray;
                    return_code = _innerArray.parseArray(value);
                    if(!return_code) {
                        std::cout << "parse error in key:" << key
                                  << "valueType:" << to_string(valueType)
                                  << std::endl;
                        exit = true;
                    } else {
                        return_code = true;
                        this->put(key, _innerArray);
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
            if(utils::CharsInString(json_str[i], __SPACES__))
                continue;

            if(!utils::CharsInString(json_str[i], __POSIBLE_COLON__)) {
                std::cout << "exp: ':' or '='" << std::endl;
                return_code = false;
                exit = true;
            } else {
                ChangeNextState(state, NextReadState::eValue);
            }
            break;
        }
        case eComma: {
            if(utils::CharsInString(json_str[i], __SPACES_WITHOUT_SEPARATORS__))
                continue;

//            if(json_str[i] != ',') {
            if(!utils::CharsInString(json_str[i], __SEPARATORS__)) {
                if(json_str[i] != '}') {
                    std::cout << "exp: ',' or '\\n'" << std::endl;
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

bool Json::readFile(const std::string& path) { //TODO: read from INI/YAML
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cout << "File not found" << std::endl;
        return false;
    }

    std::string temp_string;
    bool nextStrStartFromComment = false;
    char quote = 0;
    char start_comment = 0;
    char stop_comment = 0;
    std::string json_str;
    while(getline(file, temp_string)) {
        std::cout << "prepare(" << (nextStrStartFromComment ? "true" : "false") << "): " << temp_string << std::endl;
        utils::RemoveComments(temp_string, nextStrStartFromComment, quote, start_comment, stop_comment);
        if(!temp_string.empty() && !utils::OnlySpaces(temp_string)) {
            json_str += temp_string + '\n';
            std::cout << "temp: " << temp_string << std::endl;
        }
    }
    file.close();

    //обработка JSON
    return this->parseJson(json_str);
}

bool Json::writeFile(const std::string& path, int16_t tabulation_level) { //TODO: нужно расширить!
    std::ofstream file(path);
    if (!file.is_open())
        return false;

    file << this->to_string(tabulation_level) << std::endl;

    file.flush();
    file.close();
    return true;
}

std::string Json::to_string(int16_t tabulation_level, const PrintType print_type, const uint8_t column_size) const {

    std::string ret;
    bool withoutSpaces = tabulation_level < 0 && print_type == PrintType::eWithoutComment;

    if(print_type == PrintType::eWithComment
        && m_preview_comment.type != CommentType::eAfterValueOneLine
        && !m_preview_comment.value.empty()
        ) {
        ret += "\n";
        switch(m_preview_comment.type) {
        case CommentType::eBeforeValueMultiLine: {
            ret += utils::RepeatSymToStr('\t', tabulation_level)
                   + utils::RepeatSymToStr('#', m_comment_column_size + 2) + "\n";
            ret += ToComment(m_preview_comment.value, tabulation_level, m_comment_column_size) + "\n";
            ret += utils::RepeatSymToStr('\t', tabulation_level)
                   + utils::RepeatSymToStr('#', m_comment_column_size + 2) + "\n";
            break;
        }
        case CommentType::eBeforeValue: {
            ret += ToComment(m_preview_comment.value, tabulation_level) + "\n";
            break;
        }
        default: break;
        }
    }

    ret += "{"; //start of json

    if(m_values.size() == 1
        && m_values[0].second.first != ValueType::eJson
        && m_values[0].second.first != ValueType::eArray
        ) {
        //===========================================================================
        auto comment_it = m_comments.find(m_values[0].first);
        if(comment_it != m_comments.end()
            && print_type == PrintType::eWithComment
            && comment_it->second.type != CommentType::eAfterValueOneLine
            ) {
            ret += "\n";
            switch(comment_it->second.type) {
            case CommentType::eBeforeValueMultiLine: {
                ret += utils::RepeatSymToStr('\t', tabulation_level + 1)
                       + utils::RepeatSymToStr('#', m_comment_column_size + 2) + "\n";
                ret += ToComment(comment_it->second.value, tabulation_level + 1, m_comment_column_size) + "\n";
                ret += utils::RepeatSymToStr('\t', tabulation_level + 1)
                       + utils::RepeatSymToStr('#', m_comment_column_size + 2) + "\n";
                break;
            }
            case CommentType::eBeforeValue: {
                ret += utils::RepeatSymToStr('\t', tabulation_level + 1)
                       + ToComment(comment_it->second.value, tabulation_level + 1) + "\n";
                break;
            }
            default: break;
            }
            ret += utils::RepeatSymToStr('\t', tabulation_level + 1);
        }
        //===========================================================================
        else if(!withoutSpaces)
            ret += " ";

        ret += "\"" + m_values[0].first + "\"";
        if(!withoutSpaces) ret += " ";
        ret += ":";
        if(!withoutSpaces) ret += " ";
        if(column_size != 0) {
            if (m_values[0].second.first == ValueType::eJson)
                m_values[0].second.getJson().setCommentColumnSize(column_size);
            else if(m_values[0].second.first == ValueType::eArray)
                m_values[0].second.getArray().setCommentColumnSize(column_size);
        }
        ret += m_values[0].second.second->to_string(tabulation_level, print_type);
        if(!withoutSpaces) ret += " ";

        //===========================================================================
        if(comment_it != m_comments.end()
            && print_type == PrintType::eWithComment
            && comment_it->second.type == CommentType::eAfterValueOneLine
            ) {
            ret += ToComment(comment_it->second.value)
                   + "\n" + utils::RepeatSymToStr('\t', tabulation_level);
        }
        //===========================================================================
    } else {
        if(!withoutSpaces) ret += "\n";
        std::string tabs_str = !withoutSpaces ? utils::Tab(++tabulation_level) : "";

        size_t i = 0;
        for(const JPair& el : m_values) {
            //===========================================================================
            auto comment_it = m_comments.find(el.first);
            if(comment_it != m_comments.end()
                && print_type == PrintType::eWithComment
                && comment_it->second.type != CommentType::eAfterValueOneLine
                ) {
                ret += "\n";
                switch(comment_it->second.type) {
                case CommentType::eBeforeValueMultiLine: {
                    ret += utils::RepeatSymToStr('\t', tabulation_level)
                           + utils::RepeatSymToStr('#', m_comment_column_size + 2) + "\n";
                    ret += ToComment(comment_it->second.value, tabulation_level, m_comment_column_size) + "\n";
                    ret += utils::RepeatSymToStr('\t', tabulation_level)
                           + utils::RepeatSymToStr('#', m_comment_column_size + 2) + "\n";
                    break;
                }
                case CommentType::eBeforeValue: {
                    ret += ToComment(comment_it->second.value, tabulation_level, 0) + "\n";
                    break;
                }
                default: break;
                }
            }
            //===========================================================================

            ret += tabs_str + "\"" + el.first + "\"";
            if(!withoutSpaces) ret += " ";
            ret += ":";
            if(!withoutSpaces) ret += " ";
            if(column_size != 0) {
                if (el.second.first == ValueType::eJson)
                    el.second.getJson().setCommentColumnSize(column_size);
                else if(el.second.first == ValueType::eArray)
                    el.second.getArray().setCommentColumnSize(column_size);
            }
            ret += el.second.second->to_string(tabulation_level, print_type);
            if(i < m_values.size() - 1) ret += ",";

            //===========================================================================
            if(comment_it != m_comments.end()
                && print_type == PrintType::eWithComment
                && comment_it->second.type == CommentType::eAfterValueOneLine
                ) {
                ret += " " + ToComment(comment_it->second.value);
            }
            //===========================================================================

            if(!withoutSpaces) ret += "\n";
            i++;
        }

        if(!withoutSpaces) ret += utils::Tab(--tabulation_level);
    }

    ret += "}"; //end of json

    if(print_type == PrintType::eWithComment
        && m_preview_comment.type == CommentType::eAfterValueOneLine
        && !m_preview_comment.value.empty()
        ) {
        ret += " " + ToComment(m_preview_comment.value);
    }

    return ret;
}

bool Json::contains(const std::string &key) {
    for(auto& el : m_values) {
        if(el.first == key)
            return true;
    }

    return false;
}

Json &Json::updateValue(const std::string &key, const Element &new_value) {
    if(contains(key))
        (*this)[key] = Element(new_value);
    else
        put(key, Element(new_value));

    return *this;
}

bool Json::operator==(const Json &other) const {
    if(this->size() != other.size()) return false;

    for(auto it1 = this->m_values.begin(), it2 = other.m_values.begin();
         it1 != this->m_values.end() && it2 != other.m_values.end();
         it1++, it2++
         ) {
        if(it1->second != it2->second) return false;
    }

    return true;
}

Element &Json::operator[](const size_t index) {
    if(m_values.empty())
        __JSON_EMPTY_EXCEPTION__
    __CHECK_INDEX_BOUND__(this, index)

    return m_values[index].second;
}

Element &Json::operator[](const std::string &name) {
    if(m_values.empty())
        __JSON_EMPTY_EXCEPTION__

    bool element_found = false;
    Element* el = &m_values[0].second;
    for(size_t i = 0; i < m_values.size(); i++)
        if(m_values[i].first == name) {
            el = &m_values[i].second;
            element_found = true;
            break;
        }

    if(!element_found)
        __KEY_NOT_FOUND_EXCEPTION__(name)

    return *el;
}

Element &Json::operator[](const std::vector<std::string> &complex_name) {
    if(m_values.empty())
        __JSON_EMPTY_EXCEPTION__

    Element& el = (*this)[complex_name[0]]; //находим первый элемент списка
    std::vector<std::string>::const_iterator it = complex_name.begin() + 1; //первый элемент пропускаем
    for (; el.first != ValueType::eNull && it != complex_name.end(); it++) {
        bool isNumber = utils::isNumber(*it, false);
        switch(el.first) {
        case eJson:
            el = el.getInnerValue(*it);
            if(el.first == ValueType::eNull) {
                if(isNumber)
                    el = el.getInnerValue(stoi(*it));
                else
                    __JSON_KEY_NOT_FOUND_EXCEPTION__
            }
            break;
        case eArray:
            //для массива возможно обращение только по числовому индексу!
            if(isNumber)
                el = el.getInnerValue(stoi(*it));
            else
                __ARRAY_INCORRECT_INDEX_EXCEPTION__
            break;
        default:
            //продолжать поиск можно только по двум структурам!
            __INCORRECT_TYPE_ELEMENT_FOR_INDEX_EXCEPTION__
        }
    }

    return el;
}

Json &Json::erase(const size_t index) {
    if(index <= m_values.size() - 1)
        m_values.erase(m_values.cbegin() + index);

    return *this;
}

Json &Json::erase(const std::string &key) {
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

Json &Json::erase(const std::vector<std::string> &keys) {
    for(const std::string &key : keys)
        this->erase(key);

    return *this;
}

Comment &Json::getComment(const std::string &key) {
    auto it = m_comments.find(key);
    if(it == m_comments.end())
        throw std::invalid_argument("key '" + key + "' not found");
    return it->second;
}

Comment &Json::getComment(const size_t index) {
    __CHECK_INDEX_BOUND2__(m_values, index);

    auto it = m_comments.find(m_values[index].first);
    if(it == m_comments.end())
        throw std::invalid_argument("key '" + m_values[index].first + "' not found");
    return it->second;
}
// ======================================================================================== Json
// *
// *
// STATIC FUNCTIONS ============================================================================
ValueType CheckValue(std::string& value) {
    //    std::cout << "CheckValue(): \"" << value << "\"" << std::endl;
    bool isValue = false;
    std::string _value;
    ValueType vType = eNull;
    for(size_t i = 0; i < value.length(); i++) {
        if(!isValue && !utils::CharsInString(value[i], __SPACES__))
            isValue = true;

        if(isValue) {
            if(vType == ValueType::eNull) {
                if(utils::isNumber(value[i]))   vType = ValueType::eNumber;
                else if(value[i] == '"')        vType = ValueType::eString;
                else if(value[i] == '{')        vType = ValueType::eJson;
                else if(value[i] == '[')        vType = ValueType::eArray;
                else if(!utils::CharsInString(value[i], __SPACES__))
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

bool CheckDouble(std::string& value) {
//    std::cout << "CheckDouble(): \"" << value << "\"" << std::endl;
    uint32_t pCounter = 0;
    //remove spaces
    std::string temp;
    for(char ch : value)
        if(!utils::CharsInString(ch, __SPACES__)) temp += ch;

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

bool CheckBool(std::string& value) {
//    std::cout << "CheckBool(): \"" << value << "\"" << std::endl;
    std::string temp;
    bool flag = false;
    for(char c : value) {
        if(utils::CharsInString(c, __SPACES__)) {
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

bool CheckString(std::string& value) {
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
                if(!utils::CharsInString(value[i], __SPACES__)) {
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

bool CheckJson(std::string& value) {
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
                if(!utils::CharsInString(value[i], __SPACES__)) {
                    std::cout << "Error with parse Json in: [" << value[i] << "]" << std::endl;
                    return false;
                }
            }
        } else if(!utils::CharsInString(value[i], __SPACES__)) {
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

bool CheckArray(std::string& value) {
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
                if(!utils::CharsInString(value[i], __SPACES__)) {
                    std::cout << "Error with parse JArray in: [" << value[i] << "]" << std::endl;
                    return false;
                }
            }
        } else if(!utils::CharsInString(value[i], __SPACES__)) {
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

//удалить пробелы в начале и конце строки
void RemoveIllegalSpaces(std::string& string) {
    if(!string.empty()) {
        if(string.find(' ') == -1) return;
        auto start_index = string.find_first_not_of(' ');

        while(string.back() == ' ')
            string.pop_back();

        string = string.substr((start_index != -1) ? start_index : 0, string.size());
    }
}

std::string separators_symbols(" \t.,;:->+?!/\\*$#@&()[]\n");
std::string ToComment(const std::string &comment_string, const uint8_t tabulation_level, const uint8_t column_size) {
    std::string ret;
    uint8_t column_counter = 0;
    std::string current_string = "";
    std::string prefix = utils::RepeatSymToStr('\t', tabulation_level) + "# ";
    char last_symbol = ' ';
    std::vector<size_t> separators;
    separators.reserve(10);
    bool isLastSymbol = false;

    for(size_t i = 0; i < comment_string.size(); i++) {
        char ch = comment_string[i];

        //игнор "двойного" пробела
        if(last_symbol == ' ' && ch == ' ')
            continue;

        if(i == comment_string.size() - 1)
            isLastSymbol = true;

        //если встретили разделитель
        if(utils::CharsInString(ch, separators_symbols))
            separators.push_back(current_string.size());

        current_string += ch;
        column_counter++;
        last_symbol = ch;

        if(ch == '\n') {
            //удалить пробелы в начале и конце строки
            RemoveIllegalSpaces(current_string);

            //вывести если не пустое
            if(!current_string.empty())
                ret += prefix + current_string;

            current_string = "";
            separators.clear();
        }

        if((column_counter >= column_size)
            && (utils::CharsInString(ch, separators_symbols) || isLastSymbol)
            && column_size != 0
            ) {
            column_counter = 0;

            //удалить пробелы в начале и конце строки
            RemoveIllegalSpaces(current_string);

            //вывести если не пустое
            if(!current_string.empty()) {
                ret += prefix;

                //если превышен максимальный размер строки
                if(current_string.size() > column_size) {
                    std::string left = current_string.substr(0, separators[separators.size() - ((!isLastSymbol) ? 2 : 1)] + 1);
                    RemoveIllegalSpaces(left);
                    current_string = current_string.substr(separators[separators.size() - ((!isLastSymbol) ? 2 : 1)] + 1);
                    if(!utils::CharsInString(current_string.back(), separators_symbols))
                        current_string += ' ';
                    ret += left + "\n";

                    //снова найти индексы разделителей
                    column_counter = current_string.size();
                    separators.clear();
                    for(size_t j = 0; j < current_string.size(); j++) {
                        if(utils::CharsInString(current_string[j], separators_symbols))
                            separators.push_back(j);
                    }
                } else {
                    ret += current_string;
                    current_string = "";
                    if(!isLastSymbol)
                        ret += "\n";
                    separators.clear();
                }
            }
        }
    }

    if(!current_string.empty())
        ret += prefix + current_string;


    return ret;
}
// ============================================================================ STATIC FUNCTIONS


