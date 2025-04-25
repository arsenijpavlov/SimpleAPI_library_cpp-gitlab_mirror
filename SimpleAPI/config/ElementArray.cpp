#include "ElementArray.h"

#include <stdexcept>
#include "../utils/Utils.h"

//предобъявление
#include "ElementBool.h"
#include "ElementJson.h"
#include "ElementNull.h"
#include "ElementNumber.h"
#include "ElementString.h"
class ElementJson;


ElementArray::ElementArray(const std::string &string, const ConfigFormat format,
                           const bool enable_comments) noexcept {
    init();
    parseArray(string, enable_comments, format);
}

void ElementArray::parseArray(const std::string &string, const bool enable_comment,
                              const ConfigFormat config_format, CommentDesign* design) {
    switch(config_format) {
    case ConfigFormat::eJSON:   parseJsonArray(string, enable_comment, design);
    case ConfigFormat::eINI:    parseIniArray(string, enable_comment, design);
    default:                    return;
    }
}

void ElementArray::parseJsonArray(const std::string &string, const bool enable_comment,
                                   CommentDesign* design) {
    clear(); //очистка списка перед новым заполнением
    if(string.empty()) return;

    using namespace utils;

    //Если пользователь не задал переменную, то использовать стандартную
    CommentDesign temp_design;
    CommentDesign& comment_design = design ? *design : temp_design;

    bool    is_oneline_comment      = false;
    bool    is_multiline_comment    = false;
    uint8_t comment_counter         = 0;
    std::string current_comment;

    bool is_critical_error              = false;
    bool is_quotes                      = false;
    bool is_word_started                = false;
    bool is_word_finished               = false;
    bool is_value_comment_after_saved   = true; //изначально TRUE, чтобы не сработало для первого прохода

    uint16_t    inner_json_counter  = 0;
    uint16_t    inner_array_counter = 0;
    std::string value_string;
    Comment     value_comment;
    IElement    value_element;

    uint16_t line_counter   = 0; //NOTE: (JArray) ограничение на FFFF строк
    uint16_t symbol_counter = 0; //NOTE: (JArray) ограничение на FFFF символов в строке

    enum State {
        ARRAY_START,
        ARRAY_VALUE,
        ARRAY_VALUE_SEPARATOR,
        ARRAY_FINISH
    } state = ARRAY_START;
    auto UpdateState = [&](State new_state) {
        state = new_state;
        std::string state_str;
        switch(new_state) {
        case ARRAY_START:           state_str = "ARRAY_START";              break;
        case ARRAY_VALUE:           state_str = "ARRAY_VALUE";              break;
        case ARRAY_VALUE_SEPARATOR: state_str = "ARRAY_VALUE_SEPARATOR";    break;
        case ARRAY_FINISH:          state_str = "ARRAY_FINISH";             break;
        }
//        std::cout << state_str << std::endl;
    };

    enum ValueFormat {
        VALUE_NOPE,
        VALUE_JSON,
        VALUE_ARRAY,
        VALUE_OTHER
    } value_format = VALUE_NOPE;

    /* TODO: для документации
     * комментарий массива
     * начало массива
     * (+комментарий перед значением)
     * значение массива
     * (+комментарий после значения) = на строке значения
     * (разделитель)
     * (+комментарий перед значением)= после разделителя
     * (+значение массива)           = после разделителя
     * (+комментарий после значения) = после разделителя
     * конец массива
    */

    CommentChecker comment_checker;
    for(size_t i = 0; i < string.size(); i++) {
        char previous_ch    = i == 0 ? 0 : string[i - 1];
        char current_ch     = string[i];
        char next_ch        = i < string.size() ? string[i] : 0;

        //счётчик строк и столбцов =============================================
        auto Counter = [&]() {
            if(current_ch == '\n') {
                line_counter++;
                symbol_counter = 0; //должен перескочить строго на следующей строке
            } else symbol_counter++;
        };
        //======================================================================

        //поиск комментариев ===================================================
        const bool ext_f = !is_quotes
                           && value_format != VALUE_ARRAY
                           && value_format != VALUE_JSON;
        CheckComments(current_ch, next_ch,
                      comment_checker, enable_comment,
                      comment_design, current_comment,
                      i, ext_f);
        //сюда зайдёт, если внутри комментария
        if(comment_checker != CommentChecker::eIsNotComment) {
            Counter();
            continue;
        } //================================================= поиск комментариев

        //работа с синтаксисом JSON_ARRAY
        switch(state) {
        case ARRAY_START: {
            //пропуск пробелов =====================================================
            if(CharsInString(current_ch, __SPACES__)) break;
            //===================================================== пропуск пробелов

            //работа с комментариями (до разбора массива) ==========================
            if(!current_comment.empty() && enable_comment) {
                addComment(FromComment(current_comment, comment_design));
                current_comment.clear();
//                std::cout << "JArray:PreviewComment: " << "\"" << current_comment << "\"" << std::endl;
            } //================================= работа с комментариями (первичный)

            if(current_ch != '[') {
                is_critical_error = true;
                break;
            }
            UpdateState(ARRAY_VALUE);

            break;
        }
        case ARRAY_VALUE: {
            //пропуск пробелов ====================================================
            if(current_ch == '\n' && !is_value_comment_after_saved) {
                //работа с комментариями (после значения #2) ==========================
                if(!current_comment.empty() && enable_comment) {
                    add_suffix_comment(m_values.size() - 1, FromComment(current_comment, comment_design));
                    current_comment.clear();
//                    std::cout << "JArray:comment:after: " << "\"" << current_comment << "\"" << std::endl;
                } //======================== работа с комментариями (после значения #2)
                is_value_comment_after_saved = true;
                break;
            }
            if(CharsInString(current_ch, __SPACES__) && !is_quotes && value_format == ValueFormat::VALUE_NOPE)
                break;
            //==================================================== пропуск пробелов

            if(current_ch == ']') {
                UpdateState(ARRAY_FINISH);
                break;
            }

            if(!is_word_started) {
                is_word_started                 = true;
                is_value_comment_after_saved    = false;
                value_string.clear();
            }

            switch(current_ch) {
            case '{': {
                if(!is_quotes) {
                    if(value_format == ValueFormat::VALUE_NOPE)
                        value_format = ValueFormat::VALUE_JSON;
                    inner_json_counter++;
                }
                break;
            }
            case '}': {
                if(!is_quotes)
                    inner_json_counter--;
                break;
            }
            case '[': {
                if(!is_quotes) {
                    if(value_format == ValueFormat::VALUE_NOPE)
                        value_format = ValueFormat::VALUE_ARRAY;
                    inner_array_counter++;
                }
                break;
            }
            case ']': {
                if(!is_quotes)
                    inner_array_counter--;
                break;
            }
            case '"': {
                if(previous_ch != '\\') {
                    is_quotes = !is_quotes;
//                    std::cout << "value."
//                              << "isQuotes: (" << to_string(isQuotes) << ") "
//                              << "'" << previous << current << next << "'"
//                              << std::endl;
                }
            }
            default: {
                if(value_format == ValueFormat::VALUE_NOPE)
                    value_format = ValueFormat::VALUE_OTHER;
                break;
            }
            }

            //поиск конца значения =================================================
            switch(value_format) {
            case VALUE_JSON: {
                if(inner_json_counter == 0) //прочли весь вложенный JSON
                    is_word_finished = true;

                value_string += current_ch;
                break;
            }
            case VALUE_ARRAY: {
                if(inner_array_counter == 0) //прочли весь вложенный массив
                    is_word_finished = true;

                value_string += current_ch;
                break;
            }
            case VALUE_OTHER: {
                if(!is_quotes && CharsInString(current_ch, __SPACES__)) //прочли всё значение
                    is_word_finished = true;

                value_string += current_ch;
                break;
            }
            default: break;
            }

            if(!is_word_finished
                && !is_quotes
                && (inner_json_counter == 0)
                && (inner_array_counter == 0))
            {
                //если ТЕКУЩИЙ символ должен обрабатываться другим кодом
                if(CharsInString(current_ch, __SEPARATORS__ + std::string((value_format != VALUE_ARRAY) ? "]" : ""))) {
                    is_word_finished = true;
                    i--;
                    value_string.pop_back();
                }
                //если СЛЕДУЮЩИЙ символ должен обрабатываться другим кодом
                if(CharsInString(next_ch, __SEPARATORS__ + std::string((value_format != VALUE_ARRAY) ? "]" : ""))) {
                    is_word_finished = true;
                }
            }
            //================================================= поиск конца значения


            //обработка итогового значения =========================================
            if(is_word_finished) {
                is_word_started     = false; //страховка
                is_word_finished    = false;

                switch(value_format) {
                case VALUE_OTHER: {
                    switch(CheckValue(value_string, ConfigFormat::eJSON)) {
                    case ValueType::eNumber: {
                        try {
                            push_back(ElementNumber(std::stod(value_string)));
                        } catch (...) {
                            is_critical_error = true;
                        }

                        break;
                    }
                    case ValueType::eBool: {
                        if(IsBool(value_string))
                            push_back(ElementBool(ToBool(value_string)));
                        else
                            is_critical_error = true;

                        break;
                    }
                    case ValueType::eNull: {
                        push_back(ElementNull());

                        break;
                    }
                    case ValueType::eString: {
                        push_back(ElementString(value_string));
                        break;
                    }
                    default: //значение не определено
                        is_critical_error = true;
                        break;
                    }

                    break;
                }
                case VALUE_JSON: {
                    try {
                        ElementJson _inner_json;
                        _inner_json.parseJson(value_string, enable_comment);
                        push_back(_inner_json);
                    } catch (std::invalid_argument& e) {
                        is_critical_error = true;
                    }

                    break;
                }
                case VALUE_ARRAY: {
                    try {
                        ElementArray _inner_array;
                        _inner_array.parseArray(value_string, enable_comment);
                        push_back(_inner_array);
                    } catch (std::invalid_argument& e) {
                        is_critical_error = true;
                    }

                    break;
                }
                default: {
                    is_critical_error = true;
                    break;
                }
                }

                //работа с комментариями (перед значением) =============================
                if(!current_comment.empty() && enable_comment) {
                    add_prefix_comment(m_values.size() - 1, FromComment(current_comment, comment_design));
                    current_comment.clear();
//                    std::cout << "JArray:comment:before: " << "\"" << current_comment << "\"" << std::endl;
                } //=========================== работа с комментариями (перед значением)

                UpdateState(ARRAY_VALUE_SEPARATOR);
                value_format = ValueFormat::VALUE_NOPE;
            } //======================================= обработка итогового значения

            break;
        }
        case ARRAY_VALUE_SEPARATOR: {
            //пропуск пробелов ====================================================
            if(CharsInString(current_ch, __SPACES_WITHOUT_SEPARATORS__))
                break;
            //=====================================================================
            if(!CharsInString(current_ch, __SEPARATORS__ "]")) {
                is_critical_error = true;
                break;
            }

            if(current_ch == '\n') {
                //работа с комментариями (после значения #1) ==========================
                if(enable_comment && !current_comment.empty()) {
                    add_suffix_comment(m_values.size() - 1, FromComment(current_comment, comment_design));
                    current_comment.clear();
//                    std::cout << "JArray:comment:after: " << "\"" << current_comment << "\"" << std::endl;
                } //===================================================================
                is_value_comment_after_saved = true;
            }
            UpdateState(current_ch == ']' ? ARRAY_FINISH : ARRAY_VALUE);

            if(enable_comment && (!value_comment.prefix().empty() || !value_comment.suffix().empty())) {
                add_comment(m_values.size() - 1, value_comment.prefix(), value_comment.suffix());
//                std::cout << "\tvalue_before: " << value_comment.prefix << std::endl
//                          << "\tvalue_after: " << value_comment.suffix << std::endl;
            }

            break;
        }
        default: break;
        }

        if(is_critical_error) {
            std::string state_str = "";
            switch(state) {
            case ARRAY_START:           state_str = "JSON_START ";              break;
            case ARRAY_VALUE:           state_str = "JSON_VALUE ";              break;
            case ARRAY_VALUE_SEPARATOR: state_str = "JSON_ELEMENT_SEPARATOR ";  break;
            case ARRAY_FINISH:          state_str = "JSON_FINISH ";             break;
            }
//            std::cout << "symbols: '" << previous << current << next << "'" << std::endl;

            clear();
            throw std::invalid_argument("JSON_ARRAY parse error at line "
                                        + std::to_string(line_counter) + ":" + std::to_string(symbol_counter)
                                        + " '" + current_ch + "', current state:" + state_str);
        }

        Counter();
    }


    if(state != ARRAY_FINISH) {
        clear();
        throw std::invalid_argument("JSON_ARRAY parse error, end of JSON array structure not found");
    }
}

void ElementArray::parseIniArray(const std::string &string, const bool enable_comment,
                                  CommentDesign* design)
{
    //TODO: ElementArray::parseIniArray()
    //TODO: std::exception
}

std::string ElementArray::toString(const ConfigFormat format, const int8_t tabulation_level) const noexcept {
    switch(format) {
    case ConfigFormat::eJSON:   return toJsonString(tabulation_level);
    case ConfigFormat::eINI:    return toIniString(tabulation_level);
    default: return "";
    }
}

std::string ElementArray::toJsonString(const int8_t tabulation_level) const noexcept {
    if(m_values.empty()) return "[]";

    using namespace utils;
    bool without_space = tabulation_level == -1;

    std::string ret = "[";
    for(auto& it : m_values) {
        if(!without_space) ret += "\n" + RepeatSymToStr('\t', tabulation_level);
        ret += it.toString(ConfigFormat::eJSON, tabulation_level+1) + ",";
    }
    if(!without_space) ret += "\n" + RepeatSymToStr('\t', tabulation_level);
    ret += "]";

    return ret;
}

std::string ElementArray::toIniString(const int8_t tabulation_level) const noexcept {
    //TODO: ElementArray::toIniString()
    return "";
}

std::string ElementArray::toString(const ConfigFormat format, const CommentDesign &design,
                                    const int8_t tabulation_level) const noexcept {
    switch(format) {
    case ConfigFormat::eJSON:   return toJsonString(design, tabulation_level);
    case ConfigFormat::eINI:    return toIniString(design, tabulation_level);
    default: return "";
    }
}

std::string ElementArray::toJsonString(const CommentDesign &design, const int8_t tabulation_level) const noexcept {
    if(m_values.empty()) return "[]";

    using namespace utils;
    std::string ret = "[";
    for(auto it : m_values) {
        ret += "\n" + RepeatSymToStr('\t', tabulation_level);
        ret += ToComment(it.getPrefixComment(), design, tabulation_level);
        ret += "\n" + RepeatSymToStr('\t', tabulation_level);
        ret += it.toString(ConfigFormat::eJSON, design, tabulation_level + 1) + ", ";
        //NOTE: суффиксный многострочный комментарий должен начинаться на той же строке, что и значение переменной
        ret += ToComment(it.getSuffixComment(), design, tabulation_level);
    }
    ret += "\n" + RepeatSymToStr('\t', tabulation_level);
    ret += "]";

    return ret;
}

std::string ElementArray::toIniString(const CommentDesign &design, const int8_t tabulation_level) const noexcept {
    //TODO: ElementArray::toIniString()
    return "";
}

void ElementArray::add_comment(const size_t index, const Comment &content) {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    m_values[index].addComment(content);
}

void ElementArray::add_comment(const size_t index, const std::string &content_before,
                              const std::string &content_after) {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    m_values[index].addComment(content_before, content_after);
}

void ElementArray::add_prefix_comment(const size_t index, const std::string &content) {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    m_values[index].addPrefixComment(content);
}

void ElementArray::add_suffix_comment(const size_t index, const std::string &content) {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    m_values[index].addSuffixComment(content);
}

Comment& ElementArray::get_comment(const size_t index) {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    return m_values[index].getComment();
}

Comment ElementArray::get_comment(const size_t index) const {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    return m_values[index].getComment();
}

std::string& ElementArray::get_prefix_comment(const size_t index) {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    return m_values[index].getPrefixComment();
}

std::string ElementArray::get_prefix_comment(const size_t index) const {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    return m_values[index].getPrefixComment();
}

std::string& ElementArray::get_suffix_comment(const size_t index) {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    return m_values[index].getSuffixComment();
}

std::string ElementArray::get_suffix_comment(const size_t index) const {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    return m_values[index].getSuffixComment();
}

void ElementArray::clear_comment(const size_t index) noexcept {
    __IF_INDEX_NOT_BOUND2__(m_values, index)
    m_values[index].deleteComment();
}

void ElementArray::clear_prefix_comment(const size_t index) noexcept {
    __IF_INDEX_NOT_BOUND2__(m_values, index)
    m_values[index].deletePrefixComment();
}

void ElementArray::clear_suffix_comment(const size_t index) noexcept {
    __IF_INDEX_NOT_BOUND2__(m_values, index)
    m_values[index].deleteSuffixComment();
}

void ElementArray::delete_comment(const size_t index) noexcept {
    __IF_INDEX_NOT_BOUND2__(m_values, index)
    m_values[index].deleteComment();
}

void ElementArray::delete_prefix_comment(const size_t index) noexcept {
    __IF_INDEX_NOT_BOUND2__(m_values, index)
    m_values[index].deletePrefixComment();
}

void ElementArray::delete_suffix_comment(const size_t index) noexcept {
    __IF_INDEX_NOT_BOUND2__(m_values, index)
    m_values[index].deleteSuffixComment();
}

ElementArray &ElementArray::clear() noexcept {
    m_values.clear();
    return *this;
}

bool ElementArray::isEqual(const IElement &other, const bool compare_comments) const noexcept {
    if(m_values != dynamic_cast<const ElementArray&>(other).m_values)   return false;
    if(compare_comments && m_comment == other.getComment())             return false;

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
        case ValueType::eJson:  return dynamic_cast<ElementJson&>((*this)[current_index])[new_complex_key];
        case ValueType::eArray: return dynamic_cast<ElementArray&>((*this)[current_index])[new_complex_key];
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
//TODO:        case ValueType::eJson:     return dynamic_cast<ElementJson&>((*this)[complex_key.front()]).get_value(new_complex_key);
        case ValueType::eArray: return dynamic_cast<ElementArray&>((*this)[complex_key.front()]).get_value(new_complex_key);
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
