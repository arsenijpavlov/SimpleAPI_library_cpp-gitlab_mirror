#include "Json.h"

#include <fstream>
#include <iostream>
#include <set>

//TODO: #define DEBUG_OUTPUT когда-нибудь потом...

#define __SPACES__                      " \n\t"
#define __KEY_VALUE_SEPARATOR__         ":="
#define __SEPARATORS__                  ",\n"
#define __SPACES_WITHOUT_SEPARATORS__   " \t"
#define __POSIBLE_COLON__               ":="
#define __BORDER_SYMBOLS__              "@#*-=@"
#define __COMMENT_SEPARATOR_SYMBOLS__   " \t.,;:->+?!/\\*$#@&()[]\n"


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
        throw std::invalid_argument("This element is not a 'Number' type: " + ::to_string(first));
    return reinterpret_cast<DoubleElement*>(second)->m_value;
}

bool &Element::getBool() const {
    if(first != ValueType::eBool)
        throw std::invalid_argument("This element is not a 'Bool' type: " + ::to_string(first));
    return reinterpret_cast<BoolElement*>(second)->m_value;
}

std::string &Element::getString() const {
    if(first != ValueType::eString)
        throw std::invalid_argument("This element is not a 'String' type: " + ::to_string(first));
    return reinterpret_cast<StringElement*>(second)->m_value;
}

Json &Element::getJson() const {
    if(first != ValueType::eJson)
        throw std::invalid_argument("This element is not a 'Json' type: " + ::to_string(first));
    return reinterpret_cast<JsonElement*>(second)->m_value;
}

JArray &Element::getArray() const {
    if(first != ValueType::eArray)
        throw std::invalid_argument("This element is not a 'JArray' type: " + ::to_string(first));
    return reinterpret_cast<JArrayElement*>(second)->m_value;
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

void JArray::parseArray(const std::string &string_of_array, const bool enable_comment,
                        const ConfigFormat config_format) {
    switch(config_format) {
    case ConfigFormat::eJSON:
        parseJSON_array(string_of_array, enable_comment);
        break;
    case ConfigFormat::eYAML:
        parseYAML_array(string_of_array, enable_comment);
        break;
    case ConfigFormat::eINI:
        parseINI_array(string_of_array, enable_comment);
        break;
    }
}

void JArray::parseJSON_array(const std::string &string_of_array, const bool enable_comment) {
    clear();

    bool isOneLineComment   = false;
    bool isMultiLineComment = false;
    char firstMLCSym, secondMLCSym;
    uint8_t commentCounter  = 0;
    std::string currentComment;
    currentComment.reserve(100);

    bool isCriticalError    = false;
    bool isQuotes           = false;
    bool isWordStarted      = false;
    bool isWordFinished     = false;

    uint16_t innerJsonCounter   = 0;
    uint16_t innerArrayCounter  = 0;
    std::string value_string;
    value_string.reserve(100);
    Comment valueComment_before;
    Comment valueComment_after;
    bool isValueCommentAfterSaved = true; //изначально TRUE, чтобы не сработало для первого прохода
    Element value_element;

    uint16_t line_counter   = 0; //NOTE: (JArray) ограничение на FFFF строк
    uint16_t symbol_counter = 0; //NOTE: (JArray) ограничение на FFFF символов в строке

    enum States {
        ARRAY_START,
        ARRAY_VALUE,
        ARRAY_ELEMENT_SEPARATOR,
        ARRAY_FINISH
    } state = ARRAY_START;
    enum ValueFormat {
        VALUE_NOPE,
        VALUE_JSON,
        VALUE_ARRAY,
        VALUE_OTHER
    } value_format = VALUE_NOPE;

    for(size_t i = 0; (i < string_of_array.length()) && (state != ARRAY_FINISH); i++) {
        char current = string_of_array[i];
        symbol_counter++; //TODO: проверить точность

        //поиск комментариев
        if(!isOneLineComment && !isMultiLineComment) {
            char next = (string_of_array.length() > i + 1 ? string_of_array[i + 1] : 0);
            switch(CheckComment(current, next, i)) {
            case CommentType::eNotComment: break;
            case CommentType::eOneLineComment: {
                isOneLineComment = true;
                if(!currentComment.empty() && enable_comment)
                    currentComment += "\n";
                //счётчик строк и столбцов =============================================
                if(current == '\n') {
                    line_counter++;
                    symbol_counter = 0; //должен перескочить строго на следующей строке
                } //====================================================================
                continue;
            }
            case CommentType::eMultiLineComment: {
                firstMLCSym = current;
                secondMLCSym = next;

                isMultiLineComment = true;
                if(!currentComment.empty() && enable_comment)
                    currentComment += "\n";
                //счётчик строк и столбцов =============================================
                if(current == '\n') {
                    line_counter++;
                    symbol_counter = 0; //должен перескочить строго на следующей строке
                } //====================================================================
                continue;
            }
            }
        }

        //обработка комментариев
        if(isOneLineComment) {
            //если следующий символ должен обрабатываться другим кодом
            if((current == '\n') || ((string_of_array.length() > i + 1) && (string_of_array[i + 1] == '\n'))) {
                isOneLineComment = false;
                currentComment += current;
            } else {
                currentComment += current;
            }

            //счётчик строк и столбцов =============================================
            if(current == '\n') {
                line_counter++;
                symbol_counter = 0; //должен перескочить строго на следующей строке
            } //====================================================================
            continue;
        }
        if(isMultiLineComment) {
            //нужен следующий символ, если нет - исключение
            if(string_of_array.length() <= i + 1)
                throw std::invalid_argument("invalid length of input JSON string");

            if((current == secondMLCSym) && (string_of_array[i + 1] == firstMLCSym)) {
                isMultiLineComment = false;
                i++;    //многострочные комментарии всегда обособляются двумя символами
            } else {
                currentComment += current;
            }

            //счётчик строк и столбцов =============================================
            if(current == '\n') {
                line_counter++;
                symbol_counter = 0; //должен перескочить строго на следующей строке
            } //====================================================================
            continue;
        }


        //работа с синтаксисом JSON_ARRAY
        if(!isOneLineComment && !isMultiLineComment) {
            switch(state) {
            case ARRAY_START: {
                //пропуск пробелов ====================================================
                if(utils::CharsInString(current, __SPACES__))
                    break;
                //=====================================================================

                //работа с комментариями (первичный) ==================================
                if(!currentComment.empty() && enable_comment) {
                    addPreviewComment(FromComment(currentComment, m_comment_column_size, m_comment_sym));
                    currentComment = "";
                } //===================================================================

                if(current != '[') {
                    isCriticalError = true;
                    break;
                }
                state = ARRAY_VALUE;

                break;
            }
            case ARRAY_VALUE: {
                //пропуск пробелов ====================================================
                if(current == '\n' && !isValueCommentAfterSaved) {
                    //работа с комментариями (после значения #2) ==========================
                    if(!currentComment.empty() && enable_comment) {
                        addComment_after(m_values.size() - 1, FromComment(currentComment, m_comment_column_size, m_comment_sym));
                        currentComment = "";
                    } //===================================================================
                    isValueCommentAfterSaved = true;
                    break;
                }
                if(utils::CharsInString(current, __SPACES__) && !isQuotes && value_format == ValueFormat::VALUE_NOPE)
                    break;
                //=====================================================================
                if(current == ']') {
                    state = ARRAY_FINISH;
                    break;
                }
                //=====================================================================

                if(!isWordStarted) {
                    isWordStarted = true;
                    isValueCommentAfterSaved = false;
                    value_string.clear();
                }

                switch(current) {
                case '{': {
                    if(!isQuotes) {
                        if(value_format == ValueFormat::VALUE_NOPE) value_format = ValueFormat::VALUE_JSON;
                        innerJsonCounter++;
                    }
                    break;
                }
                case '}': {
                    if(!isQuotes) innerJsonCounter--;
                    break;
                }
                case '[': {
                    if(!isQuotes) {
                        if(value_format == ValueFormat::VALUE_NOPE) value_format = ValueFormat::VALUE_ARRAY;
                        innerArrayCounter++;
                    }
                    break;
                }
                case ']': {
                    if(!isQuotes) innerArrayCounter--;
                    break;
                }
                case '"': {
                    isQuotes = !isQuotes;
                }
                default: {
                    if(value_format == ValueFormat::VALUE_NOPE) value_format = ValueFormat::VALUE_OTHER;
                    break;
                }
                }

                //экранированные кавычки ВСЕГДА заносится в значение
                if(current == '\\' && string_of_array.length() > i + 1) {
                    char e_ch = utils::getEscChar(string_of_array[i + 1]);
                    if(e_ch != 0) {
                        value_string += '\\' + e_ch;
                        i++;
                        break;
                    }
                }


                //поиск конца значения
                switch(value_format) {
                case VALUE_JSON: {
                    value_string += current;
                    if(innerJsonCounter == 0)
                        isWordFinished = true;

                    break;
                }
                case VALUE_ARRAY: {
                    value_string += current;
                    if(innerArrayCounter == 0)
                        isWordFinished = true;

                    break;
                }
                case VALUE_OTHER: {
                    if(!isQuotes && utils::CharsInString(current, __SPACES__))
                        isWordFinished = true;
                    if(isQuotes
                        && (innerJsonCounter == 0) && (innerArrayCounter == 0)
                        && string_of_array.length() > i + 1
                        && string_of_array[i + 1] == '"') {
                        isWordFinished = true;
                        i++;
                    }

                    if(current != '"')
                        value_string += current;
                    break;
                }
                default: break;
                }
                //если следующий символ должен обрабатываться другим кодом
                if(!isWordFinished
                    && !isQuotes
                    && (innerJsonCounter == 0) && (innerArrayCounter == 0)) {
                    if((string_of_array.length() > i + 1)
                        && utils::CharsInString(string_of_array[i + 1], __SEPARATORS__ + std::string((value_format != VALUE_ARRAY) ? "]" : "")))
                        isWordFinished = true;
                }


                if(isWordFinished) {
                    isWordStarted = false; //страховка
                    isWordFinished = false;

                    switch(value_format) {
                    case VALUE_OTHER: {
                        switch(CheckValue(value_string)) {
                        case eNumber:   {
                            double num;
                            try {
                                push_back(std::stod(value_string));
                            } catch (...) {
                                isCriticalError = true;
                            }

                            break;
                        }
                        case eBool:     {
                            if(utils::isBool(value_string))
                                push_back(utils::toBool(value_string));
                            else
                                isCriticalError = true;

                            break;
                        }
                        case eString:   {
                            push_back(value_string);
                            break;
                        }
                        default: //значение не определено
                            isCriticalError = true;
                            break;
                        }

                        break;
                    }
                    case VALUE_JSON: {
                        Json _innerJson;
                        try {
                            _innerJson.parseJSON(value_string, enable_comment);
                            push_back(_innerJson);
                        } catch (std::invalid_argument& e) {
                            isCriticalError = true;
                        }

                        break;
                    }
                    case VALUE_ARRAY:    {
                        JArray _innerArray;
                        try {
                            _innerArray.parseArray(value_string, enable_comment);
                            push_back(_innerArray);
                        } catch (std::invalid_argument& e) {
                            isCriticalError = true;
                        }

                        break;
                    }
                    default:
                        isCriticalError = true;
                        break;
                    }

                    //работа с комментариями (перед значением) ============================
                    if(!currentComment.empty() && enable_comment) {
                        addComment_before(m_values.size() - 1, FromComment(currentComment, m_comment_column_size, m_comment_sym));
                        currentComment = "";
                    } //===================================================================

                    state = ARRAY_ELEMENT_SEPARATOR;
                    value_format = ValueFormat::VALUE_NOPE;
                    isQuotes = false;
                }

                break;
            }
            case ARRAY_ELEMENT_SEPARATOR: {
                //пропуск пробелов ====================================================
                if(utils::CharsInString(current, __SPACES_WITHOUT_SEPARATORS__))
                    break;
                //=====================================================================
                if(!utils::CharsInString(current, __SEPARATORS__ "]")) {
                    isCriticalError = true;
                    break;
                }

                if(current == '\n') {
                    //работа с комментариями (после значения #1) ==========================
                    if(!currentComment.empty() && enable_comment) {
                        addComment_after(m_values.size() - 1, FromComment(currentComment, m_comment_column_size, m_comment_sym));
                        currentComment = "";
                    } //===================================================================
                    isValueCommentAfterSaved = true;
                } else if(current == ']') {
                    state = ARRAY_FINISH;
                    break;
                }
                state = ARRAY_VALUE;

                break;
            }
            default: break;
            }
        }

        if(isCriticalError) {
            clear();
            throw std::invalid_argument("JArray parse error at line " + std::to_string(line_counter) + ":" + std::to_string(symbol_counter));
        }

        //счётчик строк и столбцов =============================================
        if(current == '\n') {
            line_counter++;
            symbol_counter = 0; //должен перескочить строго на следующей строке
        } //====================================================================
    }


    if(state != ARRAY_FINISH) {
        clear();
        throw std::invalid_argument("JArray parse error, end of JSON array structure not found");
    }
}

void JArray::parseYAML_array(const std::string &string_of_array, const bool enable_comment) {
    //TODO: JArray::parseYAML_array()
}

void JArray::parseINI_array(const std::string &string_of_array, const bool enable_comment) {
    //TODO: JArray::parseINI_array()
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

std::string JArray::to_string(int16_t tabulation_level, const bool enable_comment, const
                              uint8_t column_size, const ConfigFormat config_format) const {
    switch(config_format) {
    case ConfigFormat::eJSON:
        return to_JSON_string(tabulation_level, enable_comment, column_size);
    case ConfigFormat::eYAML:
        return to_YAML_string(tabulation_level, enable_comment, column_size);
    case ConfigFormat::eINI:
        return to_INI_string(tabulation_level, enable_comment, column_size);
    }

    return "";
}

std::string JArray::to_JSON_string(int16_t tabulation_level, const bool enable_comment,
                                   const uint8_t column_size) const {
    if(m_values.empty()) return "[]";

    std::string ret;
    bool withoutSpaces = tabulation_level < 0 && !enable_comment;

    if(enable_comment && !m_preview_comment.before.empty()) {
        ret += "\n";
        if(m_preview_comment.before.find('\n') != -1) {
            ret += utils::RepeatSymToStr('\t', tabulation_level)
                   + utils::RepeatSymToStr('#', m_comment_column_size + 2) + "\n";
            ret += ToComment(m_preview_comment.before, tabulation_level, m_comment_column_size) + "\n";
            ret += utils::RepeatSymToStr('\t', tabulation_level)
                   + utils::RepeatSymToStr('#', m_comment_column_size + 2) + "\n";
        } else
            ret += ToComment(m_preview_comment.before, tabulation_level) + "\n";
    }

    ret += "["; //start of array

    if(m_values.size() == 1
        && m_values[0].first != ValueType::eJson
        && m_values[0].first != ValueType::eArray
        ) {

        //===========================================================================
        auto comment_it = m_comments.find(0);
        if(comment_it != m_comments.end()
            && enable_comment
            && !comment_it->second.before.empty()
            ) {
            ret += "\n";
            if(comment_it->second.before.find('\n') != -1) {
                ret += utils::RepeatSymToStr('\t', tabulation_level + 1)
                       + utils::RepeatSymToStr('#', m_comment_column_size + 2) + "\n";
                ret += ToComment(comment_it->second.before, tabulation_level + 1, m_comment_column_size) + "\n";
                ret += utils::RepeatSymToStr('\t', tabulation_level + 1)
                       + utils::RepeatSymToStr('#', m_comment_column_size + 2) + "\n";
            } else
                ret += utils::RepeatSymToStr('\t', tabulation_level + 1)
                       + ToComment(comment_it->second.before, tabulation_level + 1) + "\n";
            ret += utils::RepeatSymToStr('\t', tabulation_level + 1);
        }
        //===========================================================================
        else if(!withoutSpaces)
            ret += " ";

        ret += m_values[0].second->to_string(tabulation_level, enable_comment, ConfigFormat::eJSON);
        if(!withoutSpaces) ret += " ";

        //===========================================================================
        if(comment_it != m_comments.end()
            && enable_comment
            && !comment_it->second.after.empty()
            ) {
            ret += ToComment(comment_it->second.after)
                   + "\n" + utils::RepeatSymToStr('\t', tabulation_level);
        }
        //===========================================================================
    } else {
        if(!withoutSpaces) ret += "\n";
        std::string tabs_str = !withoutSpaces ? utils::RepeatSymToStr('\t', ++tabulation_level) : "";

        for(size_t i = 0; i < m_values.size(); i++) {
            //===========================================================================
            auto comment_it = m_comments.find(i);
            if(comment_it != m_comments.end()
                && enable_comment
                && !comment_it->second.before.empty()
                ) {
                ret += "\n";
                if(comment_it->second.before.find('\n') != -1) {
                    ret += utils::RepeatSymToStr('\t', tabulation_level)
                           + utils::RepeatSymToStr('#', m_comment_column_size + 2) + "\n";
                    ret += ToComment(comment_it->second.before, tabulation_level, m_comment_column_size) + "\n";
                    ret += utils::RepeatSymToStr('\t', tabulation_level)
                           + utils::RepeatSymToStr('#', m_comment_column_size + 2) + "\n";
                } else
                    ret += ToComment(comment_it->second.before, tabulation_level, 0) + "\n";
            }
            //===========================================================================

            if(!withoutSpaces) ret += tabs_str;
            if(column_size != 0) {
                if (m_values[i].first == ValueType::eJson)
                    m_values[i].getJson().setCommentColumnSize(column_size);
                else if(m_values[i].first == ValueType::eArray)
                    m_values[i].getArray().setCommentColumnSize(column_size);
            }
            ret += m_values[i].second->to_string(tabulation_level, enable_comment, ConfigFormat::eJSON);
            if(i < m_values.size() - 1) ret += ",";

            //===========================================================================
            if(comment_it != m_comments.end()
                && enable_comment
                && !comment_it->second.after.empty()
                ) {
                ret += " " + ToComment(comment_it->second.after);
            }
            //===========================================================================

            if(!withoutSpaces) ret += "\n";
        }

        if(!withoutSpaces) ret += utils::RepeatSymToStr('\t', --tabulation_level);
    }

    ret += "]"; //end of array

    if(enable_comment && !m_preview_comment.before.empty())
        ret += " " + ToComment(m_preview_comment.after);

    return ret;
}

std::string JArray::to_YAML_string(int16_t tabulation_level, const bool enable_comment,
                                   const uint8_t column_size) const {
    //TODO: JArray::to_YAML_string
    return "";
}

std::string JArray::to_INI_string(int16_t tabulation_level, const bool enable_comment,
                                   const uint8_t column_size) const {
    //TODO: JArray::to_INI_string
    return "";
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

Element &JArray::operator[](const std::vector<std::string> &complex_key) {
    if(m_values.empty())
        __ARRAY_EMPTY_EXCEPTION__

    if(complex_key.size() == 0)
        throw std::invalid_argument("complex_key argument cannot be empty");

    size_t index;
    try {
        index = std::stoi(complex_key[0]);
    } catch(...) {
        __ARRAY_INCORRECT_INDEX_EXCEPTION__
    }

    if(index < size()) { //если индекс внутри допустимого диапазона
        if(complex_key.size() == 1)
            return (*this)[index];
        else {
            Element& el = (*this)[index];

            auto new_complex_key = complex_key;
            new_complex_key.erase(new_complex_key.begin());
            switch(el.first) {
            case eJson:     return (*this)[index].getJson()[new_complex_key];
            case eArray:    return (*this)[index].getArray()[new_complex_key];
            default: __INCORRECT_TYPE_ELEMENT_FOR_INDEX_EXCEPTION__
            }
        }
    } else
        __ARRAY_INCORRECT_INDEX_EXCEPTION__
}

JArray &JArray::erase(const size_t index) {
    if(index <= m_values.size() - 1)
        m_values.erase(m_values.cbegin() + index);

    return *this;
}
// ====================================================================================== JArray
// *
// *
// Json ========================================================================================
Json::Json(const Json& other) {
    for(const JPair &el : other.m_values)
        m_values.push_back(std::make_pair(el.first, Element(el.second)));
}

Json::Json(const std::string &input_string, ConfigFormat config_format) {
    switch (config_format) {
    case ConfigFormat::eJSON:
        parseJSON(input_string);
        break;
    case ConfigFormat::eYAML:
        parseYAML(input_string);
        break;
    case ConfigFormat::eINI:
        parseINI(input_string);
        break;
    }
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

void Json::parseJSON(const std::string &string_of_json, const bool enable_comment) {
    clear();

    bool isOneLineComment   = false;
    bool isMultiLineComment = false;
    char firstMLCSym, secondMLCSym;
    uint8_t commentCounter  = 0;
    std::string currentComment;
    currentComment.reserve(100);

    bool isCriticalError    = false;
    bool isQuotes           = false;
    bool isWordStarted      = false;
    bool isWordFinished     = false;

    std::string key_string;
    key_string.reserve(20);
    Comment keyComment_before;
    Comment keyComment_after;

    uint16_t innerJsonCounter   = 0;
    uint16_t innerArrayCounter  = 0;
    std::string value_string;
    value_string.reserve(100);
    Comment valueComment_before;
    Comment valueComment_after;
    bool isValueCommentAfterSaved = true; //изначально TRUE, чтобы не сработало для первого прохода
    Element value_element;

    uint16_t line_counter   = 0; //NOTE: (Json) ограничение на FFFF строк
    uint16_t symbol_counter = 0; //NOTE: (Json) ограничение на FFFF символов в строке

    enum States {
        JSON_START,
        JSON_KEY,
        JSON_KEY_VALUE_SEPARATOR,
        JSON_VALUE,
        JSON_ELEMENT_SEPARATOR,
        JSON_FINISH
    } state = JSON_START;
    enum ValueFormat {
        VALUE_NOPE,
        VALUE_JSON,
        VALUE_ARRAY,
        VALUE_OTHER
    } value_format = VALUE_NOPE;

    for(size_t i = 0; (i < string_of_json.length()) && (state != JSON_FINISH); i++) {
        char current = string_of_json[i];
        symbol_counter++; //TODO: проверить точность

        //поиск комментариев
        if(!isOneLineComment && !isMultiLineComment) {
            char next = (string_of_json.length() > i + 1 ? string_of_json[i + 1] : 0);
            switch(CheckComment(current, next, i)) {
            case CommentType::eNotComment: break;
            case CommentType::eOneLineComment: {
                isOneLineComment = true;
                if(!currentComment.empty() && enable_comment)
                    currentComment += "\n";
                //счётчик строк и столбцов =============================================
                if(current == '\n') {
                    line_counter++;
                    symbol_counter = 0; //должен перескочить строго на следующей строке
                } //====================================================================
                continue;
            }
            case CommentType::eMultiLineComment: {
                firstMLCSym = current;
                secondMLCSym = next;

                isMultiLineComment = true;
                if(!currentComment.empty() && enable_comment)
                    currentComment += "\n";
                //счётчик строк и столбцов =============================================
                if(current == '\n') {
                    line_counter++;
                    symbol_counter = 0; //должен перескочить строго на следующей строке
                } //====================================================================
                continue;
            }
            }
        }

        //обработка комментариев
        if(isOneLineComment) {
            //если следующий символ должен обрабатываться другим кодом
            if((current == '\n') || ((string_of_json.length() > i + 1) && (string_of_json[i + 1] == '\n'))) {
                isOneLineComment = false;
                currentComment += current;
            } else {
                currentComment += current;
            }

            //счётчик строк и столбцов =============================================
            if(current == '\n') {
                line_counter++;
                symbol_counter = 0; //должен перескочить строго на следующей строке
            } //====================================================================
            continue;
        }
        if(isMultiLineComment) {
            //нужен следующий символ, если нет - исключение
            if(string_of_json.length() <= i + 1)
                throw std::invalid_argument("invalid length of input JSON string");

            if((current == secondMLCSym) && (string_of_json[i + 1] == firstMLCSym)) {
                isMultiLineComment = false;
                i++;    //многострочные комментарии всегда обособляются двумя символами
            } else {
                currentComment += current;
            }

            //счётчик строк и столбцов =============================================
            if(current == '\n') {
                line_counter++;
                symbol_counter = 0; //должен перескочить строго на следующей строке
            } //====================================================================
            continue;
        }


        //работа с синтаксисом JSON
        if(!isOneLineComment && !isMultiLineComment) {
            switch(state) {
            case JSON_START: {
                //пропуск пробелов ====================================================
                if(utils::CharsInString(current, __SPACES__))
                    break;
                //=====================================================================

                //работа с комментариями (первичный) ==================================
                if(!currentComment.empty() && enable_comment) {
                    addPreviewComment(FromComment(currentComment, m_comment_column_size, m_comment_sym));
                    currentComment = "";
                } //===================================================================

                if(current != '{') {
                    isCriticalError = true;
                    break;
                }
                state = JSON_KEY;

                break;
            }
            case JSON_KEY: {
                //пропуск пробелов ====================================================
                if(current == '\n' && !isValueCommentAfterSaved) {
                    //работа с комментариями (после значения #2) ==========================
                    if(!currentComment.empty() && enable_comment) {
                        addComment_after(key_string, FromComment(currentComment, m_comment_column_size, m_comment_sym));
                        currentComment = "";
                    } //===================================================================
                    isValueCommentAfterSaved = true;
                    break;
                }
                if(utils::CharsInString(current, __SPACES__) && !isQuotes && !isWordStarted)
                    break;
                //=====================================================================
                if(current == '}') {
                    state = JSON_FINISH;
                    break;
                }
                //=====================================================================
                if(!isWordStarted) {
                    isWordStarted = true;
                    key_string.clear();
                    isQuotes = false;
                }

                //экранированные кавычки ВСЕГДА заносится в значение
                if(current == '\\' && string_of_json.length() > i + 1) {
                    char e_ch = utils::getEscChar(string_of_json[i + 1]);
                    if(e_ch != 0) {
                        value_string += '\\' + e_ch;
                        i++;
                        break;
                    }
                }

                if(current == '"') {
                    isQuotes = !isQuotes;
                }
                //поиск конца значения
                if(!isQuotes && utils::CharsInString(current, __SPACES__))
                    isWordFinished = true;
                if(isQuotes
                    && string_of_json.length() > i + 1
                    && string_of_json[i + 1] == '"') {
                    isWordFinished = true;
                    i++;
                }

                if(current != '"')
                    key_string += current;

                //если следующий символ должен обрабатываться другим кодом
                if(!isWordFinished && !isQuotes) {
                    if((string_of_json.length() > i + 1)
                        && utils::CharsInString(string_of_json[i + 1], __KEY_VALUE_SEPARATOR__))
                        isWordFinished = true;
                }


                if(isWordFinished) {
                    isWordStarted = false; //страховка
                    isWordFinished = false;

                    //работа с комментариями (перед ключом) ===============================
                    if(!currentComment.empty() && enable_comment) {
                        addComment_before(key_string, FromComment(currentComment, m_comment_column_size, m_comment_sym));
                        currentComment = "";
                    } //===================================================================

                    state = JSON_KEY_VALUE_SEPARATOR;
                }

                break;
            }
            case JSON_KEY_VALUE_SEPARATOR: {
                //пропуск пробелов ====================================================
                if(utils::CharsInString(current, __SPACES__) && !isQuotes)
                    break;
                //=====================================================================

                if(!utils::CharsInString(current, __KEY_VALUE_SEPARATOR__)) {
                    isCriticalError = true;
                    break;
                }

                //работа с комментариями (после ключа (НЕ используется)) ==============
                if(!currentComment.empty() && enable_comment) {
//                    addComment_after(key_string, FromComment(currentComment, m_comment_column_size, m_comment_sym));
                    currentComment = "";
                } //===================================================================

                state = JSON_VALUE;

                break;
            }
            case JSON_VALUE: {
                //пропуск пробелов ====================================================
                if(utils::CharsInString(current, __SPACES__) && !isQuotes && value_format == ValueFormat::VALUE_NOPE)
                    break;
                //=====================================================================
                if(!isWordStarted) {
                    isWordStarted = true;
                    isValueCommentAfterSaved = false;
                    value_string.clear();
                    isQuotes = false;
                }

                switch(current) {
                case '{': {
                    if(!isQuotes) {
                        if(value_format == ValueFormat::VALUE_NOPE) value_format = ValueFormat::VALUE_JSON;
                        innerJsonCounter++;
                    }
                    break;
                }
                case '}': {
                    if(!isQuotes) innerJsonCounter--;
                    break;
                }
                case '[': {
                    if(!isQuotes) {
                        if(value_format == ValueFormat::VALUE_NOPE) value_format = ValueFormat::VALUE_ARRAY;
                        innerArrayCounter++;
                    }
                    break;
                }
                case ']': {
                    if(!isQuotes) innerArrayCounter--;
                    break;
                }
                case '"': {
                    isQuotes = !isQuotes;
                }
                default: {
                    if(value_format == ValueFormat::VALUE_NOPE) value_format = ValueFormat::VALUE_OTHER;
                    break;
                }
                }

                //экранированные кавычки ВСЕГДА заносится в значение
                if(current == '\\' && string_of_json.length() > i + 1) {
                    char e_ch = utils::getEscChar(string_of_json[i + 1]);
                    if(e_ch != 0) {
                        value_string += '\\' + e_ch;
                        i++;
                        break;
                    }
                }


                //поиск конца значения
                switch(value_format) {
                case VALUE_JSON: {
                    value_string += current;
                    if(innerJsonCounter == 0)
                        isWordFinished = true;

                    break;
                }
                case VALUE_ARRAY: {
                    value_string += current;
                    if(innerArrayCounter == 0)
                        isWordFinished = true;

                    break;
                }
                case VALUE_OTHER: {
                    if(!isQuotes && utils::CharsInString(current, __SPACES__))
                        isWordFinished = true;
                    if(isQuotes
                        && (innerJsonCounter == 0) && (innerArrayCounter == 0)
                        && string_of_json.length() > i + 1
                        && string_of_json[i + 1] == '"') {
                        isWordFinished = true;
                        i++;
                    }

                    if(current != '"')
                        value_string += current;
                    break;
                }
                default: break;
                }
                //если следующий символ должен обрабатываться другим кодом
                if(!isWordFinished
                    && !isQuotes
                    && (innerJsonCounter == 0) && (innerArrayCounter == 0)) {
                    if((string_of_json.length() > i + 1)
                        && utils::CharsInString(string_of_json[i + 1], __SEPARATORS__ + std::string((value_format != VALUE_JSON) ? "}" : "")))
                        isWordFinished = true;
                }

                if(isWordFinished) {
                    isWordStarted = false; //страховка
                    isWordFinished = false;

                    switch(value_format) {
                    case VALUE_OTHER: {
                        switch(CheckValue(value_string)) {
                        case eNumber:   {
                            double num;
                            try {
                                put(key_string, std::stod(value_string));
                            } catch (...) {
                                isCriticalError = true;
                            }

                            break;
                        }
                        case eBool:     {
                            if(utils::isBool(value_string))
                                put(key_string, utils::toBool(value_string));
                            else
                                isCriticalError = true;

                            break;
                        }
                        case eString:   {
                            put(key_string, value_string);
                            break;
                        }
                        default: //значение не определено
                            isCriticalError = true;
                            break;
                        }

                        break;
                    }
                    case VALUE_JSON: {
                        Json _innerJson;
                        try {
                            _innerJson.parseJSON(value_string, enable_comment);
                            put(key_string, _innerJson);
                        } catch (std::invalid_argument& e) {
                            isCriticalError = true;
                        }

                        break;
                    }
                    case VALUE_ARRAY: {
                        JArray _innerArray;
                        try {
                            _innerArray.parseArray(value_string, enable_comment);
                            put(key_string, _innerArray);
                        } catch (std::invalid_argument& e) {
                            isCriticalError = true;
                        }

                        break;
                    }
                    default:
                        isCriticalError = true;
                        break;
                    }

                    //работа с комментариями (перед значением (НЕ используется)) ==========
                    if(!currentComment.empty() && enable_comment) {
//                        addComment_before(key_string, FromComment(currentComment, m_comment_column_size, m_comment_sym));
                        currentComment = "";
                    } //===================================================================

                    state = JSON_ELEMENT_SEPARATOR;
                    value_format = VALUE_NOPE;
                }

                break;
            }
            case JSON_ELEMENT_SEPARATOR: {
                //пропуск пробелов ====================================================
                if(utils::CharsInString(current, __SPACES_WITHOUT_SEPARATORS__))
                    break;
                //=====================================================================
                if(!utils::CharsInString(current, __SEPARATORS__ "}")) {
                    isCriticalError = true;
                    break;
                }

                if(current == '\n') {
                    //работа с комментариями (после значения #1) ==========================
                    if(!currentComment.empty() && enable_comment) {
                        addComment_after(key_string, FromComment(currentComment, m_comment_column_size, m_comment_sym));
                        currentComment = "";
                    } //===================================================================
                    isValueCommentAfterSaved = true;
                } else if(current == '}') {
                    state = JSON_FINISH;
                    break;
                }
                state = JSON_KEY;

                break;
            }
            default: break;
            }
        }

        if(isCriticalError) {
            clear();
            throw std::invalid_argument("Json parse syntax error at line " + std::to_string(line_counter) + ":" + std::to_string(symbol_counter));
        }

        //счётчик строк и столбцов =============================================
        if(current == '\n') {
            line_counter++;
            symbol_counter = 0; //должен перескочить строго на следующей строке
        } //====================================================================
    }

    if(state != JSON_FINISH) {
        clear();
        throw std::invalid_argument("JSON parse error, end of JSON structure not found");
    }
}

void Json::parseYAML(const std::string &string_of_yaml, const bool enable_comment) {
    //TODO: Json::parseYAML()
}

void Json::parseINI(const std::string &string_of_ini, const bool enable_comment) {
    //TODO: Json::parseINI()
}

bool Json::readFile(const std::string& path, const bool enable_comment,
                    const ConfigFormat config_format) {
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
    std::string config_str;
    while(getline(file, temp_string))
            config_str += temp_string + '\n';
    file.close();

    //обработка
    try{
        switch(config_format) {
        case ConfigFormat::eJSON:   parseJSON(config_str, enable_comment);  break;
        case ConfigFormat::eYAML:   parseYAML(config_str, enable_comment);  break;
        case ConfigFormat::eINI:    parseINI(config_str, enable_comment);   break;
        }
        return true;
    } catch (...) {
        return false;
    }
}

bool Json::writeFile(const std::string& path, int16_t tabulation_level,
                     const bool enable_comment, const ConfigFormat config_format) {
    std::ofstream file(path);
    if (!file.is_open())
        return false;

    switch(config_format) {
    case ConfigFormat::eJSON:
        file << this->to_JSON_string(tabulation_level, enable_comment, m_comment_column_size) << std::endl;
        break;
    case ConfigFormat::eYAML:
        file << this->to_YAML_string(tabulation_level, enable_comment, m_comment_column_size) << std::endl;
        break;
    case ConfigFormat::eINI:
        file << this->to_INI_string(tabulation_level, enable_comment, m_comment_column_size) << std::endl;
        break;
    }

    file.flush();
    file.close();
    return true;
}

std::string Json::to_string(int16_t tabulation_level, const bool enable_comment,
                            const uint8_t column_size, const ConfigFormat config_format) const {
    switch(config_format) {
    case ConfigFormat::eJSON:
        return to_JSON_string(tabulation_level, enable_comment, column_size);
    case ConfigFormat::eYAML:
        return to_YAML_string(tabulation_level, enable_comment, column_size);
    case ConfigFormat::eINI:
        return to_INI_string(tabulation_level, enable_comment, column_size);
    }

    return "";
}

std::string Json::to_JSON_string(int16_t tabulation_level, const bool enable_comment,
                                 const uint8_t column_size) const {
    std::string ret;
    bool withoutSpaces = tabulation_level < 0 && !enable_comment;

    if(enable_comment && !m_preview_comment.before.empty()) {
        ret += "\n";
        if(m_preview_comment.before.find('\n') != -1) {
            ret += utils::RepeatSymToStr('\t', tabulation_level)
                   + utils::RepeatSymToStr('#', m_comment_column_size + 2) + "\n";
            ret += ToComment(m_preview_comment.before, tabulation_level, m_comment_column_size) + "\n";
            ret += utils::RepeatSymToStr('\t', tabulation_level)
                   + utils::RepeatSymToStr('#', m_comment_column_size + 2) + "\n";
        } else
            ret += ToComment(m_preview_comment.before, tabulation_level) + "\n";
    }

    ret += "{"; //start of json

    if(m_values.size() == 1
        && m_values[0].second.first != ValueType::eJson
        && m_values[0].second.first != ValueType::eArray
        ) {
        //===========================================================================
        auto comment_it = m_comments.find(m_values[0].first);
        if(comment_it != m_comments.end()
            && enable_comment
            && !comment_it->second.before.empty()
            ) {
            ret += "\n";
            if(comment_it->second.before.find('\n') != -1) {
                ret += utils::RepeatSymToStr('\t', tabulation_level + 1)
                       + utils::RepeatSymToStr('#', m_comment_column_size + 2) + "\n";
                ret += ToComment(comment_it->second.before, tabulation_level + 1, m_comment_column_size) + "\n";
                ret += utils::RepeatSymToStr('\t', tabulation_level + 1)
                       + utils::RepeatSymToStr('#', m_comment_column_size + 2) + "\n";
            } else
                ret += utils::RepeatSymToStr('\t', tabulation_level + 1)
                       + ToComment(comment_it->second.before, tabulation_level + 1) + "\n";
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
        ret += m_values[0].second.second->to_string(tabulation_level, enable_comment, ConfigFormat::eJSON);
        if(!withoutSpaces) ret += " ";

        //===========================================================================
        if(comment_it != m_comments.end()
            && enable_comment
            && !comment_it->second.after.empty()
            ) {
            ret += ToComment(comment_it->second.after)
                   + "\n" + utils::RepeatSymToStr('\t', tabulation_level);
        }
        //===========================================================================
    } else {
        if(!withoutSpaces) ret += "\n";
        std::string tabs_str = !withoutSpaces ? utils::RepeatSymToStr('\t', ++tabulation_level) : "";

        size_t i = 0;
        for(const JPair& el : m_values) {
            //===========================================================================
            auto comment_it = m_comments.find(el.first);
            if(comment_it != m_comments.end()
                && enable_comment
                && !comment_it->second.before.empty()
                ) {
                ret += "\n";
                if(comment_it->second.before.find('\n') != -1) {
                    ret += utils::RepeatSymToStr('\t', tabulation_level)
                           + utils::RepeatSymToStr('#', m_comment_column_size + 2) + "\n";
                    ret += ToComment(comment_it->second.before, tabulation_level, m_comment_column_size) + "\n";
                    ret += utils::RepeatSymToStr('\t', tabulation_level)
                           + utils::RepeatSymToStr('#', m_comment_column_size + 2) + "\n";
                } else
                    ret += ToComment(comment_it->second.before, tabulation_level, 0) + "\n";
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
            ret += el.second.second->to_string(tabulation_level, enable_comment, ConfigFormat::eJSON);
            if(i < m_values.size() - 1) ret += ",";

            //===========================================================================
            if(comment_it != m_comments.end()
                && enable_comment
                && !comment_it->second.after.empty()
                ) {
                ret += " " + ToComment(comment_it->second.after);
            }
            //===========================================================================

            if(!withoutSpaces) ret += "\n";
            i++;
        }

        if(!withoutSpaces) ret += utils::RepeatSymToStr('\t', --tabulation_level);
    }

    ret += "}"; //end of json

    if(enable_comment && !m_preview_comment.after.empty())
        ret += " " + ToComment(m_preview_comment.after);

    return ret;
}

std::string Json::to_YAML_string(int16_t tabulation_level, const bool enable_comment,
                                   const uint8_t column_size) const {
    //TODO: Json::to_YAML_string
    return "";
}

std::string Json::to_INI_string(int16_t tabulation_level, const bool enable_comment,
                                  const uint8_t column_size) const {
    //TODO: Json::to_INI_string
    return "";
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

Element &Json::operator[](const std::string &key) {
    if(m_values.empty())
        __JSON_EMPTY_EXCEPTION__

    bool element_found = false;
    for(size_t i = 0; i < m_values.size(); i++)
        if(m_values[i].first == key)
            return m_values[i].second;

    __KEY_NOT_FOUND_EXCEPTION__(key)
}

Element &Json::operator[](const std::vector<std::string> &complex_key) {
    if(m_values.empty())
        __JSON_EMPTY_EXCEPTION__

    if(complex_key.size() == 0)
        throw std::invalid_argument("complex_key argument cannot be empty");

    if(contains(complex_key[0])) { //если ключ с таким именем существует
        if(complex_key.size() == 1)
            return (*this)[complex_key[0]];
        else {
            std::string key = complex_key[0];
            Element& el = (*this)[key];

            auto new_complex_key = complex_key;
            new_complex_key.erase(new_complex_key.begin());
            switch(el.first) {
            case eJson:     return (*this)[key].getJson()[new_complex_key];
            case eArray:    return (*this)[key].getArray()[new_complex_key];
            default: __INCORRECT_TYPE_ELEMENT_FOR_INDEX_EXCEPTION__
            }
        }
    }

    size_t index;
    try {
        index = std::stoi(complex_key[0]);
    } catch(...) {
        __JSON_KEY_NOT_FOUND_EXCEPTION__
    }

    if(index < size()) { //если индекс внутри допустимого диапазона
        if(complex_key.size() == 1)
            return (*this)[index];
        else {
            Element& el = (*this)[index];

            auto new_complex_key = complex_key;
            new_complex_key.erase(new_complex_key.begin());
            switch(el.first) {
            case eJson:     return (*this)[index].getJson()[new_complex_key];
            case eArray:    return (*this)[index].getArray()[new_complex_key];
            default: __INCORRECT_TYPE_ELEMENT_FOR_INDEX_EXCEPTION__
            }
        }
    } else
        __JSON_KEY_NOT_FOUND_EXCEPTION__
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
// ======================================================================================== Json
// *
// *
// STATIC FUNCTIONS ============================================================================
CommentType CheckComment(char& first, const char second, size_t& iterator) {
    //сперва искать многострочные комментарии!
    for(uint8_t i = 0; i < utils::cmt::SIZE_comment_multi_line; i++) {
        if(first == utils::cmt::comment_multi_line[i][0] && second == utils::cmt::comment_multi_line[i][1]) {
            //изменение завершающего символа
            if(first == '<') first = '>';
            iterator++; //проскакиваем следующий символ при парсинге
            return CommentType::eMultiLineComment;
        }
    }
    //поиск однострочных комментариев
    for(uint8_t i = 0; i < utils::cmt::SIZE_comment_one_line; i++) {
        if(first == utils::cmt::comment_one_line[i][0]) {
            if((utils::cmt::comment_one_line[i][1] != 0) && (second == utils::cmt::comment_one_line[i][1]))
                iterator++;
            return CommentType::eOneLineComment;
        }
    }

    return CommentType::eNotComment;
}

//только для ЧИСЕЛ, BOOL и СТРОК
ValueType CheckValue(std::string& value) {
//    std::cout << "CheckValue(): \"" << value << "\"" << std::endl;
    bool isValue = false;
    std::string _value;
    ValueType vType = eNull;
    for(size_t i = 0; i < value.length(); i++) {
        if(vType == ValueType::eNull) {
            RemoveIllegalSpaces(value);
            if(utils::isNumber(value[i]))   vType = ValueType::eNumber;
//            else if(value[i] == '{')        vType = ValueType::eJson;
//            else if(value[i] == '[')        vType = ValueType::eArray;
            else if(!utils::CharsInString(value[i], __SPACES__)
                     && (value[0] == 't'
                         || value[0] == 'f'
                         || value[0] == 'T'
                         || value[0] == 'F'))
                                            vType = ValueType::eBool;
            else                            vType = ValueType::eString;
        }
        _value += value[i];
    }

    switch(vType) {
    case ValueType::eNumber:    {
        isValue = CheckDouble(_value);
        if(!isValue) {
            vType = ValueType::eString;
            isValue = CheckString(_value);
        }
        break;
    }
    case ValueType::eBool:      { isValue = CheckBool(_value);      break; }
    case ValueType::eString:    { isValue = CheckString(_value);    break; }
//    case ValueType::eJson:      { isValue = CheckJson(_value);      break; }
//    case ValueType::eArray:     { isValue = CheckArray(_value);     break; }
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

    //удалить пробелы в начале и конце строки
    RemoveIllegalSpaces(value);

    bool done = false;
    if(value[0] == '"') {
        char ch = 0;
        std::string temp;
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
    } else {
        for(char ch : value)
            if(utils::CharsInString(ch, __SPACES__))
                return false;
        return true;
    }
}

//TODO: CheckJson() не проходит тест
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
            //экранированные кавычки ВСЕГДА заносится в значение
            if(value[i] == '\\' && value.length() > i + 1) {
                char e_ch = utils::getEscChar(value[i + 1]);
                if(e_ch != 0) {
                    temp += '\\' + e_ch;
                    i++;
                    break;
                }
            }

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
        } else if(!utils::CharsInString(value[i], __SPACES_WITHOUT_SEPARATORS__)) {
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

//TODO: ToComment(), доп. параметр - comment_symbols
//TODO: исправить ToComment()
std::string ToComment(const std::string &comment_string, const uint8_t tabulation_level,
                      const uint8_t column_size, const char border_symbol) {
    std::string ret;
    std::string current_string = "";
    std::string prefix = utils::RepeatSymToStr('\t', tabulation_level);
    if(border_symbol != 0)
        prefix += border_symbol + std::string(" ");
    char last_symbol = ' ';
    std::vector<size_t> separators;
    separators.reserve(10);
    bool isLastSymbol = false;

    for(size_t i = 0; i < comment_string.length(); i++) {
        char ch = comment_string[i];

        //игнор "двойного" пробела
        if(last_symbol == ' ' && ch == ' ')
            continue;

        if(i == comment_string.length() - 1)
            isLastSymbol = true;

        //если встретили разделитель
        if(utils::CharsInString(ch, __COMMENT_SEPARATOR_SYMBOLS__))
            separators.push_back(current_string.length());

        current_string += ch;
        last_symbol = ch;

//        char number[20];
//        sprintf(number, "%x", current_string.back());
//        std::cout << "current_string(" << current_string.length() << "): <"
//                  << current_string << ">"
//                  << " [" << number << "]"
//                  << std::endl;

        if(ch == '\n') {
            //удалить пробелы в начале и конце строки
            RemoveIllegalSpaces(current_string);

            //вывести если не пустое
            if(!current_string.empty()) {
                ret += prefix + current_string;
                std::cout << "\\n.current_string:" << std::to_string(current_string.length()) << std::endl;
            }

            current_string = "";
            separators.clear();
        }

        if((utils::getStrignSize(current_string) >= column_size)
            && (utils::CharsInString(ch, __COMMENT_SEPARATOR_SYMBOLS__) || isLastSymbol)
            ) {
            std::cout << "PV.current_string_size:" << std::to_string(current_string.length()) << std::endl;
            std::cout << "current_string: <" << current_string << ">" << std::endl;

            //удалить пробелы в начале и конце строки
            RemoveIllegalSpaces(current_string);

            //вывести если не пустое
            if(!current_string.empty()) {
                ret += prefix;

                //если превышен максимальный размер строки
                if(utils::getStrignSize(current_string) > column_size) {
                    std::string left = current_string.substr(0, separators[separators.size() - ((!isLastSymbol) ? 2 : 1)] + 1);
                    RemoveIllegalSpaces(left);
                    current_string = current_string.substr(separators[separators.size() - ((!isLastSymbol) ? 2 : 1)] + 1);
                    std::cout << "left:" << std::to_string(left.length())
                              << ", new current_string:" << std::to_string(current_string.length())
                              << std::endl;

                    if(!utils::CharsInString(current_string.back(), __COMMENT_SEPARATOR_SYMBOLS__))
                        current_string += ' ';
                    ret += left + "\n";

                    //снова найти индексы разделителей
                    separators.clear();
                    for(size_t j = 0; j < current_string.length(); j++) {
                        if(utils::CharsInString(current_string[j], __COMMENT_SEPARATOR_SYMBOLS__))
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

    if(!current_string.empty()) {
        ret += prefix + current_string;
        std::cout << "(last) current_string_size:" << std::to_string(current_string.length())
                  << std::endl;
    }

    ret = "/*" + utils::RepeatSymToStr(border_symbol, column_size) + "\n" + ret;
    ret += "\n" + utils::RepeatSymToStr(border_symbol, column_size) + "*/";


    return ret;
}

//NOTE: если символ в списке и в первой строке комментария повторяется минимум 5 раз - это граница, иначе - часть комментария
//NOTE: для всего файла конфига подменяется символ границы только если не задан (первый комментарий с границей)
std::string FromComment(const std::string &comment_string, uint8_t &column_size, char &border_symbol) {
    std::string ret;

    bool isBorderExists = utils::CharsInString(comment_string[0], __BORDER_SYMBOLS__); //от 5 до 0xFF символов
    if(border_symbol == 0 && isBorderExists) border_symbol = comment_string[0];
    bool isFirstBorderLine = isBorderExists;
    uint8_t border_size = 0;

    std::string current_string;
    bool isBorderLine = isBorderExists;
    for(char ch : comment_string) {
        if(ch == '\n') {
            if(isFirstBorderLine) isFirstBorderLine = false;
            if(isBorderLine && border_size < 5) {
                isBorderExists = false;
                isBorderLine = false;
            }
            if(isBorderLine) {
                current_string = "";
                continue;
            }

            RemoveIllegalSpaces(current_string);
            if(isBorderExists) {
                if(((comment_string[0] == '-' || comment_string[0] == '=') && current_string[0] == '|')
                    || (current_string[0] == comment_string[0]))
                    current_string.erase(current_string.begin());
            }
            RemoveIllegalSpaces(current_string);

            if(!current_string.empty()) {
                if(!ret.empty())
                    ret += "\n";
                ret += current_string;
            }
            current_string = "";
            isBorderLine = isBorderExists;
        } else {
            //сработает только для первой строки
            if(isFirstBorderLine) {
                if(ch == comment_string[0])
                    border_size++;
                else { //встречен лишний символ, строка - часть комментария
                    isBorderExists = false;
                    border_size = 0;
                }
            }
            //является ли строка границей
            if(isBorderLine && ch != comment_string[0])
                isBorderLine = false;
            current_string += ch;
        }
    }
    if(!isBorderLine && !current_string.empty())
        ret += current_string;

    if(isBorderExists && column_size == 0 && border_size != 0)
        column_size = border_size;

    return ret;
}


// ============================================================================ STATIC FUNCTIONS
