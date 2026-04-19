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


namespace simpleapi {

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
    reinterpret_cast<tools::IElementContainer*>(m_value)->set_comment(index, content);
    return *this;
}

Config &Config::set_comment(const std::string &key, const Comment &content) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    reinterpret_cast<tools::ElementJson*>(m_value)->set_comment(key, content);
    return *this;
}

Config &Config::set_comment(const size_t& index, const std::string &content_before,
                            const std::string &content_after)
{
    __CHECK_TYPE_IS_CONTAINER__((*this))
    reinterpret_cast<tools::IElementContainer*>(m_value)->set_comment(index, content_before, content_after);
    return *this;
}

Config &Config::set_comment(const std::string &key, const std::string &content_before,
                            const std::string &content_after)
{
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    reinterpret_cast<tools::ElementJson*>(m_value)->set_comment(key, content_before, content_after);
    return *this;
}

Config &Config::set_prefix_comment(const size_t& index, const std::string &content) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    reinterpret_cast<tools::IElementContainer*>(m_value)->set_prefix_comment(index, content);
    return *this;
}

Config &Config::set_prefix_comment(const std::string &key, const std::string &content) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    reinterpret_cast<tools::ElementJson*>(m_value)->set_prefix_comment(key, content);
    return *this;
}

Config &Config::set_suffix_comment(const size_t& index, const std::string &content) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    reinterpret_cast<tools::IElementContainer*>(m_value)->set_suffix_comment(index, content);
    return *this;
}

Config &Config::set_suffix_comment(const std::string &key, const std::string &content) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    reinterpret_cast<tools::ElementJson*>(m_value)->set_suffix_comment(key, content);
    return *this;
}

Comment &Config::get_comment(const size_t& index) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    return reinterpret_cast<tools::IElementContainer*>(m_value)->get_comment(index);
}

Comment &Config::get_comment(const std::string &key)
{
    __CHECK_TYPE_IS_CONTAINER__((*this))
    return reinterpret_cast<tools::ElementJson*>(m_value)->get_comment(key);
}

Comment Config::get_comment(const size_t& index) const {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    return reinterpret_cast<tools::IElementContainer*>(m_value)->get_comment(index);
}

Comment Config::get_comment(const std::string &key) const
{
    __CHECK_TYPE_IS_CONTAINER__((*this))
    return reinterpret_cast<tools::ElementJson*>(m_value)->get_comment(key);
}

std::string Config::get_prefix_comment(const size_t& index) const {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    return reinterpret_cast<tools::IElementContainer*>(m_value)->get_prefix_comment(index);
}

std::string Config::get_prefix_comment(const std::string &key) const {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    return reinterpret_cast<tools::ElementJson*>(m_value)->get_prefix_comment(key);
}

//std::string &Config::get_prefix_comment(const size_t& index) {
//    __CHECK_TYPE_IS_CONTAINER__((*this))
//    return reinterpret_cast<tools::IElementContainer*>(m_value)->get_prefix_comment(index);
//}

//std::string &Config::get_prefix_comment(const std::string &key) {
//    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
//    return reinterpret_cast<tools::ElementJson*>(m_value)->get_prefix_comment(key);
//}

std::string Config::get_suffix_comment(const size_t& index) const {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    return reinterpret_cast<tools::IElementContainer*>(m_value)->get_suffix_comment(index);
}

std::string Config::get_suffix_comment(const std::string &key) const {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    return reinterpret_cast<tools::ElementJson*>(m_value)->get_suffix_comment(key);
}

//std::string &Config::get_suffix_comment(const size_t& index){
//    __CHECK_TYPE_IS_CONTAINER__((*this))
//    return reinterpret_cast<tools::IElementContainer*>(m_value)->get_suffix_comment(index);
//}

//std::string &Config::get_suffix_comment(const std::string &key) {
//    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
//    return reinterpret_cast<tools::ElementJson*>(m_value)->get_suffix_comment(key);
//}

Config &Config::clear_comment(const size_t& index) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    reinterpret_cast<tools::IElementContainer*>(m_value)->clear_comment(index);
    return *this;
}

Config &Config::clear_comment(const std::string &key) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    reinterpret_cast<tools::ElementJson*>(m_value)->clear_comment(key);
    return *this;
}

Config &Config::clear_prefix_comment(const size_t& index) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    reinterpret_cast<tools::IElementContainer*>(m_value)->clear_prefix_comment(index);
    return *this;
}

Config &Config::clear_prefix_comment(const std::string &key) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    reinterpret_cast<tools::ElementJson*>(m_value)->clear_prefix_comment(key);
    return *this;
}

Config &Config::clear_suffix_comment(const size_t& index) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    reinterpret_cast<tools::IElementContainer*>(m_value)->clear_suffix_comment(index);
    return *this;
}

Config &Config::clear_suffix_comment(const std::string &key) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    reinterpret_cast<tools::ElementJson*>(m_value)->clear_suffix_comment(key);
    return *this;
}

Config &Config::delete_comment(const size_t& index) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    reinterpret_cast<tools::IElementContainer*>(m_value)->delete_comment(index);
    return *this;
}

Config &Config::delete_comment(const std::string &key) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    reinterpret_cast<tools::ElementJson*>(m_value)->delete_comment(key);
    return *this;
}

Config &Config::delete_prefix_comment(const size_t& index) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    reinterpret_cast<tools::IElementContainer*>(m_value)->delete_prefix_comment(index);
    return *this;
}

Config &Config::delete_prefix_comment(const std::string &key) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    reinterpret_cast<tools::ElementJson*>(m_value)->delete_prefix_comment(key);
    return *this;
}

Config &Config::delete_suffix_comment(const size_t& index) {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    reinterpret_cast<tools::IElementContainer*>(m_value)->delete_suffix_comment(index);
    return *this;
}

Config &Config::delete_suffix_comment(const std::string &key) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    reinterpret_cast<tools::ElementJson*>(m_value)->delete_suffix_comment(key);
    return *this;
}

Config &Config::setValue() noexcept {
    release();
    m_value = dynamic_cast<tools::IElement*>(new tools::ElementNull());
    return *this;
}

//полностью дублирует метод выше, но с передачей аргумента
Config &Config::setValue(std::nullptr_t) noexcept
{
    return setValue();
}

Config &Config::setValue(const Config &other) noexcept {
    using namespace tools;

    if(this != &other)
    {
        release();
        switch(other.getType()) {
        case ValueType::eNull:      {
            setValue();
            //FIXME: решение, откровенно говоря, странное, может потом исправлю
            if(other.error())
                setError(other.getError());
            break;
        }
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
    using namespace tools;

    if(this != &other)
    {
        release();

        switch(other.getType()) {
        case ValueType::eNull:      {
            setValue();
            //FIXME: решение, откровенно говоря, странное, может потом исправлю
            if(other.error())
                setError(other.getError());
            break;
        }
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

Config &Config::setValue(const tools::IElement &other) noexcept {
    using namespace tools;

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

Config &Config::setValue(tools::IElement &&other) noexcept {
    using namespace tools;

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
    m_value = dynamic_cast<tools::IElement*>(new tools::ElementBool(other));
    return *this;
}

Config &Config::setValue(const long double &other) noexcept {
    release();
    m_value = dynamic_cast<tools::IElement*>(new tools::ElementNumber(other));
    return *this;
}

Config &Config::setValue(long double &&other) noexcept {
    release();
    m_value = dynamic_cast<tools::IElement*>(new tools::ElementNumber(std::move(other)));
    return *this;
}

Config &Config::setValue(const std::string &other) noexcept {
    release();
    m_value = dynamic_cast<tools::IElement*>(new tools::ElementString(other));
    return *this;
}

Config &Config::setValue(std::string &&other) noexcept {
    release();
    m_value = dynamic_cast<tools::IElement*>(new tools::ElementString(std::move(other)));
    return *this;
}

Config &Config::setValue(const tools::ElementArray &other) noexcept {
    release();
    m_value = dynamic_cast<tools::IElement*>(new tools::ElementArray(other));
    return *this;
}

Config &Config::setValue(tools::ElementArray &&other) noexcept {
    release();
    m_value = dynamic_cast<tools::IElement*>(new tools::ElementArray(std::move(other)));
    return *this;
}

Config &Config::setValue(const tools::ElementJson &other) noexcept {
    release();
    m_value = dynamic_cast<tools::IElement*>(new tools::ElementJson(other));
    return *this;
}

Config &Config::setValue(tools::ElementJson &&other) noexcept {
    release();
    m_value = dynamic_cast<tools::IElement*>(new tools::ElementJson(std::move(other)));
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
    return dynamic_cast<tools::ElementBool*>(m_value)->getValue();
}

bool Config::getBool() const {
    __CHECK_TYPE_IS_BOOL__((*this))
    return dynamic_cast<const tools::ElementBool*>(m_value)->getValue();
}

long double &Config::getNumber() {
    __CHECK_TYPE_IS_NUMBER__((*this))
    return dynamic_cast<tools::ElementNumber*>(m_value)->getValue();
}

long double Config::getNumber() const {
    __CHECK_TYPE_IS_NUMBER__((*this))
    return dynamic_cast<const tools::ElementNumber*>(m_value)->getValue();
}

std::string &Config::getString() {
    __CHECK_TYPE_IS_STRING__((*this))
    return dynamic_cast<tools::ElementString*>(m_value)->getValue();
}

std::string Config::getString() const {
    __CHECK_TYPE_IS_STRING__((*this))
    return dynamic_cast<const tools::ElementString*>(m_value)->getValue();
}

bool Config::error() const noexcept {
    using namespace tools;

    IErrorField* ptr = dynamic_cast<IErrorField*>(m_value);
    if(ptr == nullptr)
        return false;
    return ptr->error();
}

std::string Config::getError() const noexcept {
    using namespace tools;

    IErrorField* ptr = dynamic_cast<IErrorField*>(m_value);
    if(ptr == nullptr)
        return "";
    return ptr->getError();
}

void Config::setError()
{
    using namespace tools;

    IErrorField* ptr = dynamic_cast<IErrorField*>(m_value);
    if(ptr == nullptr)
        throw std::logic_error("this type does not support of \"error\" field");
    return ptr->setError();
}

void Config::setError(const std::string &error_string)
{
    using namespace tools;

    IErrorField* ptr = dynamic_cast<IErrorField*>(m_value);
    if(ptr == nullptr)
        throw std::logic_error("this type does not support of \"error\" field");
    return ptr->setError(error_string);
}

void Config::setError(std::string &&error_string)
{
    using namespace tools;

    IErrorField* ptr = dynamic_cast<IErrorField*>(m_value);
    if(ptr == nullptr)
        throw std::logic_error("this type does not support of \"error\" field");
    return ptr->setError(std::move(error_string));
}

Config &Config::get_front() {
    using namespace tools;

    __CHECK_TYPE_IS_CONTAINER__((*this))
    switch(getType()) {
    case ValueType::eArray: return dynamic_cast<ElementArray*>(m_value)->get_front();
    case ValueType::eJson:  return dynamic_cast<ElementJson*>(m_value)->get_front();
    default:                break;
    }

    return *this;
}

Config Config::get_front() const {
    using namespace tools;

    __CHECK_TYPE_IS_CONTAINER__((*this))
    switch(getType()) {
    case ValueType::eArray: return dynamic_cast<const ElementArray*>(m_value)->get_front();
    case ValueType::eJson:  return dynamic_cast<const ElementJson*>(m_value)->get_front();
    default:                break;
    }

    return *this;
}

Config &Config::get_at(const size_t& index) {
    using namespace tools;

    __CHECK_TYPE_IS_CONTAINER__((*this))
    switch(getType()) {
    case ValueType::eArray: return dynamic_cast<ElementArray*>(m_value)->get_at(index);
    case ValueType::eJson:  return dynamic_cast<ElementJson*>(m_value)->get_at(index);
    default:                break;
    }

    return *this;
}

Config Config::get_at(const size_t& index) const {
    using namespace tools;

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
    return dynamic_cast<tools::ElementJson*>(m_value)->get_at(key);
}

Config Config::get_at(const std::string& key) const {
    __CHECK_TYPE_IS_JSON__((*this))
    return dynamic_cast<const tools::ElementJson*>(m_value)->get_at(key);
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
    using namespace tools;

    __CHECK_TYPE_IS_CONTAINER__((*this))
    switch(getType()) {
    case ValueType::eArray: return dynamic_cast<ElementArray*>(m_value)->get_back();
    case ValueType::eJson:  return dynamic_cast<ElementJson*>(m_value)->get_back();
    default:                break;
    }

    return *this;
}

Config Config::get_back() const {
    using namespace tools;

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

    return dynamic_cast<tools::ElementBool*>(config.m_value)->getValue();
}

bool Config::get_front_bool() const {
    const Config& config = get_front();
    __CHECK_TYPE_IS_BOOL__(config)

    return dynamic_cast<const tools::ElementBool*>(config.m_value)->getValue();
}

long double &Config::get_front_number() {
    Config& config = get_front();
    __CHECK_TYPE_IS_NUMBER__(config)

    return dynamic_cast<tools::ElementNumber*>(config.m_value)->getValue();
}

long double Config::get_front_number() const {
    const Config& config = get_front();
    __CHECK_TYPE_IS_NUMBER__(config)

    return dynamic_cast<const tools::ElementNumber*>(config.m_value)->getValue();
}

std::string &Config::get_front_string() {
    Config& config = get_front();
    __CHECK_TYPE_IS_STRING__(config)

    return dynamic_cast<tools::ElementString*>(config.m_value)->getValue();
}

std::string Config::get_front_string() const {
    const Config& config = get_front();
    __CHECK_TYPE_IS_STRING__(config)

    return dynamic_cast<const tools::ElementString*>(config.m_value)->getValue();
}

bool &Config::get_bool_at(const size_t& index) {
    const Config& config = get_at(index);
    __CHECK_TYPE_IS_BOOL__(config)

    return dynamic_cast<tools::ElementBool*>(config.m_value)->getValue();
}

bool Config::get_bool_at(const size_t& index) const {
    const Config& config = get_at(index);
    __CHECK_TYPE_IS_BOOL__(config)

    return dynamic_cast<const tools::ElementBool*>(config.m_value)->getValue();
}

bool &Config::get_bool_at(const std::string& key) {
    const Config& config = get_at(key);
    __CHECK_TYPE_IS_BOOL__(config)

    return dynamic_cast<tools::ElementBool*>(config.m_value)->getValue();
}

bool Config::get_bool_at(const std::string& key) const {
    const Config& config = get_at(key);
    __CHECK_TYPE_IS_BOOL__(config)

    return dynamic_cast<const tools::ElementBool*>(config.m_value)->getValue();
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

    return dynamic_cast<tools::ElementNumber*>(config.m_value)->getValue();
}

long double Config::get_number_at(const size_t& index) const {
    const Config& config = get_at(index);
    __CHECK_TYPE_IS_NUMBER__(config)

    return dynamic_cast<const tools::ElementNumber*>(config.m_value)->getValue();
}

long double &Config::get_number_at(const std::string& key) {
    const Config& config = get_at(key);
    __CHECK_TYPE_IS_NUMBER__(config)

    return dynamic_cast<tools::ElementNumber*>(config.m_value)->getValue();
}

long double Config::get_number_at(const std::string& key) const {
    const Config& config = get_at(key);
    __CHECK_TYPE_IS_NUMBER__(config)

    return dynamic_cast<const tools::ElementNumber*>(config.m_value)->getValue();
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

    return dynamic_cast<tools::ElementString*>(config.m_value)->getValue();
}

std::string Config::get_string_at(const size_t& index) const {
    const Config& config = get_at(index);
    __CHECK_TYPE_IS_STRING__(config)

    return dynamic_cast<const tools::ElementString*>(config.m_value)->getValue();
}

std::string &Config::get_string_at(const std::string& key) {
    const Config& config = get_at(key);
    __CHECK_TYPE_IS_STRING__(config)

    return dynamic_cast<tools::ElementString*>(config.m_value)->getValue();
}

std::string Config::get_string_at(const std::string& key) const {
    const Config& config = get_at(key);
    __CHECK_TYPE_IS_STRING__(config)

    return dynamic_cast<const tools::ElementString*>(config.m_value)->getValue();
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

    return dynamic_cast<tools::ElementBool*>(config.m_value)->getValue();
}

bool Config::get_bool_back() const {
    const Config& config = get_back();
    __CHECK_TYPE_IS_BOOL__(config)

    return dynamic_cast<const tools::ElementBool*>(config.m_value)->getValue();
}

long double &Config::get_number_back() {
    const Config& config = get_back();
    __CHECK_TYPE_IS_NUMBER__(config)

    return dynamic_cast<tools::ElementNumber*>(config.m_value)->getValue();
}

long double Config::get_number_back() const {
    const Config& config = get_back();
    __CHECK_TYPE_IS_NUMBER__(config)

    return dynamic_cast<const tools::ElementNumber*>(config.m_value)->getValue();
}

std::string &Config::get_string_back() {
    const Config& config = get_back();
    __CHECK_TYPE_IS_STRING__(config)

    return dynamic_cast<tools::ElementString*>(config.m_value)->getValue();
}

std::string Config::get_string_back() const {
    const Config& config = get_back();
    __CHECK_TYPE_IS_STRING__(config)

    return dynamic_cast<const tools::ElementString*>(config.m_value)->getValue();
}

void Config::try_convert_null_to_json() noexcept
{
    using namespace tools;

    if(isNull()) {
        release();
        m_value = dynamic_cast<IElement*>(new ElementJson());
    }
}

void Config::try_convert_null_to_json_array() noexcept
{
    using namespace tools;

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
    dynamic_cast<tools::ElementArray*>(m_value)->insert_front(std::move(other));

    return *this;
}

Config& Config::insert_front(const std::string& key, const Config& other) {
    Config config(other);
    return insert_front(key, std::move(config));
}

Config& Config::insert_front(const std::string& key, Config&& other) {
    try_convert_null_to_json();

    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    dynamic_cast<tools::ElementJson*>(m_value)->insert_front(key, std::move(other));

    return *this;
}

Config& Config::insert_at(const size_t& index, const Config& other) {
    Config config(other);
    return insert_at(index, std::move(config));
}

Config& Config::insert_at(const size_t& index, Config&& other) {
    try_convert_null_to_json_array();

    __CHECK_TYPE_IS_INDEX_CONTAINER__((*this))
    dynamic_cast<tools::ElementArray*>(m_value)->insert_at(index, std::move(other));

    return *this;
}

Config& Config::insert_at(const std::string& key, const Config& other) {
    Config config(other);
    return insert_at(key, std::move(config));
}

Config& Config::insert_at(const std::string& key, Config&& other) {
    try_convert_null_to_json();

    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    dynamic_cast<tools::ElementJson*>(m_value)->insert_at(key, std::move(other));

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
    dynamic_cast<tools::ElementJson*>(m_value)->insert_at(index, key, std::move(other));

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
    dynamic_cast<tools::ElementArray*>(m_value)->insert_at(iterator, std::move(other));

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
    dynamic_cast<tools::ElementJson*>(m_value)->insert_at(iterator, key, std::move(other));

    return *this;
}

Config& Config::insert_back(const Config& other) {
    Config config(other);
    return insert_back(std::move(config));
}

Config& Config::insert_back(Config&& other) {
    try_convert_null_to_json_array();

    __CHECK_TYPE_IS_INDEX_CONTAINER__((*this))
    dynamic_cast<tools::ElementArray*>(m_value)->insert_back(std::move(other));

    return *this;
}

Config& Config::insert_back(const std::string& key, const Config& other) {
    Config config(other);
    return insert_back(key, std::move(config));
}

Config& Config::insert_back(const std::string& key, Config&& other) {
    try_convert_null_to_json();

    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    dynamic_cast<tools::ElementJson*>(m_value)->insert_back(key, std::move(other));

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
    dynamic_cast<tools::ElementJson*>(m_value)->insert_before(before_key, key, std::move(other));

    return *this;
}

Config& Config::insert_after(const std::string& after_key, const std::string& key, const Config& other) {
    Config config(other);
    return insert_after(after_key, key, std::move(config));
}

Config& Config::insert_after(const std::string& after_key, const std::string& key, Config&& other) {
    try_convert_null_to_json();

    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    dynamic_cast<tools::ElementJson*>(m_value)->insert_after(after_key, key, std::move(other));

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
            dynamic_cast<tools::ElementArray*>(m_value)->append(std::move(dynamic_cast<tools::ElementArray&&>(std::move(*config.m_value))));
            break;
        }
        case ValueType::eJson: {
            dynamic_cast<tools::ElementJson*>(m_value)->append(std::move(dynamic_cast<tools::ElementJson&&>(std::move(*config.m_value))));
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
    dynamic_cast<tools::IElementContainer*>(m_value)->erase_at(index);

    return *this;
}

Config& Config::erase_at(const std::string& key) {
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    dynamic_cast<tools::ElementJson*>(m_value)->erase_at(key);

    return *this;
}

Config& Config::erase_back() {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    dynamic_cast<tools::IElementContainer*>(m_value)->erase_back();

    return *this;
}

Config &Config::erase_at(const shared_VElement::iterator iterator)
{
    __CHECK_TYPE_IS_ARRAY__((*this))
    dynamic_cast<tools::ElementArray*>(m_value)->erase_at(iterator);

    return *this;
}

Config &Config::erase_at(const shared_VPairElement::iterator iterator)
{
    __CHECK_TYPE_IS_MAP_CONTAINER__((*this))
    dynamic_cast<tools::ElementJson*>(m_value)->erase_at(iterator);

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

bool Config::isEqual(const tools::IElement &other, const bool compare_comments,
                     const bool map_sort_important) const noexcept
{
    __CHECK_TYPES_NOT_EQUAL_ACTION__((*m_value), other)
        return false;

    return m_value->isEqual(other, compare_comments, map_sort_important);
}

bool Config::isEqual(const bool other) const noexcept {
    __CHECK_TYPE_NOT_BOOL_ACTION__((*this))
        return false;

    return dynamic_cast<const tools::ElementBool*>(m_value)->getValue() == other;
}

bool Config::isEqual(const long double &other) const noexcept {
    __CHECK_TYPE_NOT_NUMBER_ACTION__((*this))
        return false;

    return dynamic_cast<const tools::ElementNumber*>(m_value)->getValue() == other;
}

bool Config::isEqual(const std::string &other) const noexcept {
    __CHECK_TYPE_NOT_STRING_ACTION__((*this))
        return false;

    return dynamic_cast<const tools::ElementString*>(m_value)->getValue() == other;
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
    CommentDesign n_design = design;
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

std::ostream &operator<<(std::ostream &os, const tools::IElement &config) noexcept {
    os << config.toString();
    return os;
}

//WIKI: если
bool Config::readFile(const std::string &file_path, const ConfigFormat format,
                      const CommentDesign &design) noexcept
{
    release();
    switch(format) {
    case ConfigFormat::eJSON:   return readFileJson(file_path, design);
    case ConfigFormat::eINI:    return readFileIni(file_path, design);
    // case ConfigFormat::eYAML:
    // case ConfigFormat::eXML:
    default: break;
    }

    setError("unexpected ConfigFormat");
    return false;
}

//FIXME: m_error_str должен обнуляться
bool Config::readFileJson(const std::string &file_path, const CommentDesign &design) noexcept
{
    CommentDesign n_design = design;
    if(design == CommentDesign{}) { // если переменная не заполнена, используется соб
        n_design = m_value->getCommentDesign();
    }

    std::string input_str;
    std::string error_str;
    if(GetAllStringsFromFile(file_path, input_str, &error_str)) {
        return parseJson(input_str, n_design);
    }

    setError(error_str);
    return false;
}

//FIXME: m_error_str должен обнуляться
bool Config::readFileIni(const std::string &file_path, const CommentDesign &design) noexcept
{
    CommentDesign n_design = design;
    if(design == CommentDesign{}) { // если переменная не заполнена, используется соб
        n_design = m_value->getCommentDesign();
    }

    std::string input_str;
    std::string error_str;
    if(GetAllStringsFromFile(file_path, input_str, &error_str)) {
        return parseIni(input_str, n_design);
    }

    setError(error_str);
    return false;
}

bool Config::writeFile(const std::string &file_path,  const ConfigFormat format,
                       const CommentDesign &design,
                       const int8_t custom_tabulation_level) noexcept
{
    CommentDesign n_design = design;
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
    CommentDesign n_design = design;
    if(design == CommentDesign{}) { // если переменная не заполнена, используется соб
        n_design = m_value->getCommentDesign();
    }
    return WriteFileJson(*this, file_path, n_design, custom_tabulation_level);
}

bool Config::writeFileIni(const std::string &file_path, const CommentDesign &design,
                          const int8_t custom_tabulation_level) noexcept
{
    CommentDesign n_design = design;
    if(design == CommentDesign{}) { // если переменная не заполнена, используется собственная
        n_design = m_value->getCommentDesign();
    }
    return WriteFileIni(*this, file_path, n_design, custom_tabulation_level);
}

bool Config::parse(const std::string &content, const ConfigFormat format,
                   const CommentDesign &design) noexcept
{
    CommentDesign n_design = design;
    if(design == CommentDesign{}) { // если переменная не заполнена, используется соб
        n_design = m_value->getCommentDesign();
    }

    release();

    switch(format) {
    case ConfigFormat::eJSON:   return parseJson(content, n_design);
    case ConfigFormat::eINI:    return parseIni(content, n_design);
    case ConfigFormat::eYAML:   return parseYaml(content, n_design);
    case ConfigFormat::eXML:    return parseXml(content, n_design);
    default:                    break;
    }

    Config out;
    out.setError("unexpected ConfigFormat");
    return false;
}

bool Config::parseJson(const std::string &content, const CommentDesign &design) noexcept
{
    release();
    *this = SpecificParserJson(content, design);
    return !error();
}

//TODO: INI PARSER
bool Config::parseIni(const std::string &content, const CommentDesign &input_design) noexcept
{
    using namespace tools;

    setValue(ElementJson());
    setCommentDesign(input_design);
    CommentDesign& design = getCommentDesign();

    VString lines;
    // разбить content на строки
    for(size_t i = 0; i < content.size(); /*i++*/) {
        const size_t i_ = i;
        i = content.find('\n', i_);
        if(i == std::string::npos) {
            // std::cout << "new iter: [" << i_ << "," << (content.size() - 1) << "]" << std::endl;
            lines.push_back(content.substr(i_, content.size() - 1));
            break;
        } else {
            const size_t end_i = i > i_ ? i - 1 : i;
            // std::cout << "new iter: [" << i_ << "," << end_i << "]" << std::endl;
            lines.push_back(content.substr(i_, end_i - i_ + (i == i_ ? 0 : 1))); //уникальный случай "нулевая длина"
        }
        i += 1;
    }

    /* совместить многострочные значения:
     * - кавычки начаты, но не закончены  (скобки тоже считаются {}[]<>())  (+ \n + следующая, следующую удалить)
     * - есть обратный слэш в конце строки                                  (+ \n + следующая, следующую удалить)
     * - на следующей строке первыми символами идёт пробел/табуляция        (+ \n + следующая, следующую удалить)
     * - начат многострочный комментарий, но не закончен                    (+ \n + следующая, следующую удалить)
     */
    // NOTE: комментарии внутри значения являются частью значения, а не комментарием самого значения

    // обработка обратного слэша на конце строкsи и пробела в начале следующей
    VVString vlines;
    while(!lines.empty()) {
        vlines.push_back({lines.front()});
        lines.erase(lines.cbegin());

        while(!lines.empty())
        {
            const char c = utils::GetLastNotSpaceChar(vlines.back().back());
            if(lines.size() >= 1) // следующая строка существует?
            {
                if(c == '\\' || utils::CharInString(lines.front().front(), "\t ")) //следующая строка является частью значения
                {
                    vlines.back().push_back(lines.front());
                    lines.erase(lines.cbegin());
                }
                else break; //выход из while
            }
            else break; //выход из while
        }
    }
    // по выходу из цикла lines должна быть пуста

    // TEST --------------------------------------
    size_t value_counter = 0;
    for(const auto& lines_ : vlines)
    {
        push_back(std::to_string(value_counter), lines_[0]);
        std::string& last_string = get_string_back();
        for(size_t i = 1; i < lines_.size(); i++) {
            if(i + 1 < lines_.size()) {
                last_string.push_back('\n');
            }
            last_string += lines_[i];
        }
        value_counter++;
    }
    push_back("__", "------------------------------------------------------");
    // TEST --------------------------------------

    // один фрагмент - одно значение (часть многострочного комментария ПОСЛЕ значеня может остаться за бортом)
    size_t k = 0;
    for(VString& fragments : vlines) {
        std::vector<Comment> comments;
        std::string current_comment;
        std::string temp_string_value;

        bool is_quotes = false;
        size_t inner_fugure_brackets_counter      = 0; // {}
        size_t inner_square_brackets_counter      = 0; // []
        size_t inner_triangulare_brackets_counter = 0; // <>
        size_t inner_parentheses_counter          = 0; // ()

        char ch_previous;
        char ch_current;
        char ch_next;
        for(size_t i = 0; i < fragments.size(); i++) {
            for(size_t j = 0; j < fragments[i].size(); j++) {
                ch_previous  = (i == 0) ? (j == 0 ? 0 : fragments[i][j - 1])
                                        : (fragments[i-1].back());
                ch_current   = fragments[i][j];
                ch_next      = j + 1 < fragments[i].size() ? (fragments[i][j + 1])
                                                           : (i + 1 < fragments.size() ? (fragments[i+1].front()) : 0);

//TODO:                counter.check(j, ch_current);

                //поиск комментариев ===================================================
                const bool ext_flag = !is_quotes
                                      && (inner_fugure_brackets_counter
                                              + inner_square_brackets_counter
                                              + inner_triangulare_brackets_counter +
                                              inner_parentheses_counter == 0);
                //вернёт комментарий без обрамления
                CheckComments(ch_current, ch_next, i, design, current_comment, ext_flag);
                if(!design.with_comments)
                    current_comment.clear();
                if(design.with_comments && design.temp_type == CommentType::eCommentEnd)
                {
                    std::cout << "current_comment: \"" << current_comment << "\"" << std::endl;
                    comments.push_back(FromComment(current_comment, design));
                    current_comment.clear();
                    design.temp_type = CommentType::eNotComment;
                    continue;
                }
                if(design.temp_type != CommentType::eNotComment)
                    continue;
                //=================================================== поиск комментариев
            }
        }

        k++;
    }

    return !error();
}

bool Config::parseYaml(const std::string &content, const CommentDesign &design) noexcept
{
    //TODO: Config::parseYaml()
    return false;
}

bool Config::parseXml(const std::string &content, const CommentDesign &design) noexcept
{
    //TODO: Config::parseXml()
    return false;
}

shared_VElement::iterator Config::array_begin() {
    __CHECK_TYPE_IS_ARRAY__((*this))
    return dynamic_cast<tools::ElementArray*>(m_value)->begin();
}

shared_VElement::const_iterator Config::array_cbegin() const {
    __CHECK_TYPE_IS_ARRAY__((*this))
    return dynamic_cast<const tools::ElementArray*>(m_value)->cbegin();
}

shared_VElement::iterator Config::array_end() {
    __CHECK_TYPE_IS_ARRAY__((*this))
    return dynamic_cast<tools::ElementArray*>(m_value)->end();
}

shared_VElement::const_iterator Config::array_cend() const {
    __CHECK_TYPE_IS_ARRAY__((*this))
    return dynamic_cast<const tools::ElementArray*>(m_value)->cend();
}

shared_VPairElement::iterator Config::map_begin() {
    __CHECK_TYPE_IS_JSON__((*this))
    return dynamic_cast<tools::ElementJson*>(m_value)->begin();
}

shared_VPairElement::const_iterator Config::map_cbegin() const {
    __CHECK_TYPE_IS_JSON__((*this))
    return dynamic_cast<const tools::ElementJson*>(m_value)->cbegin();
}

shared_VPairElement::iterator Config::map_end() {
    __CHECK_TYPE_IS_JSON__((*this))
    return dynamic_cast<tools::ElementJson*>(m_value)->end();
}

shared_VPairElement::const_iterator Config::map_cend() const {
    __CHECK_TYPE_IS_JSON__((*this))
    return dynamic_cast<const tools::ElementJson*>(m_value)->cend();
}

//функция должна быть вызвана исключительно для обработки строки значения, комменты не учитывает
Config CreateElementFromString(std::string &&value_string, const ConfigFormat format,
                               CommentDesign &design, ParserSymbolCounter& start_iterator) noexcept
{
    using namespace utils;
    using namespace tools;

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
                    ElementArray el_arr(value_string, ConfigFormat::eJSON, design);
                    array.setValue(el_arr);
                } else {
                    //есть начало массива, но нет конца
                    array.setError("not found end of Json-array value");
                }
                return array;
            }
        }
        /*JSON*/ {
            if(first == '{') {
                Config json;
                if(last == '}') {
                    json.setCommentDesign(design);
                    json.parseJson(value_string, design);
                } else {
                    //есть начало Json, но нет конца
                    json.setError("not found end of Json value");
                }
                return json;
            }
        }
    }

    // в теории, всё, что не распарсилось в другие значения, - должно считаться строкой
    //FIXME: строка не должна начинаться с технических скобок
    return Config(value_string);
}

//NOTE: функция нужна исключительно для перенаправления на внутренние парсеры
Config SpecificParser(const std::string& content, const ConfigFormat format,
                        const CommentDesign &design, const int8_t yaml_tabulation_level) noexcept
{
    switch(format) {
    case ConfigFormat::eONLY_VALUE:
    case ConfigFormat::eJSON:       return SpecificParserJson(content, design);
    case ConfigFormat::eYAML:       return SpecificParserYaml(content, design, yaml_tabulation_level);
    case ConfigFormat::eXML:        return SpecificParserXml(content, design);
    case ConfigFormat::eINI:
    default:                        break;
    }

    Config config;
    config.setError("incorrect format for SpecificParser()");
    return config;
}

// Задача функции: определить тип значения верхнего уровня и передать в соответствующий обработчик
Config SpecificParserJson(const std::string& content, const CommentDesign &design) noexcept
{
    /* игнорируя комментарий, найти первое вхождение символа ключа(значения)
     * определить следующий после "слова" символ-разделитель, если он есть
     *   - комментарий после "слова" тоже игнорируется
     * если за символами пробела есть разделитель key-value - передать на парсер ElementJson::parseJson()
     * если за "пробельными символами" есть новое "слово" - явно ошибка
     * иначе использовать парсер CreateElementFromString(), а комментарии сохранить здесь же
     */
    std::string current_comment;
    VString comments;
    bool is_quotes             = false;
    size_t inner_json_counter  = 0;
    size_t inner_array_counter = 0;

    bool value_found        = false;
    size_t value_started_at = 0;
    bool is_full_json       = false;

    ParserSymbolCounter counter;
    CommentDesign n_design = design;

    //нужно определить тип значения: "100%-json", "json с одним ключом" или что-то иное
    {
        for(size_t i = 0; i < content.size(); i++) {
            const char ch_current  = content[i];
            const char ch_next     = i < content.size() ? content[i + 1] : 0;

            counter.check(i, ch_current);

            //поиск комментариев ===================================================
            const bool ext_flag = !is_quotes && (inner_array_counter + inner_json_counter == 0);
            //вернёт комментарий без обрамления
            tools::CheckComments(ch_current, ch_next, i, n_design, current_comment, ext_flag);
            if(!n_design.with_comments)
                current_comment.clear();
            if(n_design.with_comments && n_design.temp_type == CommentType::eCommentEnd)
            {
                // запомнить комментарий, если он потом понадобится для одиночного значения
//                comments.push_back(FromComment(std::move(current_comment), design)); //FIXME: на будущее
                comments.push_back(tools::FromComment(current_comment, n_design));
                current_comment.clear();
                n_design.temp_type = CommentType::eNotComment;
                continue;
            }
            if(n_design.temp_type != CommentType::eNotComment)
                continue; // здесь комментарий, не обрабатываем символ
            //=================================================== поиск комментариев

            //первый не пробельный символ является определителем
            if(!value_found && !utils::CharInString(ch_current, __SPACES__))
            {
                value_found      = true;
                value_started_at = i;
                if(ch_current == '{') {
                    is_full_json = true;
                    break;
                }
            }

            //находим следующие определители
            if(value_found) {
                if(inner_json_counter == 0 && inner_array_counter == 0)
                {
                    if(utils::CharInString(ch_current, "=:")) {
                        is_full_json = true;
                        break;
                    }
                }
            }

            //само наличие этих знаков
            switch(ch_current){
            case '{': inner_json_counter++;     break;
            case '}': inner_json_counter--;     break;
            case '[': inner_array_counter++;    break;
            case ']': inner_array_counter--;    break;
            }
        }
    }

    Config result_cfg;
    result_cfg.setCommentDesign(n_design);
    std::string prefix_comment = tools::VStringToString(comments);
    std::string error;

    result_cfg.release();
    result_cfg.setPrefixComment(std::move(prefix_comment));

    if(is_full_json) {
        tools::ElementJson element_json(content, ConfigFormat::eJSON, n_design, &error);
        result_cfg.setValue(element_json);
        result_cfg.setError(std::move(error));
    } else if(value_found) {
        //сначала нужно отделить комментарии, затем обработать внутреннее значение через CreateElementFromString()
        //начальные комментарии уже известны
        //здесь только значение, ключа быть не может
        ParserSymbolCounter start_value_counter; //будет использовано в CreateElementFromString()
        std::string value;

        //выровнять счётчик под текущую позицию (FIXME: можно перенести выше в момент нахождения начала значения)
        for(size_t i = 0; i < value_started_at; i++)
            counter.check(i, content[i]);

        auto ConfirmValue = [&, content](const bool for_penultimate = false) -> bool {
            Config element = CreateElementFromString(std::move(value), ConfigFormat::eJSON,
                                                     result_cfg.getCommentDesign(), start_value_counter);
            if(element.error()) {
                //если случилась ошибка при внутренней конвертации прочитанного значения,
                // то эта ошибка становится основной ошибкой парсинга
                error = "value parse error[" + std::to_string(counter.getLastLineCounter())
                        + "][" + std::to_string(counter.getLastSymbolCounter()) + "]: " + element.getError();
                return false;
            }

            result_cfg = Config(std::move(element));
            if(result_cfg.getCommentDesign().opt_multiline_column_size > n_design.opt_multiline_column_size)
                n_design.opt_multiline_column_size = result_cfg.getCommentDesign().opt_multiline_column_size;

            return true;
        };

        bool value_stored = false;
        //нужно прочитать значение полностью и отделить его от замыкающего комментария
        for(size_t i = value_started_at; i < content.size(); i++) {
            char ch_previous    = i == 0 ? 0 : content[i - 1];
            char ch_current     = content[i];
            char ch_next        = i < content.size() ? content[i + 1] : 0;

            counter.check(i, ch_current);

            //поиск комментариев ===================================================
            const bool ext_flag = !is_quotes && (inner_array_counter + inner_json_counter == 0);
            //вернёт комментарий без обрамления
            tools::CheckComments(ch_current, ch_next, i, n_design, current_comment, ext_flag);
            if(!n_design.with_comments)
                current_comment.clear();
            if(n_design.with_comments && n_design.temp_type == CommentType::eCommentEnd)
            {
                comments.push_back(tools::FromComment(current_comment, n_design));
                current_comment.clear();
                n_design.temp_type = CommentType::eNotComment;
                continue;
            }
            if(n_design.temp_type != CommentType::eNotComment)
                continue;
            //=================================================== поиск комментариев


            if(!value_stored) {
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
                value += content[i];

                //значение прочитано полностью?
                if(!is_quotes && inner_json_counter + inner_array_counter == 0
                    && (ch_next == 0 || utils::CharInString(ch_next, __SEPARATORS__ " }")
                        || utils::CharInString(ch_current, __SEPARATORS__ " }")))
                {
                    if(!ConfirmValue(true))
                        break; //если словили exception при обработке - выходим из цикла for()
                    //иначе отмечаем, что значение прочитано корректно
                    value_stored = true;
                }
            } else {
                // все остальные символы кроме пробелов - признак ошибки парсера
                if(!utils::CharInString(ch_current, __SPACES__)) {
                    error = "value parse error[" + std::to_string(counter.getLastLineCounter())
                            + "][" + std::to_string(counter.getLastSymbolCounter()) + "]: \'" + content[i] + "\'";
                    break;
                }
            }
        } //for loop
        if(error.empty()) {
            result_cfg.setSuffixComment(tools::VStringToString(comments));
        } else {
            result_cfg.setValue(); //значение не распознано, то выставить в null
            result_cfg.setError(error);
        }
    }
    return result_cfg;
}

Config SpecificParserYaml(const std::string& content, const CommentDesign &design,
                            const int8_t yaml_tabulation_level) noexcept
{
    //TODO: SpecificParserYaml()
    return {};
}

Config SpecificParserXml(const std::string& content, const CommentDesign &design) noexcept
{
    /* игнорируя комментарий, найти первое вхождение символа ключа(значения)
     * определить следующий после "слова" символ-разделитель, если он есть
     *   - комментарий после "слова" тоже игнорируется
     * если за символами пробела есть разделитель key-value - передать на парсер ElementJson::parseJson()
     * если за "пробельными символами" есть новое "слово" - явно ошибка
     * иначе использовать парсер CreateElementFromString(), а комментарии сохранить здесь же
     */

    //TODO: SpecificParserXml()
    return {};
}


std::pair<bool, Config> ReadFile(const std::string &file_path, const ConfigFormat format,
                const CommentDesign &design) noexcept
{
    switch(format) {
    case ConfigFormat::eJSON:   return ReadFileJson(file_path, design);
    case ConfigFormat::eINI:    return ReadFileIni(file_path, design);
//    case ConfigFormat::eYAML:
//    case ConfigFormat::eXML:
    default: break;
    }

    Config out;
    out.setError("unexpected ConfigFormat");
    return std::make_pair(false, out);
}

//FIXME: m_error_str должен обнуляться
std::pair<bool, Config> ReadFileJson(const std::string &file_path, const CommentDesign &design) noexcept
{
    std::string input_str;
    std::string error_str;

    if(GetAllStringsFromFile(file_path, input_str, &error_str)) {
        return ParseJson(input_str, design);
    }

    Config out;
    out.setError(error_str);
    return std::make_pair(false, out);
}

//FIXME: m_error_str должен обнуляться
std::pair<bool, Config> ReadFileIni(const std::string &file_path, const CommentDesign &design) noexcept
{
    std::string input_str;
    std::string error_str;

    if(GetAllStringsFromFile(file_path, input_str, &error_str)) {
        return ParseIni(input_str, design);
    }

    Config out;
    out.setError(error_str);
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

std::pair<bool, Config> Parse(const std::string &content, const ConfigFormat format,
                              const CommentDesign &design) noexcept
{
    switch(format) {
    case ConfigFormat::eJSON:   return ParseJson(content, design);
    case ConfigFormat::eINI:    return ParseIni(content, design);
//    case ConfigFormat::eYAML:   return ParseYaml(content, with_comments);
//    case ConfigFormat::eXML:    return ParseXml(content, with_comments);
    default:                    break;
    }

    Config out;
    out.setError("unexpected ConfigFormat");
    return std::make_pair(false, out);
}

std::pair<bool, Config> ParseJson(const std::string &content, const CommentDesign &design) noexcept
{
    Config ret(ValueType::eJson);

    CommentDesign n_design = design;
    if(design == CommentDesign{}) { // если переменная не заполнена, используется соб
        n_design = ret.getCommentDesign();
    }

    bool result = ret.parseJson(content, n_design);
    return std::make_pair(result, ret);
}

std::pair<bool, Config> ParseIni(const std::string &content, const CommentDesign &design) noexcept
{
    Config ret(ValueType::eJson);

    CommentDesign n_design = design;
    if(design == CommentDesign{}) { // если переменная не заполнена, используется соб
        n_design = ret.getCommentDesign();
    }

    bool result = ret.parseIni(content, n_design);
    return std::make_pair(result, ret);
}

} // namespace simpleapi
