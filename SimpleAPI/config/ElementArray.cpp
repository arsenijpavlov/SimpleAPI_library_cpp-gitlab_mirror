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
