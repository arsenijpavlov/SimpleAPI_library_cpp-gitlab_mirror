#include "Config.h"

#include "ElementNull.h"
#include "ElementBool.h"
#include "ElementNumber.h"
#include "ElementString.h"
#include "ElementArray.h"
#include "ElementJson.h"

#include <regex>
//#include "ElementYaml.h"
//#include "ElementXml.h"


Config &Config::setValue() noexcept {
    if(m_value) delete m_value;

    m_value = dynamic_cast<IElement*>(new ElementNull());
    return *this;
}

Config &Config::setValue(const Config &other) noexcept {
    if(m_value) delete m_value;

    switch(other.getType()) {
    case ValueType::eNull:      { return setValue();                                                                }
    case ValueType::eBool:      { return setValue(dynamic_cast<const ElementBool*>(other.m_value)->getValue());     }
    case ValueType::eNumber:    { return setValue(dynamic_cast<const ElementNumber*>(other.m_value)->getValue());   }
    case ValueType::eString:    { return setValue(dynamic_cast<const ElementString*>(other.m_value)->getValue());   }
    case ValueType::eArray:     { return setValue(dynamic_cast<const ElementArray&>(*other.m_value));               }
    case ValueType::eJson:      { return setValue(dynamic_cast<const ElementJson&>(*other.m_value));                }
    default: break;
    }

    return *this;
}

Config &Config::setValue(Config &&other) noexcept {
    if(m_value) delete m_value;

    switch(other.getType()) {
    case ValueType::eNull:      { return setValue();                                                                            }
    case ValueType::eBool:      { return setValue(dynamic_cast<const ElementBool*>(other.m_value)->getValue());                 }
    case ValueType::eNumber:    { return setValue(std::move(dynamic_cast<const ElementNumber*>(other.m_value)->getValue()));    }
    case ValueType::eString:    { return setValue(std::move(dynamic_cast<const ElementString*>(other.m_value)->getValue()));    }
    case ValueType::eArray:     { return setValue(std::move(dynamic_cast<const ElementArray&>(*other.m_value)));                }
    case ValueType::eJson:      { return setValue(std::move(dynamic_cast<const ElementJson&>(*other.m_value)));                 }
    default:                    break;
    }

    other.init(); //обнулить значение
    return *this;
}

Config &Config::setValue(const IElement &other) noexcept {
    if(m_value) delete m_value;

    switch(other.getType()) {
    case ValueType::eNull:      { return setValue();                                                        }
    case ValueType::eBool:      { return setValue(dynamic_cast<const ElementBool*>(&other)->getValue());    }
    case ValueType::eNumber:    { return setValue(dynamic_cast<const ElementNumber*>(&other)->getValue());  }
    case ValueType::eString:    { return setValue(dynamic_cast<const ElementString*>(&other)->getValue());  }
    case ValueType::eArray:     { return setValue(dynamic_cast<const ElementArray&>(other));                }
    case ValueType::eJson:      { return setValue(dynamic_cast<const ElementJson&>(other));                 }
    default:                    break;
    }

    return *this;
}

Config &Config::setValue(IElement &&other) noexcept {
    if(m_value) delete m_value;

    switch(other.getType()) {
    case ValueType::eNull:      { return setValue();                                                                    }
    case ValueType::eBool:      { return setValue(dynamic_cast<const ElementBool*>(&other)->getValue());                }
    case ValueType::eNumber:    { return setValue(std::move(dynamic_cast<const ElementNumber*>(&other)->getValue()));   }
    case ValueType::eString:    { return setValue(std::move(dynamic_cast<const ElementString*>(&other)->getValue()));   }
    case ValueType::eArray:     { return setValue(std::move(dynamic_cast<ElementArray&&>(other)));                      }
    case ValueType::eJson:      { return setValue(std::move(dynamic_cast<ElementJson&&>(other)));                       }
    default:                    break;
    }

    return *this;
}

Config &Config::setValue(const bool other) noexcept {
    if(m_value) delete m_value;

    m_value = dynamic_cast<IElement*>(new ElementBool(other));
    return *this;
}

Config &Config::setValue(const long double &other) noexcept {
    if(m_value) delete m_value;

    m_value = dynamic_cast<IElement*>(new ElementNumber(other));
    return *this;
}

Config &Config::setValue(long double &&other) noexcept {
    if(m_value) delete m_value;

    m_value = dynamic_cast<IElement*>(new ElementNumber(std::move(other)));
    return *this;
}

Config &Config::setValue(const std::string &other) noexcept {
    if(m_value) delete m_value;

    m_value = dynamic_cast<IElement*>(new ElementString(other));
    return *this;
}

Config &Config::setValue(std::string &&other) noexcept {
    if(m_value) delete m_value;

    m_value = dynamic_cast<IElement*>(new ElementString(std::move(other)));
    return *this;
}

Config &Config::setValue(const ElementArray &other) noexcept {
    if(m_value) delete m_value;

    m_value = dynamic_cast<IElement*>(new ElementArray(other));
    return *this;
}

Config &Config::setValue(ElementArray &&other) noexcept {
    if(m_value) delete m_value;

    m_value = dynamic_cast<IElement*>(new ElementArray(std::move(other)));
    return *this;
}

Config &Config::setValue(const ElementJson &other) noexcept {
    if(m_value) delete m_value;

    m_value = dynamic_cast<IElement*>(new ElementJson(other));
    return *this;
}

Config &Config::setValue(ElementJson &&other) noexcept {
    if(m_value) delete m_value;

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
    __CHECK_TYPE_IS_CONTAINER__((*this))

    Config& config = get_front();
    __CHECK_TYPE_IS_BOOL__(config)

    return dynamic_cast<ElementBool*>(config.m_value)->getValue();
}

bool Config::get_front_bool() const {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    const Config& config = get_front();
    __CHECK_TYPE_IS_BOOL__(config)

    return dynamic_cast<const ElementBool*>(config.m_value)->getValue();
}

long double &Config::get_front_number() {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    Config& config = get_front();
    __CHECK_TYPE_IS_NUMBER__(config)

    return dynamic_cast<ElementNumber*>(config.m_value)->getValue();
}

long double Config::get_front_number() const {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    const Config& config = get_front();
    __CHECK_TYPE_IS_NUMBER__(config)

    return dynamic_cast<const ElementNumber*>(config.m_value)->getValue();
}

std::string &Config::get_front_string() {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    Config& config = get_front();
    __CHECK_TYPE_IS_STRING__(config)

    return dynamic_cast<ElementString*>(config.m_value)->getValue();
}

std::string Config::get_front_string() const {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    const Config& config = get_front();
    __CHECK_TYPE_IS_STRING__(config)

    return dynamic_cast<const ElementString*>(config.m_value)->getValue();
}

bool &Config::get_bool_at(const size_t index) {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    const Config& config = get_at(index);
    __CHECK_TYPE_IS_BOOL__(config)

    return dynamic_cast<ElementBool*>(config.m_value)->getValue();
}

bool Config::get_bool_at(const size_t index) const {
    __CHECK_TYPE_IS_CONTAINER__((*this))

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
    __CHECK_TYPE_IS_CONTAINER__((*this))

    const Config& config = get_at(index);
    __CHECK_TYPE_IS_NUMBER__(config)

    return dynamic_cast<ElementNumber*>(config.m_value)->getValue();
}

long double Config::get_number_at(const size_t index) const {
    __CHECK_TYPE_IS_CONTAINER__((*this))

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
    __CHECK_TYPE_IS_CONTAINER__((*this))

    const Config& config = get_at(index);
    __CHECK_TYPE_IS_STRING__(config)

    return dynamic_cast<ElementString*>(config.m_value)->getValue();
}

std::string Config::get_string_at(const size_t index) const {
    __CHECK_TYPE_IS_CONTAINER__((*this))

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
    __CHECK_TYPE_IS_CONTAINER__((*this))

    const Config& config = get_at(key);
    __CHECK_TYPE_IS_BOOL__(config)

    return dynamic_cast<ElementBool*>(config.m_value)->getValue();
}

bool Config::get_bool_at(const std::string& key) const {
    __CHECK_TYPE_IS_CONTAINER__((*this))

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
    __CHECK_TYPE_IS_CONTAINER__((*this))

    const Config& config = get_at(key);
    __CHECK_TYPE_IS_NUMBER__(config)

    return dynamic_cast<ElementNumber*>(config.m_value)->getValue();
}

long double Config::get_number_at(const std::string& key) const {
    __CHECK_TYPE_IS_CONTAINER__((*this))

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
    __CHECK_TYPE_IS_CONTAINER__((*this))

    const Config& config = get_at(key);
    __CHECK_TYPE_IS_STRING__(config)

    return dynamic_cast<ElementString*>(config.m_value)->getValue();
}

std::string Config::get_string_at(const std::string& key) const {
    __CHECK_TYPE_IS_CONTAINER__((*this))

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
    __CHECK_TYPE_IS_CONTAINER__((*this))

    const Config& config = get_back();
    __CHECK_TYPE_IS_BOOL__(config)

    return dynamic_cast<ElementBool*>(config.m_value)->getValue();
}

bool Config::get_bool_back() const {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    const Config& config = get_back();
    __CHECK_TYPE_IS_BOOL__(config)

    return dynamic_cast<const ElementBool*>(config.m_value)->getValue();
}

long double &Config::get_number_back() {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    const Config& config = get_back();
    __CHECK_TYPE_IS_NUMBER__(config)

    return dynamic_cast<ElementNumber*>(config.m_value)->getValue();
}

long double Config::get_number_back() const {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    const Config& config = get_back();
    __CHECK_TYPE_IS_NUMBER__(config)

    return dynamic_cast<const ElementNumber*>(config.m_value)->getValue();
}

std::string &Config::get_string_back() {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    const Config& config = get_back();
    __CHECK_TYPE_IS_STRING__(config)

    return dynamic_cast<ElementString*>(config.m_value)->getValue();
}

std::string Config::get_string_back() const {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    const Config& config = get_back();
    __CHECK_TYPE_IS_STRING__(config)

    return dynamic_cast<const ElementString*>(config.m_value)->getValue();
}

bool Config::isEqual(const IElement &other, const bool compare_comments) const {
    __CHECK_TYPES_IS_EQUAL__((*this), other)
    return m_value->isEqual(other, compare_comments);
}

bool Config::isEqual(const bool other) const {
    __CHECK_TYPE_IS_BOOL__((*this))
    return dynamic_cast<const ElementBool*>(m_value)->getValue() == other;
}

bool Config::isEqual(const long double &other) const {
    __CHECK_TYPE_IS_NUMBER__((*this))
    return dynamic_cast<const ElementNumber*>(m_value)->getValue() == other;
}

bool Config::isEqual(const std::string &other) const {
    __CHECK_TYPE_IS_STRING__((*this))
    return dynamic_cast<const ElementString*>(m_value)->getValue() == other;
}

Config &Config::addComment(const Comment &content) noexcept {
    m_value->addComment(content);
    return *this;
}

Config &Config::addComment(const std::string &content_before, const std::string &content_after) noexcept {
    m_value->addComment(content_before, content_after);
    return *this;
}

Config &Config::addPrefixComment(const std::string &content) noexcept {
    m_value->addPrefixComment(content);
    return *this;
}

Config &Config::addSuffixComment(const std::string &content) noexcept {
    m_value->addSuffixComment(content);
    return *this;
}

Config &Config::clearComment() noexcept {
    m_value->clearComment();
    return *this;
}

Config &Config::clearPrefixComment() noexcept {
    m_value->clearPrefixComment();
    return *this;
}

Config &Config::clearSuffixComment() noexcept {
    m_value->clearSuffixComment();
    return *this;
}

Config &Config::deleteComment() noexcept {
    m_value->deleteComment();
    return *this;
}

Config &Config::deletePrefixComment() noexcept {
    m_value->deletePrefixComment();
    return *this;
}

Config &Config::deleteSuffixComment() noexcept {
    m_value->deleteSuffixComment();
    return *this;
}

Config &Config::setCommentDesign(const CommentDesign &design) noexcept {
    m_value->setCommentDesign(design);
    return *this;
}

Config &Config::clearCommentDesign() noexcept {
    m_value->clearCommentDesign();
    return *this;
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

bool Config::writeFile(const std::string &file_path, const ConfigFormat format,
                       const bool with_comments) noexcept
{
    return WriteFile(*this, file_path, format, with_comments);
}

bool Config::writeFileJson(const std::string &file_path, const bool with_comments) noexcept
{
    return WriteFileJson(*this, file_path, with_comments);
}

bool Config::writeFileIni(const std::string &file_path, const bool with_comments) noexcept
{
    return WriteFileIni(*this, file_path, with_comments);
}

Config &Config::parse(const std::string &content, const ConfigFormat format,
                      const bool with_comments, std::string *error_log)
{
    if(m_value != nullptr)
        delete m_value;

    *this = Parse(content, format, with_comments, error_log);
    return *this;
}

Config &Config::parseJson(const std::string &content, const bool with_comments,
                          std::string *error_log)
{
    if(m_value != nullptr)
        delete m_value;

    *this = ParseJson(content, with_comments, error_log);
    return *this;
}

Config &Config::parseIni(const std::string &content, const bool with_comments,
                          std::string *error_log)
{
    if(m_value != nullptr)
        delete m_value;

    *this = ParseIni(content, with_comments, error_log);
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

shared_VPairElement::iterator Config::json_begin() {
    __CHECK_TYPE_IS_JSON__((*this))
    return dynamic_cast<ElementJson*>(m_value)->begin();
}

shared_VPairElement::const_iterator Config::json_cbegin() const {
    __CHECK_TYPE_IS_JSON__((*this))
    return dynamic_cast<const ElementJson*>(m_value)->cbegin();
}

shared_VPairElement::iterator Config::json_end() {
    __CHECK_TYPE_IS_JSON__((*this))
    return dynamic_cast<ElementJson*>(m_value)->end();
}

shared_VPairElement::const_iterator Config::json_cend() const {
    __CHECK_TYPE_IS_JSON__((*this))
    return dynamic_cast<const ElementJson*>(m_value)->cend();
}

Config Config::CreateElementFromString(std::string &&value_string, const ConfigFormat format,
                                       const bool enable_comments, const CommentDesign& design)
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
    /*ARRAY*/ {
        if(first == '[' && last == ']') {
            try {
                ElementArray array;
                array.setCommentDesign(design);
                array.parse(value_string, format, enable_comments);
                return Config(array);
            } catch(...) {}
        }
    }
    /*JSON*/ {
        if(first == '{' && last == '}') {
            try {
                ElementJson json;
                json.setCommentDesign(design);
                json.parse(value_string, format, enable_comments);
                return Config(json);
            } catch(...) {}
        }
    }

    throw std::invalid_argument("incorrect value format");
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
    //TODO: ReadFileJson
    return Config{};
}

Config ReadFileIni(const std::string &file_path, const bool with_comments,
                    std::string *error_log)
{
    //TODO: ReadFileIni
    return Config{};
}

bool WriteFile(const Config &config, const std::string &file_path,
               const ConfigFormat format, const bool with_comments) noexcept
{
    switch(format) {
    case ConfigFormat::eJSON:   WriteFileJson(config, file_path, with_comments);
    case ConfigFormat::eINI:    WriteFileIni(config, file_path, with_comments);
//    case ConfigFormat::eYAML:
//    case ConfigFormat::eXML:
    default:                    return false;
    }
}

bool WriteFileJson(const Config &config, const std::string &file_path,
                   const bool with_comments) noexcept
{
    //TODO: WriteFileJson
    return false;
}

bool WriteFileIni(const Config &config, const std::string &file_path,
                   const bool with_comments) noexcept
{
    //TODO: WriteFileIni
    return false;
}

Config Parse(const std::string &content, const ConfigFormat format,
             const bool with_comments, std::string *error_log)
{
    switch(format) {
    case ConfigFormat::eJSON:   return ParseJson(content, with_comments, error_log);
    case ConfigFormat::eINI:    return ParseIni(content, with_comments, error_log);
//    case ConfigFormat::eYAML:
//    case ConfigFormat::eXML:
    default:                    return Config{};
    }
}

Config ParseJson(const std::string &content, const bool with_comments,
                 std::string *error_log)
{
    //TODO: ParseJson
    return {};
}

Config ParseIni(const std::string &content, const bool with_comments,
                 std::string *error_log)
{
    //TODO: ParseIni
    return {};
}
