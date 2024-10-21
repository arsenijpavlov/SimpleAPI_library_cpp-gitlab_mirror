#include "Json.h"

#include <fstream>
#include <iostream>
#include <regex>
#include <set>

//TODO <skip>: #define DEBUG_OUTPUT когда-нибудь потом...


// Element =====================================================================================
std::string to_string(const ValueType type) noexcept {
    switch(type) {
    case eNumber:   return "Number";    break;
    case eBool:     return "Bool";      break;
    case eString:   return "String";    break;
    case eJson:     return "Json";      break;
    case eArray:    return "Array";     break;
    default:        return "null";      break;
    }
}

Element::Element() noexcept : first(ValueType::eNull) {
    second = reinterpret_cast<BaseElement*>(new NullElement());
}

Element::Element(std::nullptr_t) noexcept : first(ValueType::eNull) {
    second = reinterpret_cast<BaseElement*>(new NullElement());
}

Element::Element(const Json& value) noexcept : first(ValueType::eJson) {
    second = reinterpret_cast<BaseElement*>(new JsonElement(value));
}

Element::Element(const JArray& value) noexcept : first(ValueType::eArray) {
    second = reinterpret_cast<BaseElement*>(new JArrayElement(value));
}

Element::Element(const Element &other) noexcept {
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
        second = reinterpret_cast<BaseElement*>(new NullElement());
        break;
    }
}

bool Element::operator==(const Element &other) const noexcept {
    if(first != other.first) return false;

    switch(first) {
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

Element &Element::operator=(const Element &other) noexcept {
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
        second = reinterpret_cast<BaseElement*>(new NullElement());
        break;
    }

    return *this;
}

Element &Element::operator[](const std::string &key) {
    if(first != ValueType::eJson)
        __NOT_JSON_ELEMENT_EXCEPTION__
            return getJson()[key];
}

Element &Element::operator[](const size_t index) {
    if(first != ValueType::eJson && first != ValueType::eArray)
        __NOT_ARRAY_OR_JSON_ELEMENT_EXCEPTION__
    if(first == ValueType::eJson)
        return getJson()[index];
    else
        return getArray()[index];
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

////TODO: Element::readFile()
//bool Element::readFile(const std::string& path, const bool enable_comment,
//                           const ConfigFormat config_format) const noexcept {
//    std::string config_str;

//    try{
//        std::ifstream file(path);
//        if (!file.is_open()) {
//            std::cout << "File not found" << std::endl;
//            return false;
//        }

//        std::string temp_string;
//        while(getline(file, temp_string))
//            config_str += temp_string + '\n';
//        file.close();
//    } catch (...) {
//        std::cout << "exception for read file !!!" << std::endl;
//        return false;
//    }

//    //обработка
//    try{
//        switch(config_format) {
//        case ConfigFormat::eJSON:   parseJSON(config_str, enable_comment);  break;
//        case ConfigFormat::eYAML:   parseYAML(config_str, enable_comment);  break;
//        case ConfigFormat::eINI:    parseINI(config_str, enable_comment);   break;
//        }
//        return true;
//    } catch (...) {
//        return false;
//    }
//}

////TODO: Element::writeFile()
//Element& Element::writeFile(const std::string& path, const bool enable_comment,
//                            const ConfigFormat config_format) const noexcept {

//}

////TODO: Element& Parse()
//Element& Parse(const std::string& element_string, const ConfigFormat format,
//               const bool enable_comment) noexcept {

//}

////TODO: Element& ReadFile()
//Element& ReadFile(const std::string& path, const bool enable_comment,
//                  const ConfigFormat format) noexcept {

//}

////TODO: Element& ReadFileJSON()
//Element& ReadFileJSON(const std::string& path, const bool enable_comment) noexcept {

//}

////TODO: Element& ReadFileYAML()
//Element& ReadFileYAML(const std::string& path, const bool enable_comment) noexcept {

//}

////TODO: Element& ReadFileINI()
//Element& ReadFileINI(const std::string& path, const bool enable_comment) noexcept {

//}

////TODO: void WriteFile()
//void WriteFile(const Element& element, const ConfigFormat format,
//               const bool enable_comment) noexcept {

//}

////TODO: void WriteFileJSON()
//void WriteFileJSON(const Element& element) noexcept {

//}

////TODO: void WriteFileYAML()
//void WriteFileYAML(const Element& element) noexcept {

//}

////TODO: void WriteFileINI()
//void WriteFileINI(const Element& element) noexcept {

//}
// ===================================================================================== Element


// JArray ======================================================================================
JArray::JArray(const JArray& other) noexcept {
    for(AVector::const_iterator it = other.m_values.cbegin();
         it != other.m_values.cend(); it++) {
        m_values.push_back(Element(*it));
    }

    //save comment's logic
    m_comment_column_size   = other.m_comment_column_size;
    m_comment_sym           = other.m_comment_sym;
    m_preview_comment       = other.m_preview_comment;
    m_comments              = other.m_comments;
}

void JArray::parseArray(const std::string &string_of_array, const bool enable_comment,
                        const ConfigFormat config_format) {
    switch(config_format) {
    case ConfigFormat::eJSON:
        parseJSON_array(string_of_array, enable_comment); //функция может вернуть exception!
        break;
    case ConfigFormat::eYAML:
        parseYAML_array(string_of_array, enable_comment); //функция может вернуть exception!
        break;
    case ConfigFormat::eINI:
        parseINI_array(string_of_array, enable_comment); //функция может вернуть exception!
        break;
    }
}

void JArray::parseJSON_array(const std::string &string_of_array, const bool enable_comment) {
    clear();

    if(string_of_array.empty()) return;

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
    Comment valueComment;
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
        char previous = (i - 1 >= 0) ? string_of_array[i - 1] : 0;
        char current = string_of_array[i];
        char next = (i + 1 < string_of_array.length()) ? string_of_array[i + 1] : 0;

        //поиск комментариев
        const bool ext_f = !isQuotes && value_format != VALUE_ARRAY && value_format != VALUE_JSON;
        utils::CommentChecker c_checker = c_checker = utils::CheckComments(current, next,
                                                                           isOneLineComment, isMultiLineComment,
                                                                           firstMLCSym, secondMLCSym,
                                                                           enable_comment, currentComment,
                                                                           i, ext_f);
        if(c_checker != utils::CommentChecker::isNotComment) {
            //сюда зайдёт, если внутри комментария
            //счётчик строк и столбцов =============================================
            if(current == '\n') {
                line_counter++;
                symbol_counter = 0; //должен перескочить строго на следующей строке
            } else symbol_counter++;
            //======================================================================
            continue;
        }

        //работа с синтаксисом JSON_ARRAY
        switch(state) {
        case ARRAY_START: {
            //пропуск пробелов ====================================================
            if(utils::CharsInString(current, __SPACES__))
                break;
            //=====================================================================

            //работа с комментариями (первичный) ==================================
            if(!currentComment.empty() && enable_comment) {
                addPreviewComment(FromComment(currentComment, m_comment_column_size, m_comment_sym));
//                std::cout << "JArray:PreviewComment: " << "\"" << currentComment << "\"" << std::endl;
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
//                    std::cout << "JArray:comment:after: " << "\"" << currentComment << "\"" << std::endl;
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
                if(previous != '\\') {
                    isQuotes = !isQuotes;
//                    std::cout << "value."
//                              << "isQuotes: (" << utils::to_string(isQuotes) << ") "
//                              << "'" << previous << current << next << "'"
//                              << std::endl;
                }
            }
            default: {
                if(value_format == ValueFormat::VALUE_NOPE) value_format = ValueFormat::VALUE_OTHER;
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

                value_string += current;
                break;
            }
            default: break;
            }

            if(!isWordFinished && !isQuotes
                && (innerJsonCounter == 0) && (innerArrayCounter == 0)) {
                //если текущий символ должен обрабатываться другим кодом
                if(utils::CharsInString(current, __SEPARATORS__ + std::string((value_format != VALUE_ARRAY) ? "]" : ""))) {
                    isWordFinished = true;
                    i--;
                    value_string.pop_back();
                }
                //если следующий символ должен обрабатываться другим кодом
                if(utils::CharsInString(next, __SEPARATORS__ + std::string((value_format != VALUE_ARRAY) ? "]" : ""))) {
                    isWordFinished = true;
                }
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
                    case eNull:     {
                        push_back(Element());

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
                    valueComment.before = FromComment(currentComment, m_comment_column_size, m_comment_sym);
//                    std::cout << "JArray:comment:before: " << "\"" << currentComment << "\"" << std::endl;
                    currentComment = "";
                } //===================================================================

                state = ARRAY_ELEMENT_SEPARATOR;
                value_format = ValueFormat::VALUE_NOPE;
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
                if(enable_comment) {
                    if(!currentComment.empty()) {
                        valueComment.after = FromComment(currentComment, m_comment_column_size, m_comment_sym);
//                        std::cout << "JArray:comment:after: " << "\"" << currentComment << "\"" << std::endl;
                        currentComment = "";
                    }
//                    addComment(m_values.size() - 1, valueComment);
                } //===================================================================
                isValueCommentAfterSaved = true;
            }
            if(current == ']') {
                state = ARRAY_FINISH;
            } else
                state = ARRAY_VALUE;

            if(enable_comment && (!valueComment.before.empty() || !valueComment.after.empty())) {
//                std::cout << "\tvalue_before: " << valueComment.before << std::endl
//                          << "\tvalue_after: " << valueComment.after << std::endl;
                addComment(m_values.size() - 1, valueComment.before, valueComment.after);
            }

            break;
        }
        default: break;
        }

        if(isCriticalError) {
            std::string state_str = "";
            switch(state) {
            case ARRAY_START:               state_str = "JSON_START ";             break;
            case ARRAY_VALUE:               state_str = "JSON_VALUE ";             break;
            case ARRAY_ELEMENT_SEPARATOR:   state_str = "JSON_ELEMENT_SEPARATOR "; break;
            case ARRAY_FINISH:              state_str = "JSON_FINISH ";            break;
            }
//            std::cout << "symbols: '" << previous << current << next << "'" << std::endl;
            //--------------------------
            clear();
            throw std::invalid_argument("JSON_ARRAY parse error at line "
                                        + std::to_string(line_counter) + ":" + std::to_string(symbol_counter)
                                        + " '" + current + "', current state:" + state_str);
        }

        //счётчик строк и столбцов =============================================
        if(current == '\n') {
            line_counter++;
            symbol_counter = 0; //должен перескочить строго на следующей строке
        } else symbol_counter++;
        //======================================================================
    }


    if(state != ARRAY_FINISH) {
        clear();
        throw std::invalid_argument("JSON_ARRAY parse error, end of JSON array structure not found");
    }
}

void JArray::parseYAML_array(const std::string &string_of_array, const bool enable_comment) {
    //TODO: JArray::parseYAML_array()
    //TODO: JArray::parseYAML_array(), return std::exception
}

void JArray::parseINI_array(const std::string &string_of_array, const bool enable_comment) {
    //TODO: JArray::parseINI_array()
    //TODO: JArray::parseINI_array(), return std::exception
}

JArray &JArray::append(const JArray &array) noexcept {
    for(const Element& el : array.m_values) {
        switch(el.first) {
        case eNumber:   push_back(el.getNum());     break;
        case eBool:     push_back(el.getBool());    break;
        case eString:   push_back(el.getString());  break;
        case eJson:     push_back(el.getJson());    break;
        case eArray:    push_back(el.getArray());   break;
        case eNull:     push_back(Element());       break;
        }
    }

    return *this;
}

ValueType JArray::getType(const size_t index) {
    __CHECK_INDEX_BOUND_EXCEPTION__(this, index)
    return m_values[index].first;
}

ValueType JArray::getTypeFront() {
    __NO_ELEMENTS_EXCEPTION__
    return getType(0);
}

ValueType JArray::getTypeBack() {
    __NO_ELEMENTS_EXCEPTION__
    return getType(m_values.size() - 1);
}

Element& JArray::getFront() {
    __NO_ELEMENTS_EXCEPTION__
    return m_values.front();
}

Element& JArray::getBack() {
    __NO_ELEMENTS_EXCEPTION__
    return m_values.back();
}

JArray &JArray::popFront() {
    __NO_ELEMENTS_EXCEPTION__
    m_values.erase(m_values.begin());
    return *this;
}

JArray &JArray::popAt(const size_t index) {
    __CHECK_INDEX_BOUND_EXCEPTION__(this, index)
    m_values.erase(m_values.begin() + index);
    return *this;
}

JArray &JArray::popBack() {
    __NO_ELEMENTS_EXCEPTION__
    m_values.pop_back();
    return *this;
}

JArray &JArray::clear() {
    m_values.clear();
    m_comments.clear();
    return *this;
}

std::string JArray::to_string(int16_t tabulation_level, const bool enable_comment, const
                              uint8_t column_size, const ConfigFormat config_format) const noexcept {
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
                                   const uint8_t column_size) const noexcept {
    if(m_values.empty()) return "[]";

    std::string ret;
    bool withoutSpaces = tabulation_level < 0 && !enable_comment;

    if(enable_comment && !m_preview_comment.before.empty()) {
        ret += "\n";
        if(m_preview_comment.before.find('\n') != -1) {
            ret += ToComment(m_preview_comment.before, tabulation_level, m_comment_column_size) + "\n";
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
                ret += ToComment(comment_it->second.before, tabulation_level + 1, m_comment_column_size) + "\n";
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
                    ret += ToComment(comment_it->second.before, tabulation_level, m_comment_column_size) + "\n";
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
                if(comment_it->second.after.find('\n') == -1)
                    ret += " " + ToComment(comment_it->second.after);
                else {
                    std::string toComment = ToComment(comment_it->second.after, tabulation_level, m_comment_column_size, m_comment_sym);
                    //NOTE: многострочные комментарии ПОСЛЕ значения должны начинаться на той же строке, что и значение
                    while(toComment.size() > 1 && toComment[0] == '\t')
                        toComment.erase(toComment.cbegin(), toComment.cbegin() + 1);
//                    std::cout << "toComment: \"" << toComment << "\"" << std::endl;
                    ret += " " + toComment;
                }
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
                                   const uint8_t column_size) const noexcept {
    if(m_values.empty()) return "";

    //TODO: JArray::to_YAML_string()
    return "";
}

std::string JArray::to_INI_string(int16_t tabulation_level, const bool enable_comment,
                                  const uint8_t column_size, const std::string& preview_title) const noexcept {
    if(m_values.empty()) return "";

    //TODO: JArray::to_INI_string()
    std::string ret;



    return ret;
}

bool JArray::operator==(const JArray &other) const noexcept {
    if(size() != other.size()) return false;

    for(auto it1 = m_values.begin(), it2 = other.m_values.begin();
         it1 != m_values.end() && it2 != other.m_values.end();
         it1++, it2++) {
        if(*it1 != *it2) return false;
    }

    return true;
}

Element &JArray::getValue(const size_t index) {
    __CHECK_INDEX_BOUND_EXCEPTION__(this, index)
    return (*this)[index];
}

Element &JArray::getValue(std::vector<std::string> &complex_key) {
    try {
        return (*this)[complex_key];
    } catch(std::exception e){ throw e; }
}

Element &JArray::operator[](const size_t index) {
    if(m_values.empty())
        __ARRAY_EMPTY_EXCEPTION__
            __CHECK_INDEX_BOUND_EXCEPTION__(this, index)

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
    __CHECK_INDEX_BOUND_EXCEPTION__(this, index)
    if(index <= m_values.size() - 1)
        m_values.erase(m_values.cbegin() + index);

    return *this;
}
// ====================================================================================== JArray
//TODO: Json json; json["not_found_key"] = new_value;

// Json ========================================================================================
Json::Json(const Json& other) noexcept : m_comment_sym(0) {
    for(const JPair &el : other.m_values)
        m_values.push_back(std::make_pair(el.first, Element(el.second)));

    //save comment's logic
    m_comment_column_size   = other.m_comment_column_size;
    m_comment_sym           = other.m_comment_sym;
    m_preview_comment       = other.m_preview_comment;
    m_comments              = other.m_comments;
}

Json::Json(const std::string &input_string, const ConfigFormat config_format,
           const bool enable_comment) : m_comment_sym(0) {
    parse(input_string, config_format, enable_comment);
}

Json::Json(const JVector &vec) noexcept : m_comment_sym(0) {
    for(JVector::const_iterator j_it = vec.begin(); j_it != vec.end(); j_it++)
        put(j_it->first, j_it->second);
}

Json &Json::operator=(const Json &other) noexcept {
    clear();
    m_comment_sym = other.m_comment_sym;
    for(const JPair &el : other.m_values)
        m_values.push_back(std::make_pair(el.first, Element(el.second)));

    return *this;
}

Json &Json::put(const std::string &key, const Element &element, const bool rewrite) noexcept {
    if(!contains(key))
        m_values.push_back(JPair(key, element));
    else if(rewrite)
        updateValue(key, element);
    return *this;
}

Json &Json::put(const Json &json, const bool rewrite) noexcept {
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

void Json::parse(const std::string &input_string, const ConfigFormat config_format,
                 const bool enable_comment) {
    switch (config_format) {
    case ConfigFormat::eJSON:
        parseJSON(input_string, enable_comment); //функция может вернуть exception!
        break;
    case ConfigFormat::eYAML:
        parseYAML(input_string, enable_comment); //функция может вернуть exception!
        break;
    case ConfigFormat::eINI:
        parseINI(input_string, enable_comment); //функция может вернуть exception!
        break;
    }
}

void Json::parseJSON(const std::string &string_of_json, const bool enable_comment) {
    clear();

    if(string_of_json.empty()) return;

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
    Comment keyComment;

    uint16_t innerJsonCounter   = 0;
    uint16_t innerArrayCounter  = 0;
    std::string value_string;
    value_string.reserve(100);
    Comment valueComment;
    bool isValueCommentAfterSaved = true; //изначально TRUE, чтобы не сработало для первого прохода
    Element value_element;

    uint16_t line_counter   = 0; //NOTE: (Json) ограничение на FFFF строк
    uint16_t symbol_counter = 0; //NOTE: (Json) ограничение на FFFF символов в строке

    bool isSimpleElement = false; //JSON может состоять из любого объекта

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
        char previous = (i - 1 >= 0) ? string_of_json[i - 1] : 0;
        char current = string_of_json[i];
        char next = (i + 1 < string_of_json.length()) ? string_of_json[i + 1] : 0;

        //поиск комментариев
        const bool ext_f = !isQuotes && value_format != VALUE_ARRAY && value_format != VALUE_JSON;
        utils::CommentChecker c_checker = c_checker = utils::CheckComments(current, next,
                                                                           isOneLineComment, isMultiLineComment,
                                                                           firstMLCSym, secondMLCSym,
                                                                           enable_comment, currentComment,
                                                                           i, ext_f);
        if(c_checker != utils::CommentChecker::isNotComment) {
            //сюда зайдёт, если внутри комментария
            //счётчик строк и столбцов =============================================
            if(current == '\n') {
                line_counter++;
                symbol_counter = 0; //должен перескочить строго на следующей строке
            } else symbol_counter++;
            //======================================================================
            continue;
        }

        //работа с синтаксисом JSON
        switch(state) {
        case JSON_START: {
            //пропуск пробелов ====================================================
            if(utils::CharsInString(current, __SPACES__))
                break;
            //=====================================================================

            //работа с комментариями (первичный) ==================================
            if(!currentComment.empty() && enable_comment) {
                addPreviewComment(FromComment(currentComment, m_comment_column_size, m_comment_sym));
//                std::cout << "Json:PreviewComment: " << "\"" << currentComment << "\"" << std::endl;
                currentComment = "";
            } //===================================================================

            if(current != '{') { //JSON может содержать 1 элемент без скобок
//                isCriticalError = true;
                isSimpleElement = true;
                state = JSON_KEY;
                i--;
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
//                    std::cout << "Json:comment:after: " << "\"" << currentComment << "\"" << std::endl;
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
                keyComment = Comment();
                valueComment = Comment();
                isQuotes = false;
            }

            //экранированные кавычки ВСЕГДА заносятся в значение
            if(current == '\\' && string_of_json.length() > i + 1) {
                char e_ch = utils::getEscChar(string_of_json[i + 1]);
                if(e_ch != 0) {
                    value_string += '\\' + e_ch;
                    i++;
                    break;
                }
            }

            if(current == '"')  isQuotes = !isQuotes;
            else                key_string += current;

            //поиск конца значения
            if(!isQuotes && utils::CharsInString(current, __SPACES__))
                isWordFinished = true;
            if(isQuotes
                && string_of_json.length() > i + 1
                && string_of_json[i + 1] == '"') {
                //сброс флага для корректной обработки
                isQuotes = !isQuotes;
//                std::cout << "key."
//                          << "isQuotes: (" << utils::to_string(isQuotes) << ") "
//                          << "'" << previous << current << next << "'"
//                          << std::endl;

                isWordFinished = true;
                i++;
            }


            //если следующий символ должен обрабатываться другим кодом
            if(!isWordFinished && !isQuotes) {
                if((string_of_json.length() > i + 1)
                    && utils::CharsInString(string_of_json[i + 1], __KEY_VALUE_SEPARATOR__))
                    isWordFinished = true;
            }


            if(isWordFinished) {
                isWordStarted = false; //страховка
                isWordFinished = false;
//                std::cout << "key: " << key_string << std::endl;

                //работа с комментариями (перед ключом) ===============================
                if(!currentComment.empty() && enable_comment) {
                    keyComment.before = FromComment(currentComment, m_comment_column_size, m_comment_sym);
//                    std::cout << "Json:comment:before: " << "\"" << currentComment << "\"" << std::endl;
                    currentComment = "";
                } //===================================================================

                state = JSON_KEY_VALUE_SEPARATOR;
            }

            break;
        }
        case JSON_KEY_VALUE_SEPARATOR: {
//            std::cout << "separator."
//                      << "isQuotes: (" << utils::to_string(isQuotes) << ") "
//                      << "'" << previous << current << next << "'"
//                      << std::endl;
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
//                keyComment.after = FromComment(currentComment, m_comment_column_size, m_comment_sym);
                currentComment = "";
            } //===================================================================

            state = JSON_VALUE;

            break;
        }
        case JSON_VALUE: {
            //пропуск пробелов ====================================================
            if(utils::CharsInString(current, __SPACES__) && !isQuotes && value_format == ValueFormat::VALUE_NOPE)
                if(next != 0) break;
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
                if(previous != '\\') {
                    isQuotes = !isQuotes;
//                    std::cout << "value."
//                              << "isQuotes: (" << utils::to_string(isQuotes) << ") "
//                              << "'" << previous << current << next << "'"
//                              << std::endl;
                }
            }
            default: {
                if(value_format == ValueFormat::VALUE_NOPE) {
                    value_format = ValueFormat::VALUE_OTHER;
                }
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

                value_string += current;
                break;
            }
            default: break;
            }
            if(!isWordFinished && !isQuotes
                && (innerJsonCounter == 0) && (innerArrayCounter == 0)) {
                //если текущий символ должен обрабатываться другим кодом
                if(utils::CharsInString(current, __SEPARATORS__ + std::string((value_format != VALUE_JSON) ? "}" : ""))) {
                    isWordFinished = true;
                    i--;
                    value_string.pop_back();
                }
                //если следующий символ должен обрабатываться другим кодом
                if(utils::CharsInString(next, __SEPARATORS__ + std::string((value_format != VALUE_JSON) ? "}" : ""))
                    || next == 0 /*конец строки*/) {
                    isWordFinished = true;
                }
            }

            if(isWordFinished) {
                isWordStarted = false; //страховка
                isWordFinished = false;

//                std::cout << "value: " << value_string << std::endl;

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
                    case eNull:     {
                        put(key_string, Element());

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
//                    valueComment.before = FromComment(currentComment, m_comment_column_size, m_comment_sym);
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
                if(enable_comment) {
                    if(!currentComment.empty()) {
                        valueComment.after = FromComment(currentComment, m_comment_column_size, m_comment_sym);
//                        std::cout << "Json:comment:after: " << "\"" << currentComment << "\"" << std::endl;
                        currentComment = "";
                    }
                } //===================================================================
                isValueCommentAfterSaved = true;
            }
            if(current == '}') {
                state = JSON_FINISH;
//                break;
            } else
                state = JSON_KEY;

            if(enable_comment && (!keyComment.before.empty() || !valueComment.after.empty())) {
//                std::cout << "\tkey_before: " << keyComment.before << std::endl
//                          << "\tkey_after: " << keyComment.after << std::endl;
//                std::cout << "\tvalue_before: " << valueComment.before << std::endl
//                          << "\tvalue_after: " << valueComment.after << std::endl;
                addComment(key_string, keyComment.before, valueComment.after);
            }

            break;
        }
        default: break;
        }

        if(isCriticalError) {
            std::string state_str = "";
            switch(state) {
            case JSON_START:                state_str = "JSON_START ";                 break;
            case JSON_KEY:                  state_str = "JSON_KEY ";                   break;
            case JSON_KEY_VALUE_SEPARATOR:  state_str = "JSON_KEY_VALUE_SEPARATOR ";   break;
            case JSON_VALUE:                state_str = "JSON_VALUE ";                 break;
            case JSON_ELEMENT_SEPARATOR:    state_str = "JSON_ELEMENT_SEPARATOR ";     break;
            case JSON_FINISH:               state_str = "JSON_FINISH ";                break;
            }
//            std::cout << "symbols: '" << previous << current << next << "'" << std::endl;
            //--------------------------
            clear();
            throw std::invalid_argument("JSON parse syntax error at line "
                                        + std::to_string(line_counter) + ":" + std::to_string(symbol_counter)
                                        + " '" + current + "', current state:" + state_str);
        }

        //счётчик строк и столбцов =============================================
        if(current == '\n') {
            line_counter++;
            symbol_counter = 0; //должен перескочить строго на следующей строке
        } else symbol_counter++;
        //======================================================================
    }

//    std::cout << "finish, last state: ";
//    switch(state) {
//    case JSON_START:                std::cout << "JSON_START ";                 break;
//    case JSON_KEY:                  std::cout << "JSON_KEY ";                   break;
//    case JSON_KEY_VALUE_SEPARATOR:  std::cout << "JSON_KEY_VALUE_SEPARATOR ";   break;
//    case JSON_VALUE:                std::cout << "JSON_VALUE ";                 break;
//    case JSON_ELEMENT_SEPARATOR:    std::cout << "JSON_ELEMENT_SEPARATOR ";     break;
//    case JSON_FINISH:               std::cout << "JSON_FINISH ";                break;
//    }
//    std::cout << std::endl;

    if(state != JSON_FINISH && !isSimpleElement) {
        clear();
        throw std::invalid_argument("JSON parse error, end of JSON structure not found");
    }
}

void Json::parseYAML(const std::string &string_of_yaml, const bool enable_comment) {
    //TODO: Json::parseYAML()
    //TODO: Json::parseYAML(), std::exception
}

std::vector<std::string> parseIniKeys(std::string& ini_key_value) noexcept {
    std::vector<std::string> ret;
    ret.reserve(3);
    bool flag = true;

    std::string local_string;
    local_string.reserve(200);
    for(char c : ini_key_value) {
        if(utils::CharsInString(c, __KEY_VALUE_SEPARATOR__) && !local_string.empty() && flag) {
            RemoveIllegalSpaces(local_string);

            std::regex reg(__POSSIBLE_KEY_SYMBOLS__);
            if(std::regex_match(local_string, reg)) {
                ret.push_back(local_string);
                local_string = "";
                continue;
            } else
                flag = false;
        }
        local_string += c;
    }
    RemoveIllegalSpaces(local_string);
    ini_key_value = local_string;

    return ret;
}

std::vector<std::string> parseIniCustomKeys(std::string& preview_key) noexcept {
    std::vector<std::string> ret;
    ret.reserve(2);

    std::string local_string;
    local_string.reserve(200);
    for(char c : preview_key) {
        if(utils::CharsInString(c, "/\\")) {
            RemoveIllegalSpaces(local_string);
            ret.push_back(local_string);
            local_string = "";
            continue;
        }
        local_string += c;
    }
    if(!local_string.empty()) {
        RemoveIllegalSpaces(local_string);
        ret.push_back(local_string);
    }

    return ret;
}

Json *GetObjectForIniCustomKey(Json* json, std::vector<std::string> &keys) noexcept {
    if(json == nullptr || keys.size() == 0) return json;

    if(keys.size() == 1) {
        if(json->contains(keys[0])) {
            switch((*json)[keys[0]].first) {
            case eJson: return &(*json)[keys[0]].getJson();
            case eNull:
            default: {
                Element temp_e = (*json)[keys[0]];
                JArray temp_ja(temp_e);
                json->updateValue(keys[0], temp_ja);
                break;
            }
            case eArray: break;
            }
        } else {
            json->put(keys[0], nullptr);
        }

        return json;
    } else {
        std::string current_key = keys[0];

        keys.erase(keys.cbegin(), keys.cbegin() + 1);
        Json* next_json = json;
        if(json->contains(current_key)) {
            switch((*json)[current_key].first) {
            case eJson: {
                next_json = &(*json)[current_key].getJson();
                break;
            }
            case eNull: {
                json->updateValue(current_key, Json());
                next_json = &(*json)[current_key].getJson();
                break;
            }
            default: {
                Element temp_e = (*json)[current_key];
                JArray temp_ja(temp_e);
                //создаём поле для следующего ключа в списке
                Json temp_j = Json(std::make_pair(keys[0], nullptr));
                temp_ja.push_back(temp_j);
                json->updateValue(current_key, temp_ja);

                next_json = &(*json)[current_key].getArray().getBack().getJson();
                break;
            }
            case eArray: { //TODO: GetObjectForIniCustomKey(), нужен тест
                JArray& temp_ja = (*json)[current_key].getArray();
                //создаём поле для следующего ключа в списке
                temp_ja.push_back(Json(std::make_pair(keys[0], nullptr)));
                next_json = &(*json)[current_key].getArray().getBack().getJson();
                break;
            }
            }
        } else {
            json->put(current_key, Json());
            next_json = &(*json)[current_key].getJson();
        }
        return GetObjectForIniCustomKey(next_json, keys);
    }
}

Element ParseValueFromString(std::string& value, const bool enable_comments, const ConfigFormat format) {
    std::string temp;
    temp.reserve(value.size());
    for(char c : value) temp += c;

    //NULL
    if(value.empty()) return Element();
    if(value.size() == 4 && temp[0] == 'n')
        if(temp == "null") return Element();

    //BOOL
    if(value.length() == 4 || value.length() == 5)
        if(temp[0] == 't' || temp[0] == 'f') {
            if(temp == "true")          return Element(true);
            else if(temp == "false")    return Element(false);
        }

    //NUMBER
    {
        std::regex reg("^[+-]?[0-9]*[.]?[0-9]*[eE]?[+-]?[0-9]*[fF]?$");
        bool matched = std::regex_match(value, reg);
        bool e_is_last = value[value.length() - 1] == 'e' || value[value.length() - 1] == 'E';
        bool f_is_last = value[value.length() - 1] == 'f' || value[value.length() - 1] == 'F';

        if(f_is_last && value.length() > 1)
            e_is_last = value[value.length() - 2] == 'e' || value[value.length() - 2] == 'E';

        if(matched && !e_is_last) {
            try {
                double result_number = std::stod(value);
                return Element(result_number);
            } catch (...) {}
        }
    }

    //JSON
    if(value[0] == '{') {
        Json json;
        try {
            json.parseJSON(value, enable_comments);
            return Element(json);
        } catch (std::exception e) {
            if(format == ConfigFormat::eJSON) throw e;
        }
    }

    //JSON_ARRAY
    if(value[0] == '[') {
        JArray jarray;
        try {
            jarray.parseJSON_array(value, enable_comments);
            return Element(jarray);
        } catch (std::exception e) {
            if(format == ConfigFormat::eJSON) throw e;
        }
    }

    //STRING - всё остальное
    //кавычки по краям строкового значения актуальны только для Json, NOTE: YAML ещё не изучал
    if(value.length() > 2 && value[0] == '\"' && value.back() == '\"') {
        value.erase(value.cbegin(), value.cbegin() + 1);
        value.pop_back();
    }
    return Element(value);
}

void Json::parseINI(const std::string &string_of_ini, const bool enable_comment) {
    clear();

    if(string_of_ini.empty()) return;

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

    std::string key_value_string;
    key_value_string.reserve(100);
    Comment keyValueComment;

    std::string group_string;
    group_string.reserve(20);
    Comment groupComment;

    bool isValueCommentAfterSaved = true; //изначально TRUE, чтобы не сработало для первого прохода
    Element value_element;

    uint16_t line_counter   = 0; //NOTE: (Ini) ограничение на FFFF строк
    uint16_t symbol_counter = 0; //NOTE: (Ini) ограничение на FFFF символов в строке

    bool isBeforeStringIsEmpty = false; //для работы с комментариями внутри групп

    for(size_t i = 0; (i < string_of_ini.length()); i++) {
        char previous = (i - 1 >= 0) ? string_of_ini[i - 1] : 0;
        char current = string_of_ini[i];
        char next = (i + 1 < string_of_ini.length()) ? string_of_ini[i + 1] : 0;

        //поиск комментариев
        const bool ext_f = !isQuotes;
        utils::CommentChecker c_checker = utils::CheckComments(current, next,
                                                               isOneLineComment, isMultiLineComment,
                                                               firstMLCSym, secondMLCSym,
                                                               enable_comment, currentComment,
                                                               i, ext_f);
        if(c_checker != utils::CommentChecker::isNotComment) { //сюда зайдёт, если внутри комментария
            //счётчик строк и столбцов =============================================
            if(current == '\n') {
                line_counter++;
                symbol_counter = 0; //должен перескочить строго на следующей строке
            } else symbol_counter++;
            //======================================================================
            continue;
        }

        //работа с синтаксисом INI
        if(!isOneLineComment && !isMultiLineComment) {
            //пропуск пробелов ====================================================
            if(current == '\n' && !isValueCommentAfterSaved) {
                //работа с комментариями (после значения #2) ==========================
                if(!currentComment.empty() && enable_comment && !key_value_string.empty()) {
                    addComment_after(key_value_string, FromComment(currentComment, m_comment_column_size, m_comment_sym));
//                    std::cout << "Ini:comment:after: " << "\"" << currentComment << "\"" << std::endl;
                    currentComment = "";
                } //===================================================================
                isValueCommentAfterSaved = true;
            }
            if(utils::CharsInString(current, __SPACES_WITHOUT_SEPARATORS__) && !isQuotes && !isWordStarted) {
                continue;
            }
            //=====================================================================

            if(!isWordStarted) {
                if(utils::CharsInString(current, __SPACES_WITHOUT_SEPARATORS__))
                    continue;

                isWordStarted = true;
                if(key_value_string.back() != '\\') {
                    key_value_string.clear();
                    keyValueComment = Comment();
                    isQuotes = false;
                } else {
                    key_value_string += '\n';
                }
            }

            if(current == '\n') {
                isWordStarted = false;
                isWordFinished = true;
            } else {
                //работа с комментариями (перед ключом) ===============================
                if(!currentComment.empty() && enable_comment) {
//                    std::cout << "COMMENT(BEFORE): \"" << currentComment << "\"" << std::endl;
                    keyValueComment.before = FromComment(currentComment, m_comment_column_size, m_comment_sym);
//                    std::cout << "Ini:comment:before: " << "\"" << currentComment << "\"" << std::endl;
                    currentComment = "";
                } //===================================================================
                key_value_string += current;
            }

            if(isWordFinished) {
                isWordFinished = false;

//                std::cout << "key_value_string: [" << key_value_string << "]"
//                          << ", comment: [" << currentComment << "]"
//                          << ", kv_comment_before: [" << keyValueComment.before << "]"
//                          << ", kv_comment_after: [" << keyValueComment.after << "]";
                RemoveIllegalSpaces(key_value_string);

                if(key_value_string.front() == '[' && key_value_string.back() == ']') {
                    groupComment = keyValueComment;
                    keyValueComment = Comment();

                    group_string = key_value_string;
                    group_string.erase(0, 1);//erase [
                    group_string.pop_back(); //erase ]
                    put(group_string, Json());
                    addComment(group_string, groupComment);

                    isBeforeStringIsEmpty = false;
//                    std::cout << ", is group: \"" << group_string << "\""
//                              << ", isBeforeStringIsEmpty: "
//                              << (isBeforeStringIsEmpty ? "true" : "false")
//                              << std::endl;
                } else {
//                    std::cout << "is key_value: " << key_value_string << std::endl;
                    if(key_value_string.back() == '\\') {
                        //счётчик строк и столбцов =============================================
                        if(current == '\n') {
                            line_counter++;
                            symbol_counter = 0; //должен перескочить строго на следующей строке
                        } else symbol_counter++;
                        //======================================================================
                        isWordFinished = false;
                        continue;
                    }

                    std::vector<std::string> keys = parseIniKeys(key_value_string);
                    utils::UpdEscSymbols(key_value_string);

                    if(isBeforeStringIsEmpty) {
//                        std::cout << ", disable group name \"" << group_string << "\"";
                        group_string.clear();
                    }
                    isBeforeStringIsEmpty = key_value_string.empty()
                                            && keys.size() == 0
                                            && currentComment.empty();

//                    std::cout << ", isBeforeStringIsEmpty: "
//                              << (isBeforeStringIsEmpty ? "true" : "false")
//                              << std::endl;

                    for(std::string key : keys) {
                        std::vector<std::string> inner_keys = parseIniCustomKeys(key);
                        if(!group_string.empty()) inner_keys.insert(inner_keys.cbegin(), group_string);

                        Json* current_object    = GetObjectForIniCustomKey(this, inner_keys);
                        Json& result_object     = (*current_object);
                        Element new_value       = ParseValueFromString(key_value_string, enable_comment, ConfigFormat::eINI);

                        //TEST: применить настройки комментариев от корневого объекта
                        switch(new_value.first) {
                        case eJson: {
                            new_value.getJson().setCommentColumnSize(m_comment_column_size);
                            new_value.getJson().setCommentSymbol(m_comment_sym);
                            break;
                        }
                        case eArray: {
                            new_value.getArray().setCommentColumnSize(m_comment_column_size);
                            new_value.getArray().setCommentSymbol(m_comment_sym);
                                break;
                            }
                        default: break;
                        }

                        //упаковка значений
                        if(current_object->contains(inner_keys.back())) {
                            switch(result_object[inner_keys.back()].first) {
                            case eNull: { //перезапись значения
                                result_object.updateValue(inner_keys.back(), new_value);
                                break;
                            }
                            case eArray:{ //дополнить список
                                if(result_object[inner_keys.back()].getArray().size() == 1
                                    && result_object[inner_keys.back()].getArray()[0].first == eNull) {
                                    result_object[inner_keys.back()] = JArray(new_value);
                                } else
                                    result_object[inner_keys.back()].getArray().push_back(new_value);
                                break;
                            }
                            default:    { //создать список значений
                                Element temp_e = result_object[inner_keys.back()];
                                JArray temp_ja(temp_e);
                                temp_ja.push_back(new_value);
                                result_object.updateValue(inner_keys.back(), temp_ja);
                                break;
                            }
                            }
                        } else {
                            result_object.put(inner_keys.back(), new_value);
                        }

                        //работа с комментариями (после значения #1) ==========================
                        if(enable_comment) {
                            if(!currentComment.empty()) {
//                                std::cout << "COMMENT(AFTER#1): \"" << currentComment << "\"" << std::endl;
                                keyValueComment.after = FromComment(currentComment, m_comment_column_size, m_comment_sym);
//                                std::cout << "Json:comment:after: " << "\"" << currentComment << "\"" << std::endl;
                                currentComment = "";
                            }
                            //применение комментариев
                            if(!keyValueComment.before.empty() || !keyValueComment.after.empty()) {
                                    result_object.addComment(inner_keys.back(), keyValueComment);
                                keyValueComment = Comment();
                            }
                        } //===================================================================
                    }
                }
                key_value_string.clear();
            }

            if(isCriticalError) {
//                std::cout << "symbols: '" << previous << current << next << "'" << std::endl;
                clear();
                throw std::invalid_argument("INI parse syntax error at line "
                                            + std::to_string(line_counter) + ":" + std::to_string(symbol_counter)
                                            + " '" + current + "'");
            }

            //счётчик строк и столбцов =============================================
            if(current == '\n') {
                line_counter++;
                symbol_counter = 0; //должен перескочить строго на следующей строке
            } else symbol_counter++;
            //======================================================================
        }
    }
//    std::cout << "LAST COMMENT: " << "\"" << currentComment << "\"" << std::endl;
}

bool Json::readFile(const std::string& path, const bool enable_comment,
                    const ConfigFormat config_format) noexcept {
    std::string config_str;

    try{
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cout << "File not found" << std::endl;
            return false;
        }

        std::string temp_string;
        while(getline(file, temp_string))
            config_str += temp_string + '\n';
        file.close();
    } catch (...) {
        std::cout << "exception for read file !!!" << std::endl;
        return false;
    }

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
                     const bool enable_comment, const ConfigFormat config_format) noexcept {
    std::ofstream file(path);
    if (!file.is_open())
        return false;

    switch(config_format) {
    case ConfigFormat::eJSON:
        file << to_JSON_string(tabulation_level, enable_comment, m_comment_column_size) << std::endl;
        break;
    case ConfigFormat::eYAML:
        file << to_YAML_string(tabulation_level, enable_comment, m_comment_column_size) << std::endl;
        break;
    case ConfigFormat::eINI:
        file << to_INI_string(tabulation_level, enable_comment, m_comment_column_size) << std::endl;
        break;
    }

    file.flush();
    file.close();
    return true;
}

std::string Json::to_string(int16_t tabulation_level, const bool enable_comment,
                            const uint8_t column_size, const ConfigFormat config_format) const noexcept {
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
                                 const uint8_t column_size) const noexcept {
    std::string ret;
    bool withoutSpaces = tabulation_level < 0 && !enable_comment;

    if(enable_comment && !m_preview_comment.before.empty()) {
        ret += "\n";
        ret += ToComment(m_preview_comment.before, tabulation_level, m_comment_column_size, m_comment_sym) + "\n";
    }

    ret += "{"; //start of json

    if(m_values.size() == 1 && m_comments.empty()
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
            ret += ToComment(comment_it->second.before, tabulation_level + 1, m_comment_column_size, m_comment_sym) + "\n";
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
                && !comment_it->second.before.empty()) {
                ret += ToComment(comment_it->second.before, tabulation_level, m_comment_column_size, m_comment_sym) + "\n";
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
                if(comment_it->second.after.find('\n') == -1)
                    ret += " " + ToComment(comment_it->second.after);
                else {
                    std::string toComment = ToComment(comment_it->second.after, tabulation_level, m_comment_column_size, m_comment_sym);
                    //NOTE: многострочные комментарии ПОСЛЕ значения должны начинаться на той же строке, что и значение
                    while(toComment.size() > 1 && toComment[0] == '\t')
                        toComment.erase(toComment.cbegin(), toComment.cbegin() + 1);
//                    std::cout << "toComment: \"" << toComment << "\"" << std::endl;
                    ret += " " + toComment;
                }
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
                                   const uint8_t column_size) const noexcept {
    //TODO: Json::to_YAML_string()
    return "";
}

std::string PrintRecursiveIniElements(const ConfigFormat cfg, const Element& el,
                                      const std::string& preview_key) noexcept {
    std::cout << "el: prew:<" << preview_key + ">:" << el.to_string() << std::endl;
    std::string ret;

    std::string key = preview_key;
    RemoveIllegalSpaces(key);
    if(!key.empty() && key.back() != '/')
        key += "/";
    switch(el.first) {
    case eJson: {
        for(const JPair& jp : el.getJson()) {
            ret += PrintRecursiveIniElements(cfg, jp, key);
        }
        break;
    }
    case eArray: {
        for(const Element& e : el.getArray()) {
            ret += PrintRecursiveIniElements(cfg, e, key);
        }
        break;
    }
    default: {
        if(key.back() == '/') key.pop_back();
        ret += key + " = ";
        if(el.first == eString) {
            std::string temp = el.to_string();
            if(temp[0] == '"' && temp.back() == '"') {
                temp.erase(temp.cbegin(), temp.cbegin() + 1);
                temp.pop_back();
            }
            ret += temp + "\n";
        } else
            ret += el.to_string() + "\n";
        break;
    }
    }

    return ret;
}
std::string PrintRecursiveIniElements(const ConfigFormat cfg, const JPair& jp,
                                      const std::string& preview_key) noexcept {
    std::cout << "jp:  prew:<" << preview_key + ">" << jp.first + ":" << jp.second.to_string() << std::endl;
    std::string ret;

    std::string key = preview_key;
    RemoveIllegalSpaces(key);
    if(!key.empty() && key.back() != '/')
        key += "/";
    switch(jp.second.first) {
    case eJson: {
        if(ret.back() != '\n') ret += "\n";
        ret += PrintRecursiveIniElements(cfg, jp.second, key + jp.first);

        break;
    }
    case eArray: {
        ret += PrintRecursiveIniElements(cfg, jp.second, "\t" + jp.first);

        break;
    }
    default: {
        ret += key + jp.first + " = ";
        if(jp.second.first == eString) {
            std::string temp = jp.second.to_string();
            if(temp[0] == '"' && temp.back() == '"') {
                temp.erase(temp.cbegin(), temp.cbegin() + 1);
                temp.pop_back();
            }
            ret += temp + "\n";
        } else
            ret += jp.second.to_string() + "\n";
        break;
    }
    }


    return ret;
}

//TODO: Json::to_INI_string(), не закончил
std::string Json::to_INI_string(int16_t tabulation_level, const bool enable_comment,
                                const uint8_t column_size, const std::string& preview_title) const noexcept {
    std::string ret = "";

    for(const JPair& jp : m_values) {
        const Comment& cmt = getComment(jp.first);
        if(jp.second.first != eJson) {
            ret += PrintRecursiveIniElements(ConfigFormat::eINI, jp);
        }
    }
    ret += "\n";

    for(const JPair& jp : m_values) {
        const Comment& cmt = getComment(jp.first);
        if(jp.second.first == eJson) {
            ret += "[" + jp.first + "]";
            ret += "\n";
            ret += PrintRecursiveIniElements(ConfigFormat::eINI, jp.second);
            ret += "\n";
        }
    }

    return ret;
}

bool Json::contains(const std::string &key) const noexcept {
    for(auto& el : m_values)
        if(el.first == key) return true;

    return false;
}

Json &Json::updateValue(const std::string &key, const Element &new_value) noexcept {
    if(contains(key))   (*this)[key] = Element(new_value);
    else                put(key, Element(new_value));

    return *this;
}

bool Json::operator==(const Json &other) const noexcept {
    if(size() != other.size()) return false;

    for(auto it1 = m_values.begin(), it2 = other.m_values.begin();
         it1 != m_values.end() && it2 != other.m_values.end();
         it1++, it2++
         ) {
        if(it1->second != it2->second) return false;
    }

    return true;
}

Element &Json::operator[](const size_t index) {
    if(m_values.empty())
        __JSON_EMPTY_EXCEPTION__
    __CHECK_INDEX_BOUND_EXCEPTION__(this, index)

    return m_values[index].second;
}

Element &Json::operator[](const std::string &key) {
    for(size_t i = 0; i < m_values.size(); i++)
        if(m_values[i].first == key)
            return m_values[i].second;
    put(key, nullptr);
    return (*this)[key];
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
    __CHECK_INDEX_BOUND_EXCEPTION__(this, index)
    if(index <= m_values.size() - 1)
        m_values.erase(m_values.cbegin() + index);

    return *this;
}

Json &Json::erase(const std::string &key) noexcept {
    bool flag = false;
    size_t index;
    for(index = 0; index < size(); index++) {
        if(m_values[index].first == key) {
            flag = true;
            break;
        }
    }

    if(flag) m_values.erase(m_values.cbegin() + index);

    return *this;
}

Json &Json::erase(const std::vector<std::string> &keys) noexcept {
    for(const std::string &key : keys)
        erase(key);

    return *this;
}
// ======================================================================================== Json


// STATIC FUNCTIONS ============================================================================
//только для ЧИСЕЛ, BOOL, NULL и СТРОК
//TODO: checkValue(string), возможно лишняя теперь
ValueType CheckValue(std::string& value, const ConfigFormat& format) noexcept {
//    std::cout << "CheckValue(): \"" << value << "\"" << std::endl;
    if(value.empty()) return eNull;

    bool isValue = false;
    std::string _value;
    ValueType vType = eNull;
    for(size_t i = 0; i < value.length(); i++) {
        if(vType == ValueType::eNull) {
            //удалить пустоту в начале и конце строки
            RemoveIllegalSpaces(value);

            if(utils::isNumber(value[i])
                || utils::CharsInString(value[i], "-+"))
                vType = ValueType::eNumber;
            else if(!utils::CharsInString(value[i], __SPACES__)
                     && (value[0] == 't'
                         || value[0] == 'f'
                         || value[0] == 'T'
                         || value[0] == 'F'))
                                            vType = ValueType::eBool;
            else if(!utils::CharsInString(value[i], __SPACES__)
                     && (value[0] == 'N' || value[0] == 'n'))
                                            vType = ValueType::eNull;
            else                            vType = ValueType::eString;
        }
        _value += value[i];
    }

    switch(vType) {
    case ValueType::eNumber:    {
        isValue = CheckNumber(_value);
        if(!isValue) {
            vType = ValueType::eString;
            isValue = CheckString(_value);
        }
        break;
    }
    case ValueType::eBool:      {
        isValue = CheckBool(_value);
        if(!isValue) {
            vType = ValueType::eString;
            isValue = CheckString(_value);
        }
        break;
    }
    case ValueType::eNull:      {
        isValue = CheckNull(_value);
        if(!isValue) {
            vType = ValueType::eString;
            isValue = CheckString(_value);
        }
        break;
    }
    case ValueType::eString:    { isValue = CheckString(_value, format); break; }
    default:                    return ValueType::eNull;
    }

    if(isValue) {
        value = _value;
        return vType;
    } else
        return ValueType::eNull;
}

bool CheckNumber(const std::string &value) noexcept {
    if(value.empty()) return false;
    if(value[0] == 'e' || value[0] == 'E' || value[0] == 'f' || value[0] == 'F')
        return false;

    std::regex reg("^[+-]?[0-9]*[.]?[0-9]*[eE]?[+-]?[0-9]*[fF]?$");
    bool matched = std::regex_match(value, reg);

    bool e_is_last = value[value.length() - 1] == 'e' || value[value.length() - 1] == 'E';
    bool f_is_last = value[value.length() - 1] == 'f' || value[value.length() - 1] == 'F';

    if(f_is_last && value.length() > 1) {
        e_is_last = value[value.length() - 2] == 'e' || value[value.length() - 2] == 'E';
    }

    return matched && !e_is_last;
}

bool CheckBool(std::string& value) noexcept {
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

bool CheckNull(std::string& value) noexcept {
//    std::cout << "CheckNull(): \"" << value << "\"" << std::endl;
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

    if(temp != "null") return false;
    value = temp;
    return true;
}

//TODO: CheckString(string)
bool CheckString(std::string& value, const ConfigFormat& format) noexcept {
//    std::cout << "CheckString(): \"" << value << "\"" << std::endl;

    //удалить пробелы в начале и конце строки
    RemoveIllegalSpaces(value);

    bool done = false;
    switch(format) {
    case ConfigFormat::eJSON: {
        if(value[0] == '"') {
            char ch = 0;
            std::string temp;
            for(size_t i = 0; i < value.length(); i++) {
                                            if(ch != 0) { //начинаем запись слова
                    if(!done) {
                        //экранированные кавычки ВСЕГДА заносятся в значение
                        if(value[i] == '\\' && value.length() > i + 1) {
                            char e_ch = utils::getEscChar(value[i + 1]);
                            if(e_ch != 0) {
                                temp += e_ch;
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
    case ConfigFormat::eINI: {
        char ch = 0;
        std::string temp;
        for(size_t i = 0; i < value.length(); i++) {
//            //экранированные кавычки ВСЕГДА заносятся в значение
//            if(value[i] == '\\' && value.length() > i + 1) {
//                char e_ch = utils::getEscChar(value[i + 1]);
//                if(e_ch != 0) {
//                    temp += e_ch;
//                    i++;
//                    continue;
//                }
//            } else
                temp += value[i];
        }
        value = temp;
        return true;
    }
    case ConfigFormat::eYAML:
    default: break;
    }
    return false;
}

bool CheckJson(std::string& value) noexcept {
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

bool CheckArray(std::string& value) noexcept {
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
void RemoveIllegalSpaces(std::string& string) noexcept {
    if(!string.empty()) {
        while(utils::CharsInString(string.back(), __SPACES_WITHOUT_SEPARATORS__))
            string.pop_back();

        while(utils::CharsInString(*string.begin(), __SPACES_WITHOUT_SEPARATORS__))
            string = string.erase(0, 1);
    }
}

std::string ToComment(const std::string &comment_string, const uint8_t tabulation_level,
                      const uint8_t column_size, const char border_symbol) noexcept {
    std::string result;
    std::string current_string = "";
    std::string prefix = utils::RepeatSymToStr('\t', tabulation_level);
    if(border_symbol != 0) {
        char border = border_symbol;
        if(border == '=' || border == '-')
            border = '|';
        prefix += border + std::string(" ");
    }
    char last_symbol = ' ';
    std::vector<size_t> separators;
    separators.reserve(10);

    for(size_t i = 0; i < comment_string.length(); i++) {
        char ch = comment_string[i];

        //игнор "двойного" пробела
        if(last_symbol == ' ' && ch == ' ')
            continue;

        //если встретили разделитель
        if(utils::CharsInString(ch, __COMMENT_SEPARATOR_SYMBOLS__))
            separators.push_back(current_string.length());

        current_string += ch;
        last_symbol = ch;

        if(ch == '\n') {
            //удалить пробелы в начале и конце строки
            RemoveIllegalSpaces(current_string);

            //вывести если не пустое
            if(!current_string.empty()) {
                result += prefix + current_string;
            }

            current_string = "";
            separators.clear();
        }

        if((column_size != 0
            && utils::getStringSize(current_string) >= column_size)
            && (utils::CharsInString(ch, __COMMENT_SEPARATOR_SYMBOLS__) || (i == comment_string.length() - 1))
            ) {
            //удалить пробелы в начале и конце строки
            RemoveIllegalSpaces(current_string);

            //вывести если не пустое
            if(!current_string.empty()) {
                result += prefix;

                //если превышен максимальный размер строки
                if(utils::getStringSize(current_string) > column_size && separators.size() > 0) {
                    uint8_t separate_size;
                    switch(separators.size()) {
                    case 0:     separate_size = 0;              break;
                    case 1:     separate_size = 0;              break;
                    default:    separate_size = separators.size() - 2;
                    }
                    if(current_string.size() > separators[separate_size])
                        separate_size = separators[separate_size] + 1;
                    else
                        separate_size = separators[separate_size];


                    std::string left = utils::SeparateString(current_string, separate_size);
                    RemoveIllegalSpaces(left);

                    if(!utils::CharsInString(current_string.back(), __COMMENT_SEPARATOR_SYMBOLS__))
                        current_string += ' ';
                    result += left + "\n";

                    //снова найти индексы разделителей
                    separators.clear();
                    for(size_t j = 0; j < current_string.length(); j++) {
                        if(utils::CharsInString(current_string[j], __COMMENT_SEPARATOR_SYMBOLS__))
                            separators.push_back(j);
                    }
                } else {
                    result += current_string + "\n";
                    current_string = "";
                    separators.clear();
                }
            }
        }
    }

    if(!current_string.empty()) {
        RemoveIllegalSpaces(current_string);
        result += prefix + current_string;
    }

    std::string ret;
    bool isMulti = result.find('\n') != -1;
    if(isMulti) {
        ret = utils::RepeatSymToStr('\t', tabulation_level) + "/*"
              + (border_symbol != 0 ? utils::RepeatSymToStr(border_symbol, column_size) : "")
              + "\n";
    } else {
        ret += utils::RepeatSymToStr('\t', tabulation_level)
               + "// ";
        RemoveIllegalSpaces(result);
        if(result[0] == border_symbol)
            result.erase(0, 1);
        RemoveIllegalSpaces(result);
    }
    ret += result;
    if(isMulti) {
        ret += (ret.back() == '\n' ? "" : "\n")
               + (border_symbol != 0 ? utils::RepeatSymToStr(border_symbol, column_size) : "")
               + utils::RepeatSymToStr('\t', tabulation_level) + "*/";
    }

    return ret;
}

//NOTE: если символ в списке и в первой строке комментария повторяется минимум 5 раз - это граница, иначе - часть комментария
//NOTE: для всего файла конфига подменяется символ границы только если не задан (первый комментарий с границей)
std::string FromComment(const std::string &comment_string, uint8_t &column_size, char &border_symbol) noexcept {
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

    RemoveIllegalSpaces(current_string);
    if(!isBorderLine && !current_string.empty()) {
        if(!ret.empty()) ret += '\n';
        ret += current_string;
    }

    if(isBorderExists && column_size == 0 && border_size != 0)
        if(column_size < border_size)
            column_size = border_size;

    return ret;
}


// ============================================================================ STATIC FUNCTIONS
