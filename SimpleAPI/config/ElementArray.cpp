#include "ElementArray.h"

#include <stdexcept>
#include "../utils/Utils.h"

//предобъявление
#include "ElementJson.h"
#include "ElementNull.h"
class ElementJson;


ElementArray::ElementArray(const std::string &string, const ConfigFormat format,
                           const bool enable_comments) noexcept {
    init();
    parse_array(string, enable_comments, format);
}

void ElementArray::parse_array(const std::string &string, const bool enable_comment,
                              const ConfigFormat config_format, const CommentDesign& design) {
    switch(config_format) {
    case ConfigFormat::eJSON:   parse_JSON_array(string, enable_comment, design);
    case ConfigFormat::eINI:    parse_INI_array(string, enable_comment, design);
    default:                    return;
    }
}

void ElementArray::parse_JSON_array(const std::string &string, const bool enable_comment,
                                   const CommentDesign& design) {
    clear(); //очистка списка перед новым заполнением
    if(string.empty()) return;

    using namespace utils;
    bool        is_oneline_comment      = false;
    bool        is_multiline_comment    = false;
    char        first_MLC_sym       = 0;
    char        second_MLC_sym      = 0;
    uint8_t     comment_counter     = 0;
    std::string current_comment;
    current_comment.reserve(100);

    bool        is_critical_error   = false;
    bool        is_quotes           = false;
    bool        is_word_started     = false;
    bool        is_word_finished    = false;
    bool        is_value_comment_after_saved = true; //изначально TRUE, чтобы не сработало для первого прохода

    uint16_t    inner_json_counter  = 0;
    uint16_t    inner_array_counter = 0;
    std::string value_string;
    value_string.reserve(100);
    Comment     value_comment;
    IElement    value_element;

    uint16_t    line_counter        = 0; //NOTE: (JArray) ограничение на FFFF строк
    uint16_t    symbol_counter      = 0; //NOTE: (JArray) ограничение на FFFF символов в строке

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

    for(size_t i = 0; i < string.size(); i++) {
        char previous_ch    = i == 0 ? 0 : string[i - 1];
        char current_ch     = string[i];
        char next_ch        = i < string.size() ? string[i] : 0;

        //поиск комментариев
        const bool ext_f = !is_quotes && value_format != VALUE_ARRAY && value_format != VALUE_JSON;
        CommentChecker c_checker = CheckComments(current_ch, next_ch,
                                                 is_oneline_comment,
                                                 is_multiline_comment,
                                                 first_MLC_sym, second_MLC_sym,
                                                 enable_comment, current_comment,
                                                 i, ext_f);
        if(c_checker != CommentChecker::isNotComment) {
            //сюда зайдёт, если внутри комментария
            //счётчик строк и столбцов =============================================
            if(current_ch == '\n') {
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
            if(CharsInString(current_ch, __SPACES__))
                break;
            //=====================================================================

            //работа с комментариями (первичный) ==================================
            if(!current_comment.empty() && enable_comment) {
                addPreviewComment(FromComment(current_comment, m_comment_column_size, m_comment_sym));
                //                std::cout << "JArray:PreviewComment: " << "\"" << current_comment << "\"" << std::endl;
                current_comment = "";
            } //===================================================================

            if(current_ch != '[') {
                is_critical_error = true;
                break;
            }
            state = ARRAY_VALUE;

            break;
        }
        case ARRAY_VALUE: {
            //пропуск пробелов ====================================================
            if(current_ch == '\n' && !is_value_comment_after_saved) {
                //работа с комментариями (после значения #2) ==========================
                if(!current_comment.empty() && enable_comment) {
                    add_comment_after(m_values.size() - 1, FromComment(current_comment, m_comment_column_size, m_comment_sym));
                    //                    std::cout << "JArray:comment:after: " << "\"" << current_comment << "\"" << std::endl;
                    current_comment = "";
                } //===================================================================
                is_value_comment_after_saved = true;
                break;
            }
            if(utils::CharsInString(current_ch, __SPACES__) && !is_quotes && value_format == ValueFormat::VALUE_NOPE)
                break;
            //=====================================================================
            if(current_ch == ']') {
                state = ARRAY_FINISH;
                break;
            }
            //=====================================================================

            if(!is_word_started) {
                is_word_started = true;
                is_value_comment_after_saved = false;
                value_string.clear();
            }

            switch(current_ch) {
            case '{': {
                if(!is_quotes) {
                    if(value_format == ValueFormat::VALUE_NOPE) value_format = ValueFormat::VALUE_JSON;
                    innerJsonCounter++;
                }
                break;
            }
            case '}': {
                if(!is_quotes) innerJsonCounter--;
                break;
            }
            case '[': {
                if(!is_quotes) {
                    if(value_format == ValueFormat::VALUE_NOPE) value_format = ValueFormat::VALUE_ARRAY;
                    innerArrayCounter++;
                }
                break;
            }
            case ']': {
                if(!is_quotes) innerArrayCounter--;
                break;
            }
            case '"': {
                if(previous_ch != '\\') {
                    is_quotes = !is_quotes;
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
                value_string += current_ch;
                if(innerJsonCounter == 0)
                    is_word_finished = true;

                break;
            }
            case VALUE_ARRAY: {
                value_string += current_ch;
                if(innerArrayCounter == 0)
                    is_word_finished = true;

                break;
            }
            case VALUE_OTHER: {
                if(!is_quotes && utils::CharsInString(current_ch, __SPACES__))
                    is_word_finished = true;

                value_string += current_ch;
                break;
            }
            default: break;
            }

            if(!is_word_finished && !is_quotes
                && (innerJsonCounter == 0) && (innerArrayCounter == 0)) {
                //если текущий символ должен обрабатываться другим кодом
                if(utils::CharsInString(current_ch, __SEPARATORS__ + std::string((value_format != VALUE_ARRAY) ? "]" : ""))) {
                    is_word_finished = true;
                    i--;
                    value_string.pop_back();
                }
                //если следующий символ должен обрабатываться другим кодом
                if(utils::CharsInString(next_ch, __SEPARATORS__ + std::string((value_format != VALUE_ARRAY) ? "]" : ""))) {
                    is_word_finished = true;
                }
            }

            if(is_word_finished) {
                is_word_started = false; //страховка
                is_word_finished = false;

                switch(value_format) {
                case VALUE_OTHER: {
                    switch(CheckValue(value_string)) {
                    case eNumber:   {
                        double num;
                        try {
                            push_back(std::stod(value_string));
                        } catch (...) {
                            is_critical_error = true;
                        }

                        break;
                    }
                    case eBool:     {
                        if(utils::isBool(value_string))
                            push_back(utils::toBool(value_string));
                        else
                            is_critical_error = true;

                        break;
                    }
                    case eNull:     {
                        push_back(ElementNull());

                        break;
                    }
                    case eString:   {
                        push_back(value_string);
                        break;
                    }
                    default: //значение не определено
                        is_critical_error = true;
                        break;
                    }

                    break;
                }
                case VALUE_JSON: {
                    ElementJson _innerJson;
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
                        _innerArray.parse_array(value_string, enable_comment);
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
                if(!current_comment.empty() && enable_comment) {
                    valueComment.setPrefix(FromComment(current_comment, m_comment_column_size, m_comment_sym));
                    //                    std::cout << "JArray:comment:before: " << "\"" << current_comment << "\"" << std::endl;
                    current_comment = "";
                } //===================================================================

                state = ARRAY_ELEMENT_SEPARATOR;
                value_format = ValueFormat::VALUE_NOPE;
            }

            break;
        }
        case ARRAY_ELEMENT_SEPARATOR: {
            //пропуск пробелов ====================================================
            if(utils::CharsInString(current_ch, __SPACES_WITHOUT_SEPARATORS__))
                break;
            //=====================================================================
            if(!utils::CharsInString(current_ch, __SEPARATORS__ "]")) {
                is_critical_error = true;
                break;
            }

            if(current_ch == '\n') {
                //работа с комментариями (после значения #1) ==========================
                if(enable_comment) {
                    if(!current_comment.empty()) {
                        valueComment.setSuffix(FromComment(current_comment, m_comment_column_size, m_comment_sym));
//                        std::cout << "JArray:comment:after: " << "\"" << current_comment << "\"" << std::endl;
                        current_comment = "";
                    }
//                    add_comment(m_values.size() - 1, valueComment);
                } //===================================================================
                is_value_comment_after_saved = true;
            }
            if(current_ch == ']') {
                state = ARRAY_FINISH;
            } else
                state = ARRAY_VALUE;

            if(enable_comment && (!valueComment.prefix().empty() || !valueComment.suffix().empty())) {
                //                std::cout << "\tvalue_before: " << valueComment.prefix << std::endl
                //                          << "\tvalue_after: " << valueComment.suffix << std::endl;
                add_comment(m_values.size() - 1, valueComment.prefix(), valueComment.suffix());
            }

            break;
        }
        default: break;
        }

        if(is_criticalError) {
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
                                        + " '" + current_ch + "', current state:" + state_str);
        }

        //счётчик строк и столбцов =============================================
        if(current_ch == '\n') {
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

void ElementArray::parse_INI_array(const std::string &string, const bool enable_comment,
                                  const CommentDesign& design)
{
    //TODO: ElementArray::parse_INI_array()
    //TODO: std::exception
}

std::string ElementArray::to_string(const ConfigFormat format, const int8_t tabulation_level) const noexcept {
    switch(format) {
    case ConfigFormat::eJSON:   return to_JSON_string(tabulation_level);
    case ConfigFormat::eINI:    return to_INI_string(tabulation_level);
    default: return "";
    }
}

std::string ElementArray::to_JSON_string(const int8_t tabulation_level) const noexcept {
    if(m_values.empty()) return "[]";

    bool without_space = tabulation_level == -1;

    std::string ret = "[";
    for(auto& it : m_values) {
        if(!without_space) ret += "\n" + utils::RepeatSymToStr('\t', tabulation_level);
        ret += it.to_string(ConfigFormat::eJSON, tabulation_level+1) + ",";
    }
    if(!without_space) ret += "\n" + utils::RepeatSymToStr('\t', tabulation_level);
    ret += "]";

    return ret;
}

std::string ElementArray::to_INI_string(const int8_t tabulation_level) const noexcept {
    //TODO: ElementArray::to_INI_string()
    return "";
}

std::string ElementArray::to_string(const ConfigFormat format, const CommentDesign &design,
                                    const int8_t tabulation_level) const noexcept {
    switch(format) {
    case ConfigFormat::eJSON:   return to_JSON_string(design, tabulation_level);
    case ConfigFormat::eINI:    return to_INI_string(design, tabulation_level);
    default: return "";
    }
}

std::string ElementArray::to_JSON_string(const CommentDesign &design, const int8_t tabulation_level) const noexcept {
    if(m_values.empty()) return "[]";

    std::string ret = "[";
    for(auto it : m_values) {
        ret += "\n" + utils::RepeatSymToStr('\t', tabulation_level);
        ret += ToComment(it.get_prefix_comment(), design, tabulation_level);
        ret += "\n" + utils::RepeatSymToStr('\t', tabulation_level);
        ret += it.to_string(ConfigFormat::eJSON, design, tabulation_level + 1) + ", ";
        //NOTE: суффиксный многострочный комментарий должен начинаться на той же строке, что и значение переменной
        ret += ToComment(it.get_suffix_comment(), design, tabulation_level);
    }
    ret += "\n" + utils::RepeatSymToStr('\t', tabulation_level);
    ret += "]";

    return ret;
}

std::string ElementArray::to_INI_string(const CommentDesign &design, const int8_t tabulation_level) const noexcept {
    //TODO: ElementArray::to_INI_string()
    return "";
}

void ElementArray::add_comment(const size_t index, const Comment &content) {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    m_values[index].add_comment(content);
}

void ElementArray::add_comment(const size_t index, const std::string &content_before,
                              const std::string &content_after) {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    m_values[index].add_comment(content_before, content_after);
}

void ElementArray::add_prefix_comment(const size_t index, const std::string &content) {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    m_values[index].add_prefix_comment(content);
}

void ElementArray::add_suffix_comment(const size_t index, const std::string &content) {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    m_values[index].add_suffix_comment(content);
}

Comment& ElementArray::get_comment(const size_t index) {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    return m_values[index].get_comment();
}

Comment ElementArray::get_comment(const size_t index) const {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    return m_values[index].get_comment();
}

std::string& ElementArray::get_prefix_comment(const size_t index) {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    return m_values[index].get_prefix_comment();
}

std::string ElementArray::get_prefix_comment(const size_t index) const {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    return m_values[index].get_prefix_comment();
}

std::string& ElementArray::get_suffix_comment(const size_t index) {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    return m_values[index].get_suffix_comment();
}

std::string ElementArray::get_suffix_comment(const size_t index) const {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    return m_values[index].get_suffix_comment();
}

void ElementArray::clear_comment(const size_t index) noexcept {
    __IF_INDEX_NOT_BOUND2__(m_values, index)
    m_values[index].delete_comment();
}

void ElementArray::clear_prefix_comment(const size_t index) noexcept {
    __IF_INDEX_NOT_BOUND2__(m_values, index)
    m_values[index].delete_prefix_comment();
}

void ElementArray::clear_suffix_comment(const size_t index) noexcept {
    __IF_INDEX_NOT_BOUND2__(m_values, index)
    m_values[index].delete_suffix_comment();
}

void ElementArray::delete_comment(const size_t index) noexcept {
    __IF_INDEX_NOT_BOUND2__(m_values, index)
    m_values[index].delete_comment();
}

void ElementArray::delete_prefix_comment(const size_t index) noexcept {
    __IF_INDEX_NOT_BOUND2__(m_values, index)
    m_values[index].delete_prefix_comment();
}

void ElementArray::delete_suffix_comment(const size_t index) noexcept {
    __IF_INDEX_NOT_BOUND2__(m_values, index)
    m_values[index].delete_suffix_comment();
}

ElementArray &ElementArray::clear() noexcept {
    m_values.clear();
    return *this;
}

bool ElementArray::is_equal(const IElement &other, const bool compare_comments) const noexcept {
    if(m_values != dynamic_cast<const ElementArray&>(other).m_values)   return false;
    if(compare_comments && m_comment == other.get_comment())             return false;

    return true;
}

ValueType ElementArray::get_type_front() {
    __CHECK_ARRAY_EMPTY_EXCEPTION__(m_values)
    return m_values.front().getType();
}

ValueType ElementArray::get_type_at(const size_t index) {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    return m_values[index].getType();
}

ValueType ElementArray::get_type_back() {
    __CHECK_ARRAY_EMPTY_EXCEPTION__(m_values)
    return m_values.back().getType();
}

ElementArray& ElementArray::push_front(const IElement &element) noexcept {
    m_values.insert(m_values.cbegin(), element);
    return *this;
}

ElementArray& ElementArray::push_at(const IElement &element, const size_t index) {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index - 1) //проверяем, что size() < index - 1 (+1[нумерация с 0])
    m_values.insert(m_values.cbegin() + index, element);
    return *this;
}

ElementArray& ElementArray::push_back(const IElement &element) noexcept {
    m_values.push_back(element);
    return *this;
}

IElement& ElementArray::get_value(const size_t index) {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    return m_values[index];
}

IElement& ElementArray::get_value(const VString& complex_key) {
    if(complex_key.size() == 0)
        throw std::invalid_argument("complex_key vector is empty");

    __CHECK_ARRAY_EMPTY_EXCEPTION__(m_values)

    if(complex_key.front().empty())
        throw std::invalid_argument("complex_key argument cannot be empty");

    size_t current_index;
    try         { current_index = std::stoi(complex_key.front()); }
    catch(...)  { __ARRAY_INCORRECT_INDEX_EXCEPTION__ }

    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, current_index)

    if(complex_key.size() == 1)
        return (*this)[current_index];
    else {
        IElement& el = (*this)[current_index];

        auto new_complex_key = complex_key;
        new_complex_key.erase(new_complex_key.begin());
        switch(el.getType()) {
        case eJson:     return dynamic_cast<ElementJson&>((*this)[current_index])[new_complex_key];
        case eArray:    return dynamic_cast<ElementArray&>((*this)[current_index])[new_complex_key];
        default: __INCORRECT_TYPE_ELEMENT_FOR_INDEX_EXCEPTION__
        }
    }
}

IElement& ElementArray::get_value(const std::vector<size_t>& complex_key) {
    if(complex_key.size() == 0)
        throw std::invalid_argument("complex_key vector is empty");

    __CHECK_ARRAY_EMPTY_EXCEPTION__(m_values)

    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, complex_key.front())

    if(complex_key.size() == 1)
        return (*this)[complex_key.front()];
    else {
        IElement& el = (*this)[complex_key.front()];

        auto new_complex_key = complex_key;
        new_complex_key.erase(new_complex_key.begin());
        switch(el.getType()) {
//TODO:        case eJson:     return dynamic_cast<ElementJson&>((*this)[complex_key.front()]).get_value(new_complex_key);
        case eArray:    return dynamic_cast<ElementArray&>((*this)[complex_key.front()]).get_value(new_complex_key);
        default: __INCORRECT_TYPE_ELEMENT_FOR_INDEX_EXCEPTION__
        }
    }
}

ElementArray &ElementArray::pop_front() {
    __CHECK_ARRAY_EMPTY_EXCEPTION__(m_values)
    m_values.erase(m_values.cbegin());
    return *this;
}

ElementArray &ElementArray::pop_at(const size_t index) {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    m_values.erase(m_values.cbegin() + index);
    return *this;
}

ElementArray &ElementArray::pop_back() {
    __CHECK_ARRAY_EMPTY_EXCEPTION__(m_values)
    m_values.pop_back();
    return *this;
}

ElementArray &ElementArray::erase(const size_t index) {
    pop_at(index);
    return *this;
}

ElementArray &ElementArray::erase(const VElement::iterator &iterator) {
    m_values.erase(iterator);
    return *this;
}

ElementArray &ElementArray::erase(const VElement::iterator &begin, const VElement::iterator &end) {
    m_values.erase(begin, end);
    return *this;
}

ElementArray &ElementArray::erase(const std::vector<size_t> indexes) {
    for(auto i : indexes) pop_at(i);
    return *this;
}

void ElementArray::operator=(const VElement &other) noexcept {
    m_values = other;
}


bool IsElementArray(const std::string &str, const ConfigFormat format) noexcept {
    switch(format) {
    case ConfigFormat::eJSON:   return IsElementJsonArray(str);
    case ConfigFormat::eINI:    return IsElementIniArray(str);
    default: return false;
    }

    return false;
}

bool IsElementJsonArray(const std::string &str) noexcept {
    //TODO: IsElementJsonArray()
    return false;
}

bool IsElementIniArray(const std::string &str) noexcept {
    //TODO: IsElementIniArray()
    return false;
}
