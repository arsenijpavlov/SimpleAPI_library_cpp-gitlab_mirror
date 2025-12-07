#include "ElementJson.h"

#include "Config.h"
#include <algorithm>
#include "../utils/Utils.h"


ElementJson::ElementJson(const JPair &pair) noexcept {
    init();
    insert_back(pair.first, pair.second);
}

void ElementJson::add_comment(const size_t index, const Comment &content) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index).second->addComment(content);
}

void ElementJson::add_comment(const size_t index, const std::string &content_before,
                              const std::string &content_after)
{
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index).second->addComment(content_before, content_after);
}

void ElementJson::add_prefix_comment(const size_t index, const std::string &content) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index).second->addPrefixComment(content);
}

void ElementJson::add_suffix_comment(const size_t index, const std::string &content) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index).second->addSuffixComment(content);
}

Comment &ElementJson::get_comment(const size_t index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    return m_values.at(index).second->getComment();
}

Comment ElementJson::get_comment(const size_t index) const {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    return m_values.at(index).second->getComment();
}

std::string &ElementJson::get_prefix_comment(const size_t index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    return m_values.at(index).second->getPrefixComment();
}

std::string ElementJson::get_prefix_comment(const size_t index) const {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    return m_values.at(index).second->getPrefixComment();
}

std::string &ElementJson::get_suffix_comment(const size_t index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    return m_values.at(index).second->getSuffixComment();
}

std::string ElementJson::get_suffix_comment(const size_t index) const {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    return m_values.at(index).second->getSuffixComment();
}

void ElementJson::clear_comment(const size_t index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index).second->clearComment();
}

void ElementJson::clear_prefix_comment(const size_t index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index).second->clearPrefixComment();
}

void ElementJson::clear_suffix_comment(const size_t index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index).second->clearSuffixComment();
}

void ElementJson::delete_comment(const size_t index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index).second->deleteComment();
}

void ElementJson::delete_prefix_comment(const size_t index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index).second->deletePrefixComment();
}

void ElementJson::delete_suffix_comment(const size_t index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index).second->deleteSuffixComment();
}

void ElementJson::add_comment(const std::string& key, const Comment &content) {
    add_comment(get_index_from(key), content);
}

void ElementJson::add_comment(const std::string& key, const std::string &content_before,
                              const std::string &content_after)
{
    add_comment(get_index_from(key), content_before, content_after);
}

void ElementJson::add_prefix_comment(const std::string& key, const std::string &content) {
    add_prefix_comment(get_index_from(key), content);
}

void ElementJson::add_suffix_comment(const std::string& key, const std::string &content) {
    add_suffix_comment(get_index_from(key), content);
}

Comment &ElementJson::get_comment(const std::string& key) {
    return get_comment(get_index_from(key));
}

Comment ElementJson::get_comment(const std::string& key) const {
    return get_comment(get_index_from(key));
}

std::string &ElementJson::get_prefix_comment(const std::string& key) {
    return get_prefix_comment(get_index_from(key));
}

std::string ElementJson::get_prefix_comment(const std::string& key) const {
    return get_prefix_comment(get_index_from(key));
}

std::string &ElementJson::get_suffix_comment(const std::string& key) {
    return get_suffix_comment(get_index_from(key));
}

std::string ElementJson::get_suffix_comment(const std::string& key) const {
    return get_suffix_comment(get_index_from(key));
}

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

Config& ElementJson::get_at(const size_t index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    return *m_values.at(index).second;
}

Config ElementJson::get_at(const size_t index) const {
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
bool ElementJson::insert_at(const size_t index, const std::string &key, const Config &value) noexcept {
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
bool ElementJson::insert_at(const size_t index, const std::string &key, Config &&value) noexcept {
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

void ElementJson::insert_at(VPairElement::iterator iterator, const std::string &key,
                            const Config &value)
{
    //TODO: void ElementJson::insert_at()

    //если ключ существует - удалить и перезаписать на новом итераторе
}

void ElementJson::insert_at(VPairElement::iterator iterator, const std::string &key,
                            Config &&value)
{
    //TODO: void ElementJson::insert_at()

    //если ключ существует - удалить и перезаписать на новом итераторе
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
void ElementJson::insert_at(const size_t index, const VPairElement &elements) noexcept {
    size_t counter = 0;
    for(const JPair &pair : elements) {
        if(!insert_at(index + counter, pair.first, pair.second))
            counter++;
    }
}

//если индекса не существует - добавится в конец списка
void ElementJson::insert_at(const size_t index, VPairElement &&elements) noexcept {
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

void ElementJson::pop_front() {
    m_values.erase(cbegin());
}

void ElementJson::pop_at(const size_t index) {
    m_values.erase(cbegin() + index);
}

void ElementJson::pop_back() {
    m_values.erase(cend());
}

Config ElementJson::get_and_pop_front() {
    Config cfg = get_front();
    pop_front();
    return cfg;
}

Config ElementJson::get_and_pop_at(const size_t index) {
    Config cfg = get_at(index);
    pop_at(index);
    return cfg;
}

Config ElementJson::get_and_pop_back() {
    Config cfg = get_back();
    pop_back();
    return cfg;
}

void ElementJson::erase_front() {
    __CHECK_JSON_EMPTY_EXCEPTION__((*this))
    m_values.erase(m_values.cbegin());
}

void ElementJson::erase_at(const size_t index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.erase(m_values.cbegin() + index);
}

void ElementJson::erase_back() {
    __CHECK_JSON_EMPTY_EXCEPTION__((*this))
    m_values.erase(m_values.cend());
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
    erase_at(key); //FIXME: ссылка на самого себя
}

// NOTE: точно знаю, что можно ускорить, а не запрашивать каждый раз поиск по key
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

Config ElementJson::operator[](const size_t index) const {
    return get_at(index);
}

Config &ElementJson::operator[](const size_t index) {
    return get_at(index);
}

Config &ElementJson::operator[](const std::string &key) noexcept {
    return get_at(key);
}

Config ElementJson::operator[](const std::string &key) const noexcept {
    return get_at(key);
}

//TODO: std::string ElementJson::toString()
std::string ElementJson::toString(const ConfigFormat format, const CommentDesign &design,
                                  const int8_t custom_tabulation_level) const noexcept
{
    std::string ret;
    const std::string tablulation_str   = utils::RepeatSymToStr('\t', custom_tabulation_level);
    const std::string tablulation_str_1 = utils::RepeatSymToStr('\t', custom_tabulation_level + 1);

    bool with_spaces = format != ConfigFormat::eONLY_VALUE && custom_tabulation_level != -1;
    CommentDesign inner_design = design;
    inner_design.is_in_container = true;

    switch(format){
    case ConfigFormat::eONLY_VALUE:
    case ConfigFormat::eJSON:
    {
        //вывод комментария с рамкой
        if(design.with_comments && !design.is_in_container && !getPrefixComment().empty()) {
            ret += ToComment(getPrefixComment(), inner_design, custom_tabulation_level);
            ret += "\n";
        }

        if(with_spaces)
            ret += tablulation_str;
        ret += "{";
        if(with_spaces)
            ret += "\n";

        for(size_t i = 0; i < size(); i++) {
            //вывод комментария с рамкой
            if(inner_design.with_comments && !m_values[i].second->getPrefixComment().empty()) {
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

            std::string temp = m_values[i].second->toString(format, inner_design, (custom_tabulation_level == -1 ? -1 : custom_tabulation_level + 1));
            if(m_values[i].second->isContainer()) {
                temp = utils::RemoveStartTabulations(temp);
            }
            if(m_values[i].second->isString() && format == ConfigFormat::eONLY_VALUE) {
                temp = "\"" + temp + "\"";
            }

            ret += temp;
            if(i < size() - 1)
                ret += ",";

            //вывод комментария без рамки
            if(inner_design.with_comments && !m_values[i].second->getSuffixComment().empty()) {
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

std::string ElementJson::toJsonString(const CommentDesign &design, const int8_t custom_tabulation_level) const noexcept
{
    //TODO: std::string ElementJson::toJsonString()
    return "";
}

std::string ElementJson::toIniString(const CommentDesign &design, const int8_t custom_tabulation_level) const noexcept
{
    //TODO: std::string ElementJson::toIniString()
    return "";
}

std::string ElementJson::toYamlString(const CommentDesign &design, const int8_t custom_tabulation_level) const noexcept
{
    //TODO: std::string ElementJson::toYamlString()
    return "";
}

std::string ElementJson::toXmlString(const CommentDesign &design, const int8_t custom_tabulation_level) const noexcept
{
    //TODO: std::string ElementJson::toXmlString()
    return "";
}

std::string ElementJson::to_string(const ParseState state) const noexcept {
    switch (state) {
    case ParseState::eJSON_START:               return "[JSON_START]";
    case ParseState::eJSON_KEY:                 return "[JSON_KEY]";
    case ParseState::eJSON_KEY_VALUE_SEPARATOR: return "[JSON_KEY_VALUE_SEPARATOR]";
    case ParseState::eJSON_VALUE:               return "[JSON_VALUE]";
    case ParseState::eJSON_SEPARATOR:           return "[JSON_SEPARATOR]";
    case ParseState::eJSON_COMMENT:             return "[JSON_COMMENT]";
    case ParseState::eJSON_FINISH:              return "[JSON_FINISH]";
    case ParseState::eJSON_ERROR_STATE:
    default:                                    return "[JSON_ERROR_STATE]";
    }
}

void ElementJson::UpdateState(ParseState &state, const ParseState new_state) const noexcept {
    state = new_state;
    DEBUG_LOG("Parse Json, upd state: " << to_string(state));
}

void ElementJson::parse(const std::string &input_string, CommentDesign &design,
                        const ConfigFormat format, const int8_t tabulation_level)
{
    parse(std::move(std::string(input_string)), design, format);
}

void ElementJson::parse(const std::string &input_string, const ConfigFormat format,
                        const bool parse_comments, const int8_t tabulation_level)
{
    CommentDesign design;
    design.with_comments = parse_comments;
    parse(input_string, design, format);
}

void ElementJson::parse(std::string &&input_string, CommentDesign &design,
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

void ElementJson::parse(std::string &&input_string, const ConfigFormat format,
                        const bool parse_comments, const int8_t tabulation_level)
{
    CommentDesign design;
    parse(std::move(input_string), design, format, parse_comments);
}

void ElementJson::parseJson(const std::string &input_string, CommentDesign &design,
                            const int8_t tabulation_level)
{
    parseJson(std::move(std::string(input_string)), design);
}

void ElementJson::parseJson(const std::string &input_string, const bool parse_comments,
                            const int8_t tabulation_level)
{
    CommentDesign design;
    design.with_comments = parse_comments;
    parseXml(input_string, design);
}

// MAIN
// @TEST(JSON, parse)
void ElementJson::parseJson(std::string &&input_string, CommentDesign &design,
                            const int8_t tabulation_level)
{
    //TODO: void ElementJson::parseJson()

    using namespace utils;

    /* NOTE: для документации
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
    if(input_string.empty()) return;

    size_t line_counter   = 0;
    size_t symbol_counter = 0;

    design.temp_type = CommentType::eNotComment;

    ParseState state                = ParseState::eJSON_START;
    ParseState state_comment        = ParseState::eJSON_ERROR_STATE;
    std::string comment             = "";
    std::string key                 = "";
    std::string value               = "";
    std::string error_string        = "";
    bool is_quotes                  = false;
    bool is_separator_comma         = false;
    uint16_t inner_json_counter     = 0;
    uint16_t inner_array_counter    = 0;

    for(size_t i = 0; i < input_string.size(); i++) {
        char ch_previous    = i == 0 ? 0 : input_string[i - 1];
        char ch_current     = input_string[i];
        char ch_next        = i < input_string.size() ? input_string[i + 1] : 0;
        SymbolCounter(ch_current, line_counter, symbol_counter);

        //поиск комментариев ===================================================
        const bool ext_flag = !is_quotes;
        //вернёт комментарий без обрамления
        CheckComments(ch_current, ch_next, i, design, comment, ext_flag);
        if(!design.with_comments)
            comment.clear();
        //сюда зайдёт, если внутри комментария либо если встречен конец комментария
        if(design.temp_type != CommentType::eNotComment)
            continue;
        //=================================================== поиск комментариев

        switch (state) {
        case ParseState::eJSON_START: {
            //пропуск пробелов =====================================================
            if(CharInString(ch_current, __SPACES__)) break;
            //===================================================== пропуск пробелов

            if(ch_current == '{') {
                //работа с комментариями (до разбора json) =============================
                if(!comment.empty() && (design.temp_type == CommentType::eCommentEnd || design.temp_type == CommentType::eNotComment)) {
                    addPrefixComment(FromComment(comment, design, tabulation_level));
                    DEBUG_LOG("ElementJson: PreviewComment: " << "\"" << comment << "\"");
                    comment.clear();
                }
                //============================= работа с комментариями (до разбора json)
                UpdateState(state, ParseState::eJSON_KEY);
                break;
            }

            UpdateState(state, ParseState::eJSON_ERROR_STATE);
            error_string = "Not found start of JSON.";
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
                if(!RemoveQuotes(key)) {
                    error_string = "incorrect json key \"" + key + "\"";
                    UpdateState(state, ParseState::eJSON_ERROR_STATE);
                    break;
                }
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
                && (CharInString(ch_next, __SEPARATORS__ " }")
                    || CharInString(ch_current, __SEPARATORS__ " }")))
            {
                DEBUG_LOG("ElementJson: current value done: \"" << value << "\"");
                try {
                    Config element = Config::CreateElementFromString(std::move(value), ConfigFormat::eJSON, design, tabulation_level);
                    push_back(std::move(key), std::move(element));
                } catch (std::exception& e) {
                    error_string = e.what();
                    UpdateState(state, ParseState::eJSON_ERROR_STATE);
                    push_back(key, value);
                    break;
                }
                key.clear();

                if(!comment.empty() && (design.temp_type == CommentType::eCommentEnd || design.temp_type == CommentType::eNotComment)) {
                    get_back().addPrefixComment(FromComment(comment, design, tabulation_level));
                    DEBUG_LOG("ElementJson: inner Element add PreviewComment: " << "\"" << comment << "\"");
                    comment.clear();
                }
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

            if(CharInString(ch_current, __SEPARATORS__)) {
                is_separator_comma = ch_current == ',';
                if(CharInString(ch_current, __SPACES__ ","))
                    i--;
                UpdateState(state_comment, ParseState::eJSON_KEY);
                UpdateState(state, ParseState::eJSON_COMMENT);
                break;
            }
            if(CharInString(ch_current, "}")) {
                UpdateState(state_comment, ParseState::eJSON_FINISH);
                UpdateState(state, ParseState::eJSON_COMMENT);
                break;
            }

            UpdateState(state, ParseState::eJSON_ERROR_STATE);
            error_string = "Not found stop of JSON.";
            break;
        }
        case ParseState::eJSON_COMMENT: {
            //пропуск пробелов
            if(CharInString(ch_current, __SPACES_WITHOUT_SEPARATORS__))
                break;

            //(комментарий после значения, на строке значения после запятой)
            if(!is_separator_comma || ch_current == '\n')
            {
                if(!comment.empty() && (design.temp_type == CommentType::eCommentEnd || design.temp_type == CommentType::eNotComment)) {
                    get_back().addSuffixComment(FromComment(comment, design, tabulation_level));
                    DEBUG_LOG("ElementJson: inner Element add SuffixComment: " << "\"" << comment << "\"");
                    comment.clear();
                }
            }
            UpdateState(state, (ch_next == 0 && ch_current == '}') ? ParseState::eJSON_FINISH : state_comment);

            break;
        }
        case ParseState::eJSON_ERROR_STATE: {
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
    if(state == ParseState::eJSON_COMMENT) // предполагается, что заполнен state_comment
    {
        if(!comment.empty() && (design.temp_type == CommentType::eCommentEnd || design.temp_type == CommentType::eNotComment)) {
            addSuffixComment(FromComment(comment, design, tabulation_level));
            DEBUG_LOG("ElementJson: SuffixComment: " << "\"" << comment << "\"");
            comment.clear();
        }
        UpdateState(state, state_comment);
    }

    if(state != ParseState::eJSON_FINISH && state != ParseState::eJSON_KEY) {
//        clear();
        throw std::invalid_argument("JSON parse error, end of JSON structure not found");
    }
}

void ElementJson::parseJson(std::string &&input_string, const bool parse_comments, const int8_t tabulation_level) {
    CommentDesign design;
    parseJson(std::move(input_string), design, parse_comments);
}

void ElementJson::parseIni(const std::string &input_string, CommentDesign &design,
                           const int8_t tabulation_level)
{
    parseIni(std::move(std::string(input_string)), design);
}

void ElementJson::parseIni(const std::string &input_string, const bool parse_comments,
                           const int8_t tabulation_level)
{
    CommentDesign design;
    design.with_comments = parse_comments;
    parseIni(input_string, design);
}

void ElementJson::parseIni(std::string &&input_string, CommentDesign &design,
                           const int8_t tabulation_level)
{
    //TODO: void ElementJson::parseIni()
}

void ElementJson::parseIni(std::string &&input_string, const bool parse_comments,
                           const int8_t tabulation_level)
{
    CommentDesign design;
    design.with_comments = parse_comments;
    parseIni(std::move(input_string), design);
}

void ElementJson::parseYaml(const std::string &input_string, CommentDesign &design,
                            const int8_t tabulation_level)
{
    parseYaml(std::move(std::string(input_string)), design);
}

void ElementJson::parseYaml(const std::string &input_string, const bool parse_comments,
                            const int8_t tabulation_level)
{
    CommentDesign design;
    design.with_comments = parse_comments;
    parseYaml(input_string, design);
}

void ElementJson::parseYaml(std::string &&input_string, CommentDesign &design,
                            const int8_t tabulation_level)
{
    //TODO: void ElementJson::parseYaml()
}

void ElementJson::parseYaml(std::string &&input_string, const bool parse_comments,
                            const int8_t tabulation_level)
{
    CommentDesign design;
    design.with_comments = parse_comments;
    parseYaml(std::move(input_string), design);
}

void ElementJson::parseXml(const std::string &input_string, CommentDesign &design,
                           const int8_t tabulation_level)
{
    parseXml(std::move(std::string(input_string)), design);
}

void ElementJson::parseXml(const std::string &input_string, const bool parse_comments,
                           const int8_t tabulation_level)
{
    CommentDesign design;
    design.with_comments = parse_comments;
    parseXml(input_string, design);
}

void ElementJson::parseXml(std::string &&input_string, CommentDesign &design,
                           const int8_t tabulation_level)
{
    //TODO: void ElementJson::parseXml()
}

void ElementJson::parseXml(std::string &&input_string, const bool parse_comments,
                           const int8_t tabulation_level)
{
    CommentDesign design;
    design.with_comments = parse_comments;
    parseXml(std::move(input_string), design);
}

