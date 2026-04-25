#include "ElementArray.h"

#include <algorithm>
#include <stdexcept>
#include "../utils/Utils.h"
#include "../utils/ParserSymbolCounter.h"

//предобъявление
#include "Config.h"
#include "ConfigDefines.h"
#include "ElementArray.h"


namespace simpleapi {
namespace tools {

class ElementArray;

ElementArray::ElementArray(const std::string &input_string, const ConfigFormat config_format,
                           const CommentDesign& design, std::string *error_str) noexcept
{
    ElementArray temp;
    CommentDesign n_design = design;
    temp.init();

    std::string error = temp.parse(input_string, n_design, config_format);
    if(error_str != nullptr)
        *error_str = error;

    *this = std::move(temp);
}

void ElementArray::set_comment(const size_t& index, const Comment &content) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index)->setComment(content);
}

void ElementArray::set_comment(const size_t& index, const std::string &content_before,
                               const std::string &content_after)
{
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index)->setComment(content_before, content_after);
}

void ElementArray::set_prefix_comment(const size_t& index, const std::string &content) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index)->setPrefixComment(content);
}

void ElementArray::set_suffix_comment(const size_t& index, const std::string &content) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index)->setSuffixComment(content);
}

Comment &ElementArray::get_comment(const size_t& index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    return m_values.at(index)->getComment();
}

Comment ElementArray::get_comment(const size_t& index) const {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    return m_values.at(index)->getComment();
}

std::string ElementArray::get_prefix_comment(const size_t& index) const {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    return m_values.at(index)->getPrefixComment();
}

std::string ElementArray::get_suffix_comment(const size_t& index) const {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    return m_values.at(index)->getSuffixComment();
}

//std::string &ElementArray::get_prefix_comment(const size_t& index) {
//    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
//    return m_values.at(index)->getPrefixComment();
//}

//std::string &ElementArray::get_suffix_comment(const size_t& index) {
//    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
//    return m_values.at(index)->getSuffixComment();
//}

void ElementArray::clear_comment(const size_t& index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index)->clearComment();
}

void ElementArray::clear_prefix_comment(const size_t& index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index)->clearPrefixComment();
}

void ElementArray::clear_suffix_comment(const size_t& index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index)->clearSuffixComment();
}

void ElementArray::delete_comment(const size_t& index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index)->deleteComment();
}

void ElementArray::delete_prefix_comment(const size_t& index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index)->deletePrefixComment();
}

void ElementArray::delete_suffix_comment(const size_t& index) {
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

Config &ElementArray::get_at(const size_t& index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    return *m_values[index];
}

Config ElementArray::get_at(const size_t& index) const {
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

void ElementArray::insert_front(const VElement &elements) noexcept {
    for(size_t i = 0; i < elements.size(); i++)
        m_values.insert(cbegin() + i, std::make_shared<Config>(elements[i]));
}

void ElementArray::insert_front(VElement &&elements) noexcept {
    for(size_t i = 0; i < elements.size(); i++)
        m_values.insert(cbegin() + i, std::make_shared<Config>(std::move(elements[i])));
}

//если индекс больше существующего - добавить в конец массива
void ElementArray::insert_at(const size_t& index, const VElement &elements) noexcept {
    VElement temp_elements = elements;
    insert_at(index, std::move(temp_elements));
}

//если индекс больше существующего - добавить в конец массива
void ElementArray::insert_at(const size_t& index, VElement &&elements) noexcept {
    size_t counter = 0;
    for(auto &element : elements) {
        if(index+counter < size())
            m_values.insert(m_values.begin() + index + counter, std::make_shared<Config>(std::move(element)));
        else
            m_values.push_back(std::make_shared<Config>(std::move(element)));
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

shared_VElement::iterator ElementArray::insert_at(const shared_VElement::iterator iterator, const Config &value) {
    Config temp = value;
    return insert_at(iterator, std::move(temp));
}

shared_VElement::iterator ElementArray::insert_at(const shared_VElement::iterator iterator, Config &&value) {
    if(iterator >= m_values.begin() && iterator < m_values.end())
        return m_values.insert(iterator, std::make_shared<Config>(std::move(value)));
    else
        return m_values.insert(m_values.end(), std::make_shared<Config>(std::move(value)));
}

void ElementArray::append(const ElementArray &other) noexcept {
    ElementArray array_copy(other);
    append(std::move(array_copy));
}

void ElementArray::append(ElementArray &&other) noexcept {
    for(auto& o : other.m_values)
        insert_back(std::move(*o.get()));
}

void ElementArray::append_null(size_t size) noexcept {
    for(size_t i = size; i > 0; --i)
        m_values.push_back(std::make_shared<Config>());
}

void ElementArray::pop_front() noexcept {
    if(!m_values.empty())
        m_values.erase(cbegin());
}

void ElementArray::pop_at(const size_t& index) noexcept {
    if(m_values.size() > index)
        m_values.erase(cbegin() + index);
}

void ElementArray::pop_back() noexcept {
    if(!m_values.empty())
        m_values.pop_back();
}

Config ElementArray::get_and_pop_front() {
    Config cfg = get_front();
    pop_front();
    return cfg;
}

Config ElementArray::get_and_pop_at(const size_t& index) {
    Config cfg = get_at(index);
    pop_at(index);
    return cfg;
}

Config ElementArray::get_and_pop_back() {
    Config cfg = get_back();
    pop_back();
    return cfg;
}

shared_VElement::iterator ElementArray::pop_at(const shared_VElement::iterator iterator)
{
    if(iterator >= m_values.begin() && iterator < m_values.end())
        return m_values.erase(iterator);
    else
        return m_values.end();
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
            if(!m_values[i]->isEqual(*other_array.m_values[i], compare_comments, map_sort_important)) {
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

Config ElementArray::operator[](const size_t &index) const    {
    return get_at(index);
}

bool ElementArray::operator==(const Config &config) const
{
    if(!config.isArray())
        return false;

    if(size() != config.size())
        return false;

    for(size_t i = 0; i < size(); i++) {
        if((*this)[i] != config[i])
            return false;
    }

    return true;
}

std::string ElementArray::toString(const ConfigFormat format, const CommentDesign &design,
                                   const int8_t custom_tabulation_level) const noexcept
{
    switch(format){
    //предполагается, что eONLY_VALUE - это JSON без пробелов и переносов строки
    case ConfigFormat::eONLY_VALUE: return toJsonString(design, -1);
    case ConfigFormat::eJSON:       return toJsonString(design, custom_tabulation_level);
    case ConfigFormat::eINI:        return toIniString(design, custom_tabulation_level);
    case ConfigFormat::eYAML:       return toYamlString(design, custom_tabulation_level);
    case ConfigFormat::eXML:        return toXmlString(design, custom_tabulation_level);
    default:                        return "[UNKNOWN_CONFIG_FORMAT]";
    }
}

std::string ElementArray::toJsonString(const CommentDesign &design, const int8_t custom_tabulation_level) const noexcept
{
    std::string ret;
    const std::string tablulation_str   = utils::RepeatSymToStr('\t', custom_tabulation_level);
    const std::string tablulation_str_1 = utils::RepeatSymToStr('\t', custom_tabulation_level + 1);

    //(для отладки)
    //ret += "custom_tabulation_level:" + std::to_string(custom_tabulation_level) + "\n";

    bool with_spaces = custom_tabulation_level != -1;
    CommentDesign inner_design = design;
    inner_design.is_in_container = true;

    if(with_spaces)
        ret += tablulation_str;
    ret += "[";
    if(with_spaces)
        ret += "\n";

    for(size_t i = 0; i < size(); i++)
    {
        //вывод комментария с рамкой
        if(with_spaces
            && design.with_comments
            && !m_values[i]->getPrefixComment().empty())
        {
            //вернули в исходное состояние после записи КОММЕНТАРИЯ ПОСЛЕ ЗНАЧЕНИЯ
            inner_design.opt_multiline_column_size = design.opt_multiline_column_size;

            if(m_values[i]->getPrefixComment().find('\n' != std::string::npos))
                ret += "\n";

            ret += ToComment(m_values[i]->getPrefixComment(), inner_design, custom_tabulation_level + 1);
            ret += "\n";
        }

        //вывод значения
        if(with_spaces)
            ret += tablulation_str_1;

        std::string temp = m_values[i]->toString(ConfigFormat::eJSON, inner_design,
                                                 (with_spaces ? custom_tabulation_level + 1 : -1));
        if(m_values[i]->isContainer()) {
            temp = utils::RemoveStartTabulations(temp);
        }

        ret += temp;
        if(i < size() - 1)
            ret += ",";

        //вывод комментария без рамки
        if(!m_values[i]->isContainer()) { //NOTE: контейнеры сами себя описывают
            if(with_spaces
                && inner_design.with_comments
                && !m_values[i]->getSuffixComment().empty())
            {
                //NOTE: (ширина колонки многосторчного комментария после значения не влияет на вывод)
                inner_design.opt_multiline_column_size = 0;

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
    if(with_spaces
        && design.with_comments
        && !design.is_in_container
        && !getSuffixComment().empty())
    {
        //NOTE: (ширина колонки многосторчного комментария после значения не влияет на вывод)
        inner_design.opt_multiline_column_size = 0;

        std::string temp = ToComment(getSuffixComment(), inner_design);
        const size_t pos = ret.rfind('\n');
        std::string temp__ = (ret.size() > pos +1 ? (ret.substr(pos + 1, ret.size())) : "") + " ";
        utils::SetStringAsOnlySpaces(temp__);
        utils::AddStringForFromLine(temp, 2, temp__);
        ret += " " + temp;
    }

    return ret;
}

std::string ElementArray::toIniString(const CommentDesign &design, const int8_t custom_tabulation_level) const noexcept
{
    return ""; //скорее всего не стоит здесь формировать строку, только в Json-парсере
}

std::string ElementArray::toYamlString(const CommentDesign &design, const int8_t custom_tabulation_level) const noexcept
{
    //TODO (потом): std::string ElementArray::toYamlString()
    return "[TODO]";
}

std::string ElementArray::toXmlString(const CommentDesign &design, const int8_t custom_tabulation_level) const noexcept
{
    //TODO (потом): std::string ElementArray::toXmlString()
    return "[TODO]";
}

std::string ElementArray::to_string(const ParseState state) const noexcept {
    switch (state) {
    case ParseState::eARRAY_START:          return "[ARRAY_START]";
    case ParseState::eARRAY_VALUE:          return "[ARRAY_VALUE]";
    case ParseState::eARRAY_SEPARATOR:      return "[ARRAY_SEPARATOR]";
    case ParseState::eARRAY_FINISH:         return "[ARRAY_FINISH]";
    case ParseState::eARRAY_ERROR_STATE:
    default:                                return "[ARRAY_ERROR_STATE]";
    }
}

void ElementArray::UpdateState(ParseState &state, const ParseState new_state) const noexcept {
    state = new_state;
    DEBUG_LOG("Parse Array, upd state: " << to_string(state) << std::endl);
}

std::string ElementArray::parse(const std::string &input_string, CommentDesign &design,
                         const ConfigFormat format, const int8_t tabulation_level) noexcept
{
    return parse(std::move(std::string(input_string)), design, format);
}

std::string ElementArray::parse(std::string &&input_string, CommentDesign &design,
                         const ConfigFormat format, const int8_t tabulation_level) noexcept
{
    switch(format) {
    case ConfigFormat::eJSON:   return parseJson(std::move(input_string), design);
    case ConfigFormat::eYAML:   return parseYaml(std::move(input_string), design, tabulation_level);
    case ConfigFormat::eXML:    return parseXml(std::move(input_string), design);
    default:                    return "unexpected format";
    }
}

std::string ElementArray::parseJson(const std::string &input_string, CommentDesign &design) noexcept
{
    return parseJson(std::move(std::string(input_string)), design);
}

// @TEST(JSON, parse)
std::string ElementArray::parseJson(std::string &&input_string, CommentDesign &design) noexcept
{
    using namespace utils;

    /* NOTE: для документации
     * комментарий массива
     * начало массива (формат JSON)
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
    if(input_string.empty()) return {};

    design.temp_type = CommentType::eNotComment;

    ParseState state                = ParseState::eARRAY_START;
    std::string key                 = "";
    std::string value               = "";
    std::string error_string        = "";
    bool is_quotes                  = false;
    char last_separator_symbol      = '\n';
    uint16_t inner_json_counter     = 0;
    uint16_t inner_array_counter    = 0;
    ParserSymbolCounter counter;
    ParserSymbolCounter start_value_counter; //для счётчика внутри значения

    std::string current_comment     = ""; // текущее значение при парсинге
    VString comments;                     // обработанные комментарии
    size_t value_read_at_line       = 0;

    auto AppendMainPreviewComment = [&]() {
        if(!comments.empty() && (design.temp_type == CommentType::eCommentEnd || design.temp_type == CommentType::eNotComment)) {
            setPrefixComment(VStringToString(comments));
            DEBUG_LOG("ElementArray: PreviewComment: " << "\"" << getPrefixComment() << "\"");
            comments.clear();
        }
    };
    auto AppendMainSuffixComment = [&]() {
        if(!comments.empty() && (design.temp_type == CommentType::eCommentEnd || design.temp_type == CommentType::eNotComment)) {
            setSuffixComment(VStringToString(comments));
            DEBUG_LOG("ElementArray: SuffixComment: " << "\"" << getSuffixComment() << "\"");
            comments.clear();
        }
    };

    /* Логика работы комментариев:
     * - комментарий после значения применяется только при начале на той же строке,
     * что и разделитель этого значения
     * - все остальные комментарии добавляются перед следующим значением
     */
    auto AppendElementPrefixComment = [&](){
        if(!comments.empty()
            && !empty()
            && (design.temp_type == CommentType::eCommentEnd || design.temp_type == CommentType::eNotComment))
        {
            get_back().setPrefixComment(VStringToString(comments));
            DEBUG_LOG("ElementArray: inner Element add PreviewComment: " << "\"" << get_back().getPrefixComment() << "\"");
            comments.clear();
        }
    };
    auto AppendElementSuffixComment = [&](const bool for_penultimate = false){
        if(!comments.empty()
            && size() > (for_penultimate ? 2 : 1)
            && (design.temp_type == CommentType::eCommentEnd || design.temp_type == CommentType::eNotComment))
        {
            if(for_penultimate) {
                if(get_at(size() - 2).getSuffixComment().empty())
                {
                    get_at(size() - 2).setSuffixComment(comments[0]);
                    DEBUG_LOG("ElementArray: inner Element(penultimate) add SuffixComment: " << "\""
                              << get_at(size() - 2).getSuffixComment() << "\"");
                }
                comments.erase(comments.cbegin());
            } else {
                if(get_back().getSuffixComment().empty())
                {
                    get_back().setSuffixComment(comments[0]);
                    DEBUG_LOG("ElementArray: inner Element(back) add SuffixComment: " << "\""
                              << get_back().getSuffixComment() << "\"");
                }
                comments.pop_back();
            }

        }
    };

    for(size_t i = 0; i < input_string.size() && error_string.empty(); i++) {
        char ch_previous    = i == 0 ? 0 : input_string[i - 1];
        char ch_current     = input_string[i];
        char ch_next        = i < input_string.size() ? input_string[i + 1] : 0;

        counter.check(i, ch_current); //TODO: написать тест для проверки счётчика символов

        //поиск комментариев ===================================================
        const bool ext_flag = !is_quotes && (inner_array_counter + inner_json_counter == 0);
        //вернёт комментарий без обрамления
        CheckComments(ch_current, ch_next, i, design, current_comment, ext_flag);
        if(!design.with_comments)
            current_comment.clear();
        if(design.with_comments && design.temp_type == CommentType::eCommentEnd)
        {
            comments.push_back(FromComment(current_comment, design));
            current_comment.clear();
            design.temp_type = CommentType::eNotComment;
            continue;
        }
        if(design.temp_type != CommentType::eNotComment)
            continue;
        //=================================================== поиск комментариев

        // обработка комментария ПОСЛЕ значения
        if(utils::CharInString(ch_current, __SEPARATORS__)) {
            if(state == ParseState::eARRAY_VALUE
                && key.empty()
                && !comments.empty()
                && (last_separator_symbol == ','
                    || (ch_current == '\n' && last_separator_symbol == ',' && comments[0].find('\n') == std::string::npos)))
            {
                std::cout << "last_separator_symbol: '"
                          << (last_separator_symbol == '\n' ? "\\n" : std::string(&last_separator_symbol, 1))
                          << "'" << std::endl;
                AppendElementSuffixComment();
            }
            /* таким образом запоминаются последние два разделителя:
             * - last_separator_symbol  - предыдущий
             * - ch_current             - текущий
             */
            last_separator_symbol = ch_current;
        }

        switch(state) {
        case ParseState::eARRAY_START: {
            //пропуск пробелов =====================================================
            if(CharInString(ch_current, __SPACES__)) break;
            //===================================================== пропуск пробелов

            if(ch_current == '[') {
                // работа с комментариями (до разбора json)
                AppendMainPreviewComment();

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
                && CharInString(ch_current, __SPACES__ ",")) // пустое значение между запятыми - не ошибка
            {
                break;
            }
            if(ch_current == ']') {
                // работа с комментарием после элемента
                if(get_back().getSuffixComment().empty())
                    AppendElementSuffixComment();

                UpdateState(state, ParseState::eARRAY_FINISH);
                break;
            }

            //если ключ в кавычках, то ждём кавычки, иначе любой __SPACES__
            if(ch_current == '\"'
                && ch_previous != '\\'
                && inner_json_counter + inner_array_counter == 0)
            {
                is_quotes = !is_quotes;
            }

            //кавычки, именованные списки, массивы
            if(!is_quotes) {
                switch(ch_current) {
                case '{':   { ++inner_json_counter;     break; }
                case '}':   { --inner_json_counter;     break; }
                case '[':   { ++inner_array_counter;    break; }
                case ']':   { --inner_array_counter;    break; }
                default: break;
                }
            }
            value += ch_current;

            //если значение пустое, есть только разделитель - запятая
            if(!is_quotes
                && inner_json_counter + inner_array_counter == 0
                && ch_current == ',')
            {
                value.clear();
                i--;
            }

            //значение прочитано полностью?
            if(!is_quotes
                && inner_json_counter + inner_array_counter == 0
                && (CharInString(ch_next, __SEPARATORS__ " ]")
                    || CharInString(ch_current, __SEPARATORS__ "]")))
            {
                DEBUG_LOG("ElementArray: current value done: \"" << value << "\"");
                if(value.empty()) {
                    DEBUG_LOG("ElementArray: found empty value, skip...");
                    UpdateState(state, ParseState::eARRAY_SEPARATOR);
                    break;
                }

                Config element = CreateElementFromString(std::move(value), ConfigFormat::eJSON, design, start_value_counter);
                if(element.error()) {
                    //если случилась ошибка при внутренней конвертации прочитанного значения,
                    // то эта ошибка становится основной ошибкой парсинга
                    error_string = "ElementArray value parse error[" + std::to_string(counter.getLastLineCounter())
                                   + "][" + std::to_string(counter.getLastSymbolCounter()) + "]: " + element.getError();
                    UpdateState(state, ParseState::eARRAY_ERROR_STATE);
                    break;
                }

                push_back(std::move(element));
                if(get_back().getCommentDesign().opt_multiline_column_size > design.opt_multiline_column_size)
                    design.opt_multiline_column_size = get_back().getCommentDesign().opt_multiline_column_size;
                key.clear();
                value.clear();

                // проверка замыкающего комментария (вторичная)
                if(value_read_at_line == counter.getLastLineCounter())
                {
                    if(!comments.empty()
                        && size() > 1
                        && get_at(size() - 2).getSuffixComment().empty()
                        && (design.temp_type == CommentType::eCommentEnd || design.temp_type == CommentType::eNotComment))
                    {
                        get_at(size() - 2).setSuffixComment(comments[0]);
                        DEBUG_LOG("ElementJson: inner Element add SuffixComment: " << "\"" << get_at(size() - 2).getSuffixComment() << "\"");
                        comments.erase(comments.cbegin());
                    }
                }

                // проверка замыкающего комментария (вторичная)
                if(value_read_at_line == counter.getLastLineCounter())
                    AppendElementSuffixComment(true);

                // работа с комментариями перед элементом
                AppendElementPrefixComment();

                UpdateState(state, ParseState::eARRAY_SEPARATOR);
            }

            break;
        }
        case ParseState::eARRAY_SEPARATOR: {
            //пропуск пробелов =====================================================
            if(CharInString(ch_current, __SPACES_WITHOUT_SEPARATORS__)) break;
            //===================================================== пропуск пробелов

            //может встретиться разделитель или знак завершения массива
            //  комментарий ПОСЛЕ значения может начинаться только на той же строке, что и значение
            //  разделитель может быть как ДО, так и ПОСЛЕ комментария
            //  если комментарий расписан после переноса строки, но до знака }, то комментарий попадёт в суффикс основы

            // запоминаем номер строки, на котором закончили считывать значение
            value_read_at_line = counter.getLastLineCounter(); //применится перед } и перед считыванием значения
            value.clear();

            if(CharInString(ch_current, __SEPARATORS__)) {
                i--;
                UpdateState(state, ParseState::eARRAY_VALUE);
                break;
            }
            if(CharInString(ch_current, "]")) {
                i--;
                UpdateState(state, ParseState::eARRAY_FINISH);
                break;
            }

            UpdateState(state, ParseState::eARRAY_ERROR_STATE);
            error_string = "Not found stop of ARRAY.";
            break;
        }
        default: break;
        }
    }

    //конечный комментарий ...
    if(state == ParseState::eARRAY_FINISH)
    {
        AppendMainSuffixComment();
    }

    setCommentDesign(design);
    if(state != ParseState::eARRAY_FINISH && state != ParseState::eARRAY_VALUE) {
        error_string = std::string("JSON parse error, unexpected symbol at [")
                       + std::to_string(counter.getLastLineCounter())
                       + "][" + std::to_string(counter.getLastSymbolCounter()) + "]: '"
                       + input_string[counter.getLastIterator()] + "'. "
                       + error_string;
        DEBUG_LOG("ERROR: " << error_string);
        //NOTE: (ElementArray) в случае ошибки парсинга корректно прочитанные значения сохраняются
        //clear();
    }

    return error_string;
}

std::string ElementArray::parseYaml(const std::string &input_string, CommentDesign &design,
                             const int8_t tabulation_level) noexcept
{
    return parseYaml(std::move(std::string(input_string)), design);
}

std::string ElementArray::parseYaml(std::string &&input_string, CommentDesign &design,
                             const int8_t tabulation_level) noexcept
{
    //TODO (потом): void ElementArray::parseYaml()
    return "error";
}

std::string ElementArray::parseXml(const std::string &input_string, CommentDesign &design) noexcept
{
    return parseXml(std::move(std::string(input_string)), design);
}

std::string ElementArray::parseXml(std::string &&input_string, CommentDesign &design) noexcept
{
    //TODO (потом): void ElementArray::parseXml()
    return "error";
}

bool IsElementArray(const IElement &e) noexcept
{
    return e.getType() == ValueType::eArray;
}

bool IsElementArray(const Config &cfg) noexcept
{
    return cfg.isArray();
}

} // namespace tools
} // namespace simpleapi
