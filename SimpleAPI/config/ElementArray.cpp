#include "ElementArray.h"

#include <algorithm>
#include <stdexcept>
#include "../utils/Utils.h"

//предобъявление
#include "Config.h"
#include "ElementBool.h"
#include "ElementArray.h"
#include "ElementNumber.h"
#include "ElementString.h"
class ElementArray;


//void ElementArray::parseJsonArray(const std::string &string, const bool enable_comment,
//                                   CommentDesign* design) {
//    clear(); //очистка списка перед новым заполнением
//    if(string.empty()) return;

//    using namespace utils;

//    //Если пользователь не задал переменную, то использовать стандартную
//    CommentDesign temp_design;
//    CommentDesign& comment_design = design ? *design : temp_design;

//    bool    is_oneline_comment      = false;
//    bool    is_multiline_comment    = false;
//    uint8_t comment_counter         = 0;
//    std::string current_comment;

//    bool is_critical_error              = false;
//    bool is_quotes                      = false;
//    bool is_word_started                = false;
//    bool is_word_finished               = false;
//    bool is_value_comment_after_saved   = true; //изначально TRUE, чтобы не сработало для первого прохода

//    uint16_t    inner_json_counter  = 0;
//    uint16_t    inner_array_counter = 0;
//    std::string value_string;
//    Comment     value_comment;
//    IElement    value_element;

//    uint16_t line_counter   = 0; //NOTE: (ElementArray) ограничение на FFFF строк
//    uint16_t symbol_counter = 0; //NOTE: (ElementArray) ограничение на FFFF символов в строке

//    enum class State {
//        eARRAY_START,
//        eARRAY_VALUE,
//        eARRAY_VALUE_SEPARATOR,
//        eARRAY_FINISH
//    } state = State::eARRAY_START;

//    auto ToString = [](State state) {
//        switch(state) {
//        case State::eARRAY_START:           return "ARRAY_START";
//        case State::eARRAY_VALUE:           return "ARRAY_VALUE";
//        case State::eARRAY_VALUE_SEPARATOR: return "ARRAY_VALUE_SEPARATOR";
//        case State::eARRAY_FINISH:          return "ARRAY_FINISH";
//        default:                            return "UNKNOWN_STAGE";
//        }
//    };
//    auto UpdateState = [&](State new_state) {
//        state = new_state;
////        std::cout << ToString(new_state) << std::endl;
//    };

//    enum ValueFormat {
//        VALUE_NOPE,
//        VALUE_JSON,
//        VALUE_ARRAY,
//        VALUE_OTHER
//    } value_format = VALUE_NOPE;

//    /* NOTE: для документации
//     * комментарий массива
//     * начало массива
//     * (+комментарий перед значением)
//     * значение массива
//     * (+комментарий после значения) = на строке значения
//     * (разделитель)
//     * (+комментарий перед значением)= после разделителя
//     * (+значение массива)           = после разделителя
//     * (+комментарий после значения) = после разделителя
//     * конец массива
//    */

//    CommentChecker comment_checker = CommentChecker::eIsNotComment;
//    for(size_t i = 0; i < string.size(); i++) {
//        char previous_ch    = i == 0 ? 0 : string[i - 1];
//        char current_ch     = string[i];
//        char next_ch        = i < string.size() ? string[i + 1] : 0;

//        //счётчик строк и столбцов =============================================
//        auto Counter = [&]() {
//            if(current_ch == '\n') {
//                line_counter++;
//                symbol_counter = 0; //должен перескочить строго на следующей строке
//            } else symbol_counter++;
//        };
//        //======================================================================

//        //поиск комментариев ===================================================
//        const bool ext_f = !is_quotes
//                           && value_format != VALUE_ARRAY
//                           && value_format != VALUE_JSON;
//        CheckComments(current_ch, next_ch,
//                      comment_checker, enable_comment,
//                      comment_design, current_comment,
//                      i, ext_f);
//        //сюда зайдёт, если внутри комментария
//        if(comment_checker != CommentChecker::eIsNotComment) {
//            Counter();
//            continue;
//        } //================================================= поиск комментариев

//        //работа с синтаксисом JSON_ARRAY
//        switch(state) {
//        case State::eARRAY_START: {
//            //пропуск пробелов =====================================================
//            if(CharsInString(current_ch, __SPACES__)) break;
//            //===================================================== пропуск пробелов

//            //работа с комментариями (до разбора массива) ==========================
//            if(!current_comment.empty() && enable_comment) {
//                addComment(FromComment(current_comment, comment_design));
//                current_comment.clear();
////                std::cout << "ElementArray:PreviewComment: " << "\"" << current_comment << "\"" << std::endl;
//            } //================================= работа с комментариями (первичный)

//            if(current_ch != '[') {
//                is_critical_error = true;
//                break;
//            }
//            UpdateState(State::eARRAY_VALUE);

//            break;
//        }
//        case State::eARRAY_VALUE: {
//            //пропуск пробелов ====================================================
//            if(current_ch == '\n' && !is_value_comment_after_saved) {
//                //работа с комментариями (после значения #2) ==========================
//                if(!current_comment.empty() && enable_comment) {
//                    add_suffix_comment(m_values.size() - 1, FromComment(current_comment, comment_design));
//                    current_comment.clear();
////                    std::cout << "ElementArray:comment:after: " << "\"" << current_comment << "\"" << std::endl;
//                } //======================== работа с комментариями (после значения #2)
//                is_value_comment_after_saved = true;
//                break;
//            }
//            if(CharsInString(current_ch, __SPACES__) && !is_quotes && value_format == ValueFormat::VALUE_NOPE)
//                break;
//            //==================================================== пропуск пробелов

//            if(current_ch == ']') {
//                UpdateState(State::eARRAY_FINISH);
//                break;
//            }

//            if(!is_word_started) {
//                is_word_started                 = true;
//                is_value_comment_after_saved    = false;
//                value_string.clear();
//            }

//            switch(current_ch) {
//            case '{': {
//                if(!is_quotes) {
//                    if(value_format == ValueFormat::VALUE_NOPE)
//                        value_format = ValueFormat::VALUE_JSON;
//                    inner_json_counter++;
//                }
//                break;
//            }
//            case '}': {
//                if(!is_quotes)
//                    inner_json_counter--;
//                break;
//            }
//            case '[': {
//                if(!is_quotes) {
//                    if(value_format == ValueFormat::VALUE_NOPE)
//                        value_format = ValueFormat::VALUE_ARRAY;
//                    inner_array_counter++;
//                }
//                break;
//            }
//            case ']': {
//                if(!is_quotes)
//                    inner_array_counter--;
//                break;
//            }
//            case '"': {
//                if(previous_ch != '\\') {
//                    is_quotes = !is_quotes;
////                    std::cout << "value."
////                              << "isQuotes: (" << to_string(isQuotes) << ") "
////                              << "'" << previous << current << next << "'"
////                              << std::endl;
//                }
//            }
//            default: {
//                if(value_format == ValueFormat::VALUE_NOPE)
//                    value_format = ValueFormat::VALUE_OTHER;
//                break;
//            }
//            }

//            //поиск конца значения =================================================
//            switch(value_format) {
//            case VALUE_JSON: {
//                if(inner_json_counter == 0) //прочли весь вложенный JSON
//                    is_word_finished = true;

//                value_string += current_ch;
//                break;
//            }
//            case VALUE_ARRAY: {
//                if(inner_array_counter == 0) //прочли весь вложенный массив
//                    is_word_finished = true;

//                value_string += current_ch;
//                break;
//            }
//            case VALUE_OTHER: {
//                if(!is_quotes && CharsInString(current_ch, __SPACES__)) //прочли всё значение
//                    is_word_finished = true;

//                value_string += current_ch;
//                break;
//            }
//            default: break;
//            }

//            if(!is_word_finished
//                && !is_quotes
//                && (inner_json_counter == 0)
//                && (inner_array_counter == 0))
//            {
//                //если ТЕКУЩИЙ символ должен обрабатываться другим кодом
//                if(CharsInString(current_ch, __SEPARATORS__ + std::string((value_format != VALUE_ARRAY) ? "]" : ""))) {
//                    is_word_finished = true;
//                    i--;
//                    value_string.pop_back();
//                }
//                //если СЛЕДУЮЩИЙ символ должен обрабатываться другим кодом
//                if(CharsInString(next_ch, __SEPARATORS__ + std::string((value_format != VALUE_ARRAY) ? "]" : ""))) {
//                    is_word_finished = true;
//                }
//            }
//            //================================================= поиск конца значения


//            //обработка итогового значения =========================================
//            if(is_word_finished) {
//                is_word_started     = false; //страховка
//                is_word_finished    = false;

//                switch(value_format) {
//                case VALUE_OTHER: {
//                    switch(CheckValue(value_string, ConfigFormat::eJSON)) {
//                    case ValueType::eNumber: {
//                        try {
//                            push_back(ElementNumber(std::stod(value_string)));
//                        } catch (...) {
//                            is_critical_error = true;
//                        }

//                        break;
//                    }
//                    case ValueType::eBool: {
//                        if(IsBool(value_string))
//                            push_back(ElementBool(ToBool(value_string)));
//                        else
//                            is_critical_error = true;

//                        break;
//                    }
//                    case ValueType::eNull: {
//                        push_back(IElement());

//                        break;
//                    }
//                    case ValueType::eString: {
//                        push_back(ElementString(value_string));
//                        break;
//                    }
//                    default: //значение не определено
//                        is_critical_error = true;
//                        break;
//                    }

//                    break;
//                }
//                case VALUE_JSON: {
//                    try {
////                        ElementArray _inner_json;
////TODO:                        _inner_json.parseJson(value_string, enable_comment);
////                        push_back(_inner_json);
//                    } catch (std::invalid_argument& e) {
//                        is_critical_error = true;
//                    }

//                    break;
//                }
//                case VALUE_ARRAY: {
//                    try {
//                        ElementArray _inner_array;
//                        _inner_array.parseArray(value_string, enable_comment);
//                        push_back(_inner_array);
//                    } catch (std::invalid_argument& e) {
//                        is_critical_error = true;
//                    }

//                    break;
//                }
//                default: {
//                    is_critical_error = true;
//                    break;
//                }
//                }

//                //работа с комментариями (перед значением) =============================
//                if(!current_comment.empty() && enable_comment) {
//                    add_prefix_comment(m_values.size() - 1, FromComment(current_comment, comment_design));
//                    current_comment.clear();
////                    std::cout << "ElementArray:comment:before: " << "\"" << current_comment << "\"" << std::endl;
//                } //=========================== работа с комментариями (перед значением)

//                UpdateState(State::eARRAY_VALUE_SEPARATOR);
//                value_format = ValueFormat::VALUE_NOPE;
//            } //======================================= обработка итогового значения

//            break;
//        }
//        case State::eARRAY_VALUE_SEPARATOR: {
//            //пропуск пробелов ====================================================
//            if(CharsInString(current_ch, __SPACES_WITHOUT_SEPARATORS__))
//                break;
//            //=====================================================================
//            if(!CharsInString(current_ch, __SEPARATORS__ "]")) {
//                is_critical_error = true;
//                break;
//            }

//            if(current_ch == '\n') {
//                //работа с комментариями (после значения #1) ==========================
//                if(enable_comment && !current_comment.empty()) {
//                    add_suffix_comment(m_values.size() - 1, FromComment(current_comment, comment_design));
//                    current_comment.clear();
////                    std::cout << "ElementArray:comment:after: " << "\"" << current_comment << "\"" << std::endl;
//                } //===================================================================
//                is_value_comment_after_saved = true;
//            }
//            UpdateState(current_ch == ']' ? State::eARRAY_FINISH : State::eARRAY_VALUE);

//            if(enable_comment && (!value_comment.prefix().empty() || !value_comment.suffix().empty())) {
//                add_comment(m_values.size() - 1, value_comment.prefix(), value_comment.suffix());
////                std::cout << "\tvalue_before: " << value_comment.prefix << std::endl
////                          << "\tvalue_after: " << value_comment.suffix << std::endl;
//            }

//            break;
//        }
//        default: break;
//        }

//        if(is_critical_error) {
////            std::cout << "symbols: '" << previous << current << next << "'" << std::endl;
//            clear();
//            throw std::invalid_argument("JSON_ARRAY parse error at line "
//                                        + std::to_string(line_counter) + ":" + std::to_string(symbol_counter)
//                                        + " '" + current_ch + "', current state:" + ToString(state));
//        }

//        Counter();
//    }


//    if(state != State::eARRAY_FINISH) {
//        clear();
//        throw std::invalid_argument("JSON_ARRAY parse error, end of JSON array structure not found");
//    }
//}

//void ElementArray::parseIniArray(const std::string &string, const bool enable_comment,
//                                  CommentDesign* design)
//{
//    //TODO: ElementArray::parseIniArray()
//    //TODO: std::exception
//}

//std::string ElementArray::toString(const ConfigFormat format, const int8_t tabulation_level) const noexcept {
//    switch(format) {
//    case ConfigFormat::eJSON:   return toJsonString(tabulation_level);
//    case ConfigFormat::eINI:    return toIniString(tabulation_level);
//    default: return "";
//    }
//}

//std::string ElementArray::toJsonString(const int8_t tabulation_level) const noexcept {
//    if(m_values.empty()) return "[]";

//    using namespace utils;
//    bool without_space = tabulation_level == -1;

//    std::string ret = "[";
//    for(auto& it : m_values) {
//        if(!without_space) ret += "\n" + RepeatSymToStr('\t', tabulation_level);
//        ret += it.toString(ConfigFormat::eJSON, tabulation_level+1) + ",";
//    }
//    if(!without_space) ret += "\n" + RepeatSymToStr('\t', tabulation_level);
//    ret += "]";

//    return ret;
//}

//std::string ElementArray::toIniString(const int8_t tabulation_level) const noexcept {
//    //TODO: ElementArray::toIniString()
//    return "";
//}

//std::string ElementArray::toString(const ConfigFormat format, const CommentDesign &design,
//                                    const int8_t tabulation_level) const noexcept {
//    switch(format) {
//    case ConfigFormat::eJSON:   return toJsonString(design, tabulation_level);
//    case ConfigFormat::eINI:    return toIniString(design, tabulation_level);
//    default: return "";
//    }
//}

//std::string ElementArray::toJsonString(const CommentDesign &design, const int8_t tabulation_level) const noexcept {
//    if(m_values.empty()) return "[]";

//    using namespace utils;
//    std::string ret = "[";
//    for(auto it : m_values) {
//        ret += "\n" + RepeatSymToStr('\t', tabulation_level);
//        ret += ToComment(it.getPrefixComment(), design, tabulation_level);
//        ret += "\n" + RepeatSymToStr('\t', tabulation_level);
//        ret += it.toString(ConfigFormat::eJSON, design, tabulation_level + 1) + ", ";
//        //NOTE: суффиксный многострочный комментарий должен начинаться на той же строке, что и значение переменной
//        ret += ToComment(it.getSuffixComment(), design, tabulation_level);
//    }
//    ret += "\n" + RepeatSymToStr('\t', tabulation_level);
//    ret += "]";

//    return ret;
//}

void ElementArray::add_comment(const size_t index, const Comment &content) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index)->addComment(content);
}

void ElementArray::add_comment(const size_t index, const std::string &content_before,
                              const std::string &content_after)
{
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index)->addComment(content_before, content_after);
}

void ElementArray::add_prefix_comment(const size_t index, const std::string &content) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index)->addPrefixComment(content);
}

void ElementArray::add_suffix_comment(const size_t index, const std::string &content) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index)->addSuffixComment(content);
}

Comment &ElementArray::get_comment(const size_t index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    return m_values.at(index)->getComment();
}

Comment ElementArray::get_comment(const size_t index) const {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    return m_values.at(index)->getComment();
}

std::string &ElementArray::get_prefix_comment(const size_t index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    return m_values.at(index)->getPrefixComment();
}

std::string ElementArray::get_prefix_comment(const size_t index) const {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    return m_values.at(index)->getPrefixComment();
}

std::string &ElementArray::get_suffix_comment(const size_t index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    return m_values.at(index)->getSuffixComment();
}

std::string ElementArray::get_suffix_comment(const size_t index) const {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    return m_values.at(index)->getSuffixComment();
}

void ElementArray::clear_comment(const size_t index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index)->clearComment();
}

void ElementArray::clear_prefix_comment(const size_t index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index)->clearPrefixComment();
}

void ElementArray::clear_suffix_comment(const size_t index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index)->clearSuffixComment();
}

void ElementArray::delete_comment(const size_t index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index)->deleteComment();
}

void ElementArray::delete_prefix_comment(const size_t index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index)->deletePrefixComment();
}

void ElementArray::delete_suffix_comment(const size_t index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index)->deleteSuffixComment();
}

void ElementArray::append_if_not_contains(const Config &config) noexcept {
    if(!contains(config))
        m_values.push_back(std::make_shared<Config>(config));
}

void ElementArray::append_if_not_contains(Config &&config) noexcept {
    if(!contains(config))
        m_values.push_back(std::make_shared<Config>(std::move(config)));
}

Config &ElementArray::get_front() {
    __CHECK_ARRAY_EMPTY_EXCEPTION__((*this))
    return *m_values.front();
}

Config ElementArray::get_front() const {
    __CHECK_ARRAY_EMPTY_EXCEPTION__((*this))
    return *m_values.front();
}

Config &ElementArray::get_at(const size_t index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    return *m_values[index];
}

Config ElementArray::get_at(const size_t index) const {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    return *m_values[index];
}

Config &ElementArray::get_back() {
    __CHECK_ARRAY_EMPTY_EXCEPTION__((*this))
    return *m_values.back();
}

Config ElementArray::get_back() const {
    __CHECK_ARRAY_EMPTY_EXCEPTION__((*this))
    return *m_values.back();
}

void ElementArray::clear() noexcept {
    clearComment();
    m_values.clear();
}

void ElementArray::insert_front(const Config &value) noexcept {
    m_values.insert(cbegin(), std::make_shared<Config>(value));
}

void ElementArray::insert_front(Config &&value) noexcept {
    m_values.insert(cbegin(), std::make_shared<Config>(std::move(value)));
}

void ElementArray::insert_at(const size_t index, const Config &value) noexcept {
    if(size() < index - 1) {
        m_values.push_back(std::make_shared<Config>(value));
    } else {
        m_values.insert(cbegin() + (index - 1), std::make_shared<Config>(value));
    }
}

void ElementArray::insert_at(const size_t index, Config &&value) noexcept {
    if(size() < index - 1) {
        m_values.push_back(std::make_shared<Config>(std::move(value)));
    } else {
        m_values.insert(cbegin() + (index - 1), std::make_shared<Config>(std::move(value)));
    }
}

void ElementArray::insert_at(shared_VElement::iterator iterator, const Config &value) {
    m_values.insert(iterator, std::make_shared<Config>(value));
}

void ElementArray::insert_at(shared_VElement::iterator iterator, Config &&value) {
    m_values.insert(iterator, std::make_shared<Config>(std::move(value)));
}

void ElementArray::insert_back(const Config &value) noexcept {
    m_values.push_back(std::make_shared<Config>(value));
}

void ElementArray::insert_back(Config &&value) noexcept {
    m_values.push_back(std::make_shared<Config>(std::move(value)));
}

void ElementArray::insert_front(const VElement &elements) noexcept {
    for(size_t i = 0; i < elements.size(); i++)
        m_values.insert(cbegin() + i, std::make_shared<Config>(elements[i]));
}

void ElementArray::insert_front(VElement &&elements) noexcept {
    for(size_t i = 0; i < elements.size(); i++)
        m_values.insert(cbegin() + i, std::make_shared<Config>(std::move(elements[i])));
}

void ElementArray::insert_at(const size_t index, const VElement &elements) noexcept {
    size_t counter = 0;
    for(const auto &element : elements) {
        insert_at(index + counter, element);
        counter++;
    }
}

void ElementArray::insert_at(const size_t index, VElement &&elements) noexcept {
    size_t counter = 0;
    for(auto &element : elements) {
        insert_at(index + counter, std::move(element));
        counter++;
    }
}

void ElementArray::insert_back(const VElement &elements) noexcept {
    std::transform(elements.cbegin(), elements.cend(), std::back_inserter(m_values),
                   [](const Config& element){ return std::make_shared<Config>(element); });
}

void ElementArray::insert_back(VElement &&elements) noexcept {
    std::transform(std::make_move_iterator(elements.begin()),
                   std::make_move_iterator(elements.end()),
                   std::back_inserter(m_values),
                   [](const Config& element) {
                       return std::make_shared<Config>(std::move(element));
                   });
}

void ElementArray::append_null(size_t size) noexcept {
    for(size_t i = size; i > 0; --i)
        m_values.push_back(std::make_shared<Config>());
}

void ElementArray::pop_front() {
    __CHECK_ARRAY_EMPTY_EXCEPTION__((*this))
    m_values.erase(cbegin());
}

void ElementArray::pop_at(const size_t index) {
    __CHECK_INDEX_BOUND__((*this), index)
    m_values.erase(cbegin() + index);
}

void ElementArray::pop_back() {
    __CHECK_ARRAY_EMPTY_EXCEPTION__((*this))
    m_values.pop_back();
}

Config ElementArray::get_and_pop_front() {
    Config cfg = get_front();
    pop_front();
    return cfg;
}

Config ElementArray::get_and_pop_at(const size_t index) {
    Config cfg = get_at(index);
    pop_at(index);
    return cfg;
}

Config ElementArray::get_and_pop_back() {
    Config cfg = get_back();
    pop_back();
    return cfg;
}

void ElementArray::erase_front() {
    __CHECK_ARRAY_EMPTY_EXCEPTION__((*this))
    m_values.erase(cbegin());
}

void ElementArray::erase_at(const size_t index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.erase(cbegin() + index);
}

void ElementArray::erase_back() {
    __CHECK_ARRAY_EMPTY_EXCEPTION__((*this))
    m_values.pop_back();
}

//комментарии при поиске не учитываются
bool ElementArray::contains(const Config &config) const noexcept {
    return std::any_of(cbegin(), cend(), [&config](const std::shared_ptr<Config> &value){ return value->isEqual(config); });
}

Config &ElementArray::operator[](const size_t index) {
    return *m_values[index];
}

Config ElementArray::operator[](const size_t index) const {
    return *m_values[index];
}

std::string ElementArray::toString(const ConfigFormat format, const int8_t tabulation_level,
                                   const CommentDesign &design) const noexcept
{
    //TODO: std::string ElementArray::toString()
    return "";
}

std::string ElementArray::toJsonString(const int8_t tabulation_level, const CommentDesign &design) const noexcept
{
    //TODO: std::string ElementArray::toJsonString()
    return "";
}

std::string ElementArray::toIniString(const int8_t tabulation_level, const CommentDesign &design) const noexcept
{
    //TODO: std::string ElementArray::toIniString()
    return "";
}

std::string ElementArray::toYamlString(const int8_t tabulation_level, const CommentDesign &design) const noexcept
{
    //TODO: std::string ElementArray::toYamlString()
    return "";
}

std::string ElementArray::toXmlString(const int8_t tabulation_level, const CommentDesign &design) const noexcept
{
    //TODO: std::string ElementArray::toXmlString()
    return "";
}

std::string ElementArray::to_string(const ParseState state) const noexcept {
    switch (state) {
    case ParseState::eARRAY_START:          return "[ARRAY_START]";
    case ParseState::eARRAY_VALUE:          return "[ARRAY_VALUE]";
    case ParseState::eARRAY_SEPARATOR:      return "[ARRAY_SEPARATOR]";
    case ParseState::eARRAY_FINISH:         return "[ARRAY_FINISH]";
    default:
    case ParseState::eARRAY_ERROR_STATE:    return "[ARRAY_ERROR_STATE]";
    }
}

void ElementArray::UpdateState(ParseState &state, const ParseState new_state) const noexcept {
    state = new_state;
    DEBUG_LOG("upd state: " << to_string(state) << std::endl);
}

void ElementArray::SymbolCounter(const char current_ch, size_t &line_counter,
                               size_t &symbol_counter) const noexcept
{
    if(current_ch == '\n') {
        line_counter++;
        symbol_counter = 0; //должен перескочить строго на следующей строке
    } else symbol_counter++;
}

void ElementArray::parse(const std::string &input_string, CommentDesign &design,
                         const ConfigFormat format, bool parse_comments)
{
    parse(std::move(std::string(input_string)), design, format, parse_comments);
}

void ElementArray::parse(const std::string &input_string, const ConfigFormat format,
                         const bool parse_comments)
{
    CommentDesign design;
    parse(input_string, design, format, parse_comments);
}

void ElementArray::parse(std::string &&input_string, CommentDesign &design,
                         const ConfigFormat format, bool parse_comments)
{
    switch(format) {
    default:
    case ConfigFormat::eJSON:   parseJson(std::move(input_string), design, parse_comments); break;
    case ConfigFormat::eINI:    parseIni(std::move(input_string), design, parse_comments);  break;
    case ConfigFormat::eYAML:   parseYaml(std::move(input_string), design, parse_comments); break;
    case ConfigFormat::eXML:    parseXml(std::move(input_string), design, parse_comments);  break;
    }
}

void ElementArray::parse(std::string &&input_string, const ConfigFormat format,
                         const bool parse_comments)
{
    CommentDesign design;
    parse(std::move(input_string), design, format, parse_comments);
}

void ElementArray::parseJson(const std::string &input_string, CommentDesign &design,
                             bool parse_comments)
{
    parseJson(std::move(std::string(input_string)), design, parse_comments);
}

void ElementArray::parseJson(const std::string &input_string, bool parse_comments) {
    CommentDesign design;
    parseXml(input_string, design, parse_comments);
}

void ElementArray::parseJson(std::string &&input_string, CommentDesign &design,
                             bool parse_comments)
{
    using namespace utils;

    /* NOTE: для документации
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

    //первоначальная очистка всех элементов
    clear();
    //пустой документ не является ошибкой синтаксиса
    RemoveIllegalSpaces(input_string);
    if(input_string.empty()) return;

    size_t line_counter   = 0;
    size_t symbol_counter = 0;

    design.temp_type = CommentType::eNotComment;

    ParseState state = ParseState::eARRAY_START;
    std::string comment;
    ValueFormat value_format = ValueFormat::eVALUE_NOPE;
    std::string value;
    bool is_quotes = false;

    for(size_t i = 0; i < input_string.size(); i++) {
        char previous_ch    = i == 0 ? 0 : input_string[i - 1];
        char current_ch     = input_string[i];
        char next_ch        = i < input_string.size() ? input_string[i + 1] : 0;

        //поиск комментариев ===================================================
        const bool ext_flag = !is_quotes
                              && value_format != ValueFormat::eVALUE_ARRAY
                              && value_format != ValueFormat::eVALUE_JSON;
        CheckComments(current_ch, next_ch, i, temp_comment_settings, comment, ext_flag);
        //сюда зайдёт, если внутри комментария либо если встречен конец комментария
        if(temp_comment_settings.type != CommentType::eNotComment) {
            SymbolCounter(current_ch, line_counter, symbol_counter);
            continue;
        } //================================================= поиск комментариев

        switch(state) {
        case ParseState::eARRAY_START: {
            //начальный комментарий будет сохранён после входа в массив
            if(current_ch == '[') {
                UpdateState(state, ParseState::eARRAY_VALUE);
                //начальный комментарий
                if(parse_comments && !comment.empty()) {
                    FromComment(comment, design);
                }
            }

            break;
        }
        case ParseState::eARRAY_VALUE: {
            //(комментарий перед значением ...)
            //значение ...

            break;
        }
        case ParseState::eARRAY_SEPARATOR: {
            //(комментарий после значения, на строке значения ...)
            //разделитель ...

            break;
        }
        case ParseState::eARRAY_FINISH: {
            //закрывающая скобка ...

            break;
        }
        case ParseState::eARRAY_ERROR_STATE: {

            break;
        }
        }
        //конечный комментарий ...
    }
}

void ElementArray::parseJson(std::string &&input_string, bool parse_comments) {
    CommentDesign design;
    parseJson(std::move(input_string), design, parse_comments);
}

void ElementArray::parseIni(const std::string &input_string, CommentDesign &design,
                            bool parse_comments)
{
    parseIni(std::move(std::string(input_string)), design, parse_comments);
}

void ElementArray::parseIni(const std::string &input_string, bool parse_comments) {
    CommentDesign design;
    parseIni(input_string, design, parse_comments);
}

void ElementArray::parseIni(std::string &&input_string, CommentDesign &design,
                            bool parse_comments)
{
    //TODO: void ElementArray::parseIni()
}

void ElementArray::parseIni(std::string &&input_string, bool parse_comments) {
    CommentDesign design;
    parseIni(std::move(input_string), design, parse_comments);
}

void ElementArray::parseYaml(const std::string &input_string, CommentDesign &design,
                             bool parse_comments)
{
    parseYaml(std::move(std::string(input_string)), design, parse_comments);
}

void ElementArray::parseYaml(const std::string &input_string, bool parse_comments) {
    CommentDesign design;
    parseYaml(input_string, design, parse_comments);
}

void ElementArray::parseYaml(std::string &&input_string, CommentDesign &design,
                             bool parse_comments)
{
    //TODO: void ElementArray::parseYaml()
}

void ElementArray::parseYaml(std::string &&input_string, bool parse_comments) {
    CommentDesign design;
    parseYaml(std::move(input_string), design, parse_comments);
}

void ElementArray::parseXml(const std::string &input_string, CommentDesign &design,
                            bool parse_comments)
{
    parseXml(std::move(std::string(input_string)), design, parse_comments);
}

void ElementArray::parseXml(const std::string &input_string, bool parse_comments) {
    CommentDesign design;
    parseXml(input_string, design, parse_comments);
}

void ElementArray::parseXml(std::string &&input_string, CommentDesign &design,
                            bool parse_comments)
{
    //TODO: void ElementArray::parseXml()
}

void ElementArray::parseXml(std::string &&input_string, bool parse_comments) {
    CommentDesign design;
    parseXml(std::move(input_string), design, parse_comments);
}
