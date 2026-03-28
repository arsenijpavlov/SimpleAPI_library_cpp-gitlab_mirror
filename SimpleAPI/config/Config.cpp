#include "Config.h"

#include "ConfigDefines.h"
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

//Вспомогательная функция
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


void Config::release() noexcept {
    if(!m_value)
    {
        delete m_value;
        m_value = nullptr;
    }
    m_error_str.unset();
}

Config &Config::setComment(const Comment &content) noexcept {
    if(m_value)
        m_value->setComment(content);
    return *this;
}

Config &Config::setComment(const std::string &content_before, const std::string &content_after) noexcept {
    if(m_value)
        m_value->setComment(content_before, content_after);
    return *this;
}

Config &Config::setPrefixComment(const std::string &content) noexcept {
    if(m_value)
        m_value->setPrefixComment(content);
    return *this;
}

Config &Config::setSuffixComment(const std::string &content) noexcept {
    if(m_value)
        m_value->setSuffixComment(content);
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

Config &Config::set_comment(const size_t& index, const Comment &content) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    reinterpret_cast<IElementContainer*>(m_value)->set_comment(index, content);
    return *this;
}

Config &Config::set_comment(const std::string &key, const Comment &content) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    reinterpret_cast<ElementJson*>(m_value)->set_comment(key, content);
    return *this;
}

Config &Config::set_comment(const size_t& index, const std::string &content_before,
                            const std::string &content_after)
{
    __CHECK_TYPE_IS_CONTAINER__((*this))
    reinterpret_cast<IElementContainer*>(m_value)->set_comment(index, content_before, content_after);
    return *this;
}

Config &Config::set_comment(const std::string &key, const std::string &content_before,
                            const std::string &content_after)
{
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    reinterpret_cast<ElementJson*>(m_value)->set_comment(key, content_before, content_after);
    return *this;
}

Config &Config::set_prefix_comment(const size_t& index, const std::string &content) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    reinterpret_cast<IElementContainer*>(m_value)->set_prefix_comment(index, content);
    return *this;
}

Config &Config::set_prefix_comment(const std::string &key, const std::string &content) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    reinterpret_cast<ElementJson*>(m_value)->set_prefix_comment(key, content);
    return *this;
}

Config &Config::set_suffix_comment(const size_t& index, const std::string &content) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    reinterpret_cast<IElementContainer*>(m_value)->set_suffix_comment(index, content);
    return *this;
}

Config &Config::set_suffix_comment(const std::string &key, const std::string &content) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    reinterpret_cast<ElementJson*>(m_value)->set_suffix_comment(key, content);
    return *this;
}

Comment &Config::get_comment(const size_t& index) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    return reinterpret_cast<IElementContainer*>(m_value)->get_comment(index);
}

Comment &Config::get_comment(const std::string &key)
{
    __CHECK_TYPE_IS_CONTAINER__((*this))
    return reinterpret_cast<ElementJson*>(m_value)->get_comment(key);
}

Comment Config::get_comment(const size_t& index) const {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    return reinterpret_cast<IElementContainer*>(m_value)->get_comment(index);
}

Comment Config::get_comment(const std::string &key) const
{
    __CHECK_TYPE_IS_CONTAINER__((*this))
    return reinterpret_cast<ElementJson*>(m_value)->get_comment(key);
}

std::string Config::get_prefix_comment(const size_t& index) const {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    return reinterpret_cast<IElementContainer*>(m_value)->get_prefix_comment(index);
}

std::string Config::get_prefix_comment(const std::string &key) const {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    return reinterpret_cast<ElementJson*>(m_value)->get_prefix_comment(key);
}

//std::string &Config::get_prefix_comment(const size_t& index) {
//    __CHECK_TYPE_IS_CONTAINER__((*this))
//    return reinterpret_cast<IElementContainer*>(m_value)->get_prefix_comment(index);
//}

//std::string &Config::get_prefix_comment(const std::string &key) {
//    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
//    return reinterpret_cast<ElementJson*>(m_value)->get_prefix_comment(key);
//}

std::string Config::get_suffix_comment(const size_t& index) const {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    return reinterpret_cast<IElementContainer*>(m_value)->get_suffix_comment(index);
}

std::string Config::get_suffix_comment(const std::string &key) const {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    return reinterpret_cast<ElementJson*>(m_value)->get_suffix_comment(key);
}

//std::string &Config::get_suffix_comment(const size_t& index){
//    __CHECK_TYPE_IS_CONTAINER__((*this))
//    return reinterpret_cast<IElementContainer*>(m_value)->get_suffix_comment(index);
//}

//std::string &Config::get_suffix_comment(const std::string &key) {
//    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
//    return reinterpret_cast<ElementJson*>(m_value)->get_suffix_comment(key);
//}

Config &Config::clear_comment(const size_t& index) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    reinterpret_cast<IElementContainer*>(m_value)->clear_comment(index);
    return *this;
}

Config &Config::clear_comment(const std::string &key) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    reinterpret_cast<ElementJson*>(m_value)->clear_comment(key);
    return *this;
}

Config &Config::clear_prefix_comment(const size_t& index) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    reinterpret_cast<IElementContainer*>(m_value)->clear_prefix_comment(index);
    return *this;
}

Config &Config::clear_prefix_comment(const std::string &key) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    reinterpret_cast<ElementJson*>(m_value)->clear_prefix_comment(key);
    return *this;
}

Config &Config::clear_suffix_comment(const size_t& index) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    reinterpret_cast<IElementContainer*>(m_value)->clear_suffix_comment(index);
    return *this;
}

Config &Config::clear_suffix_comment(const std::string &key) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    reinterpret_cast<ElementJson*>(m_value)->clear_suffix_comment(key);
    return *this;
}

Config &Config::delete_comment(const size_t& index) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    reinterpret_cast<IElementContainer*>(m_value)->delete_comment(index);
    return *this;
}

Config &Config::delete_comment(const std::string &key) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    reinterpret_cast<ElementJson*>(m_value)->delete_comment(key);
    return *this;
}

Config &Config::delete_prefix_comment(const size_t& index) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    reinterpret_cast<IElementContainer*>(m_value)->delete_prefix_comment(index);
    return *this;
}

Config &Config::delete_prefix_comment(const std::string &key) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    reinterpret_cast<ElementJson*>(m_value)->delete_prefix_comment(key);
    return *this;
}

Config &Config::delete_suffix_comment(const size_t& index) {
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

        setCommentDesign(other.getCommentDesign());
        setComment(other.getComment());
    }

    return *this;
}

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

        setCommentDesign(other.getCommentDesign());
        setComment(other.getComment());
        other.release(); //обнулить значение
    }
    return *this;
}

Config &Config::setValue(const IElement &other) noexcept {
    if(this->m_value == &other)
        return *this;

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

    setComment(other.getComment());

    return *this;
}

Config &Config::setValue(IElement &&other) noexcept {
    if(this->m_value == &other)
        return *this;

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

    setComment(other.getComment());

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

Config &Config::set(const std::string &key, const Config &value)
{
    insert_at(key, value);
    return *this;
}

Config &Config::set(const std::string &key, Config &&value)
{
    insert_at(key, std::move(value));
    return *this;
}

Config &Config::set(const size_t& index, const Config &value)
{
    insert_at(index, value);
    return *this;
}

Config &Config::set(const size_t& index, Config &&value)
{
    insert_at(index, std::move(value));
    return *this;
}

Config &Config::set(const size_t& index, const std::string &key, const Config &value)
{
    insert_at(index, key, value);
    return *this;
}

Config &Config::set(const size_t& index, const std::string &key, Config &&value)
{
    insert_at(index, key, std::move(value));
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

Config &Config::get_at(const size_t& index) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    switch(getType()) {
    case ValueType::eArray: return dynamic_cast<ElementArray*>(m_value)->get_at(index);
    case ValueType::eJson:  return dynamic_cast<ElementJson*>(m_value)->get_at(index);
    default:                break;
    }

    return *this;
}

Config Config::get_at(const size_t& index) const {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    switch(getType()) {
    case ValueType::eArray: return dynamic_cast<const ElementArray*>(m_value)->get_at(index);
    case ValueType::eJson:  return dynamic_cast<const ElementJson*>(m_value)->get_at(index);
    default:                break;
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

Config &Config::get_at(const std::vector<OnlySizetOrString> &complex_key)
{
    __CHECK_TYPE_IS_CONTAINER__((*this))
    if(complex_key.size() == 0)
        throw std::invalid_argument("get_at({}) - try a get incorrect index/key (no value)");

    //определить тип значения первого индекса/ключа
    Config* ret;
    switch (complex_key[0].getType()) {
    case OnlySizetOrString::Type::type_string:
        ret = &get_at(complex_key[0].getStringValue());
        break;
    case OnlySizetOrString::Type::type_sizet:
        ret = &get_at(complex_key[0].getIndexValue());
        break;
    default:
        //это значение пользователь библиотеки не увидит
        throw std::invalid_argument("incorrect complex_key type");
    }

    if(complex_key.size() == 1)
        return *ret;

    //создать следующую итерацию списка
    std::vector<OnlySizetOrString> new_key = complex_key;
    new_key.erase(new_key.cbegin());
    return ret->get_at(new_key);
}

Config Config::get_at(const std::vector<OnlySizetOrString> &complex_key) const
{
    __CHECK_TYPE_IS_CONTAINER__((*this))
    if(complex_key.size() == 0)
        throw std::invalid_argument("get_at({}) - try a get incorrect index/key (no value)");

    //определить тип значения первого индекса/ключа
    Config ret;
    switch (complex_key[0].getType()) {
    case OnlySizetOrString::Type::type_string:
        ret = get_at(complex_key[0].getStringValue());
        break;
    case OnlySizetOrString::Type::type_sizet:
        ret = get_at(complex_key[0].getIndexValue());
        break;
    default:
        //это значение пользователь библиотеки не увидит
        throw std::invalid_argument("incorrect complex_key type");
    }

    if(complex_key.size() == 1)
        return ret;

    //создать следующую итерацию списка
    std::vector<OnlySizetOrString> new_key = complex_key;
    new_key.erase(new_key.cbegin());
    return ret.get_at(new_key);
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

bool &Config::get_bool_at(const size_t& index) {
    const Config& config = get_at(index);
    __CHECK_TYPE_IS_BOOL__(config)

    return dynamic_cast<ElementBool*>(config.m_value)->getValue();
}

bool Config::get_bool_at(const size_t& index) const {
    const Config& config = get_at(index);
    __CHECK_TYPE_IS_BOOL__(config)

    return dynamic_cast<const ElementBool*>(config.m_value)->getValue();
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

bool &Config::get_bool_at(const std::vector<OnlySizetOrString> &complex_key)
{
    __CHECK_TYPE_IS_CONTAINER__((*this))
    if(complex_key.size() == 0)
        throw std::invalid_argument("get_bool_at({}) - try a get incorrect index/key (no value)");

    //определить тип значения первого индекса/ключа
    Config* ret;
    switch (complex_key[0].getType()) {
    case OnlySizetOrString::Type::type_string:  ret = &get_at(complex_key[0].getStringValue());
    case OnlySizetOrString::Type::type_sizet:   ret = &get_at(complex_key[0].getIndexValue());
    default:
        //это значение пользователь библиотеки не увидит
        throw std::invalid_argument("incorrect complex_key type");
    }

    if(complex_key.size() == 1)
        return (*ret).getBool();

    //создать следующую итерацию списка
    std::vector<OnlySizetOrString> new_key = complex_key;
    new_key.erase(new_key.cbegin());
    return ret->get_bool_at(new_key);
}

bool Config::get_bool_at(const std::vector<OnlySizetOrString> &complex_key) const
{
    __CHECK_TYPE_IS_CONTAINER__((*this))
    if(complex_key.size() == 0)
        throw std::invalid_argument("get_bool_at({}) - try a get incorrect index/key (no value)");

    //определить тип значения первого индекса/ключа
    Config ret;
    switch (complex_key[0].getType()) {
    case OnlySizetOrString::Type::type_string:  ret = get_at(complex_key[0].getStringValue());
    case OnlySizetOrString::Type::type_sizet:   ret = get_at(complex_key[0].getIndexValue());
    default:
        //это значение пользователь библиотеки не увидит
        throw std::invalid_argument("incorrect complex_key type");
    }

    if(complex_key.size() == 1)
        return ret.getBool();

    //создать следующую итерацию списка
    std::vector<OnlySizetOrString> new_key = complex_key;
    new_key.erase(new_key.cbegin());
    return ret.get_bool_at(new_key);
}

long double &Config::get_number_at(const size_t& index) {
    const Config& config = get_at(index);
    __CHECK_TYPE_IS_NUMBER__(config)

    return dynamic_cast<ElementNumber*>(config.m_value)->getValue();
}

long double Config::get_number_at(const size_t& index) const {
    const Config& config = get_at(index);
    __CHECK_TYPE_IS_NUMBER__(config)

    return dynamic_cast<const ElementNumber*>(config.m_value)->getValue();
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

long double &Config::get_number_at(const std::vector<OnlySizetOrString> &complex_key)
{
    __CHECK_TYPE_IS_CONTAINER__((*this))
    if(complex_key.size() == 0)
        throw std::invalid_argument("get_number_at({}) - try a get incorrect index/key (no value)");

    //определить тип значения первого индекса/ключа
    Config* ret;
    switch (complex_key[0].getType()) {
    case OnlySizetOrString::Type::type_string:  ret = &get_at(complex_key[0].getStringValue());
    case OnlySizetOrString::Type::type_sizet:   ret = &get_at(complex_key[0].getIndexValue());
    default:
        //это значение пользователь библиотеки не увидит
        throw std::invalid_argument("incorrect complex_key type");
    }

    if(complex_key.size() == 1)
        return (*ret).getNumber();

    //создать следующую итерацию списка
    std::vector<OnlySizetOrString> new_key = complex_key;
    new_key.erase(new_key.cbegin());
    return ret->get_number_at(new_key);
}

long double Config::get_number_at(const std::vector<OnlySizetOrString> &complex_key) const
{
    __CHECK_TYPE_IS_CONTAINER__((*this))
    if(complex_key.size() == 0)
        throw std::invalid_argument("get_number_at({}) - try a get incorrect index/key (no value)");

    //определить тип значения первого индекса/ключа
    Config ret;
    switch (complex_key[0].getType()) {
    case OnlySizetOrString::Type::type_string:  ret = get_at(complex_key[0].getStringValue());
    case OnlySizetOrString::Type::type_sizet:   ret = get_at(complex_key[0].getIndexValue());
    default:
        //это значение пользователь библиотеки не увидит
        throw std::invalid_argument("incorrect complex_key type");
    }

    if(complex_key.size() == 1)
        return ret.getNumber();

    //создать следующую итерацию списка
    std::vector<OnlySizetOrString> new_key = complex_key;
    new_key.erase(new_key.cbegin());
    return ret.get_number_at(new_key);
}

std::string &Config::get_string_at(const size_t& index) {
    const Config& config = get_at(index);
    __CHECK_TYPE_IS_STRING__(config)

    return dynamic_cast<ElementString*>(config.m_value)->getValue();
}

std::string Config::get_string_at(const size_t& index) const {
    const Config& config = get_at(index);
    __CHECK_TYPE_IS_STRING__(config)

    return dynamic_cast<const ElementString*>(config.m_value)->getValue();
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

std::string &Config::get_string_at(const std::vector<OnlySizetOrString> &complex_key)
{
    __CHECK_TYPE_IS_CONTAINER__((*this))
    if(complex_key.size() == 0)
        throw std::invalid_argument("get_string_at({}) - try a get incorrect index/key (no value)");

    //определить тип значения первого индекса/ключа
    Config* ret;
    switch (complex_key[0].getType()) {
    case OnlySizetOrString::Type::type_string:  ret = &get_at(complex_key[0].getStringValue());
    case OnlySizetOrString::Type::type_sizet:   ret = &get_at(complex_key[0].getIndexValue());
    default:
        //это значение пользователь библиотеки не увидит
        throw std::invalid_argument("incorrect complex_key type");
    }

    if(complex_key.size() == 1)
        return (*ret).getString();

    //создать следующую итерацию списка
    std::vector<OnlySizetOrString> new_key = complex_key;
    new_key.erase(new_key.cbegin());
    return ret->get_string_at(new_key);
}

std::string Config::get_string_at(const std::vector<OnlySizetOrString> &complex_key) const
{
    __CHECK_TYPE_IS_CONTAINER__((*this))
    if(complex_key.size() == 0)
        throw std::invalid_argument("get_string_at({}) - try a get incorrect index/key (no value)");

    //определить тип значения первого индекса/ключа
    Config ret;
    switch (complex_key[0].getType()) {
    case OnlySizetOrString::Type::type_string:  ret = get_at(complex_key[0].getStringValue());
    case OnlySizetOrString::Type::type_sizet:   ret = get_at(complex_key[0].getIndexValue());
    default:
        //это значение пользователь библиотеки не увидит
        throw std::invalid_argument("incorrect complex_key type");
    }

    if(complex_key.size() == 1)
        return ret.getString();

    //создать следующую итерацию списка
    std::vector<OnlySizetOrString> new_key = complex_key;
    new_key.erase(new_key.cbegin());
    return ret.get_string_at(new_key);
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

void Config::try_convert_null_to_json() noexcept
{
    if(isNull()) {
        release();
        m_value = dynamic_cast<IElement*>(new ElementJson());
    }
}

void Config::try_convert_null_to_json_array() noexcept
{
    if(isNull()){
        release();
        m_value = dynamic_cast<IElement*>(new ElementArray());
    }
}

Config& Config::insert_front(const Config& other) {
    Config config(other);
    return insert_front(std::move(config));
}

Config& Config::insert_front(Config&& other) {
    try_convert_null_to_json_array();

    __CHECK_TYPE_IS_INDEX_CONTAINER__((*this))
    dynamic_cast<ElementArray*>(m_value)->insert_front(std::move(other));

    return *this;
}

Config& Config::insert_front(const std::string& key, const Config& other) {
    Config config(other);
    return insert_front(key, std::move(config));
}

Config& Config::insert_front(const std::string& key, Config&& other) {
    try_convert_null_to_json();

    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    dynamic_cast<ElementJson*>(m_value)->insert_front(key, std::move(other));

    return *this;
}

Config& Config::insert_at(const size_t& index, const Config& other) {
    Config config(other);
    return insert_at(index, std::move(config));
}

Config& Config::insert_at(const size_t& index, Config&& other) {
    try_convert_null_to_json_array();

    __CHECK_TYPE_IS_INDEX_CONTAINER__((*this))
    dynamic_cast<ElementArray*>(m_value)->insert_at(index, std::move(other));

    return *this;
}

Config& Config::insert_at(const std::string& key, const Config& other) {
    Config config(other);
    return insert_at(key, std::move(config));
}

Config& Config::insert_at(const std::string& key, Config&& other) {
    try_convert_null_to_json();

    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    dynamic_cast<ElementJson*>(m_value)->insert_at(key, std::move(other));

    return *this;
}

Config &Config::insert_at(const size_t& index, const std::string &key, const Config &other)
{
    Config config(other);
    return insert_at(index, key, std::move(config));
}

Config &Config::insert_at(const size_t& index, const std::string &key, Config &&other)
{
    try_convert_null_to_json();

    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    dynamic_cast<ElementJson*>(m_value)->insert_at(index, key, std::move(other));

    return *this;
}

Config &Config::insert_at(const shared_VElement::iterator iterator, const Config &other)
{
    Config temp(other);
    return insert_at(iterator, std::move(other));
}

Config &Config::insert_at(const shared_VElement::iterator iterator, Config &&other)
{
    try_convert_null_to_json_array();

    __CHECK_TYPE_IS_INDEX_CONTAINER__((*this))
    dynamic_cast<ElementArray*>(m_value)->insert_at(iterator, std::move(other));

    return *this;
}

Config &Config::insert_at(const shared_VPairElement::iterator iterator, const std::string& key,
                          const Config &other)
{
    Config temp(other);
    return insert_at(iterator, key, std::move(other));
}

Config &Config::insert_at(const shared_VPairElement::iterator iterator, const std::string& key,
                          Config &&other)
{
    try_convert_null_to_json();

    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    dynamic_cast<ElementJson*>(m_value)->insert_at(iterator, key, std::move(other));

    return *this;
}

Config& Config::insert_back(const Config& other) {
    Config config(other);
    return insert_back(std::move(config));
}

Config& Config::insert_back(Config&& other) {
    try_convert_null_to_json_array();

    __CHECK_TYPE_IS_INDEX_CONTAINER__((*this))
    dynamic_cast<ElementArray*>(m_value)->insert_back(std::move(other));

    return *this;
}

Config& Config::insert_back(const std::string& key, const Config& other) {
    Config config(other);
    return insert_back(key, std::move(config));
}

Config& Config::insert_back(const std::string& key, Config&& other) {
    try_convert_null_to_json();

    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    dynamic_cast<ElementJson*>(m_value)->insert_back(key, std::move(other));

    return *this;
}

Config &Config::insert_before(const std::string &before_key, const std::string &key, const Config &other)
{
    Config config(other);
    return insert_before(before_key, key, std::move(config));
}

Config &Config::insert_before(const std::string &before_key, const std::string &key, Config &&other)
{
    try_convert_null_to_json();

    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    dynamic_cast<ElementJson*>(m_value)->insert_before(before_key, key, std::move(other));

    return *this;
}

Config& Config::insert_after(const std::string& after_key, const std::string& key, const Config& other) {
    Config config(other);
    return insert_after(after_key, key, std::move(config));
}

Config& Config::insert_after(const std::string& after_key, const std::string& key, Config&& other) {
    try_convert_null_to_json();

    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    dynamic_cast<ElementJson*>(m_value)->insert_after(after_key, key, std::move(other));

    return *this;
}

Config &Config::append(const Config &config) {
    Config cfg_copy(config);
    return append(std::move(cfg_copy));
}

Config &Config::append(Config &&config) {
    if(config.isNull())
        return *this;

    if(!isNull())
    {
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
    } else {
        setValue(config);
    }

    return *this;
}

Config& Config::erase_front() {
    erase_at(0);
    return *this;
}

//если элемента не существует - проигнорировать
Config& Config::erase_at(const size_t& index) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    dynamic_cast<IElementContainer*>(m_value)->erase_at(index);

    return *this;
}

Config& Config::erase_at(const std::string& key) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    dynamic_cast<ElementJson*>(m_value)->erase_at(key);

    return *this;
}

Config& Config::erase_back() {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    dynamic_cast<IElementContainer*>(m_value)->erase_back();

    return *this;
}

Config &Config::erase_at(const shared_VElement::iterator iterator)
{
    __CHECK_TYPE_IS_ARRAY__((*this))
    dynamic_cast<ElementArray*>(m_value)->erase_at(iterator);

    return *this;
}

Config &Config::erase_at(const shared_VPairElement::iterator iterator)
{
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    dynamic_cast<ElementJson*>(m_value)->erase_at(iterator);

    return *this;
}

Config Config::get_and_pop_front() {
    Config config = get_front();
    erase_front();

    return config;
}

Config Config::get_and_pop_at(const size_t& index) {
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

bool Config::containsValue(const Config &config) const noexcept {
    switch(getType()) {
    case ValueType::eNull:      return true;
    case ValueType::eBool:      return config.isBool() && getBool() == config.getBool();
    case ValueType::eNumber:    return config.isNumber() && getNumber() == config.getNumber();
    case ValueType::eString:    return config.isString() && getString() == config.getString();
    case ValueType::eArray:     return std::any_of(getRange().cbegin(), getRange().cend(),
                                    [&config](const std::shared_ptr<Config>& value)
                                    { return *value == config; });
    case ValueType::eJson:
                                return std::any_of(getNamedRange().cbegin(), getNamedRange().cend(),
                                    [&config](const std::pair<std::string, std::shared_ptr<Config>>& pair)
                                    { return *pair.second == config; });
    }

    return false;
}

bool Config::containsKey(const std::string &key) const noexcept {
    if(!isMapContainer()) return false;

    return std::any_of(getNamedRange().cbegin(), getNamedRange().cend(),
                       [&key](const std::pair<std::string, std::shared_ptr<Config>>& pair)
                       { return pair.first == key; });
}

std::string Config::toString(const ConfigFormat format, const CommentDesign &design,
                             const int8_t tabulation_level) const noexcept
{
    CommentDesign n_design;
    if(design == CommentDesign{}) { // если переменная не заполнена, используется собственная
        n_design = m_value->getCommentDesign();
    } else {
        n_design = design;
    }
    return m_value->toString(format, n_design, tabulation_level);
}

std::ostream &operator<<(std::ostream &os, const Config &config) noexcept {
    os << config.toString();
    return os;
}

std::ostream &operator<<(std::ostream &os, const IElement &config) noexcept {
    os << config.toString();
    return os;
}

bool Config::readFile(const std::string &file_path, const ConfigFormat format,
                         const bool with_comments) noexcept
{
    switch(format) {
    case ConfigFormat::eJSON:   return readFileJson(file_path, with_comments);
    case ConfigFormat::eINI:    return readFileIni(file_path, with_comments);
    // case ConfigFormat::eYAML:
    // case ConfigFormat::eXML:
    default: break;
    }

    m_error_str = "unexpected ConfigFormat";
    return false;
}

//FIXME: m_error_str должен обнуляться
bool Config::readFileJson(const std::string &file_path, const bool with_comments) noexcept
{    
    std::string input_str;
    if(GetAllStringsFromFile(file_path, input_str, &m_error_str.value())) {
        return parseJson(input_str, with_comments);
    }

    return false;
}

//FIXME: m_error_str должен обнуляться
bool Config::readFileIni(const std::string &file_path, const bool with_comments) noexcept
{
    std::string input_str;
    if(GetAllStringsFromFile(file_path, input_str, &m_error_str.value())) {
        return parseIni(input_str, with_comments);
    }

    return false;
}

bool Config::writeFile(const std::string &file_path,  const ConfigFormat format,
                       const CommentDesign &design,
                       const int8_t custom_tabulation_level) noexcept
{
    CommentDesign n_design;
    if(design == CommentDesign{}) { // если переменная не заполнена, используется собственная
        n_design = m_value->getCommentDesign();
    } else {
        n_design = design;
    }
    return WriteFile(*this, file_path, format, n_design, custom_tabulation_level);
}

bool Config::writeFileJson(const std::string &file_path, const CommentDesign &design,
                           const int8_t custom_tabulation_level) noexcept
{
    CommentDesign n_design;
    if(design == CommentDesign{}) { // если переменная не заполнена, используется соб
        n_design = m_value->getCommentDesign();
    }
    return WriteFileJson(*this, file_path, n_design, custom_tabulation_level);
}

bool Config::writeFileIni(const std::string &file_path, const CommentDesign &design,
                          const int8_t custom_tabulation_level) noexcept
{
    CommentDesign n_design;
    if(design == CommentDesign{}) { // если переменная не заполнена, используется собственная
        n_design = m_value->getCommentDesign();
    }
    return WriteFileIni(*this, file_path, n_design, custom_tabulation_level);
}

//NOTE: функция нужна исключительно для перенаправления на внутренние парсеры
bool Config::parseSimpleValue(const std::string &content, const ConfigFormat format) noexcept
{
    /* Суть функции:
     * - начать запись начального комментария
     * - найти границу начала значения (не считывать комментарий в границах значения)
     * - определить тип значения (на основе формата)
     * - запомнить начальный комментарий
     * - найти границу конечного комментария (с конца строки), если он есть
     * - запомнить конечный комментарий
     * - передать строку без начального и конечного комментариев на дальнейший парсинг в дочерний IElement
     */

//    size_t start_pos_comment_before;
    size_t finish_pos_comment_before;
    size_t start_pos_comment_after;
//    size_t finish_pos_comment_after;
    std::string comment_string;
    VString previous_comments;
    CommentDesign& cd = getCommentDesign();

    size_t i = 0; //начало строки

    //определение границ начального комментария
    {
        for(; i < content.size(); i++) {
            char ch_previous    = i == 0 ? 0 : content[i - 1];
            char ch_current     = content[i];
            char ch_next        = i < content.size() ? content[i + 1] : 0;

            CheckComments(ch_current, ch_next, i, cd, comment_string);
            if(cd.temp_type == CommentType::eCommentEnd)
            {
//                previous_comments.push_back(FromComment(current_comment, design, tabulation_level));
//                current_comment.clear();
//                design.temp_type = CommentType::eNotComment;
                break;
            }
        }

        if(cd.with_comments && !comment_string.empty())
        {
            //сохранить комментарий
        }
    }

    return false;
}

bool Config::parse(const std::string &content, const ConfigFormat format,
                      const bool with_comments) noexcept
{
    release();

    switch(format) {
    case ConfigFormat::eJSON:   return parseJson(content, with_comments);
    case ConfigFormat::eINI:    return parseIni(content, with_comments);
    case ConfigFormat::eYAML:   return parseYaml(content, with_comments);
    case ConfigFormat::eXML:    return parseXml(content, with_comments);
    default:                    break;
    }

    Config out;
    out.m_error_str = "unexpected ConfigFormat";
    return false;
}

bool Config::parseJson(const std::string &content, const bool with_comments) noexcept
{
    release();
    std::string error;
//    m_value = new ElementJson(content, ConfigFormat::eJSON, with_comments, &error);
    m_value = CreateElementFromString(content, ConfigFormat::eJSON, getCommentDesign(), &error);
    if(!error.empty())
        m_error_str = error;

    return !error.empty();
}

bool Config::parseIni(const std::string &content, const bool with_comments) noexcept
{
    release();
    m_value = new ElementJson(content, ConfigFormat::eINI, with_comments);
    return !m_error_str.isValid();
}

bool Config::parseYaml(const std::string &content, const bool with_comments) noexcept
{
    //TODO: Config::parseYaml()
    return false;
}

bool Config::parseXml(const std::string &content, const bool with_comments) noexcept
{
    //TODO: Config::parseXml()
    return false;
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

shared_VPairElement::iterator Config::map_begin() {
    __CHECK_TYPE_IS_JSON__((*this))
    return dynamic_cast<ElementJson*>(m_value)->begin();
}

shared_VPairElement::const_iterator Config::map_cbegin() const {
    __CHECK_TYPE_IS_JSON__((*this))
    return dynamic_cast<const ElementJson*>(m_value)->cbegin();
}

shared_VPairElement::iterator Config::map_end() {
    __CHECK_TYPE_IS_JSON__((*this))
    return dynamic_cast<ElementJson*>(m_value)->end();
}

shared_VPairElement::const_iterator Config::map_cend() const {
    __CHECK_TYPE_IS_JSON__((*this))
    return dynamic_cast<const ElementJson*>(m_value)->cend();
}

//функция должна быть вызвана исключительно для обработки строки значения, комменты не учитывает
Config CreateElementFromString(std::string &&value_string, const ConfigFormat format,
                               CommentDesign& design, ParserSymbolCounter& start_iterator) noexcept
{
    using namespace utils;
    //удаление незначащих пробелов
    RemoveIllegalSpaces(value_string);

    std::string temp;
    auto Append = [&](const char c) {
        temp += std::tolower(c);
    };

    //проверка типа, по порядку
    {
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
                std::regex reg("^[+-]?[0-9]*[.]?[0-9]*[eE]?[+-]?[0-9]*[fF]?$");
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
            if(first == '[') {
                Config array;
                if(last == ']') {
                    array.setCommentDesign(design);
                    ElementArray el_arr(value_string, ConfigFormat::eJSON, design.with_comments);
                    array.setValue(el_arr);
                } else {
                    //есть начало массива, но нет конца
                    array.m_error_str = "not found end of Json-array value";
                }
                return array;
            }
        }
        /*JSON*/ {
            if(first == '{') {
                Config json;
                if(last == '}') {
                    json.setCommentDesign(design);
                    json.parseJson(value_string, design.with_comments);
                } else {
                    //есть начало Json, но нет конца
                    json.m_error_str = "not found end of Json value";
                }
                return json;
            }
        }
    }

    // в теории, всё, что не распарсилось в другие значения, - должно считаться строкой
    //FIXME: строка не должна начинаться с технических скобок
    return Config(value_string);
}


std::pair<bool, Config> ReadFile(const std::string &file_path, const ConfigFormat format,
                const bool with_comments) noexcept
{
    switch(format) {
    case ConfigFormat::eJSON:   return ReadFileJson(file_path, with_comments);
    case ConfigFormat::eINI:    return ReadFileIni(file_path, with_comments);
//    case ConfigFormat::eYAML:
//    case ConfigFormat::eXML:
    default: break;
    }

    Config out;
    out.m_error_str = "unexpected ConfigFormat";
    return std::make_pair(false, out);
}

//FIXME: m_error_str должен обнуляться
std::pair<bool, Config> ReadFileJson(const std::string &file_path, const bool with_comments) noexcept
{
    std::string input_str;
    Config out;
    out.m_error_str.set("");

    if(GetAllStringsFromFile(file_path, input_str, &out.m_error_str.value())) {
        return ParseJson(input_str, with_comments);
    }

    return std::make_pair(false, out);
}

//FIXME: m_error_str должен обнуляться
std::pair<bool, Config> ReadFileIni(const std::string &file_path, const bool with_comments) noexcept
{
    std::string input_str;
    Config out;
    out.m_error_str.set("");

    if(GetAllStringsFromFile(file_path, input_str, &out.m_error_str.value())) {
        return ParseIni(input_str, with_comments);
    }

    return std::make_pair(false, out);
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

//NOTE: массивы отдельно спарсить нельзя - только в составе полного конфига (1 элемент - это тоже конфиг)
std::pair<bool, Config> Parse(const std::string &content, const ConfigFormat format,
             const bool with_comments) noexcept
{
    switch(format) {
    case ConfigFormat::eJSON:   return ParseJson(content, with_comments);
    case ConfigFormat::eINI:    return ParseIni(content, with_comments);
//    case ConfigFormat::eYAML:
//    case ConfigFormat::eXML:
    default:                    break;
    }

    Config out;
    out.m_error_str = "unexpected ConfigFormat";
    return std::make_pair(false, out);
}

std::pair<bool, Config> ParseJson(const std::string &content, const bool with_comments) noexcept
{
    Config ret(ValueType::eJson);
    bool result = ret.parseJson(content, with_comments);

    return std::make_pair(result, ret);
}

std::pair<bool, Config> ParseIni(const std::string &content, const bool with_comments) noexcept
{
    Config ret(ValueType::eJson);
    bool result = ret.parseIni(content, with_comments);

    return std::make_pair(result, ret);
}
