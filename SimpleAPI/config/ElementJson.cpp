#include "ElementJson.h"

#include "Config.h"
#include <algorithm>
#include "../utils/Utils.h"
#include "ConfigDefines.h"


namespace simpleapi {
namespace tools {

ElementJson::ElementJson(const ElementJson &other) noexcept {
    init();
    if(other.error()) setError(other.getError());
    m_comment = other.m_comment;
    m_values  = other.m_values;
}

ElementJson::ElementJson(ElementJson &&other) noexcept
{
    init();
    if(other.error()) setError(other.getError());
    m_comment = std::move(other.m_comment);
    m_values  = std::move(other.m_values);
}

ElementJson &ElementJson::operator=(const ElementJson &other) noexcept
{
    init();
    if(other.error()) setError(other.getError());
    m_comment = other.m_comment;
    m_values  = other.m_values;
    return *this;
}

ElementJson &ElementJson::operator=(ElementJson &&other) noexcept
{
    init();
    if(other.error()) setError(other.getError());
    m_comment = std::move(other.m_comment);
    m_values  = std::move(other.m_values);
    return *this;
}

ElementJson::ElementJson(const JPair &pair) noexcept {
    init();
    insert_back(pair.first, pair.second);
}

ElementJson::ElementJson(const VPairElement &vec) noexcept {
    init();
    insert_back(vec);
}

void ElementJson::set_comment(const size_t& index, const Comment &content) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index).second->setComment(content);
}

void ElementJson::set_comment(const size_t& index, const std::string &content_before,
                              const std::string &content_after)
{
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index).second->setComment(content_before, content_after);
}

void ElementJson::set_prefix_comment(const size_t& index, const std::string &content) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index).second->setPrefixComment(content);
}

void ElementJson::set_suffix_comment(const size_t& index, const std::string &content) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index).second->setSuffixComment(content);
}

Comment &ElementJson::get_comment(const size_t& index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    return m_values.at(index).second->getComment();
}

Comment ElementJson::get_comment(const size_t& index) const {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    return m_values.at(index).second->getComment();
}

std::string ElementJson::get_prefix_comment(const size_t& index) const {

    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    return m_values.at(index).second->getPrefixComment();
}

std::string ElementJson::get_suffix_comment(const size_t& index) const {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    return m_values.at(index).second->getSuffixComment();
}

//std::string &ElementJson::get_prefix_comment(const size_t& index) {
//    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
//    return m_values.at(index).second->getPrefixComment();
//}

//std::string &ElementJson::get_suffix_comment(const size_t& index) {
//    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
//    return m_values.at(index).second->getSuffixComment();
//}

void ElementJson::clear_comment(const size_t& index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index).second->clearComment();
}

void ElementJson::clear_prefix_comment(const size_t& index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index).second->clearPrefixComment();
}

void ElementJson::clear_suffix_comment(const size_t& index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index).second->clearSuffixComment();
}

void ElementJson::delete_comment(const size_t& index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index).second->deleteComment();
}

void ElementJson::delete_prefix_comment(const size_t& index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index).second->deletePrefixComment();
}

void ElementJson::delete_suffix_comment(const size_t& index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index).second->deleteSuffixComment();
}

void ElementJson::set_comment(const std::string& key, const Comment &content) {
    set_comment(get_index_from(key), content);
}

void ElementJson::set_comment(const std::string& key, const std::string &content_before,
                              const std::string &content_after)
{
    set_comment(get_index_from(key), content_before, content_after);
}

void ElementJson::set_prefix_comment(const std::string& key, const std::string &content) {
    set_prefix_comment(get_index_from(key), content);
}

void ElementJson::set_suffix_comment(const std::string& key, const std::string &content) {
    set_suffix_comment(get_index_from(key), content);
}

Comment &ElementJson::get_comment(const std::string& key) {
    return get_comment(get_index_from(key));
}

Comment ElementJson::get_comment(const std::string& key) const {
    return get_comment(get_index_from(key));
}

std::string ElementJson::get_prefix_comment(const std::string& key) const {
    return get_prefix_comment(get_index_from(key));
}

std::string ElementJson::get_suffix_comment(const std::string& key) const {
    return get_suffix_comment(get_index_from(key));
}

//std::string &ElementJson::get_prefix_comment(const std::string& key) {
//    return get_prefix_comment(get_index_from(key));
//}

//std::string &ElementJson::get_suffix_comment(const std::string& key) {
//    return get_suffix_comment(get_index_from(key));
//}

void ElementJson::clear_comment(const std::string& key) {
    clear_comment(get_index_from(key));
}

void ElementJson::clear_prefix_comment(const std::string& key) {
    clear_prefix_comment(get_index_from(key));
}

void ElementJson::clear_suffix_comment(const std::string& key) {
    clear_suffix_comment(get_index_from(key));
}

void ElementJson::delete_comment(const std::string& key) {
    delete_comment(get_index_from(key));
}

void ElementJson::delete_prefix_comment(const std::string& key) {
    delete_prefix_comment(get_index_from(key));
}

void ElementJson::delete_suffix_comment(const std::string& key) {
    delete_suffix_comment(get_index_from(key));
}

//аналог insert_back(key, new_value), но с условием перезаписи
void ElementJson::set(const std::string &key, const Config &new_value, bool update) noexcept {
    if(contains(key)) {
        if(update)
            get_at(key) = new_value;
    } else {
        m_values.push_back(std::make_pair(key, std::make_shared<Config>(new_value)));
    }
}

//аналог insert_back(key, new_value), но с условием перезаписи
void ElementJson::set(const std::string &key, Config &&new_value, bool update) noexcept {
    if(contains(key)) {
        if(update)
            get_at(key) = std::move(new_value);
    } else {
        m_values.push_back(std::make_pair(key, std::make_shared<Config>(std::move(new_value))));
    }
}

void ElementJson::set_if_null(const std::string &key, const Config &new_value) noexcept {
    if(!contains(key))
        set(key, new_value);
}

void ElementJson::set_if_null(const std::string &key, Config &&new_value) noexcept {
    if(!contains(key))
        set(key, std::move(new_value));
}

Config& ElementJson::get_front() {
    __CHECK_JSON_EMPTY_EXCEPTION__((*this))
    return *m_values.front().second;
}

Config ElementJson::get_front() const {
    __CHECK_JSON_EMPTY_EXCEPTION__((*this))
    return *m_values.front().second;
}

Config& ElementJson::get_at(const size_t& index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    return *m_values.at(index).second;
}

Config ElementJson::get_at(const size_t& index) const {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    return *m_values.at(index).second;
}

Config& ElementJson::get_back() {
    __CHECK_JSON_EMPTY_EXCEPTION__((*this))
    return *m_values.back().second;
}

Config ElementJson::get_back() const {
    __CHECK_JSON_EMPTY_EXCEPTION__((*this))
    return *m_values.back().second;
}

//если элемента с таким ключом не существует - добавить пустой и вернуть его
Config& ElementJson::get_at(const std::string& key) noexcept {
    shared_VPairElement::const_iterator it = std::find_if(cbegin(), cend(),
                                                          [&key](const shared_JPair& pair){ return pair.first == key; });
    if(it == cend())
        it = m_values.insert(cend(), std::make_pair(key, std::make_shared<Config>()));
    return *it->second;
}

//если ключа не существовало, то добавлен НЕ будет
Config ElementJson::get_at(const std::string& key) const noexcept {
    shared_VPairElement::const_iterator it = std::find_if(cbegin(), cend(),
                                                          [&key](const shared_JPair& pair){ return pair.first == key; });
    if(it == cend()) {
        return Config();
    } else {
        return *it->second;
    }
}

//если ключа не существовало, то будет добавлен
Config& ElementJson::get_or_default_at(const std::string& key, Config&& default_value) noexcept {
    shared_VPairElement::const_iterator it = std::find_if(cbegin(), cend(),
                                                          [&key](const shared_JPair& pair){ return pair.first == key; });
    if(it == cend())
        it = m_values.insert(cend(), std::make_pair(key, std::make_shared<Config>(std::move(default_value))));
    return *it->second;
}

//если ключа не существовало, то добавлен НЕ будет
Config ElementJson::get_or_default_at(const std::string& key, Config&& default_value) const noexcept {
    shared_VPairElement::const_iterator it = std::find_if(cbegin(), cend(),
                                                          [&key](const shared_JPair& pair){ return pair.first == key; });
    if(it == cend()) {
        return std::move(default_value);
    } else {
        return *it->second;
    }
}

void ElementJson::clear() noexcept {
    clearComment();
    m_values.clear();
}

void ElementJson::insert_front(const std::string &key, const Config &value) noexcept {
    if(contains(key))
        erase_at(key);
    m_values.insert(cbegin(), std::make_pair(key, std::make_shared<Config>(value)));
}

void ElementJson::insert_front(const std::string &key, Config &&value) noexcept {
    if(contains(key))
        erase_at(key);
    m_values.insert(cbegin(), std::make_pair(key, std::make_shared<Config>(std::move(value))));
}

//@return - был ли изменён индекс вниз при перезаписи ключа
bool ElementJson::insert_at(const size_t& index, const std::string &key, const Config &value) noexcept {
    bool return_flag    = false;

    if(contains(key)) {
        try {
            return_flag = get_index_from(key) < index;
        } catch(...) {} //не имеет смысла
        erase_at(key);
    }

    if(return_flag) {
        if(size() < index - 2) {
            m_values.push_back(std::make_pair(key, std::make_shared<Config>(value)));
        } else {
            m_values.insert(cbegin() + (index - 1), std::make_pair(key, std::make_shared<Config>(value)));
        }
    } else {
        if(size() < index - 1) {
            m_values.push_back(std::make_pair(key, std::make_shared<Config>(value)));
        } else {
            m_values.insert(cbegin() + index, std::make_pair(key, std::make_shared<Config>(value)));
        }
    }

    return return_flag;
}

//@return - был ли изменён индекс вниз при перезаписи ключа
bool ElementJson::insert_at(const size_t& index, const std::string &key, Config &&value) noexcept {
    bool return_flag    = false;

    if(contains(key)) {
        try {
            return_flag = get_index_from(key) < index;
        } catch(...) {} //не имеет смысла
        erase_at(key);
    }

    if(return_flag) {
        if(size() < index - 2) {
            m_values.push_back(std::make_pair(key, std::make_shared<Config>(std::move(value))));
        } else {
            m_values.insert(cbegin() + (index - 1), std::make_pair(key, std::make_shared<Config>(std::move(value))));
        }
    } else {
        if(size() < index - 1) {
            m_values.push_back(std::make_pair(key, std::make_shared<Config>(std::move(value))));
        } else {
            m_values.insert(cbegin() + index, std::make_pair(key, std::make_shared<Config>(std::move(value))));
        }
    }

    return return_flag;
}

void ElementJson::insert_back(const std::string &key, const Config &value) noexcept {
    if(contains(key))
        erase_at(key);
    m_values.insert(cend(), std::make_pair(key, std::make_shared<Config>(value)));
}

void ElementJson::insert_back(const std::string &key, Config &&value) noexcept {
    if(contains(key))
        erase_at(key);
    m_values.insert(cend(), std::make_pair(key, std::make_shared<Config>(std::move(value))));
}

//аналог set(key, value)
void ElementJson::insert_at(const std::string &key, const Config &value) noexcept {
    set(key, value);
}

//аналог set(key, value)
void ElementJson::insert_at(const std::string &key, Config &&value) noexcept {
    set(key, std::move(value));
}

//если position key не существует - новый элемент добавится в конец списка
void ElementJson::insert_before(const std::string &position_key, const std::string &key,
                                const Config &value)
{
    if(position_key == key)
        throw std::invalid_argument("\"position key\" and \"key for value\" should not match");

    try {
        insert_at(get_index_from(position_key), key, value);
    } catch(...) {
        insert_back(key, value);
    }
}

void ElementJson::insert_before(const std::string &position_key, const std::string &key,
                                Config &&value)
{
    if(position_key == key)
        throw std::invalid_argument("\"position key\" and \"key for value\" should not match");

    try {
        insert_at(get_index_from(position_key), key, std::move(value));
    } catch(...) {
        insert_back(key, std::move(value));
    }
}

//если position key не существует - новый элемент добавится в конец списка
void ElementJson::insert_after(const std::string &position_key, const std::string &key,
                               const Config &value)
{
    if(position_key == key)
        throw std::invalid_argument("\"position key\" and \"key for value\" should not match");

    try {
        insert_at(get_index_from(position_key) + 1, key, value);
    } catch(...) {
        insert_back(key, value);
    }
}

//если position key не существует - новый элемент добавится в конец списка
void ElementJson::insert_after(const std::string &position_key, const std::string &key,
                               Config &&value)
{
    if(position_key == key)
        throw std::invalid_argument("\"position key\" and \"key for value\" should not match");

    try {
        insert_at(get_index_from(position_key) + 1, key, std::move(value));
    } catch(...) {
        insert_back(key, std::move(value));
    }
}

shared_VPairElement::iterator ElementJson::insert_at(shared_VPairElement::iterator iterator, const std::string &key,
                                                     const Config &value) noexcept
{
    Config temp = value;
    return insert_at(iterator, key, std::move(temp));
}

shared_VPairElement::iterator ElementJson::insert_at(shared_VPairElement::iterator iterator, const std::string &key,
                                                     Config &&value) noexcept
{
    //если ключ существует - удалить и перезаписать на новом итераторе
    erase_at(key);
    if(iterator >= m_values.begin() && iterator < m_values.end())
        return m_values.insert(iterator, std::make_pair(key, std::make_shared<Config>(std::move(value))));
    else
        return m_values.insert(m_values.end(), std::make_pair(key, std::make_shared<Config>(std::move(value))));
}

void ElementJson::insert_front(const VPairElement &elements) noexcept {
    size_t counter = 0;
    for(const JPair &pair : elements) {
        if(!insert_at(counter, pair.first, pair.second))
            counter++;
    }
}

void ElementJson::insert_front(VPairElement &&elements) noexcept {
    size_t counter = 0;
    for(const JPair &pair : elements) {
        if(!insert_at(counter, pair.first, std::move(pair.second)))
            counter++;
    }
}

//если индекса не существует - добавится в конец списка
void ElementJson::insert_at(const size_t& index, const VPairElement &elements) noexcept {
    size_t counter = 0;
    for(const JPair &pair : elements) {
        if(!insert_at(index + counter, pair.first, pair.second))
            counter++;
    }
}

//если индекса не существует - добавится в конец списка
void ElementJson::insert_at(const size_t& index, VPairElement &&elements) noexcept {
    size_t counter = 0;
    for(JPair &pair : elements) {
        if(!insert_at(index + counter, pair.first, std::move(pair.second)))
            counter++;
    }
}

void ElementJson::insert_back(const VPairElement &elements) noexcept {
    std::transform(elements.cbegin(), elements.cend(), std::back_inserter(m_values),
                   [](const JPair& pair){ return std::make_pair(pair.first, std::make_shared<Config>(pair.second)); });
}

void ElementJson::insert_back(VPairElement &&elements) noexcept {
    std::transform(std::make_move_iterator(elements.begin()),
                   std::make_move_iterator(elements.end()),
                   std::back_inserter(m_values),
                   [](const JPair& pair) {
                       return std::make_pair(std::move(pair.first), std::make_shared<Config>(std::move(pair.second)));
                   });
}

void ElementJson::append(const ElementJson &other) noexcept {
    ElementJson json_copy(other);
    append(std::move(json_copy));
}

void ElementJson::append(ElementJson &&other) noexcept {
    for(auto& o : other)
        insert_at(o.first, *o.second);
}

void ElementJson::pop_front() noexcept {
    if(!m_values.empty())
        m_values.erase(cbegin());
}

void ElementJson::pop_at(const size_t& index) noexcept {
    if(m_values.size() > index)
        m_values.erase(cbegin() + index);
}

void ElementJson::pop_back() noexcept {
    if(!m_values.empty())
        m_values.erase(cend());
}

Config ElementJson::get_and_pop_front() {
    Config cfg = get_front();
    pop_front();
    return cfg;
}

Config ElementJson::get_and_pop_at(const size_t& index) {
    Config cfg = get_at(index);
    pop_at(index);
    return cfg;
}

Config ElementJson::get_and_pop_back() {
    Config cfg = get_back();
    pop_back();
    return cfg;
}

void ElementJson::pop_at(const std::string &key) noexcept {
    shared_VPairElement::const_iterator it = std::find_if(cbegin(), cend(),
                                                          [&key](const shared_JPair& pair){ return pair.first == key; });
    if(it != cend())
        m_values.erase(it);
}

Config ElementJson::get_and_pop_at(const std::string &key) noexcept {
    Config cfg = get_at(key);
    pop_at(key);
    return cfg;
}

void ElementJson::erase_at(const std::string &key) noexcept {
    for(auto it = m_values.cbegin(); it != m_values.cend(); it++) {
        if(it->first == key) {
            m_values.erase(it);
            break;
        }
    }
}

shared_VPairElement::iterator ElementJson::pop_at(const shared_VPairElement::iterator iterator)
{
    if(iterator >= m_values.begin() && iterator < m_values.end())
        return m_values.erase(iterator);
    else
        return m_values.end();
}

bool ElementJson::isEqual(const IElement &other, const bool compare_comments,
                          const bool map_sort_important) const noexcept
{
    bool b1 = true;
    if(compare_comments)
        b1 = isCommentsEqual(other);

    bool b2 = size() == other.size();
    if(b2) {
        const ElementJson& other_json = reinterpret_cast<const ElementJson&>(other);
        if(map_sort_important) {
            for(size_t i = 0; i < size(); i++) {
                const shared_JPair& p1 = m_values[i];
                const shared_JPair& p2 = other_json.m_values[i];
                if(p1.first == p2.first) {
                    if(!p1.second->isEqual(*p2.second, compare_comments, map_sort_important)) {
                        b2 = false;
                        break;
                    }
                }
            }
        } else {
            for(size_t i = 0; i < size(); i++) {
                const shared_JPair& our = m_values[i];
                if(other_json.contains(our.first)) {
                    const Config& other_config = other_json.get_at(our.first);
                    if(!our.second->isEqual(other_config, compare_comments, map_sort_important)) {
                        b2 = false;
                        break;
                    }
                } else {
                    b2 = false;
                    break;
                }
            }
        }
    }

    return b1 && b2;
}

bool ElementJson::contains(const std::string &key) const noexcept {
    return std::any_of(cbegin(), cend(),
                       [&key](const shared_JPair &pair){ return pair.first == key; });
}

size_t ElementJson::get_index_from(const std::string &key) const {
    size_t counter = 0;
    shared_VPairElement::const_iterator it = std::find_if(cbegin(), cend(), [&counter, &key](const shared_JPair& pair) {
        counter++;
        return pair.first == key;
    });

    if(it == cend())
        throw std::invalid_argument("key not found");
    return counter - 1;
}

Config ElementJson::operator[](const size_t& index) const {
    return get_at(index);
}

bool ElementJson::operator==(const Config &config) const
{
    if(!config.isJson())
        return false;

    if(size() != config.size())
        return false;

    for(size_t i = 0; i < size(); i++) {
        if((*this)[i] != config[i])
            return false;
    }

    return true;
}

Config &ElementJson::operator[](const std::string &key) noexcept {
    return get_at(key);
}

Config ElementJson::operator[](const std::string &key) const noexcept {
    return get_at(key);
}

std::string ElementJson::toString(const ConfigFormat format, const CommentDesign &design,
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

std::string ElementJson::toJsonString(const CommentDesign &design, const int8_t custom_tabulation_level) const noexcept
{
    std::string ret;
    const std::string tablulation_str   = utils::RepeatSymToStr('\t', custom_tabulation_level);
    const std::string tablulation_str_1 = utils::RepeatSymToStr('\t', custom_tabulation_level + 1);

    bool with_spaces = custom_tabulation_level != -1;
    CommentDesign inner_design   = design;
    inner_design.is_in_container = true;

    //вывод комментария с рамкой
    if(with_spaces
        && design.with_comments
        && !design.is_in_container
        && !getPrefixComment().empty())
    {
        ret += ToComment(getPrefixComment(), inner_design, custom_tabulation_level);
        ret += "\n";
    }

    if(with_spaces)
        ret += tablulation_str;
    ret += "{";
    if(with_spaces)
        ret += "\n";

    for(size_t i = 0; i < size(); i++)
    {
        //вывод комментария с рамкой
        if(with_spaces
            && inner_design.with_comments
            && !m_values[i].second->getPrefixComment().empty())
        {
            //вернули в исходное состояние после записи КОММЕНТАРИЯ ПОСЛЕ ЗНАЧЕНИЯ
            inner_design.opt_multiline_column_size = design.opt_multiline_column_size;

            //NOTE: если переменная содержит комментарий ДО значения - делаем пустую строку для лучшей читабельности
            if(i != 0) //из-за табуляции для первого элемента итоговый стиль смотрится странно
                ret += "\n";

            ret += ToComment(m_values[i].second->getPrefixComment(), inner_design, custom_tabulation_level + 1);
            ret += "\n";
        }

        //вывод ключа
        if(with_spaces)
            ret += tablulation_str_1;
        ret += "\"" + m_values[i].first + "\"";

        //вывод разделителя
        ret += ":";

        //вывод значения
        if(with_spaces)
            ret += " ";

        std::string temp = m_values[i].second->toString(ConfigFormat::eJSON, inner_design,
                                                        (with_spaces ? custom_tabulation_level + 1 : -1));
        if(m_values[i].second->isContainer()) {
            ret += utils::RemoveStartTabulations(temp);
        } else {
            if(!m_values[i].second->isString()) {
                //в числах могут быть бесконечности
                if(m_values[i].second->isNumber()) {
                    if(std::any_of(temp.begin(), temp.end(),
                                    [](char c) {
                                        switch(c) {
                                            case '0':
                                            case '1':
                                            case '2':
                                            case '3':
                                            case '4':
                                            case '5':
                                            case '6':
                                            case '7':
                                            case '8':
                                            case '9':
                                            case '-':
                                            case '.':   return false;
                                            default:    return true;
                                        }
                                    }
                                    )
                        )
                    {
                        ret += "\"" + temp + "\"";
                    } else {
                        ret += temp;
                    }
                } else {
                    ret += temp;
                }
            } else {
                SeparatedLines sl = SeparateWithoutColumned(temp);
                size_t len_of_key = utils::GetStringCharCount(m_values[i].first) + /*кавычки*/2 + /*двоеточие и пробел*/2;
                // все строки кроме первой выровнять по первой строке
                for(size_t j = 1; j < sl.lines.size(); j++) {
                    sl.lines[j] = utils::RepeatSymToStr('\t', custom_tabulation_level + 1)
                                  + utils::RepeatSymToStr(' ', len_of_key)
                                  + sl.lines[j];
                }

                ret += "\"";
                ret += VStringToString(sl.lines);
                ret += "\"";
            }
        }

        if(i < size() - 1)
            ret += ",";

        //вывод комментария без рамки
        if(with_spaces
            && inner_design.with_comments
            && !m_values[i].second->getSuffixComment().empty())
        {
            // ширина колонки многострочного комментария после значения не влияет на вывод
            inner_design.opt_multiline_column_size = 0;

            std::string temp = ToComment(m_values[i].second->getSuffixComment(), inner_design, -1);
            const size_t pos = ret.rfind('\n');
            std::string temp__ = (ret.size() > pos +1 ? (ret.substr(pos + 1, ret.size())) : "") + " ";
            utils::SetStringAsOnlySpaces(temp__);
            utils::AddStringForFromLine(temp, 2, temp__);
            ret += " " + temp;
        }

        if(with_spaces)
            ret += "\n";
    }

    if(with_spaces)
        ret += tablulation_str;
    ret += "}";

    //вывод комментария без рамки
    if(with_spaces
        && design.with_comments
        && !design.is_in_container
        && !getSuffixComment().empty())
    {
        // ширина колонки многострочного комментария после значения не влияет на вывод
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

// TODO: нужно сделать выравнивание на первом уровне ключей (второй уровень в рамках группы)
// WARNING: метод не рекурсивный для контейнеров!
std::string ElementJson::toIniString(const CommentDesign &design, const int8_t custom_tabulation_level) const noexcept
{
    std::string ret;
    std::string temp;

    auto GetPrefixComment = [&design](const Config cfg) -> std::string {
        return (cfg.getPrefixComment().empty()) ? "" : (ToComment(cfg.getPrefixComment(), design) + "\n");
    };
    auto GetSuffixComment = [&design, &ret](const Config cfg) -> std::string {
        if(cfg.getSuffixComment().empty())
            return "";

        SeparatedLines sl = SeparateWithoutColumned(ToComment(cfg.getSuffixComment(), design));
        //начиная со второй, все строки дополнить пробелами в начале по длине
        // последней строки +1(отделение комментария от значения)
        size_t pos = ret.rfind('\n');
        std::string last_line = (pos == std::string::npos) ? ret
                                                           : (ret.substr(pos + 1, ret.size()));
        size_t end_line_size = utils::GetStringCharCount(last_line) + 1;
        for(size_t i = 1; i < sl.lines.size(); i++) {
            sl.lines[i] = utils::RepeatSymToStr(' ', end_line_size) + sl.lines[i];
        }

        return (cfg.getSuffixComment().empty()) ? "" : " " + VStringToString(sl.lines);
    };
    auto IsArrayWithPrimitives = [](const Config cfg) -> bool {
        if(!cfg.isIndexContainer())
            return false;
        for(const auto& cfg_inner : cfg.getRange()) {
            if(cfg_inner->isContainer()
                || !cfg_inner->getPrefixComment().empty()
                || !cfg_inner->getSuffixComment().empty()
                || (cfg_inner->isString() && (cfg_inner->getString().find('\n') != std::string::npos
                                              || cfg_inner->getString().size() > 50)))
            {
                return false;
            }
        }
        return true;
    };
    auto AppendMultinlineString = [&ret](const std::string& str) -> void {
        ret += "\"";
        std::string temp_str = str;
        if(temp_str.find('\n') != std::string::npos) {
            SeparatedLines sl = tools::SeparateWithoutColumned(temp_str);
            for(size_t i = 0; i < sl.lines.size(); i++) {
                ret += (i == 0 ? "" : "    ");
                if(i + 1 < sl.lines.size())
                    ret += sl.lines[i] + " \\\n";
                else
                    ret += sl.lines[i];
            }
        } else {
            ret += str;
        }
        ret += "\"";
    };
    auto AppendArrayPrimitives = [&](const std::string& key, Config& cfg) -> void {
        if(!key.empty())
            ret += key + " = ";

        ret += "[";
        for(size_t i = 0; i < cfg.size(); i++) {
            if(cfg[i].isString())
                ret += "\"";
            ret += cfg[i].toString();
            if(cfg[i].isString())
                ret += "\"";

            if(i + 1 < cfg.size())
                ret += ", ";
        }
        ret += "]";
    };
    auto AppendCollection = [&](const std::string& prefix, Config& cfg) -> void {
        std::vector<std::unique_ptr<KeysBase>> kbss = CollectKeys(cfg, prefix);
        for(auto& kbs : kbss) {
            KeysComments* ptr_comment = dynamic_cast<KeysComments*>(kbs.get());
            KeysValues* ptr_cfg       = dynamic_cast<KeysValues*>(kbs.get());
            if(ptr_comment) {
                //групповой комментарий для INI так и или иначе будет напечатан с новой строки, т.к. потеряется привязанность к группе
                ret += ToComment(ptr_comment->m_comment_str, design) + "\n";
            } else if(ptr_cfg) {
                ret += GetPrefixComment(*ptr_cfg->m_ptr_remote_cfg);
                if(!ptr_cfg->m_key.empty())
                    ret += ptr_cfg->m_key + " = ";
                if(ptr_cfg->m_ptr_remote_cfg->isString()) {
                    AppendMultinlineString(ptr_cfg->m_ptr_remote_cfg->toString());
                } else {
                    ret += ptr_cfg->m_ptr_remote_cfg->toString();
                }
                ret += GetSuffixComment(*ptr_cfg->m_ptr_remote_cfg);
                ret += "\n";
            }
        }
    };

    if(!getPrefixComment().empty()) {
        ret += ToComment(getPrefixComment(), design) + "\n\n\n";
    }

    for(const auto& cfg : m_values) {
        switch(cfg.second->getType()) {
        case ValueType::eJson: {
            if(!ret.empty())
                ret += '\n';
            ret += GetPrefixComment(*cfg.second);
            ret += "[" + cfg.first + "]";
            ret += GetSuffixComment(*cfg.second);
            ret += "\n";

            for(const auto& cfg_inner : cfg.second->getNamedRange()) {
                if(cfg_inner.second->isContainer()) {
                    if(IsArrayWithPrimitives(*cfg_inner.second))
                    {
                        ret += GetPrefixComment(*cfg_inner.second);

                        //если внутри только примитивы без комментариев - вывести их в одну строку (строки длиной <=50)
                        AppendArrayPrimitives(cfg_inner.first, *cfg_inner.second);

                        temp = GetSuffixComment(*cfg_inner.second);
                        ret += std::move(temp);
                        ret += "\n";
                    } else {
                        //комментарии элемента cfg_inner будут обработаны в рекурсивной функции
                        //нужно собрать все элементы массива и упаковать в общее имя с переходом между уровнями
                        AppendCollection(cfg_inner.first, *cfg_inner.second);
                    }
                } else if(cfg_inner.second->isString()) {
                    ret += GetPrefixComment(*cfg_inner.second);
                    if(!cfg_inner.first.empty())
                        ret += cfg_inner.first + " = ";
                    AppendMultinlineString(cfg_inner.second->toString());
                    ret += GetSuffixComment(*cfg_inner.second);
                    ret += "\n";
                } else {
                    ret += GetPrefixComment(*cfg_inner.second);
                    if(!cfg_inner.first.empty())
                        ret += cfg_inner.first + " = ";
                    ret += cfg_inner.second->toString();
                    ret += GetSuffixComment(*cfg_inner.second);
                    ret += "\n";
                }
            } // for()

            break;
        }
        case ValueType::eArray: {
            //если внутри только примитивы без комментариев - вывести их в одну строку (строки длиной <=50)
            if(IsArrayWithPrimitives(*cfg.second.get())) {
                ret += GetPrefixComment(*cfg.second);

                AppendArrayPrimitives(cfg.first, *cfg.second);

                temp = GetSuffixComment(*cfg.second);
                ret += std::move(temp);
                ret += "\n";
            } else {
                //комментарии элемента cfg будут обработаны в рекурсивной функции
                //нужно собрать все элементы массива и упаковать в общее имя с переходом между уровнями
                AppendCollection(cfg.first, *cfg.second);
            }

            break;
        }
        default: {
            ret += GetPrefixComment(*cfg.second);
            if(!cfg.first.empty())
                ret += cfg.first + " = ";

            if(cfg.second->isString()) {
                AppendMultinlineString(cfg.second->toString());
            } else {
                ret += cfg.second->toString();
            }

            ret += GetSuffixComment(*cfg.second);
            ret += "\n";
            break;
        }
        }
    } // loop for()

    if(!getSuffixComment().empty()) {
        ret += "\n\n" + ToComment(getSuffixComment(), design);
    }

    return ret;
}

std::string ElementJson::toYamlString(const CommentDesign &design, const int8_t custom_tabulation_level) const noexcept
{
    //TODO (потом): std::string ElementJson::toYamlString()
    return "";
}

std::string ElementJson::toXmlString(const CommentDesign &design, const int8_t custom_tabulation_level) const noexcept
{
    //TODO (потом): std::string ElementJson::toXmlString()
    return "";
}

bool IsElementJson(const IElement &e) noexcept
{
    return e.getType() == ValueType::eJson;
}

bool IsElementJson(const Config &cfg) noexcept
{
    return cfg.isJson();
}

} // namespace tools
} // namespace simpleapi

