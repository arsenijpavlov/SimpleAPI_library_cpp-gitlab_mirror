#include "Config.h"

#include "ElementNull.h"
#include "ElementBool.h"
#include "ElementNumber.h"
#include "ElementString.h"
#include "ElementArray.h"
#include "ElementJson.h"
//#include "ElementYaml.h"
//#include "ElementXml.h"

#include <regex>
#include <fstream>

void Config::release() noexcept {
    if(!m_value)
    {
        delete m_value;
        m_value = nullptr;
    }
}

Config &Config::addComment(const Comment &content) noexcept {
    if(m_value)
        m_value->addComment(content);
    return *this;
}

Config &Config::addComment(const std::string &content_before, const std::string &content_after) noexcept {
    if(m_value)
        m_value->addComment(content_before, content_after);
    return *this;
}

Config &Config::addPrefixComment(const std::string &content) noexcept {
    if(m_value)
        m_value->addPrefixComment(content);
    return *this;
}

Config &Config::addSuffixComment(const std::string &content) noexcept {
    if(m_value)
        m_value->addSuffixComment(content);
    return *this;
}

Config &Config::clearComment() noexcept {
    if(m_value)
        m_value->clearComment();
    return *this;
}

Config &Config::clearPrefixComment() noexcept {
    if(m_value)
        m_value->clearPrefixComment();
    return *this;
}

Config &Config::clearSuffixComment() noexcept {
    if(m_value)
        m_value->clearSuffixComment();
    return *this;
}

Config &Config::deleteComment() noexcept {
    if(m_value)
        m_value->deleteComment();
    return *this;
}

Config &Config::deletePrefixComment() noexcept {
    if(m_value)
        m_value->deletePrefixComment();
    return *this;
}

Config &Config::deleteSuffixComment() noexcept {
    if(m_value)
        m_value->deleteSuffixComment();
    return *this;
}

Config &Config::setCommentDesign(const CommentDesign &design) noexcept {
    if(m_value)
        m_value->setCommentDesign(design);
    return *this;
}

Config &Config::clearCommentDesign() noexcept {
    if(m_value)
        m_value->clearCommentDesign();
    return *this;
}

Config &Config::add_comment(const size_t index, const Comment &content) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    reinterpret_cast<IElementContainer*>(m_value)->add_comment(index, content);
    return *this;
}

Config &Config::add_comment(const std::string &key, const Comment &content) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    reinterpret_cast<ElementJson*>(m_value)->add_comment(key, content);
    return *this;
}

Config &Config::add_comment(const size_t index, const std::string &content_before,
                            const std::string &content_after)
{
    __CHECK_TYPE_IS_CONTAINER__((*this))
    reinterpret_cast<IElementContainer*>(m_value)->add_comment(index, content_before, content_after);
    return *this;
}

Config &Config::add_comment(const std::string &key, const std::string &content_before,
                            const std::string &content_after)
{
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    reinterpret_cast<ElementJson*>(m_value)->add_comment(key, content_before, content_after);
    return *this;
}

Config &Config::add_prefix_comment(const size_t index, const std::string &content) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    reinterpret_cast<IElementContainer*>(m_value)->add_prefix_comment(index, content);
    return *this;
}

Config &Config::add_prefix_comment(const std::string &key, const std::string &content) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    reinterpret_cast<ElementJson*>(m_value)->add_prefix_comment(key, content);
    return *this;
}

Config &Config::add_suffix_comment(const size_t index, const std::string &content) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    reinterpret_cast<IElementContainer*>(m_value)->add_suffix_comment(index, content);
    return *this;
}

Config &Config::add_suffix_comment(const std::string &key, const std::string &content) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    reinterpret_cast<ElementJson*>(m_value)->add_suffix_comment(key, content);
    return *this;
}

Comment &Config::get_comment(const size_t index) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    return reinterpret_cast<IElementContainer*>(m_value)->get_comment(index);
}

Comment &Config::get_comment(const std::string &key)
{
    __CHECK_TYPE_IS_CONTAINER__((*this))
    return reinterpret_cast<ElementJson*>(m_value)->get_comment(key);
}

Comment Config::get_comment(const size_t index) const {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    return reinterpret_cast<IElementContainer*>(m_value)->get_comment(index);
}

Comment Config::get_comment(const std::string &key) const
{
    __CHECK_TYPE_IS_CONTAINER__((*this))
    return reinterpret_cast<ElementJson*>(m_value)->get_comment(key);
}

std::string &Config::get_prefix_comment(const size_t index) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    return reinterpret_cast<IElementContainer*>(m_value)->get_prefix_comment(index);
}

std::string Config::get_prefix_comment(const size_t index) const {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    return reinterpret_cast<IElementContainer*>(m_value)->get_prefix_comment(index);
}

std::string &Config::get_prefix_comment(const std::string &key) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    return reinterpret_cast<ElementJson*>(m_value)->get_prefix_comment(key);
}

std::string Config::get_prefix_comment(const std::string &key) const {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    return reinterpret_cast<ElementJson*>(m_value)->get_prefix_comment(key);
}

std::string &Config::get_suffix_comment(const size_t index){
    __CHECK_TYPE_IS_CONTAINER__((*this))
    return reinterpret_cast<IElementContainer*>(m_value)->get_suffix_comment(index);
}

std::string Config::get_suffix_comment(const size_t index) const {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    return reinterpret_cast<IElementContainer*>(m_value)->get_suffix_comment(index);
}

std::string &Config::get_suffix_comment(const std::string &key) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    return reinterpret_cast<ElementJson*>(m_value)->get_suffix_comment(key);
}

std::string Config::get_suffix_comment(const std::string &key) const {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    return reinterpret_cast<ElementJson*>(m_value)->get_suffix_comment(key);
}

Config &Config::clear_comment(const size_t index) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    reinterpret_cast<IElementContainer*>(m_value)->clear_comment(index);
    return *this;
}

Config &Config::clear_comment(const std::string &key) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    reinterpret_cast<ElementJson*>(m_value)->clear_comment(key);
    return *this;
}

Config &Config::clear_prefix_comment(const size_t index) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    reinterpret_cast<IElementContainer*>(m_value)->clear_prefix_comment(index);
    return *this;
}

Config &Config::clear_prefix_comment(const std::string &key) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    reinterpret_cast<ElementJson*>(m_value)->clear_prefix_comment(key);
    return *this;
}

Config &Config::clear_suffix_comment(const size_t index) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    reinterpret_cast<IElementContainer*>(m_value)->clear_suffix_comment(index);
    return *this;
}

Config &Config::clear_suffix_comment(const std::string &key) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    reinterpret_cast<ElementJson*>(m_value)->clear_suffix_comment(key);
    return *this;
}

Config &Config::delete_comment(const size_t index) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    reinterpret_cast<IElementContainer*>(m_value)->delete_comment(index);
    return *this;
}

Config &Config::delete_comment(const std::string &key) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    reinterpret_cast<ElementJson*>(m_value)->delete_comment(key);
    return *this;
}

Config &Config::delete_prefix_comment(const size_t index) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    reinterpret_cast<IElementContainer*>(m_value)->delete_prefix_comment(index);
    return *this;
}

Config &Config::delete_prefix_comment(const std::string &key) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    reinterpret_cast<ElementJson*>(m_value)->delete_prefix_comment(key);
    return *this;
}

Config &Config::delete_suffix_comment(const size_t index) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    reinterpret_cast<IElementContainer*>(m_value)->delete_suffix_comment(index);
    return *this;
}

Config &Config::delete_suffix_comment(const std::string &key) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    reinterpret_cast<ElementJson*>(m_value)->delete_suffix_comment(key);
    return *this;
}

Config &Config::setValue() noexcept {
    release();
    m_value = dynamic_cast<IElement*>(new ElementNull());
    return *this;
}

Config &Config::setValue(const Config &other) noexcept {
    if(this != &other)
    {
        release();
        switch(other.getType()) {
        case ValueType::eNull:      { setValue();                                                               break;  }
        case ValueType::eBool:      { setValue(dynamic_cast<const ElementBool*>(other.m_value)->getValue());    break;  }
        case ValueType::eNumber:    { setValue(dynamic_cast<const ElementNumber*>(other.m_value)->getValue());  break;  }
        case ValueType::eString:    { setValue(dynamic_cast<const ElementString*>(other.m_value)->getValue());  break;  }
        case ValueType::eArray:     { setValue(dynamic_cast<const ElementArray&>(*other.m_value));              break;  }
        case ValueType::eJson:      { setValue(dynamic_cast<const ElementJson&>(*other.m_value));               break;  }
        default: break;
        }

        addComment(other.getComment());
    }

    return *this;
}

//TODO: комментарии всё ещё не учитываются
Config &Config::setValue(Config &&other) noexcept {
    if(this != &other)
    {
        release();

        switch(other.getType()) {
        case ValueType::eNull:      { setValue();                                                                           break;  }
        case ValueType::eBool:      { setValue(dynamic_cast<const ElementBool*>(other.m_value)->getValue());                break;  }
        case ValueType::eNumber:    { setValue(std::move(dynamic_cast<const ElementNumber*>(other.m_value)->getValue()));   break;  }
        case ValueType::eString:    { setValue(std::move(dynamic_cast<const ElementString*>(other.m_value)->getValue()));   break;  }
        case ValueType::eArray:     { setValue(std::move(dynamic_cast<const ElementArray&>(*other.m_value)));               break;  }
        case ValueType::eJson:      { setValue(std::move(dynamic_cast<const ElementJson&>(*other.m_value)));                break;  }
        default:                    break;
        }

        addComment(other.getComment());
        other.release(); //обнулить значение
    }
    return *this;
}

//TODO: сделать проверку на this != other
Config &Config::setValue(const IElement &other) noexcept {
    release();
    switch(other.getType()) {
    case ValueType::eNull:      { setValue();                                                       break;  }
    case ValueType::eBool:      { setValue(dynamic_cast<const ElementBool*>(&other)->getValue());   break;  }
    case ValueType::eNumber:    { setValue(dynamic_cast<const ElementNumber*>(&other)->getValue()); break;  }
    case ValueType::eString:    { setValue(dynamic_cast<const ElementString*>(&other)->getValue()); break;  }
    case ValueType::eArray:     { setValue(dynamic_cast<const ElementArray&>(other));               break;  }
    case ValueType::eJson:      { setValue(dynamic_cast<const ElementJson&>(other));                break;  }
    default:                    break;
    }

    addComment(other.getComment());

    return *this;
}

//TODO: сделать проверку на this != other
Config &Config::setValue(IElement &&other) noexcept {
    release();
    switch(other.getType()) {
    case ValueType::eNull:      { setValue();                                                                   break;  }
    case ValueType::eBool:      { setValue(dynamic_cast<const ElementBool*>(&other)->getValue());               break;  }
    case ValueType::eNumber:    { setValue(std::move(dynamic_cast<const ElementNumber*>(&other)->getValue()));  break;  }
    case ValueType::eString:    { setValue(std::move(dynamic_cast<const ElementString*>(&other)->getValue()));  break;  }
    case ValueType::eArray:     { setValue(std::move(dynamic_cast<ElementArray&&>(other)));                     break;  }
    case ValueType::eJson:      { setValue(std::move(dynamic_cast<ElementJson&&>(other)));                      break;  }
    default:                    break;
    }

    addComment(other.getComment());

    return *this;
}

Config &Config::setValue(const bool other) noexcept {
    release();
    m_value = dynamic_cast<IElement*>(new ElementBool(other));
    return *this;
}

Config &Config::setValue(const long double &other) noexcept {
    release();
    m_value = dynamic_cast<IElement*>(new ElementNumber(other));
    return *this;
}

Config &Config::setValue(long double &&other) noexcept {
    release();
    m_value = dynamic_cast<IElement*>(new ElementNumber(std::move(other)));
    return *this;
}

Config &Config::setValue(const std::string &other) noexcept {
    release();
    m_value = dynamic_cast<IElement*>(new ElementString(other));
    return *this;
}

Config &Config::setValue(std::string &&other) noexcept {
    release();
    m_value = dynamic_cast<IElement*>(new ElementString(std::move(other)));
    return *this;
}

Config &Config::setValue(const ElementArray &other) noexcept {
    release();
    m_value = dynamic_cast<IElement*>(new ElementArray(other));
    return *this;
}

Config &Config::setValue(ElementArray &&other) noexcept {
    release();
    m_value = dynamic_cast<IElement*>(new ElementArray(std::move(other)));
    return *this;
}

Config &Config::setValue(const ElementJson &other) noexcept {
    release();
    m_value = dynamic_cast<IElement*>(new ElementJson(other));
    return *this;
}

Config &Config::setValue(ElementJson &&other) noexcept {
    release();
    m_value = dynamic_cast<IElement*>(new ElementJson(std::move(other)));
    return *this;
}

bool &Config::getBool() {
    __CHECK_TYPE_IS_BOOL__((*this))
    return dynamic_cast<ElementBool*>(m_value)->getValue();
}

bool Config::getBool() const {
    __CHECK_TYPE_IS_BOOL__((*this))
    return dynamic_cast<const ElementBool*>(m_value)->getValue();
}

long double &Config::getNumber() {
    __CHECK_TYPE_IS_NUMBER__((*this))
    return dynamic_cast<ElementNumber*>(m_value)->getValue();
}

long double Config::getNumber() const {
    __CHECK_TYPE_IS_NUMBER__((*this))
    return dynamic_cast<const ElementNumber*>(m_value)->getValue();
}

std::string &Config::getString() {
    __CHECK_TYPE_IS_STRING__((*this))
    return dynamic_cast<ElementString*>(m_value)->getValue();
}

std::string Config::getString() const {
    __CHECK_TYPE_IS_STRING__((*this))
    return dynamic_cast<const ElementString*>(m_value)->getValue();
}

Config &Config::get_front() {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    switch(getType()) {
    case ValueType::eArray: return dynamic_cast<ElementArray*>(m_value)->get_front();
    case ValueType::eJson:  return dynamic_cast<ElementJson*>(m_value)->get_front();
    default:                break;
    }

    return *this;
}

Config Config::get_front() const {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    switch(getType()) {
    case ValueType::eArray: return dynamic_cast<const ElementArray*>(m_value)->get_front();
    case ValueType::eJson:  return dynamic_cast<const ElementJson*>(m_value)->get_front();
    default:                break;
    }

    return *this;
}

Config &Config::get_at(const size_t index) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    switch(getType()) {
    case ValueType::eArray: return dynamic_cast<ElementArray*>(m_value)->get_at(index);
    case ValueType::eJson:  return dynamic_cast<ElementJson*>(m_value)->get_at(index);
    default:                break;
    }

    return *this;
}

Config Config::get_at(const size_t index) const {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    switch(getType()) {
    case ValueType::eArray: return dynamic_cast<const ElementArray*>(m_value)->get_at(index);
    case ValueType::eJson:  return dynamic_cast<const ElementJson*>(m_value)->get_at(index);
    default:                break;
    }

    return *this;
}

Config &Config::get_at(const std::vector<size_t> &indexes) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    if(indexes.empty())
        throw std::invalid_argument("indexes argument cannot be empty");

    Config* cfg = nullptr;
    __CHECK_INDEX_BOUND__((*this), indexes[0])
    switch(getType()) {
    case ValueType::eArray: { cfg = &dynamic_cast<ElementArray*>(m_value)->get_at(indexes[0]);      break;  }
    case ValueType::eJson:  { cfg = &dynamic_cast<ElementJson*>(m_value)->get_at(indexes[0]);       break;  }
    default: break;
    }

    if(cfg) {
        if(indexes.size() == 1) {
            return *cfg;
        } else {
            std::vector<size_t> new_indexes;
            new_indexes.assign(indexes.cbegin() + 1, indexes.cend());

            switch(cfg->getType()) {
            case ValueType::eArray: return cfg->get_at(new_indexes);
            case ValueType::eJson:  return cfg->get_at(new_indexes);
            default: break;;
            }
        }
    }

    return *this;
}

Config Config::get_at(const std::vector<size_t> &indexes) const {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    if(indexes.empty())
        throw std::invalid_argument("indexes argument cannot be empty");

    Config cfg = Config();
    __CHECK_INDEX_BOUND__((*this), indexes[0])
    switch(getType()) {
    case ValueType::eArray: { cfg = std::move(dynamic_cast<const ElementArray*>(m_value)->get_at(indexes[0]));  break;  }
    case ValueType::eJson:  { cfg = std::move(dynamic_cast<const ElementJson*>(m_value)->get_at(indexes[0]));   break;  }
    default: break;
    }

    if(!cfg.isNull()) {
        if(indexes.size() == 1) {
            return cfg;
        } else {
            std::vector<size_t> new_indexes;
            new_indexes.assign(indexes.cbegin() + 1, indexes.cend());

            switch(cfg.getType()) {
            case ValueType::eArray: return cfg.get_at(new_indexes);
            case ValueType::eJson:  return cfg.get_at(new_indexes);
            default: break;;
            }
        }
    } else {
        __CHECK_TYPE_IS_CONTAINER__(cfg)
    }

    return *this;
}

Config &Config::get_at(const std::string& key) {
    __CHECK_TYPE_IS_JSON__((*this))
    return dynamic_cast<ElementJson*>(m_value)->get_at(key);
}

Config Config::get_at(const std::string& key) const {
    __CHECK_TYPE_IS_JSON__((*this))
    return dynamic_cast<const ElementJson*>(m_value)->get_at(key);
}

Config &Config::get_at(const VString &complex_key) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    if(complex_key.empty())
        throw std::invalid_argument("complex_key argument cannot be empty");

    //ключ может быть либо строкой, либо целым числом
    const std::string& current_key = complex_key.front();
    size_t current_index;
    bool index_parsed = utils::IsStringOfUIntNumber(current_key, current_index);

    Config* cfg = nullptr;
    if(index_parsed) {
        switch (getType()) {
        case ValueType::eArray: { cfg = &dynamic_cast<ElementArray*>(m_value)->get_at(current_index);   break;  }
        case ValueType::eJson:  { cfg = &dynamic_cast<ElementJson*>(m_value)->get_at(current_index);    break;  }
        default: break;
        }
    } else {
        __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
        //TODO: switch(getNamedMapType())
        switch(getType()) {
        case ValueType::eJson:  { cfg = &dynamic_cast<ElementJson*>(m_value)->get_at(current_key);      break;  }
        default: break;
        }
    }

    if(cfg) {
        if(complex_key.size() == 1) {
            return *cfg;
        } else {
            VString new_complex_key;
            new_complex_key.assign(complex_key.cbegin() + 1, complex_key.cend());

            switch (cfg->getType()) {
            case ValueType::eArray: return cfg->get_at(new_complex_key);
            case ValueType::eJson:  return cfg->get_at(new_complex_key);
            default: break;
            }
        }
    }

    //NOTE: в идеале, до этого кода доходить не должно никогда - либо свичи выше, либо exception
    return *this;
}

Config Config::get_at(const VString &complex_key) const {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    if(complex_key.empty())
        throw std::invalid_argument("complex_key argument cannot be empty");

    //ключ может быть либо строкой, либо целым числом
    const std::string& current_key = complex_key.front();
    size_t current_index;
    bool index_parsed = utils::IsStringOfUIntNumber(current_key, current_index);

    Config cfg = Config();
    if(index_parsed) {
        switch (getType()) {
        case ValueType::eArray: { cfg = dynamic_cast<const ElementArray*>(m_value)->get_at(current_index);  break;  }
        case ValueType::eJson:  { cfg = dynamic_cast<const ElementJson*>(m_value)->get_at(current_index);   break;  }
        default: break;
        }
    } else {
        __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
        //TODO: switch(getNamedMapType())
        switch(getType()) {
        case ValueType::eJson:  { cfg = dynamic_cast<const ElementJson*>(m_value)->get_at(current_key);     break;  }
        default: break;
        }
    }

    if(!cfg.isNull()) {
        if(complex_key.size() == 1) {
            return cfg;
        } else {
            VString new_complex_key;
            new_complex_key.assign(complex_key.cbegin() + 1, complex_key.cend());

            switch (cfg.getType()) {
            case ValueType::eArray: return cfg.get_at(new_complex_key);
            case ValueType::eJson:  return cfg.get_at(new_complex_key);
            default: break;
            }
        }
    } else {
        __CHECK_TYPE_IS_CONTAINER__(cfg)
    }

    //NOTE: в идеале, до этого кода доходить не должно никогда - либо свичи выше, либо exception
    return *this;
}

Config &Config::get_back() {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    switch(getType()) {
    case ValueType::eArray: return dynamic_cast<ElementArray*>(m_value)->get_back();
    case ValueType::eJson:  return dynamic_cast<ElementJson*>(m_value)->get_back();
    default:                break;
    }

    return *this;
}

Config Config::get_back() const {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    switch(getType()) {
    case ValueType::eArray: return dynamic_cast<const ElementArray*>(m_value)->get_back();
    case ValueType::eJson:  return dynamic_cast<const ElementJson*>(m_value)->get_back();
    default:                break;
    }

    return *this;
}

bool &Config::get_front_bool() {
    Config& config = get_front();
    __CHECK_TYPE_IS_BOOL__(config)

    return dynamic_cast<ElementBool*>(config.m_value)->getValue();
}

bool Config::get_front_bool() const {
    const Config& config = get_front();
    __CHECK_TYPE_IS_BOOL__(config)

    return dynamic_cast<const ElementBool*>(config.m_value)->getValue();
}

long double &Config::get_front_number() {
    Config& config = get_front();
    __CHECK_TYPE_IS_NUMBER__(config)

    return dynamic_cast<ElementNumber*>(config.m_value)->getValue();
}

long double Config::get_front_number() const {
    const Config& config = get_front();
    __CHECK_TYPE_IS_NUMBER__(config)

    return dynamic_cast<const ElementNumber*>(config.m_value)->getValue();
}

std::string &Config::get_front_string() {
    Config& config = get_front();
    __CHECK_TYPE_IS_STRING__(config)

    return dynamic_cast<ElementString*>(config.m_value)->getValue();
}

std::string Config::get_front_string() const {
    const Config& config = get_front();
    __CHECK_TYPE_IS_STRING__(config)

    return dynamic_cast<const ElementString*>(config.m_value)->getValue();
}

bool &Config::get_bool_at(const size_t index) {
    const Config& config = get_at(index);
    __CHECK_TYPE_IS_BOOL__(config)

    return dynamic_cast<ElementBool*>(config.m_value)->getValue();
}

bool Config::get_bool_at(const size_t index) const {
    const Config& config = get_at(index);
    __CHECK_TYPE_IS_BOOL__(config)

    return dynamic_cast<const ElementBool*>(config.m_value)->getValue();
}

bool &Config::get_bool_at(const std::vector<size_t> &indexes) {
    Config& cfg = get_at(indexes);
    __CHECK_TYPE_IS_BOOL__(cfg)
    return dynamic_cast<ElementBool*>(cfg.m_value)->getValue();
}

bool Config::get_bool_at(const std::vector<size_t> &indexes) const {
    const Config& cfg = get_at(indexes);
    __CHECK_TYPE_IS_BOOL__(cfg)
    return dynamic_cast<ElementBool*>(cfg.m_value)->getValue();
}

long double &Config::get_number_at(const size_t index) {
    const Config& config = get_at(index);
    __CHECK_TYPE_IS_NUMBER__(config)

    return dynamic_cast<ElementNumber*>(config.m_value)->getValue();
}

long double Config::get_number_at(const size_t index) const {
    const Config& config = get_at(index);
    __CHECK_TYPE_IS_NUMBER__(config)

    return dynamic_cast<const ElementNumber*>(config.m_value)->getValue();
}

long double &Config::get_number_at(const std::vector<size_t> &indexes) {
    Config& cfg = get_at(indexes);
    __CHECK_TYPE_IS_NUMBER__(cfg)

    return dynamic_cast<ElementNumber*>(cfg.m_value)->getValue();
}

long double Config::get_number_at(const std::vector<size_t> &indexes) const {
    const Config& cfg = get_at(indexes);

    __CHECK_TYPE_IS_NUMBER__(cfg)
    return dynamic_cast<ElementNumber*>(cfg.m_value)->getValue();
}

std::string &Config::get_string_at(const size_t index) {
    const Config& config = get_at(index);
    __CHECK_TYPE_IS_STRING__(config)

    return dynamic_cast<ElementString*>(config.m_value)->getValue();
}

std::string Config::get_string_at(const size_t index) const {
    const Config& config = get_at(index);
    __CHECK_TYPE_IS_STRING__(config)

    return dynamic_cast<const ElementString*>(config.m_value)->getValue();
}

std::string &Config::get_string_at(const std::vector<size_t> &indexes) {
    Config& cfg = get_at(indexes);
    __CHECK_TYPE_IS_STRING__(cfg)

    return dynamic_cast<ElementString*>(cfg.m_value)->getValue();
}

std::string Config::get_string_at(const std::vector<size_t> &indexes) const {
    const Config& cfg = get_at(indexes);
    __CHECK_TYPE_IS_STRING__(cfg)

    return dynamic_cast<ElementString*>(cfg.m_value)->getValue();
}

bool &Config::get_bool_at(const std::string& key) {
    const Config& config = get_at(key);
    __CHECK_TYPE_IS_BOOL__(config)

    return dynamic_cast<ElementBool*>(config.m_value)->getValue();
}

bool Config::get_bool_at(const std::string& key) const {
    const Config& config = get_at(key);
    __CHECK_TYPE_IS_BOOL__(config)

    return dynamic_cast<const ElementBool*>(config.m_value)->getValue();
}

bool &Config::get_bool_at(const VString &complex_key) {
    Config& cfg = get_at(complex_key);
    __CHECK_TYPE_IS_BOOL__(cfg)

    return dynamic_cast<ElementBool*>(cfg.m_value)->getValue();
}

bool Config::get_bool_at(const VString &complex_key) const {
    const Config& cfg = get_at(complex_key);
    __CHECK_TYPE_IS_BOOL__(cfg)

    return dynamic_cast<ElementBool*>(cfg.m_value)->getValue();
}

long double &Config::get_number_at(const std::string& key) {
    const Config& config = get_at(key);
    __CHECK_TYPE_IS_NUMBER__(config)

    return dynamic_cast<ElementNumber*>(config.m_value)->getValue();
}

long double Config::get_number_at(const std::string& key) const {
    const Config& config = get_at(key);
    __CHECK_TYPE_IS_NUMBER__(config)

    return dynamic_cast<const ElementNumber*>(config.m_value)->getValue();
}

long double &Config::get_number_at(const VString &complex_key) {
    Config& cfg = get_at(complex_key);
    __CHECK_TYPE_IS_NUMBER__(cfg)

    return dynamic_cast<ElementNumber*>(cfg.m_value)->getValue();
}

long double Config::get_number_at(const VString &complex_key) const {
    const Config& cfg = get_at(complex_key);
    __CHECK_TYPE_IS_NUMBER__(cfg)

    return dynamic_cast<ElementNumber*>(cfg.m_value)->getValue();
}

std::string &Config::get_string_at(const std::string& key) {
    const Config& config = get_at(key);
    __CHECK_TYPE_IS_STRING__(config)

    return dynamic_cast<ElementString*>(config.m_value)->getValue();
}

std::string Config::get_string_at(const std::string& key) const {
    const Config& config = get_at(key);
    __CHECK_TYPE_IS_STRING__(config)

    return dynamic_cast<const ElementString*>(config.m_value)->getValue();
}

std::string &Config::get_string_at(const VString &complex_key) {
    Config& cfg = get_at(complex_key);
    __CHECK_TYPE_IS_STRING__(cfg)

    return dynamic_cast<ElementString*>(cfg.m_value)->getValue();
}

std::string Config::get_string_at(const VString &complex_key) const {
    const Config& cfg = get_at(complex_key);
    __CHECK_TYPE_IS_STRING__(cfg)

    return dynamic_cast<ElementString*>(cfg.m_value)->getValue();
}

bool &Config::get_bool_back() {
    const Config& config = get_back();
    __CHECK_TYPE_IS_BOOL__(config)

    return dynamic_cast<ElementBool*>(config.m_value)->getValue();
}

bool Config::get_bool_back() const {
    const Config& config = get_back();
    __CHECK_TYPE_IS_BOOL__(config)

    return dynamic_cast<const ElementBool*>(config.m_value)->getValue();
}

long double &Config::get_number_back() {
    const Config& config = get_back();
    __CHECK_TYPE_IS_NUMBER__(config)

    return dynamic_cast<ElementNumber*>(config.m_value)->getValue();
}

long double Config::get_number_back() const {
    const Config& config = get_back();
    __CHECK_TYPE_IS_NUMBER__(config)

    return dynamic_cast<const ElementNumber*>(config.m_value)->getValue();
}

std::string &Config::get_string_back() {
    const Config& config = get_back();
    __CHECK_TYPE_IS_STRING__(config)

    return dynamic_cast<ElementString*>(config.m_value)->getValue();
}

std::string Config::get_string_back() const {
    const Config& config = get_back();
    __CHECK_TYPE_IS_STRING__(config)

    return dynamic_cast<const ElementString*>(config.m_value)->getValue();
}

Config& Config::insert_front(const Config& other) {
    Config config(other);
    return insert_front(std::move(config));
}

Config& Config::insert_front(Config&& other) {
    __CHECK_TYPE_IS_INDEX_CONTAINER__((*this))
    dynamic_cast<ElementArray*>(m_value)->insert_front(std::move(other));

    return *this;
}

Config& Config::insert_front(const std::string& key, const Config& other) {
    Config config(other);
    return insert_front(key, std::move(config));
}

Config& Config::insert_front(const std::string& key, Config&& other) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    dynamic_cast<ElementJson*>(m_value)->insert_front(key, std::move(other));

    return *this;
}

Config& Config::insert_at(const size_t index, const Config& other) {
    Config config(other);
    return insert_at(index, std::move(config));
}

Config& Config::insert_at(const size_t index, Config&& other) {
    __CHECK_TYPE_IS_INDEX_CONTAINER__((*this))
    dynamic_cast<ElementArray*>(m_value)->insert_at(index, std::move(other));

    return *this;
}

Config& Config::insert_at(const std::string& key, const Config& other) {
    Config config(other);
    return insert_at(key, std::move(config));
}

Config& Config::insert_at(const std::string& key, Config&& other) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    dynamic_cast<ElementJson*>(m_value)->insert_at(key, std::move(other));

    return *this;
}

Config& Config::insert_back(const Config& other) {
    Config config(other);
    return insert_back(std::move(config));
}

Config& Config::insert_back(Config&& other) {
    __CHECK_TYPE_IS_INDEX_CONTAINER__((*this))
    dynamic_cast<ElementArray*>(m_value)->insert_back(std::move(other));

    return *this;
}

Config& Config::insert_back(const std::string& key, const Config& other) {
    Config config(other);
    return insert_back(key, std::move(config));
}

Config& Config::insert_back(const std::string& key, Config&& other) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    dynamic_cast<ElementJson*>(m_value)->insert_back(key, std::move(other));

    return *this;
}

Config& Config::insert_after(const std::string& after_key, const std::string& key, const Config& other) {
    Config config(other);
    return insert_after(after_key, key, std::move(config));
}

Config& Config::insert_after(const std::string& after_key, const std::string& key, Config&& other) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    dynamic_cast<ElementJson*>(m_value)->insert_after(after_key, key, std::move(other));

    return *this;
}

Config &Config::append(const Config &config) {
    Config cfg_copy(config);
    return append(std::move(cfg_copy));
}

Config &Config::append(Config &&config) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    __CHECK_TYPES_IS_EQUAL__((*this), config)

    switch(getType()) {
    case ValueType::eArray: {
        dynamic_cast<ElementArray*>(m_value)->append(std::move(dynamic_cast<ElementArray&&>(std::move(*config.m_value))));
        break;
    }
    case ValueType::eJson: {
        dynamic_cast<ElementJson*>(m_value)->append(std::move(dynamic_cast<ElementJson&&>(std::move(*config.m_value))));
        break;
    }
    default: throw std::invalid_argument("Config::append(): unexpected type of config: " + ToString(config.getType()));
    }

    return *this;

    return *this;
}

bool Config::isEqual(const IElement &other, const bool compare_comments,
                     const bool map_sort_important) const noexcept
{
    __CHECK_TYPES_NOT_EQUAL_ACTION__((*m_value), other)
        return false;
    return m_value->isEqual(other, compare_comments, map_sort_important);
}

bool Config::isEqual(const bool other) const noexcept {
    __CHECK_TYPE_NOT_BOOL_ACTION__((*this))
        return false;
    return dynamic_cast<const ElementBool*>(m_value)->getValue() == other;
}

bool Config::isEqual(const long double &other) const noexcept {
    __CHECK_TYPE_NOT_NUMBER_ACTION__((*this))
        return false;
    return dynamic_cast<const ElementNumber*>(m_value)->getValue() == other;
}

bool Config::isEqual(const std::string &other) const noexcept {
    __CHECK_TYPE_NOT_STRING_ACTION__((*this))
        return false;
    return dynamic_cast<const ElementString*>(m_value)->getValue() == other;
}

Config& Config::erase_front() {
    erase_at(0);
    return *this;
}

//если элемента не существует - проигнорировать
Config& Config::erase_at(const size_t index) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    if(!isEmpty()) {
        switch(getType()){
        case ValueType::eArray: {
            dynamic_cast<ElementArray*>(m_value)->erase_at(index);
            break;
        }
        case ValueType::eJson: {
            dynamic_cast<ElementJson*>(m_value)->erase_at(index);
            break;
        }
        default: throw std::invalid_argument("unknown type for remove");
        }
    }

    return *this;
}

Config& Config::erase_at(const std::string& key) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    dynamic_cast<ElementJson*>(m_value)->erase_at(key);

    return *this;
}

Config& Config::erase_back() {
    erase_at(size() - 1); //даже если индекса не существует - exception не будет, только если тип неверный
    return *this;
}

Config Config::get_and_pop_front() {
    Config config = get_front();
    erase_front();

    return config;
}

Config Config::get_and_pop_at(const size_t index) {
    Config config = get_at(index);
    erase_at(index);

    return config;
}

Config Config::get_and_pop_at(const std::string& key) {
    Config config = get_at(key);
    erase_at(key);

    return config;
}

Config Config::get_and_pop_back() {
    Config config = get_back();
    erase_back();

    return config;
}

bool Config::containsValue(const Config &config) const noexcept {
    if(!isContainer()) return false;

    //FIXME: warning предлагает сделать всё через std::any_of, но там муть одна. Непонятно.
    switch(getType()) {
    default:
    case ValueType::eNull:
    case ValueType::eBool:
    case ValueType::eNumber:
    case ValueType::eString:    return false;
    case ValueType::eArray: {
        for(const auto& cfg : getRange()) {
            if(*cfg == config)
                return true;
        }
        break;
    }
    case ValueType::eJson: {
        for(const auto& pair : getNamedRange()) {
            if(*pair.second == config)
                return true;
        }
        break;
    }
    }

    return false;
}

bool Config::containsKey(const std::string &key) const noexcept {
    if(!isMapContainer()) return false;

//    for(const auto& pair : getNamedRange()) {
//        if(pair.first == key)
//            return true;
//    }

    //FIXME: warning предлагает сделать всё через std::any_of, но там муть одна. Непонятно.
    std::any_of(getNamedRange().cbegin(), getNamedRange().cend(),
                [&key](const std::pair<std::string, std::shared_ptr<Config>>& pair) {
                    return pair.first == key;
                });

    return false;
}

std::string Config::toString(const ConfigFormat format, const CommentDesign &design,
                             const int8_t tabulation_level) const noexcept
{
    return m_value->toString(format, design, tabulation_level);
}

std::ostream &operator<<(std::ostream &os, const Config &config) noexcept {
    os << config.toString();
    return os;
}

std::ostream &operator<<(std::ostream &os, const IElement &config) noexcept {
    os << config.toString();
    return os;
}

Config &Config::readFile(const std::string &file_path, const ConfigFormat format,
                         const bool with_comments, std::string *error_log)
{
    *this = ReadFile(file_path, format, with_comments, error_log);
    return *this;
}

Config &Config::readFileJson(const std::string &file_path, const bool with_comments,
                             std::string *error_log)
{
    *this = ReadFileJson(file_path, with_comments, error_log);
    return *this;
}

Config &Config::readFileIni(const std::string &file_path, const bool with_comments,
                            std::string *error_log)
{
    *this = ReadFileIni(file_path, with_comments, error_log);
    return *this;
}

bool Config::writeFile(const std::string &file_path,  const ConfigFormat format,
                       const CommentDesign &design,
                       const int8_t custom_tabulation_level) noexcept
{
    return WriteFile(*this, file_path, format, design, custom_tabulation_level);
}

bool Config::writeFileJson(const std::string &file_path, const CommentDesign &design,
                           const int8_t custom_tabulation_level) noexcept
{
    return WriteFileJson(*this, file_path, design, custom_tabulation_level);
}

bool Config::writeFileIni(const std::string &file_path, const CommentDesign &design,
                          const int8_t custom_tabulation_level) noexcept
{
    return WriteFileIni(*this, file_path, design, custom_tabulation_level);
}

Config &Config::parse(const std::string &content, const ConfigFormat format,
                      const bool with_comments, std::string *error_log)
{
    release();
    *this = Parse(content, format, with_comments, error_log);
    return *this;
}

Config &Config::parseArray(const std::string &content, const bool with_comments,
                           const int8_t tabulation_level, std::string* error_log)
{
    release();
    m_value = new ElementArray(content, ConfigFormat::eJSON, with_comments);
    return *this;
}

Config &Config::parseJson(const std::string &content, const bool with_comments,
                          const int8_t tabulation_level, std::string* error_log)
{
    release();
    m_value = new ElementJson(content, ConfigFormat::eJSON, with_comments);
    return *this;
}

Config &Config::parseIni(const std::string &content, const bool with_comments,
                         const int8_t tabulation_level, std::string* error_log)
{
    release();
    *this = ParseIni(content, with_comments, tabulation_level, error_log);
    return *this;
}

shared_VElement::iterator Config::array_begin() {
    __CHECK_TYPE_IS_ARRAY__((*this))
    return dynamic_cast<ElementArray*>(m_value)->begin();
}

shared_VElement::const_iterator Config::array_cbegin() const {
    __CHECK_TYPE_IS_ARRAY__((*this))
    return dynamic_cast<const ElementArray*>(m_value)->cbegin();
}

shared_VElement::iterator Config::array_end() {
    __CHECK_TYPE_IS_ARRAY__((*this))
    return dynamic_cast<ElementArray*>(m_value)->end();
}

shared_VElement::const_iterator Config::array_cend() const {
    __CHECK_TYPE_IS_ARRAY__((*this))
    return dynamic_cast<const ElementArray*>(m_value)->cend();
}

shared_VPairElement::iterator Config::named_begin() {
    __CHECK_TYPE_IS_JSON__((*this))
    return dynamic_cast<ElementJson*>(m_value)->begin();
}

shared_VPairElement::const_iterator Config::named_cbegin() const {
    __CHECK_TYPE_IS_JSON__((*this))
    return dynamic_cast<const ElementJson*>(m_value)->cbegin();
}

shared_VPairElement::iterator Config::named_end() {
    __CHECK_TYPE_IS_JSON__((*this))
    return dynamic_cast<ElementJson*>(m_value)->end();
}

shared_VPairElement::const_iterator Config::named_cend() const {
    __CHECK_TYPE_IS_JSON__((*this))
    return dynamic_cast<const ElementJson*>(m_value)->cend();
}

Config Config::CreateElementFromString(std::string &&value_string, const ConfigFormat format,
                                       const CommentDesign& design, const int8_t tabulation_level)
{
    using namespace utils;
    //удаление незначащих пробелов
    RemoveIllegalSpaces(value_string);

    std::string temp;
    auto Append = [&](const char c) {
        temp += std::tolower(c);
    };

    //проверка типа, по порядку
    /*NULL*/ {
        if(value_string.empty())
            return Config();
        if(value_string.size() == 4) {
            for(auto ch : value_string)
                Append(ch);
            if(temp == "null")  return Config();
            temp.clear();
        }
    }
    /*BOOL*/ {
        if(value_string.size() == 4 || value_string.size() == 5) {
            for(auto ch : value_string)
                Append(ch);
            if(temp == "true")  return Config(true);
            if(temp == "false") return Config(false);
            temp.clear();
        }
        if(value_string.size() == 1) {
            if(value_string == "T" || value_string == "t" || value_string == "+")  return Config(true);
            if(value_string == "F" || value_string == "f" || value_string == "-")  return Config(false);
        }
    }
    char first = value_string.front();
    char last = value_string.back();
    /*NUMBER*/ {
        try {
            std::regex reg("^[+-]?[0-9]+[.]?[0-9]*[eE]?[+-]?[0-9]*[fF]?$");
            if(std::regex_match(value_string, reg))
                return Config(std::stold(value_string));
        } catch (...) {}
    }
    /*STRING*/ {
        if(first == '"' && last == '"') {
            value_string.erase(0, 1);
            value_string.pop_back();
            return Config(value_string);
        }
    }
    int8_t new_tab_lvl = tabulation_level == -1 ? -1 : tabulation_level + 1;
    /*ARRAY*/ {
        if(first == '[' && last == ']') {
            try {
                Config array;
                array.setCommentDesign(design);
                array.parseArray(value_string, design.with_comments, new_tab_lvl);
                return array;
            } catch(...) {}
        }
    }
    /*JSON*/ {
        if(first == '{' && last == '}') {
            try {
                Config json;
                json.setCommentDesign(design);
                json.parseJson(value_string, design.with_comments, new_tab_lvl);
                return Config(json);
            } catch(...) {}
        }
    }

    // в теории, всё, что не распарсилось в другие значения, - должно считаться строкой
    return Config(value_string);
}



Config ReadFile(const std::string &file_path, const ConfigFormat format,
                const bool with_comments, std::string *error_log)
{
    switch(format) {
    case ConfigFormat::eJSON:   return ReadFileJson(file_path, with_comments, error_log);
    case ConfigFormat::eINI:    return ReadFileIni(file_path, with_comments, error_log);
//    case ConfigFormat::eYAML:
//    case ConfigFormat::eXML:
    default:                    return Config{};
    }
}

Config ReadFileJson(const std::string &file_path, const bool with_comments,
                    std::string *error_log)
{
    std::string input_str;
    if(GetAllStringsFromFile(file_path, input_str)) {
        Config ret = ParseJson(input_str, with_comments, 0, error_log);
        return ret;
    }

    return Config{};
}

Config ReadFileIni(const std::string &file_path, const bool with_comments,
                    std::string *error_log)
{
    std::string input_str;
    if(GetAllStringsFromFile(file_path, input_str)) {
        return ParseIni(input_str, with_comments, 0, error_log);
    }

    return Config{};
}

//NOTE: eONLY_VALUE выводит переменную в формате JSON без пробелов и комментариев
bool WriteFile(const Config& config, const std::string& file_path,
               const ConfigFormat format, const CommentDesign &design,
               const uint8_t custom_tabulation_level) noexcept
{
    return WriteStringToFile(file_path, config.toString(format, design, custom_tabulation_level));
}

bool WriteFileJson(const Config& config, const std::string& file_path,
                   const CommentDesign &design, const uint8_t custom_tabulation_level) noexcept
{
    return WriteStringToFile(file_path, config.toString(ConfigFormat::eJSON, design, custom_tabulation_level));
}

bool WriteFileIni(const Config& config, const std::string& file_path,
                  const CommentDesign &design, const uint8_t custom_tabulation_level) noexcept
{
    return WriteStringToFile(file_path, config.toString(ConfigFormat::eINI, design, custom_tabulation_level));
}

bool WriteStringToFile(const std::string& file_path, std::string&& content) noexcept
{
    std::ofstream file;
    file.open(file_path, std::ios::out);
    if(!file.is_open()) {
        std::cerr << "cannot create file: \"" << file_path << "\"" << std::endl;
        return false;
    }

    try {
        file.write(content.c_str(), content.length());
    } catch(std::exception& e) {
        std::cerr << "exception while \"" << file_path << "\" writing: " << strerror(errno) << std::endl;
        file.close();
        return false;
    }

    file.close();
    return true;
}

//TODO: запретить пользователю считывать массивы?
Config Parse(const std::string &content, const ConfigFormat format,
             const bool with_comments, std::string *error_log)
{
    switch(format) {
    case ConfigFormat::eJSON:   return ParseJson(content, with_comments, 0, error_log);
    case ConfigFormat::eINI:    return ParseIni(content, with_comments, 0, error_log);
//    case ConfigFormat::eYAML:
//    case ConfigFormat::eXML:
    default:                    return Config{};
    }
}

Config ParseArray(const std::string &content, const bool with_comments,
                 const int8_t tabulation_level, std::string* error_log)
{
    //TODO: ParseArray
    return {};
}

Config ParseJson(const std::string &content, const bool with_comments,
                 const int8_t tabulation_level, std::string* error_log)
{
    Config ret(ValueType::eJson);
    ret.parseJson(content, with_comments, tabulation_level, error_log);

    return ret;
}

Config ParseIni(const std::string &content, const bool with_comments,
                const int8_t tabulation_level, std::string* error_log)
{
    Config ret(ValueType::eJson);
    ret.parseIni(content, with_comments, tabulation_level, error_log);

    return {};
}
