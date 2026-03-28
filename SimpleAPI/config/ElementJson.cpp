#include "ElementJson.h"

#include "Config.h"
#include <algorithm>
#include "../utils/Utils.h"
#include "ConfigDefines.h"
#include "../utils/ParserSymbolCounter.h"


ElementJson::ElementJson(const ElementJson &json) noexcept {
    init();
    m_values = json.m_values;
}

ElementJson::ElementJson(const JPair &pair) noexcept {
    init();
    insert_back(pair.first, pair.second);
}

ElementJson::ElementJson(const std::string &input_string, const ConfigFormat config_format,
                         const bool enable_comment, std::string* error_str) noexcept {
    ElementJson temp;
    temp.init();

    m_comment.commentDesign().with_comments = enable_comment;
    std::string error = temp.parse(input_string, m_comment.commentDesign(), config_format);
    if(error_str != nullptr)
        *error_str = error;

    *this = std::move(temp);
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
    size_t _index       = index;
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
    size_t _index       = index;
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
    auto it = m_values.cbegin();

    while(it != m_values.cend()) {
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
    CommentDesign inner_design = design;
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

            if(m_values[i].second->getPrefixComment().find('\n' != std::string::npos))
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
            temp = utils::RemoveStartTabulations(temp);
        }

        ret += temp;
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

std::string ElementJson::toIniString(const CommentDesign &design, const int8_t custom_tabulation_level) const noexcept
{
    //TODO (скоро): std::string ElementJson::toIniString()
    return "";
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

std::string ElementJson::to_string(const ParseState state) const noexcept {
    switch (state) {
    case ParseState::eJSON_START:               return "[JSON_START]";
    case ParseState::eJSON_KEY:                 return "[JSON_KEY]";
    case ParseState::eJSON_KEY_VALUE_SEPARATOR: return "[JSON_KEY_VALUE_SEPARATOR]";
    case ParseState::eJSON_VALUE:               return "[JSON_VALUE]";
    case ParseState::eJSON_SEPARATOR:           return "[JSON_SEPARATOR]";
    case ParseState::eJSON_FINISH:              return "[JSON_FINISH]";
    case ParseState::eJSON_ERROR_STATE:
    default:                                    return "[JSON_ERROR_STATE]";
    }
}

void ElementJson::UpdateState(ParseState &state, const ParseState new_state) const noexcept {
    state = new_state;
    DEBUG_LOG("Parse Json, upd state: " << to_string(state));
}

std::string  ElementJson::parse(const std::string &input_string, CommentDesign &design,
                        const ConfigFormat format, const int8_t tabulation_level) noexcept
{
    return parse(std::move(std::string(input_string)), design, format);
}

std::string ElementJson::parse(const std::string &input_string, const ConfigFormat format,
                        const bool parse_comments, const int8_t tabulation_level) noexcept
{
    CommentDesign design;
    design.with_comments = parse_comments;
    parse(input_string, design, format);
}

std::string ElementJson::parse(std::string &&input_string, CommentDesign &design,
                        const ConfigFormat format, const int8_t tabulation_level) noexcept
{
    switch(format) {
    case ConfigFormat::eJSON:   return parseJson(std::move(input_string), design);
    case ConfigFormat::eINI:    return parseIni(std::move(input_string), design);
    case ConfigFormat::eYAML:   return parseYaml(std::move(input_string), design);
    case ConfigFormat::eXML:    return parseXml(std::move(input_string), design);
    default:                    return "unexpected format";
    }
}

std::string ElementJson::parse(std::string &&input_string, const ConfigFormat format,
                        const bool parse_comments, const int8_t tabulation_level) noexcept
{
    CommentDesign design;
    return parse(std::move(input_string), design, format, parse_comments);
}

std::string ElementJson::parseJson(const std::string &input_string, CommentDesign &design,
                            const int8_t tabulation_level) noexcept
{
    return parseJson(std::move(std::string(input_string)), design);
}

std::string ElementJson::parseJson(const std::string &input_string, const bool parse_comments,
                            const int8_t tabulation_level) noexcept
{
    CommentDesign design;
    design.with_comments = parse_comments;
    return parseXml(input_string, design);
}

// MAIN
// @TEST(JSON, parse)
std::string ElementJson::parseJson(std::string &&input_string, CommentDesign &design,
                            const int8_t tabulation_level) noexcept
{
    using namespace utils;

    /* NOTE: структура Json (для документации)
     * комментарий json
     * начало json
     * (+комментарий перед ключом)
     * значение элемента json
     * (+комментарий после значения)    = на строке значения
     * (разделитель)
     * (+комментарий перед ключом)      = после разделителя
     * (+значение элемента json)        = после разделителя
     * (+комментарий после значения)    = после разделителя
     * конец json
    */

    //первоначальная очистка всех элементов
    clear();
    //пустой документ не является ошибкой синтаксиса
    RemoveIllegalSpaces(input_string);
    if(input_string.empty()) return {};

    design.temp_type = CommentType::eNotComment;

    ParseState state                = ParseState::eJSON_START;
    std::string key                 = "";
    std::string value               = "";
    std::string error_string        = "";
    bool is_quotes                  = false;
    char last_separator_symbol      = '\n';
    uint16_t inner_json_counter     = 0;
    uint16_t inner_array_counter    = 0;
    bool is_one_value_format        = false; //одиночные значения не требуют фигурных скобок
    ParserSymbolCounter counter;
    ParserSymbolCounter start_value_counter; //для счётчика внутри значения

    std::string current_comment     = ""; // текущее значение при парсинге
    VString comments;                     // обработанные комментарии
    ssize_t value_read_at_line       = -1;

    auto AppendMainPreviewComment = [&]() {
        if(!comments.empty() && (design.temp_type == CommentType::eCommentEnd || design.temp_type == CommentType::eNotComment)) {
            setPrefixComment(VStringToString(comments));
            DEBUG_LOG("ElementJson: PreviewComment: " << "\"" << getPrefixComment() << "\"");
            comments.clear();
        }
    };
    auto AppendMainSuffixComment = [&]() {
        if(!comments.empty() && (design.temp_type == CommentType::eCommentEnd || design.temp_type == CommentType::eNotComment)) {
            setSuffixComment(VStringToString(comments));
            DEBUG_LOG("ElementJson: SuffixComment: " << "\"" << getSuffixComment() << "\"");
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
            DEBUG_LOG("ElementJson: inner Element add PreviewComment: " << "\"" << get_back().getPrefixComment() << "\"");
            comments.clear();
        }
    };
    auto AppendElementSuffixComment = [&](const bool for_penultimate = false){
        if(!comments.empty()
            && !empty()
            && (design.temp_type == CommentType::eCommentEnd || design.temp_type == CommentType::eNotComment))
        {
            if(for_penultimate) {
                if(get_at(size() - 2).getSuffixComment().empty())
                {
                    get_at(size() - 2).setSuffixComment(comments[0]);
                    DEBUG_LOG("ElementJson: inner Element(penultimate) add SuffixComment: " << "\""
                              << get_at(size() - 2).getSuffixComment() << "\"");
                }
                comments.erase(comments.cbegin());
            } else {
                if(get_back().getSuffixComment().empty())
                {
                    get_back().setSuffixComment(comments.back());
                    DEBUG_LOG("ElementJson: inner Element(back) add SuffixComment: " << "\""
                              << get_back().getSuffixComment() << "\"");
                }
                comments.pop_back();
            }

        }
    };

    //дублирующийся код чтения значения
    auto ConfirmValue = [&, input_string](const bool for_penultimate = false) -> bool {
        DEBUG_LOG("ElementJson: current value done: \"" << value << "\"");

        Config element = CreateElementFromString(std::move(value), ConfigFormat::eJSON, design, start_value_counter);
        push_back(std::move(key), std::move(element));
        if(get_back().getCommentDesign().opt_multiline_column_size > design.opt_multiline_column_size)
            design.opt_multiline_column_size = get_back().getCommentDesign().opt_multiline_column_size;
        key.clear();
        value.clear();

        if(element.error()) {
            //если случилась ошибка при внутренней конвертации прочитанного значения,
            // то эта ошибка становится основной ошибкой парсинга
            error_string = "JSON value parse error[" + std::to_string(counter.getLastLineCounter())
                           + "][" + std::to_string(counter.getLastSymbolCounter()) + "]: " + element.getError();
            UpdateState(state, ParseState::eJSON_ERROR_STATE);
            return false;
        }
        return true;
    };

    for(size_t i = 0; i < input_string.size() && state != ParseState::eJSON_ERROR_STATE; i++) {
        char ch_previous    = i == 0 ? 0 : input_string[i - 1];
        char ch_current     = input_string[i];
        char ch_next        = i < input_string.size() ? input_string[i + 1] : 0;

        counter.check(i, ch_current);

        //поиск комментариев ===================================================
        const bool ext_flag = !is_quotes && (inner_array_counter + inner_json_counter == 0);
        //вернёт комментарий без обрамления
        CheckComments(ch_current, ch_next, i, design, current_comment, ext_flag);
        if(!design.with_comments)
            current_comment.clear();
        if(design.with_comments && design.temp_type == CommentType::eCommentEnd)
        {
            comments.push_back(FromComment(current_comment, design, tabulation_level));
            current_comment.clear();
            design.temp_type = CommentType::eNotComment;
            continue;
        }
        if(design.temp_type != CommentType::eNotComment)
            continue;
        //=================================================== поиск комментариев

        // обработка комментария ПОСЛЕ значения
        if(utils::CharInString(ch_current, __SEPARATORS__)) {
            if(state == ParseState::eJSON_KEY
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

        switch (state) {
        case ParseState::eJSON_START: {
            //пропуск пробелов =====================================================
            if(CharInString(ch_current, __SPACES__)) break;
            //===================================================== пропуск пробелов

            if(ch_current == '{') {
                // работа с комментариями (до разбора json)
                AppendMainPreviewComment();
            } else {
                --i; //этот же символ уже является частью ключа
                is_one_value_format = true;
            }

            UpdateState(state, ParseState::eJSON_KEY);
            break;
        }
        case ParseState::eJSON_KEY: {
            //игнор пробелов и разделителей пока ключ пустой
            if(!is_quotes
                && key.empty()
                && CharInString(ch_current, __SPACES__ ",")) //сама по себе запятая не считается ошибкой
            {
                break;
            }
            if(ch_current == '}') {
                // работа с комментарием после элемента
                if(get_back().getSuffixComment().empty())
                    AppendElementSuffixComment();

                UpdateState(state, ParseState::eJSON_FINISH);
                break;
            }

            //если ключ в кавычках, то ждём кавычки, иначе любой __SPACES__
            if(ch_current == '\"'
                && ch_previous != '\\'
                && inner_json_counter + inner_array_counter == 0)
            {
                is_quotes = !is_quotes;
            }

            //защита от дурака: кавычки, именованные списки, массивы
            if(!is_quotes) {
                switch(ch_current) {
                case '{':   { ++inner_json_counter;     break; }
                case '}':   { --inner_json_counter;     break; }
                case '[':   { ++inner_array_counter;    break; }
                case ']':   { --inner_array_counter;    break; }
                default: break;
                }
            }
            key += ch_current;

            //ключ прочитан полностью?
            if(!is_quotes && CharInString(ch_next, __SPACES__ ":=")) {
                RemoveQuotes(key);
                DEBUG_LOG("ElementJson: current key done: \"" << key << "\"");
                UpdateState(state, ParseState::eJSON_KEY_VALUE_SEPARATOR);
                value.clear();
            }

            break;
        }
        case ParseState::eJSON_KEY_VALUE_SEPARATOR: {
            if(CharInString(ch_current, __SPACES__))
                break;
            if(CharInString(ch_current, ":=")) {
                UpdateState(state, ParseState::eJSON_VALUE);

                //следующего символа не существует -> значением является null
                if(ch_next == 0) {
                    ConfirmValue(); //для предпоследнего элемента заполнить замыкающий комментарий

                    // проверка замыкающего комментария (вторичная)
                    if(value_read_at_line == counter.getLastLineCounter())
                        AppendElementSuffixComment(true);
                }

                break;
            }

            error_string = "not found json key-value separator (: or =)";
            UpdateState(state, ParseState::eJSON_ERROR_STATE);
            break;
        }
        case ParseState::eJSON_VALUE: {
            //игнор пробелов пока значение пустое
            if(!is_quotes
                && value.empty()
                && CharInString(ch_current, __SPACES__))
            {
                break;
            }

            if(value.empty())
                start_value_counter = counter; //будет использовано в CreateElementFromString()

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
                default:    { break; }
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
            if(!is_quotes && inner_json_counter + inner_array_counter == 0
                && (ch_next == 0 || CharInString(ch_next, __SEPARATORS__ " }")
                    || CharInString(ch_current, __SEPARATORS__ " }")))
            {
                if(!ConfirmValue(true))
                    break; //если словили exception при обработке - выходим из цикла for()

                // проверка замыкающего комментария (вторичная)
                if(value_read_at_line == counter.getLastLineCounter())
                    AppendElementSuffixComment(true);

                // работа с комментариями перед элементом
                AppendElementPrefixComment();

                UpdateState(state, ParseState::eJSON_SEPARATOR);
            }
            break;
        }
        case ParseState::eJSON_SEPARATOR: {
            //пропуск пробелов =====================================================
            if(CharInString(ch_current, __SPACES_WITHOUT_SEPARATORS__)) break;
            //===================================================== пропуск пробелов

            //может встретиться разделитель или знак завершения массива
            //  комментарий ПОСЛЕ значения может начинаться только на той же строке, что и значение
            //  разделитель может быть как ДО, так и ПОСЛЕ комментария
            //  если комментарий расписан после переноса строки, но до знака }, то комментарий попадёт в суффикс основы

            // запоминаем номер строки, на котором закончили считывать значение
            value_read_at_line = counter.getLastLineCounter(); //применится перед } и перед считыванием значения

            if(!is_one_value_format && CharInString(ch_current, __SEPARATORS__)) {
                i--;
                UpdateState(state, ParseState::eJSON_KEY);
                break;
            }
            if(!is_one_value_format && CharInString(ch_current, "}")) {
                i--;
                UpdateState(state, ParseState::eJSON_FINISH);
                break;
            }

            //либо считано одно значение и всё следующее является ошибкой (комменты не учитываются)
            //либо считано несколько значений и не найден знак завершения (комменты не учитываются)
            UpdateState(state, ParseState::eJSON_ERROR_STATE);
            error_string = "Not found stop of JSON.";
            break;
        }
        default: break;
        }
    }

    /*если файл закончился раньше, чем было обработано последнее прочитанное значение*/
    if(!value.empty()
        && state == ParseState::eJSON_VALUE)
    {
        if(design.temp_type == CommentType::eOneLineComment)
            design.temp_type = CommentType::eNotComment; //сбрасываем для корректной обработки oneline comment

        ConfirmValue();

        // проверка замыкающего комментария (вторичная)
        AppendElementSuffixComment();

        // работа с комментариями перед элементом
        AppendElementPrefixComment();
    }

    if(!is_one_value_format)
        AppendMainSuffixComment(); //конечный комментарий для всего Json

    setCommentDesign(design);

    if(!is_one_value_format && state != ParseState::eJSON_FINISH && error_string.empty())
    {
        error_string = std::string("JSON parse error, unexpected symbol at [")
                       + std::to_string(counter.getLastLineCounter())
                       + "][" + std::to_string(counter.getLastSymbolCounter()) + "]: '"
                       + input_string[counter.getLastIterator()] + "'. "
                       + error_string;
        DEBUG_LOG("ERROR: " << error_string);
        //NOTE: (ElementJson) в случае ошибки парсинга корректно прочитанные значения сохраняются
        //clear();
    }

    return error_string;
}

std::string ElementJson::parseJson(std::string &&input_string, const bool parse_comments,
                            const int8_t tabulation_level) noexcept
{
    CommentDesign design;
    return parseJson(std::move(input_string), design, parse_comments);
}

std::string ElementJson::parseIni(const std::string &input_string, CommentDesign &design,
                           const int8_t tabulation_level) noexcept
{
    return parseIni(std::move(std::string(input_string)), design);
}

std::string ElementJson::parseIni(const std::string &input_string, const bool parse_comments,
                           const int8_t tabulation_level) noexcept
{
    CommentDesign design;
    design.with_comments = parse_comments;
    return parseIni(input_string, design);
}

std::string ElementJson::parseIni(std::string &&input_string, CommentDesign &design,
                           const int8_t tabulation_level) noexcept
{
    //TODO (скоро): void ElementJson::parseIni()
    return "error";
}

std::string ElementJson::parseIni(std::string &&input_string, const bool parse_comments,
                           const int8_t tabulation_level) noexcept
{
    CommentDesign design;
    design.with_comments = parse_comments;
    return parseIni(std::move(input_string), design);
}

std::string ElementJson::parseYaml(const std::string &input_string, CommentDesign &design,
                            const int8_t tabulation_level) noexcept
{
    return parseYaml(std::move(std::string(input_string)), design);
}

std::string ElementJson::parseYaml(const std::string &input_string, const bool parse_comments,
                            const int8_t tabulation_level) noexcept
{
    CommentDesign design;
    design.with_comments = parse_comments;
    return parseYaml(input_string, design);
}

std::string ElementJson::parseYaml(std::string &&input_string, CommentDesign &design,
                            const int8_t tabulation_level) noexcept
{
    //TODO (потом): void ElementJson::parseYaml()
    return "error";
}

std::string ElementJson::parseYaml(std::string &&input_string, const bool parse_comments,
                            const int8_t tabulation_level) noexcept
{
    CommentDesign design;
    design.with_comments = parse_comments;
    return parseYaml(std::move(input_string), design);
}

std::string ElementJson::parseXml(const std::string &input_string, CommentDesign &design,
                           const int8_t tabulation_level) noexcept
{
    return parseXml(std::move(std::string(input_string)), design);
}

std::string ElementJson::parseXml(const std::string &input_string, const bool parse_comments,
                           const int8_t tabulation_level) noexcept
{
    CommentDesign design;
    design.with_comments = parse_comments;
    return parseXml(input_string, design);
}

std::string ElementJson::parseXml(std::string &&input_string, CommentDesign &design,
                           const int8_t tabulation_level) noexcept
{
    //TODO (потом): void ElementJson::parseXml()
    return "error";
}

std::string ElementJson::parseXml(std::string &&input_string, const bool parse_comments,
                           const int8_t tabulation_level) noexcept
{
    CommentDesign design;
    design.with_comments = parse_comments;
    return parseXml(std::move(input_string), design);
}


bool IsElementJson(const IElement &e) noexcept
{
    return e.getType() == ValueType::eJson;
}

bool IsElementJson(const Config &cfg) noexcept
{
    return cfg.isJson();
}
