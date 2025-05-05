#include "Config.h"

#include "ElementNull.h"
#include "ElementBool.h"
#include "ElementNumber.h"
#include "ElementString.h"
#include "ElementArray.h"
#include "ElementJson.h"


Config &Config::operator=(const Config& other) noexcept {

}

Config &Config::operator=(Config&& other) noexcept {

}

bool Config::operator==(const Config& other) const {

}

//числа, контейнеры(размер)
bool Config::operator>(const Config& other) const noexcept {

}

bool Config::operator>(const IElement& other) const noexcept {

}

bool Config::operator>=(const Config& other) const noexcept {

}

bool Config::operator>=(const IElement& other) const noexcept {

}

bool Config::operator<(const Config& other) const noexcept {

}

bool Config::operator<(const IElement& other) const noexcept {

}

bool Config::operator<=(const Config& other) const noexcept {

}

bool Config::operator<=(const IElement& other) const noexcept {

}

Config &Config::operator<<(const Config& other) noexcept {

}

Config &Config::operator<<(const IElement& other) noexcept {

}

Config  Config::operator>>(const Config& other) noexcept {

}

Config  Config::operator>>(const IElement& other) noexcept {

}

Config &Config::operator[](const size_t index) noexcept {

}

Config  Config::operator[](const size_t index) const noexcept {

}

Config &Config::operator[](const std::string key) noexcept {

}

Config  Config::operator[](const std::string key) const noexcept {

}

size_t Config::size() const noexcept {
    switch(getType()) {
    case ValueType::eNull:      return 0; //TODO: размер NULL элемента под вопросом
    case ValueType::eNumber:
    case ValueType::eBool:
    case ValueType::eString:    return 1;
    case ValueType::eArray:     return dynamic_cast<const ElementArray*>(m_element.get())->size();
    case ValueType::eJson:      return dynamic_cast<const ElementJson*>(m_element.get())->size();
    default:                    return 0;
    }
}

bool Config::isEqual(const Config &other, const bool compare_comments) const noexcept {
    return compare_comments ? m_element->isEqualWithComments(*other.m_element)
                            : m_element->isEqual(*other.m_element);
}

bool &Config::getBool() {

}

bool Config::getBool() const {

}

long double &Config::getNumber() {

}

long double Config::getNumber() const {

}

std::string &Config::getString() {

}

std::string Config::getString() const {

}

ElementArray &Config::getArray() {

}

ElementArray Config::getArray() const {

}

ElementJson &Config::getJson() {

}

ElementJson Config::getJson() const {

}

Config &Config::get_front(const size_t index) {

}

Config &Config::get_front(const size_t index) const {

}

Config &Config::get_at(const size_t index) {

}

Config &Config::get_at(const size_t index) const {

}

Config &Config::get_back(const size_t index) {

}

Config &Config::get_back(const size_t index) const {

}

void Config::setValue() noexcept {
    m_element = std::unique_ptr<IElement>(new ElementNull());
}

void Config::setValue(const Config &value) noexcept {
    switch(value.getType()) {
    case ValueType::eNumber:    setValue(value.getNumber());
    case ValueType::eBool:      setValue(value.getBool());
    case ValueType::eString:    setValue(value.getString());
    case ValueType::eArray:     setValue(value.getArray());
    case ValueType::eJson:      setValue(value.getJson());
    default:                    init();
    }
}

void Config::setValue(const IElement& value) noexcept {
    switch(value.getType()) {
    case ValueType::eNumber:    setValue(value.getNumber());
    case ValueType::eBool:      setValue(value.getBool());
    case ValueType::eString:    setValue(value.getString());
    case ValueType::eArray:     setValue(value.getArray());
    case ValueType::eJson:      setValue(value.getJson());
    default:                    init();
    }
}

void Config::setValue(const bool value) noexcept {
    m_element   = std::unique_ptr<IElement>(new ElementBool(value));
}

void Config::setValue(const long double& value) noexcept {
    m_element   = std::unique_ptr<IElement>(new ElementNumber(value));
}

void Config::setValue(const std::string& value) noexcept {
    m_element   = std::unique_ptr<IElement>(new ElementString(value));
}

void Config::setValue(const ElementArray &value) noexcept {
    m_element   = std::unique_ptr<IElement>(new ElementArray(value));
}

void Config::setValue(const ElementJson &value) noexcept {
    m_element   = std::unique_ptr<IElement>(new ElementJson(value));
}

std::string Config::toString(const ConfigFormat format, const CommentDesign &design,
                             const int8_t tabulation_level) const noexcept
{

}

std::string Config::toString(const ConfigFormat format, const int8_t tabulation_level) const noexcept {

}

Config &Config::readFile(const std::string &file_path, const ConfigFormat format,
                         const bool with_comments, std::string *error_log)
{

}

Config &Config::readFileJson(const std::string &file_path, const bool with_comments,
                             std::string *error_log)
{

}

Config &Config::readFileIni(const std::string &file_path, const bool with_comments,
                            std::string *error_log)
{

}

bool Config::writeFile(const std::string &file_path, const ConfigFormat format,
                       const bool with_comments) noexcept
{

}

bool Config::writeFileJson(const std::string &file_path, const bool with_comments) noexcept {

}

bool Config::writeFileIni(const std::string &file_path, const bool with_comments) noexcept {

}

Config &Config::parse(const std::string &content, const ConfigFormat format,
                      const bool with_comments, std::string *error_log)
{

}

Config &Config::parseJson(const std::string &file_path, const bool with_comments,
                          std::string *error_log)
{

}

Config &Config::parseIni(const std::string &file_path, const bool with_comments,
                         std::string *error_log)
{

}

void *Config::begin() noexcept {

}

void *Config::end() noexcept {

}

void *Config::cbegin() const noexcept {

}

void *Config::cend() const noexcept {

}

VElement::iterator &Config::array_begin() noexcept {

}

VElement::iterator &Config::array_end() noexcept {

}

VElement::const_iterator Config::array_cbegin() const noexcept {

}

VElement::const_iterator Config::array_cend() const noexcept {

}

VPairElement::iterator &Config::json_begin() noexcept {

}

VPairElement::iterator &Config::json_end() noexcept {

}

VPairElement::const_iterator Config::json_cbegin() const noexcept {

}

VPairElement::const_iterator Config::json_cend() const noexcept {

}



Config ReadFile(const std::string &file_path, const ConfigFormat format,
                const bool with_comments, std::string *error_log)
{

}

Config ReadFileJson(const std::string &file_path, const bool with_comments,
                    std::string *error_log)
{

}

Config ReadFileIni(const std::string &file_path, const bool with_comments,
                   std::string *error_log)
{

}

bool WriteFile(const Config &config, const std::string &file_path,
               const ConfigFormat format, const bool with_comments) noexcept
{

}

bool WriteFileJson(const Config &config, const std::string &file_path,
                   const bool with_comments) noexcept
{

}

bool WriteFileIni(const Config &config, const std::string &file_path,
                  const bool with_comments) noexcept
{

}

Config Parse(const std::string &content, const ConfigFormat format,
             const bool with_comments, std::string *error_log)
{

}

Config ParseJson(const std::string &file_path, const bool with_comments,
                 std::string *error_log)
{

}

Config ParseIni(const std::string &file_path, const bool with_comments,
                std::string *error_log)
{

}
