#include "ElementArray.h"

#include <algorithm>
#include <stdexcept>
#include "../utils/Utils.h"

//предобъявление
#include "Config.h"
#include "ElementArray.h"

class ElementArray;


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

void ElementArray::append(const ElementArray &other) noexcept {
    ElementArray array_copy(other);
    append(std::move(array_copy));
}

void ElementArray::append(ElementArray &&other) noexcept {
    for(auto& o : other)
        insert_back(std::move(o));
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

bool ElementArray::isEqual(const IElement &other, const bool compare_comments,
                           const bool map_sort_important) const noexcept
{
    bool b1 = true;
    if(compare_comments)
        b1 = isCommentsEqual(other);

    bool b2 = size() == other.size();
    if(b2) {
        const ElementArray& other_array = reinterpret_cast<const ElementArray&>(other);
        for(size_t i = 0; i < size(); i++) {
            if(m_values[i]->isEqual(*other_array.m_values[i], compare_comments, map_sort_important)) {
                b2 = false;
                break;
            }
        }
    }

    return b1 && b2;
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

//TODO: std::string ElementArray::toString()
std::string ElementArray::toString(const ConfigFormat format, const CommentDesign &design,
                                   const int8_t custom_tabulation_level) const noexcept
{
    std::string ret;
    const std::string tablulation_str   = utils::RepeatSymToStr('\t', custom_tabulation_level);
    const std::string tablulation_str_1 = utils::RepeatSymToStr('\t', custom_tabulation_level + 1);
    //    ret += "custom_tabulation_level:" + std::to_string(custom_tabulation_level) + "\n";

    bool with_spaces = format != ConfigFormat::eONLY_VALUE && custom_tabulation_level != -1;
    CommentDesign inner_design = design;
    inner_design.is_in_container = true;

    switch(format){
    case ConfigFormat::eONLY_VALUE:
    case ConfigFormat::eJSON:
    {
        if(with_spaces)
            ret += tablulation_str;
        ret += "[";
        if(with_spaces)
            ret += "\n";

        for(size_t i = 0; i < size(); i++) {
            //вывод комментария с рамкой
            if(design.with_comments && !design.is_in_container && !m_values[i]->getPrefixComment().empty()) {
                ret += ToComment(m_values[i]->getPrefixComment(), inner_design, custom_tabulation_level + 1);
                ret += "\n";
            }

            //вывод значения
            if(with_spaces)
                ret += tablulation_str_1;

            std::string temp = m_values[i]->toString(format, inner_design, (custom_tabulation_level == -1 ? -1 : custom_tabulation_level + 1));
            if(m_values[i]->isContainer()) {
                temp = utils::RemoveStartTabulations(temp);
            }
            if(m_values[i]->isString() && format == ConfigFormat::eONLY_VALUE) {
                temp = "\"" + temp + "\"";
            }

            ret += temp;
            if(i < size() - 1)
                ret += ",";

            //вывод комментария без рамки
            if(!m_values[i]->isContainer()) { //NOTE: контейнеры сами себя описывают
                if(inner_design.with_comments && !m_values[i]->getSuffixComment().empty()) {
                    std::string temp = ToComment(m_values[i]->getSuffixComment(), inner_design, -1);
                    const size_t pos = ret.rfind('\n');
                    std::string temp__ = (ret.size() > pos +1 ? (ret.substr(pos + 1, ret.size())) : "") + " ";
                    utils::SetStringAsOnlySpaces(temp__);
                    utils::AddStringForFromLine(temp, 2, temp__);
                    ret += " " + temp;
                }
            }

            if(with_spaces)
                ret += "\n";
        }

        if(with_spaces)
            ret += tablulation_str;
        ret += "]";

        //вывод комментария без рамки
        if(design.with_comments && !design.is_in_container && !getSuffixComment().empty()) {
            std::string temp = ToComment(getSuffixComment(), design);
            const size_t pos = ret.rfind('\n');
            std::string temp__ = (ret.size() > pos +1 ? (ret.substr(pos + 1, ret.size())) : "") + " ";
            utils::SetStringAsOnlySpaces(temp__);
            utils::AddStringForFromLine(temp, 2, temp__);
            ret += " " + temp;
        }

        break;
    }
    case ConfigFormat::eINI:
    case ConfigFormat::eYAML:
    case ConfigFormat::eXML:
        break;
    }

    return ret;
}

std::string ElementArray::toJsonString(const CommentDesign &design, const int8_t custom_tabulation_level) const noexcept
{
    //TODO: std::string ElementArray::toJsonString()
    return "[TODO]";
}

std::string ElementArray::toIniString(const CommentDesign &design, const int8_t custom_tabulation_level) const noexcept
{
    //TODO: std::string ElementArray::toIniString()
    return "[TODO]";
}

std::string ElementArray::toYamlString(const CommentDesign &design, const int8_t custom_tabulation_level) const noexcept
{
    //TODO: std::string ElementArray::toYamlString()
    return "[TODO]";
}

std::string ElementArray::toXmlString(const CommentDesign &design, const int8_t custom_tabulation_level) const noexcept
{
    //TODO: std::string ElementArray::toXmlString()
    return "[TODO]";
}

std::string ElementArray::to_string(const ParseState state) const noexcept {
    switch (state) {
    case ParseState::eARRAY_START:          return "[ARRAY_START]";
    case ParseState::eARRAY_VALUE:          return "[ARRAY_VALUE]";
    case ParseState::eARRAY_SEPARATOR:      return "[ARRAY_SEPARATOR]";
    case ParseState::eARRAY_COMMENT:        return "[ARRAY_COMMENT]";
    case ParseState::eARRAY_FINISH:         return "[ARRAY_FINISH]";
    case ParseState::eARRAY_ERROR_STATE:
    default:                                return "[ARRAY_ERROR_STATE]";
    }
}

void ElementArray::UpdateState(ParseState &state, const ParseState new_state) const noexcept {
    state = new_state;
    DEBUG_LOG("Parse Array, upd state: " << to_string(state) << std::endl);
}

void ElementArray::parse(const std::string &input_string, CommentDesign &design,
                         const ConfigFormat format, const int8_t tabulation_level)
{
    parse(std::move(std::string(input_string)), design, format);
}

void ElementArray::parse(const std::string &input_string, const ConfigFormat format,
                         const bool parse_comments, const int8_t tabulation_level)
{
    CommentDesign design;
    design.with_comments = parse_comments;
    parse(input_string, design, format);
}

void ElementArray::parse(std::string &&input_string, CommentDesign &design,
                         const ConfigFormat format, const int8_t tabulation_level)
{
    switch(format) {
    default:
    case ConfigFormat::eJSON:   parseJson(std::move(input_string), design); break;
    case ConfigFormat::eINI:    parseIni(std::move(input_string), design);  break;
    case ConfigFormat::eYAML:   parseYaml(std::move(input_string), design); break;
    case ConfigFormat::eXML:    parseXml(std::move(input_string), design);  break;
    }
}

void ElementArray::parse(std::string &&input_string, const ConfigFormat format,
                         const bool parse_comments, const int8_t tabulation_level)
{
    CommentDesign design;
    design.with_comments = parse_comments;
    parse(std::move(input_string), design, format);
}

void ElementArray::parseJson(const std::string &input_string, CommentDesign &design,
                             const int8_t tabulation_level)
{
    parseJson(std::move(std::string(input_string)), design);
}

void ElementArray::parseJson(const std::string &input_string, const bool parse_comments, const int8_t tabulation_level) {
    CommentDesign design;
    design.with_comments = parse_comments;
    parseXml(input_string, design);
}

// @TEST(JSON, parse)
void ElementArray::parseJson(std::string &&input_string, CommentDesign &design,
                             const int8_t tabulation_level)
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

    ParseState state                = ParseState::eARRAY_START;
    ParseState state_comment        = ParseState::eARRAY_ERROR_STATE;
    std::string comment             = "";
    std::string value               = "";
    std::string error_string        = "";
    bool is_quotes                  = false;
    bool is_separator_comma         = false;
    uint16_t inner_json_counter     = 0;
    uint16_t inner_array_counter    = 0;

    for(size_t i = 0; i < input_string.size(); i++) {
        char previous_ch    = i == 0 ? 0 : input_string[i - 1];
        char current_ch     = input_string[i];
        char next_ch        = i < input_string.size() ? input_string[i + 1] : 0;
        SymbolCounter(current_ch, line_counter, symbol_counter);

        //поиск комментариев ===================================================
        const bool ext_flag = !is_quotes;
        // вернёт комментарий без обрамления
        CheckComments(current_ch, next_ch, i, design, comment, ext_flag);
        if(!design.with_comments)
            comment.clear();
        //сюда зайдёт, если внутри комментария либо если встречен конец комментария
        if(design.temp_type != CommentType::eNotComment)
            continue;
        //=================================================== поиск комментариев


        switch(state) {
        case ParseState::eARRAY_START: {
            //пропуск пробелов =====================================================
            if(CharInString(current_ch, __SPACES__)) break;
            //===================================================== пропуск пробелов

            if(current_ch == '[') {
                //работа с комментариями (до разбора массива) ==========================
                if(!comment.empty() && (design.temp_type == CommentType::eCommentEnd || design.temp_type == CommentType::eNotComment)) {
                    addPrefixComment(FromComment(comment, design, tabulation_level));
                    DEBUG_LOG("ElementArray: PreviewComment: " << "\"" << comment << "\"");
                    comment.clear();
                }
                //========================== работа с комментариями (до разбора массива)
                UpdateState(state, ParseState::eARRAY_VALUE);
                break;
            }

            UpdateState(state, ParseState::eARRAY_ERROR_STATE);
            error_string = "Not found start of ARRAY.";
            break;
        }
        case ParseState::eARRAY_VALUE: {
            //игнор пробелов и разделителей пока значение пустое
            if(!is_quotes
                && value.empty()
                && CharInString(current_ch, __SPACES__ ","))
            {
                break;
            }
            if(current_ch == ']') {
                UpdateState(state, ParseState::eARRAY_FINISH);
                break;
            }

            if(current_ch == '\"'
                && previous_ch != '\\'
                && inner_json_counter + inner_array_counter == 0)
            {
                is_quotes = !is_quotes;
            }

            //кавычки, именованные списки, массивы
            if(!is_quotes) {
                switch(current_ch) {
                case '{':   { ++inner_json_counter;     break; }
                case '}':   { --inner_json_counter;     break; }
                case '[':   { ++inner_array_counter;    break; }
                case ']':   { --inner_array_counter;    break; }
                default: break;
                }
            }
            value += current_ch;

            //если значение пустое, есть только разделитель - запятая
            if(!is_quotes
                && inner_json_counter + inner_array_counter == 0
                && current_ch == ',')
            {
                value.clear();
                i--;
            }

            //значение прочитано полностью?
            if(!is_quotes
                && inner_json_counter + inner_array_counter == 0
                && (CharInString(next_ch, __SEPARATORS__ " ]")
                    || CharInString(current_ch, __SEPARATORS__ "]")))
            {
                DEBUG_LOG("ElementArray: current value done: \"" << value << "\"");
                try {
                    Config element = Config::CreateElementFromString(std::move(value), ConfigFormat::eJSON, design, tabulation_level);
                    push_back(std::move(element));
                } catch (std::exception& e) {
                    error_string = e.what();
                    UpdateState(state, ParseState::eARRAY_ERROR_STATE);
                    break;
                }

                if(!comment.empty() && (design.temp_type == CommentType::eCommentEnd || design.temp_type == CommentType::eNotComment)) {
                    get_back().addPrefixComment(FromComment(comment, design, tabulation_level));
                    DEBUG_LOG("ElementArray: inner Element add PreviewComment: " << "\"" << comment << "\"");
                    comment.clear();
                }
                UpdateState(state, ParseState::eARRAY_SEPARATOR);
            }

            break;
        }
        case ParseState::eARRAY_SEPARATOR: {
            //пропуск пробелов =====================================================
            if(CharInString(current_ch, __SPACES_WITHOUT_SEPARATORS__)) break;
            //===================================================== пропуск пробелов

            //может встретиться разделитель или знак завершения массива
            //  комментарий ПОСЛЕ значения может начинаться только на той же строке, что и значение
            //  разделитель может быть как ДО, так и ПОСЛЕ комментария
            //  если комментарий расписан после переноса строки, но до знака ], то комментарий попадёт в суффикс основы

            if(CharInString(current_ch, __SEPARATORS__)) {
                is_separator_comma = current_ch == ',';
                if(CharInString(current_ch, __SPACES__ ","))
                    i--;
                UpdateState(state_comment, ParseState::eARRAY_VALUE);
                UpdateState(state, ParseState::eARRAY_COMMENT);
                break;
            }
            if(CharInString(current_ch, "]")) {
                UpdateState(state_comment, ParseState::eARRAY_FINISH);
                UpdateState(state, ParseState::eARRAY_COMMENT);
                break;
            }

            UpdateState(state, ParseState::eARRAY_ERROR_STATE);
            error_string = "Not found stop of ARRAY.";
            break;
        }
        case ParseState::eARRAY_COMMENT: {
            //пропуск пробелов
            if(CharInString(current_ch, __SPACES_WITHOUT_SEPARATORS__))
                break;

            //(комментарий после значения, на строке значения после запятой)
            if(!is_separator_comma || current_ch == '\n') {
                if(!comment.empty() && (design.temp_type == CommentType::eCommentEnd || design.temp_type == CommentType::eNotComment)) {
                    get_back().addSuffixComment(FromComment(comment, design, tabulation_level));
                    DEBUG_LOG("ElementArray: inner Element add SuffixComment: " << "\"" << comment << "\"");
                    comment.clear();
                }
            }
            UpdateState(state, state_comment);
            break;
        }
        case ParseState::eARRAY_ERROR_STATE: {
            error_string = std::string("Unexpected symbol at [")
                           + std::to_string(line_counter)
                           + "][" + std::to_string(symbol_counter) + "]. "
                           + error_string;
            DEBUG_LOG("ERROR: " << error_string);
            throw std::invalid_argument(error_string);
        }
        default: break;
        }
    }

    //конечный комментарий ...
    if(state == ParseState::eARRAY_COMMENT)
    {
        if(!comment.empty()) {
            addSuffixComment(FromComment(comment, design));
            DEBUG_LOG("ElementArray: SuffixComment: " << "\"" << comment << "\"");
            comment.clear();
        }
        UpdateState(state, state_comment);
    }

    if(state != ParseState::eARRAY_FINISH && state != ParseState::eARRAY_VALUE) {
        clear();
        throw std::invalid_argument("ARRAY parse error, end of JSON array structure not found");
    }
}

void ElementArray::parseJson(std::string &&input_string, const bool parse_comments,
                             const int8_t tabulation_level)
{
    CommentDesign design;
    design.with_comments = parse_comments;
    parseJson(std::move(input_string), design);
}

void ElementArray::parseIni(const std::string &input_string, CommentDesign &design,
                            const int8_t tabulation_level)
{
    parseIni(std::move(std::string(input_string)), design);
}

void ElementArray::parseIni(const std::string &input_string, const bool parse_comments,
                            const int8_t tabulation_level)
{
    CommentDesign design;
    design.with_comments = parse_comments;
    parseIni(input_string, design);
}

void ElementArray::parseIni(std::string &&input_string, CommentDesign &design,
                            const int8_t tabulation_level)
{
    //TODO: void ElementArray::parseIni()
}

void ElementArray::parseIni(std::string &&input_string, const bool parse_comments,
                            const int8_t tabulation_level)
{
    CommentDesign design;
    design.with_comments = parse_comments;
    parseIni(std::move(input_string), design);
}

void ElementArray::parseYaml(const std::string &input_string, CommentDesign &design,
                             const int8_t tabulation_level)
{
    parseYaml(std::move(std::string(input_string)), design);
}

void ElementArray::parseYaml(const std::string &input_string, const bool parse_comments,
                             const int8_t tabulation_level)\
{
    CommentDesign design;
    design.with_comments = parse_comments;
    parseYaml(input_string, design);
}

void ElementArray::parseYaml(std::string &&input_string, CommentDesign &design,
                             const int8_t tabulation_level)
{
    //TODO: void ElementArray::parseYaml()
}

void ElementArray::parseYaml(std::string &&input_string, const bool parse_comments,
                             const int8_t tabulation_level)
{
    CommentDesign design;
    design.with_comments = parse_comments;
    parseYaml(std::move(input_string), design);
}

void ElementArray::parseXml(const std::string &input_string, CommentDesign &design,
                            const int8_t tabulation_level)
{
    parseXml(std::move(std::string(input_string)), design);
}

void ElementArray::parseXml(const std::string &input_string, const bool parse_comments,
                            const int8_t tabulation_level)
{
    CommentDesign design;
    design.with_comments = parse_comments;
    parseXml(input_string, design);
}

void ElementArray::parseXml(std::string &&input_string, CommentDesign &design,
                            const int8_t tabulation_level)
{
    //TODO: void ElementArray::parseXml()
}

void ElementArray::parseXml(std::string &&input_string, const bool parse_comments,
                            const int8_t tabulation_level)
{
    CommentDesign design;
    design.with_comments = parse_comments;
    parseXml(std::move(input_string), design);
}
