#include "Config.h"

#include "ElementNull.h"
#include "ElementBool.h"
#include "ElementNumber.h"
#include "ElementString.h"
#include "ElementArray.h"
#include "ElementJson.h"
#include "../utils/Utils.h"

#include <stdexcept>


Config &Config::operator=(const Config& other) noexcept {
    if(m_element.get() != other.m_element.get())
        m_element = other.m_element;
    return *this;
}

Config &Config::operator=(Config&& other) noexcept {
    if(m_element.get() != other.m_element.get())
        m_element = other.m_element;
    return *this;
}

bool Config::operator==(const Config& other) const {
    return m_element == other.m_element;
}

//числа, контейнеры(размер), строки(количество выводимых СИМВОЛОВ)
bool Config::operator>(const Config& other) const noexcept {
    __CHECK_TYPE_IS_NOT_NULL_EXCEPTION__((*this))
    __CHECK_TYPES_IS_EQUAL_EXCEPTION__((*this), other)

    using namespace utils;
    switch(getType()) {
    case ValueType::eBool:
        return dynamic_cast<const ElementBool*>(m_element.get())->getValue() >
               dynamic_cast<const ElementBool*>(other.m_element.get())->getValue();
    case ValueType::eNumber:
        return dynamic_cast<const ElementNumber*>(m_element.get())->getValue() >
               dynamic_cast<const ElementNumber*>(other.m_element.get())->getValue();
    case ValueType::eString:
        return GetStringCharCount(dynamic_cast<const ElementString*>(m_element.get())->getValue()) >
               GetStringCharCount(dynamic_cast<const ElementString*>(other.m_element.get())->getValue());
    case ValueType::eJson:
    case ValueType::eArray:
        return size() > other.size();
    default: return false;
    }
}

bool Config::operator>(const IElement& other) const noexcept {
    __CHECK_TYPE_IS_NOT_NULL_EXCEPTION__((*this))
    __CHECK_TYPES_IS_EQUAL_EXCEPTION__((*this), other)

    using namespace utils;
    switch(getType()) {
    case ValueType::eBool:
        return dynamic_cast<const ElementBool*>(m_element.get())->getValue() >
               dynamic_cast<const ElementBool*>(&other)->getValue();
    case ValueType::eNumber:
        return dynamic_cast<const ElementNumber*>(m_element.get())->getValue() >
               dynamic_cast<const ElementNumber*>(&other)->getValue();
    case ValueType::eString:
        return GetStringCharCount(dynamic_cast<const ElementString*>(m_element.get())->getValue()) >
               GetStringCharCount(dynamic_cast<const ElementString*>(&other)->getValue());
    case ValueType::eJson:
    case ValueType::eArray:
        return size() > other.size();
    default: return false;
    }
}

bool Config::operator>=(const Config& other) const noexcept {
    __CHECK_TYPE_IS_NOT_NULL_EXCEPTION__((*this))
    __CHECK_TYPES_IS_EQUAL_EXCEPTION__((*this), other)

    using namespace utils;
    switch(getType()) {
    case ValueType::eBool:
        return dynamic_cast<const ElementBool*>(m_element.get())->getValue() >=
               dynamic_cast<const ElementBool*>(other.m_element.get())->getValue();
    case ValueType::eNumber:
        return dynamic_cast<const ElementNumber*>(m_element.get())->getValue() >=
               dynamic_cast<const ElementNumber*>(other.m_element.get())->getValue();
    case ValueType::eString:
        return GetStringCharCount(dynamic_cast<const ElementString*>(m_element.get())->getValue()) >=
               GetStringCharCount(dynamic_cast<const ElementString*>(other.m_element.get())->getValue());
    case ValueType::eJson:
    case ValueType::eArray:
        return size() >= other.size();
    default: return false;
    }
}

bool Config::operator>=(const IElement& other) const noexcept {
    __CHECK_TYPE_IS_NOT_NULL_EXCEPTION__((*this))
    __CHECK_TYPES_IS_EQUAL_EXCEPTION__((*this), other)

    using namespace utils;
    switch(getType()) {
    case ValueType::eBool:
        return dynamic_cast<const ElementBool*>(m_element.get())->getValue() >=
               dynamic_cast<const ElementBool*>(&other)->getValue();
    case ValueType::eNumber:
        return dynamic_cast<const ElementNumber*>(m_element.get())->getValue() >=
               dynamic_cast<const ElementNumber*>(&other)->getValue();
    case ValueType::eString:
        return GetStringCharCount(dynamic_cast<const ElementString*>(m_element.get())->getValue()) >=
               GetStringCharCount(dynamic_cast<const ElementString*>(&other)->getValue());
    case ValueType::eJson:
    case ValueType::eArray:
        return size() >= other.size();
    default: return false;
    }
}

bool Config::operator<(const Config& other) const noexcept {
    __CHECK_TYPE_IS_NOT_NULL_EXCEPTION__((*this))
    __CHECK_TYPES_IS_EQUAL_EXCEPTION__((*this), other)

    using namespace utils;
    switch(getType()) {
    case ValueType::eBool:
        return dynamic_cast<const ElementBool*>(m_element.get())->getValue() <
               dynamic_cast<const ElementBool*>(other.m_element.get())->getValue();
    case ValueType::eNumber:
        return dynamic_cast<const ElementNumber*>(m_element.get())->getValue() <
               dynamic_cast<const ElementNumber*>(other.m_element.get())->getValue();
    case ValueType::eString:
        return GetStringCharCount(dynamic_cast<const ElementString*>(m_element.get())->getValue()) <
               GetStringCharCount(dynamic_cast<const ElementString*>(other.m_element.get())->getValue());
    case ValueType::eJson:
    case ValueType::eArray:
        return size() < other.size();
    default: return false;
    }
}

bool Config::operator<(const IElement& other) const noexcept {
    __CHECK_TYPE_IS_NOT_NULL_EXCEPTION__((*this))
    __CHECK_TYPES_IS_EQUAL_EXCEPTION__((*this), other)

    using namespace utils;
    switch(getType()) {
    case ValueType::eBool:
        return dynamic_cast<const ElementBool*>(m_element.get())->getValue() <=
               dynamic_cast<const ElementBool*>(&other)->getValue();
    case ValueType::eNumber:
        return dynamic_cast<const ElementNumber*>(m_element.get())->getValue() <=
               dynamic_cast<const ElementNumber*>(&other)->getValue();
    case ValueType::eString:
        return GetStringCharCount(dynamic_cast<const ElementString*>(m_element.get())->getValue()) <=
               GetStringCharCount(dynamic_cast<const ElementString*>(&other)->getValue());
    case ValueType::eJson:
    case ValueType::eArray:
        return size() <= other.size();
    default: return false;
    }
}

bool Config::operator<=(const Config& other) const noexcept {
    __CHECK_TYPE_IS_NOT_NULL_EXCEPTION__((*this))
    __CHECK_TYPES_IS_EQUAL_EXCEPTION__((*this), other)

    using namespace utils;
    switch(getType()) {
    case ValueType::eBool:
        return dynamic_cast<const ElementBool*>(m_element.get())->getValue() <=
               dynamic_cast<const ElementBool*>(other.m_element.get())->getValue();
    case ValueType::eNumber:
        return dynamic_cast<const ElementNumber*>(m_element.get())->getValue() <=
               dynamic_cast<const ElementNumber*>(other.m_element.get())->getValue();
    case ValueType::eString:
        return GetStringCharCount(dynamic_cast<const ElementString*>(m_element.get())->getValue()) <=
               GetStringCharCount(dynamic_cast<const ElementString*>(other.m_element.get())->getValue());
    case ValueType::eJson:
    case ValueType::eArray:
        return size() <= other.size();
    default: return false;
    }
}

bool Config::operator<=(const IElement& other) const noexcept {
    __CHECK_TYPE_IS_NOT_NULL_EXCEPTION__((*this))
    __CHECK_TYPES_IS_EQUAL_EXCEPTION__((*this), other)

    using namespace utils;
    switch(getType()) {
    case ValueType::eBool:
        return dynamic_cast<const ElementBool*>(m_element.get())->getValue() <=
               dynamic_cast<const ElementBool*>(&other)->getValue();
    case ValueType::eNumber:
        return dynamic_cast<const ElementNumber*>(m_element.get())->getValue() <=
               dynamic_cast<const ElementNumber*>(&other)->getValue();
    case ValueType::eString:
        return GetStringCharCount(dynamic_cast<const ElementString*>(m_element.get())->getValue()) <=
               GetStringCharCount(dynamic_cast<const ElementString*>(&other)->getValue());
    case ValueType::eJson:
    case ValueType::eArray:
        return size() <= other.size();
    default: return false;
    }
}

Config &Config::operator<<(const Config& other) noexcept {
    ...
}

Config &Config::operator<<(const IElement& other) noexcept {
    ...
}

Config  Config::operator>>(const Config& other) noexcept {
    ...
}

Config  Config::operator>>(const IElement& other) noexcept {
    ...
}

Config &Config::operator[](const size_t index) noexcept {
    ...
}

bool Config::isContainer() const noexcept {
    switch(getType()) {
    //контейнерные типы
    case ValueType::eJson:
    case ValueType::eArray:     return true;
    //все остыльные
    case ValueType::eNull:
    case ValueType::eNumber:
    case ValueType::eBool:
    case ValueType::eString:    break;
    }

    return false;
}

Config  Config::operator[](const size_t index) const noexcept {
    ...
}

Config &Config::operator[](const std::string key) noexcept {
    ...
}

Config  Config::operator[](const std::string key) const noexcept {
    ...
}

size_t Config::size() const noexcept {
    switch(getType()) {
    case ValueType::eNull:      return 0; //TODO: размер NULL элемента под вопросом
    case ValueType::eNumber:
    case ValueType::eBool:
    case ValueType::eString:    return 1;
    case ValueType::eArray:     return dynamic_cast<const ElementArray*>(m_element.get())->size();
    case ValueType::eJson:      return dynamic_cast<const ElementJson*>(m_element.get())->size();
//    default:                    return 0;
    }
}

bool Config::isEqual(const Config &other, const bool compare_comments) const noexcept {
    return compare_comments ? m_element->isEqualWithComments(*other.m_element)
                            : m_element->isEqual(*other.m_element);
}

bool &Config::getBool() {
    __CHECK_TYPE_IS_BOOL_EXCEPTION__((*this))
    return dynamic_cast<ElementBool*>(m_element.get())->getValue();
}

bool Config::getBool() const {
    __CHECK_TYPE_IS_BOOL_EXCEPTION__((*this))
    return dynamic_cast<const ElementBool*>(m_element.get())->getValue();
}

long double &Config::getNumber() {
    __CHECK_TYPE_IS_NUMBER_EXCEPTION__((*this))
    return dynamic_cast<ElementNumber*>(m_element.get())->getValue();
}

long double Config::getNumber() const {
    __CHECK_TYPE_IS_NUMBER_EXCEPTION__((*this))
    return dynamic_cast<const ElementNumber*>(m_element.get())->getValue();
}

std::string &Config::getString() {
    __CHECK_TYPE_IS_STRING_EXCEPTION__((*this))
    return dynamic_cast<ElementString*>(m_element.get())->getValue();
}

std::string Config::getString() const {
    __CHECK_TYPE_IS_STRING_EXCEPTION__((*this))
    return dynamic_cast<const ElementString*>(m_element.get())->getValue();
}

ElementArray &Config::getArray() {
    __CHECK_TYPE_IS_ARRAY_EXCEPTION__((*this))
    return dynamic_cast<ElementArray&>(*m_element.get());
}

ElementArray Config::getArray() const {
    __CHECK_TYPE_IS_ARRAY_EXCEPTION__((*this))
    return dynamic_cast<ElementArray&>(*m_element.get());
}

ElementJson &Config::getJson() {
    __CHECK_TYPE_IS_JSON_EXCEPTION__((*this))
    return dynamic_cast<ElementJson&>(*m_element.get());
}

ElementJson Config::getJson() const {
    __CHECK_TYPE_IS_JSON_EXCEPTION__((*this))
    return dynamic_cast<ElementJson&>(*m_element.get());
}

Config &Config::get_front(const size_t index) {
    ...
}

Config &Config::get_front(const size_t index) const {
    ...
}

Config &Config::get_at(const size_t index) {
    ...
}

Config &Config::get_at(const size_t index) const {
    ...
}

Config &Config::get_back(const size_t index) {
    ...
}

Config &Config::get_back(const size_t index) const {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    switch(getType()) {
    case ValueType::eJson: {
        return
    }
    case ValueType::eArray:
//NOTE: вручную учитывать контейнерные типы
//TODO:    case ValueType::eYaml:
//TODO:    case ValueType::eXml:
    default:                break;
    }

    if(getType() == ValueType::eArray)
        //FIXME: нужно упаковать в Config
        return dynamic_cast<ElementArray*>(m_element.get())->get_back();

case ValueType::eArray: return (void*) &*(array_begin());
    case ValueType::eJson:  return (void*) &*(json_begin());
    default:
    }
}

Config &Config::setValue() noexcept {
    m_element = std::unique_ptr<IElement>(new ElementNull());
    return *this;
}

Config &Config::setValue(const Config &value) noexcept {
    switch(value.getType()) {
    case ValueType::eNumber:    setValue(value.getNumber());
    case ValueType::eBool:      setValue(value.getBool());
    case ValueType::eString:    setValue(value.getString());
    case ValueType::eArray:     setValue(value.getArray());
    case ValueType::eJson:      setValue(value.getJson());
    case ValueType::eNull:      init();
    }
    return *this;
}

Config &Config::setValue(const IElement& value) noexcept {
    switch(value.getType()) {
    case ValueType::eNumber:    setValue(value.getNumber());
    case ValueType::eBool:      setValue(value.getBool());
    case ValueType::eString:    setValue(value.getString());
    case ValueType::eArray:     setValue(value.getArray());
    case ValueType::eJson:      setValue(value.getJson());
    case ValueType::eNull:      init();
    }
    return *this;
}

Config &Config::setValue(const bool value) noexcept {
    m_element = std::unique_ptr<IElement>(new ElementBool(value));
    return *this;
}

Config &Config::setValue(const long double& value) noexcept {
    m_element = std::unique_ptr<IElement>(new ElementNumber(value));
    return *this;
}

Config &Config::setValue(const std::string& value) noexcept {
    m_element = std::unique_ptr<IElement>(new ElementString(value));
    return *this;
}

Config &Config::setValue(const ElementArray &value) noexcept {
    m_element = std::unique_ptr<IElement>(new ElementArray(value));
    return *this;
}

Config &Config::setValue(const ElementJson &value) noexcept {
    m_element = std::unique_ptr<IElement>(new ElementJson(value));
    return *this;
}

std::string Config::toString(const ConfigFormat format, const CommentDesign &design,
                             const int8_t tabulation_level) const noexcept
{
    return m_element->toString(format, design, tabulation_level);
}

std::string Config::toString(const ConfigFormat format, const int8_t tabulation_level) const noexcept {
    return m_element->toString(format, tabulation_level);
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

bool Config::writeFileJson(const std::string &file_path, const bool with_comments) noexcept {
    return WriteFileJson(*this, file_path, with_comments);
}

bool Config::writeFileIni(const std::string &file_path, const bool with_comments) noexcept {
    return WriteFileIni(*this, file_path, with_comments);
}

Config &Config::parse(const std::string &content, const ConfigFormat format,
                      const bool with_comments, std::string *error_log)
{
    *this = Parse(content, format, with_comments, error_log);
    return *this;
}

Config &Config::parseJson(const std::string &content, const bool with_comments,
                          std::string *error_log)
{
    *this = ParseJson(content, with_comments, error_log);
    return *this;
}

Config &Config::parseIni(const std::string &content, const bool with_comments,
                         std::string *error_log)
{
    *this = ParseIni(content, with_comments, error_log);
    return *this;
}

void *Config::begin() noexcept {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    switch(getType()) {
    case ValueType::eArray: return (void*) &*(array_begin());
    case ValueType::eJson:  return (void*) &*(json_begin());
    //NOTE: вручную учитывать контейнерные типы
 //TODO:    case ValueType::eYaml:
 //TODO:    case ValueType::eXml:
    default:                break;
    }

    return nullptr;
}

void *Config::end() noexcept {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    switch(getType()) {
    case ValueType::eArray: return (void*) &*(array_end());
    case ValueType::eJson:  return (void*) &*(json_end());
    //NOTE: вручную учитывать контейнерные типы
//TODO:    case ValueType::eYaml:
//TODO:    case ValueType::eXml:
    default:                break;
    }

    return nullptr;
}

void *Config::cbegin() const noexcept {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    switch(getType()) {
    case ValueType::eArray: return (void*) &*(array_cbegin());
    case ValueType::eJson:  return (void*) &*(json_cbegin());
    //NOTE: вручную учитывать контейнерные типы
//TODO:    case ValueType::eYaml:
//TODO:    case ValueType::eXml:
    default:                break;
    }

    return nullptr;
}

void *Config::cend() const noexcept {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    switch(getType()) {
    case ValueType::eArray: return (void*) &*(array_cend());
    case ValueType::eJson:  return (void*) &*(json_cend());
    //NOTE: вручную учитывать контейнерные типы
//TODO:    case ValueType::eYaml:
//TODO:    case ValueType::eXml:
    default:                break;
    }

    return nullptr;
}

VElement::iterator Config::array_begin() noexcept {
    return dynamic_cast<ElementArray*>(m_element.get())->begin();
}

VElement::iterator Config::array_end() noexcept {
    return dynamic_cast<ElementArray*>(m_element.get())->end();
}

VElement::const_iterator Config::array_cbegin() const noexcept {
    return dynamic_cast<ElementArray*>(m_element.get())->cbegin();
}

VElement::const_iterator Config::array_cend() const noexcept {
    return dynamic_cast<ElementArray*>(m_element.get())->cend();
}

VPairElement::iterator Config::json_begin() noexcept {
    return dynamic_cast<ElementJson*>(m_element.get())->begin();
}

VPairElement::iterator Config::json_end() noexcept {
    return dynamic_cast<ElementJson*>(m_element.get())->end();
}

VPairElement::const_iterator Config::json_cbegin() const noexcept {
    return dynamic_cast<ElementJson*>(m_element.get())->cbegin();
}

VPairElement::const_iterator Config::json_cend() const noexcept {
    return dynamic_cast<ElementJson*>(m_element.get())->cend();
}



Config ReadFile(const std::string &file_path, const ConfigFormat format,
                const bool with_comments, std::string *error_log)
{
    ...
}

Config ReadFileJson(const std::string &file_path, const bool with_comments,
                    std::string *error_log)
{
    ...
}

Config ReadFileIni(const std::string &file_path, const bool with_comments,
                   std::string *error_log)
{
    ...
}

bool WriteFile(const Config &config, const std::string &file_path,
               const ConfigFormat format, const bool with_comments) noexcept
{
    ...
}

bool WriteFileJson(const Config &config, const std::string &file_path,
                   const bool with_comments) noexcept
{
    ...
}

bool WriteFileIni(const Config &config, const std::string &file_path,
                  const bool with_comments) noexcept
{
    ...
}

Config Parse(const std::string &content, const ConfigFormat format,
             const bool with_comments, std::string *error_log)
{
    ...
}

Config ParseJson(const std::string &content, const bool with_comments,
                 std::string *error_log)
{
    ...
}

Config ParseIni(const std::string &content, const bool with_comments,
                std::string *error_log)
{
    ...
}
