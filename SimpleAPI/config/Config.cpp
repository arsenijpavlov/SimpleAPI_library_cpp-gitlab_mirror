#include "Config.h"

#include "ElementNull.h"
#include "ElementBool.h"
#include "ElementNumber.h"
#include "ElementString.h"
#include "ElementArray.h"
#include "ElementJson.h"
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
    case ValueType::eNull:      { setValue();                                                               break; }
    case ValueType::eBool:      { setValue(dynamic_cast<const ElementBool*>(other.m_value)->getValue());    break; }
    case ValueType::eNumber:    { setValue(dynamic_cast<const ElementNumber*>(other.m_value)->getValue());  break; }
    case ValueType::eString:    { setValue(dynamic_cast<const ElementString*>(other.m_value)->getValue());  break; }
    case ValueType::eArray:     { setValue(dynamic_cast<const ElementArray&>(*other.m_value));              break; }
    case ValueType::eJson:      { setValue(dynamic_cast<const ElementJson&>(*other.m_value));               break; }
    default: break;
    }

    return *this;
}

Config &Config::setValue(Config &&other) noexcept {
    if(m_value) delete m_value;

    switch(other.getType()) {
    case ValueType::eNull:      { setValue();                                                                           break; }
    case ValueType::eBool:      { setValue(dynamic_cast<const ElementBool*>(other.m_value)->getValue());                break; }
    case ValueType::eNumber:    { setValue(std::move(dynamic_cast<const ElementNumber*>(other.m_value)->getValue()));   break; }
    case ValueType::eString:    { setValue(std::move(dynamic_cast<const ElementString*>(other.m_value)->getValue()));   break; }
    case ValueType::eArray:     { setValue(std::move(dynamic_cast<const ElementArray&>(*other.m_value)));               break; }
    case ValueType::eJson:      { setValue(std::move(dynamic_cast<const ElementJson&>(*other.m_value)));                break; }
    default:                    break;
    }

    other.init(); //обнулить значение
    return *this;
}

Config &Config::setValue(const IElement &other) noexcept {
    if(m_value) delete m_value;

    switch(other.getType()) {
    case ValueType::eNull:      { setValue();                                                       break; }
    case ValueType::eBool:      { setValue(dynamic_cast<const ElementBool*>(&other)->getValue());   break; }
    case ValueType::eNumber:    { setValue(dynamic_cast<const ElementNumber*>(&other)->getValue()); break; }
    case ValueType::eString:    { setValue(dynamic_cast<const ElementString*>(&other)->getValue()); break; }
    case ValueType::eArray:     { setValue(dynamic_cast<const ElementArray&>(other));               break; }
    case ValueType::eJson:      { setValue(dynamic_cast<const ElementJson&>(other));                break; }
    default:                    break;
    }

    return *this;
}

Config &Config::setValue(IElement &&other) noexcept {
    if(m_value) delete m_value;

    switch(other.getType()) {
    case ValueType::eNull:      { setValue();                                                                   break; }
    case ValueType::eBool:      { setValue(dynamic_cast<const ElementBool*>(&other)->getValue());               break; }
    case ValueType::eNumber:    { setValue(std::move(dynamic_cast<const ElementNumber*>(&other)->getValue()));  break; }
    case ValueType::eString:    { setValue(std::move(dynamic_cast<const ElementString*>(&other)->getValue()));  break; }
    case ValueType::eArray:     { setValue(std::move(dynamic_cast<ElementArray&&>(other)));                     break; }
    case ValueType::eJson:      { setValue(std::move(dynamic_cast<ElementJson&&>(other)));                      break; }
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
        return *this;

    size_t current_index = indexes.front();

    if(indexes.size() == 1) {
        __CHECK_INDEX_BOUND__((*this), indexes[0])
        switch(getType()) {
        case ValueType::eArray: return dynamic_cast<ElementArray*>(m_value)->get_at(current_index);
        case ValueType::eJson:  return dynamic_cast<ElementJson*>(m_value)->get_at(current_index);
        default: break;
        }
    } else {
        std::vector<size_t> new_indexes;
        new_indexes.assign(indexes.cbegin() + 1, indexes.cend());

        switch(getType()) {
        case ValueType::eArray: return dynamic_cast<ElementArray*>(m_value)->get_at(new_indexes);
        case ValueType::eJson:  return dynamic_cast<ElementJson*>(m_value)->get_at(new_indexes);
        default: break;
        }
    }

    return *this;
}

Config Config::get_at(const std::vector<size_t> &indexes) const
{
    ...
}

Config &Config::get_at(const std::string& key) {
    __CHECK_TYPE_IS_JSON__((*this))
    return dynamic_cast<ElementJson*>(m_value)->get_at(key);
}

Config Config::get_at(const std::string& key) const {
    __CHECK_TYPE_IS_JSON__((*this))
    return dynamic_cast<const ElementJson*>(m_value)->get_at(key);
}

Config &Config::get_at(const std::vector<std::string> &complex_key)
{
    ...
}

Config Config::get_at(const std::vector<std::string> &complex_key) const
{
    ...
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

bool &Config::get_bool_at(const std::vector<size_t> &indexes)
{
    ...
}

bool Config::get_bool_at(const std::vector<size_t> &indexes) const
{
    ...
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

long double &Config::get_number_at(const std::vector<size_t> &indexes)
{
    ...
}

long double Config::get_number_at(const std::vector<size_t> &indexes) const
{
    ...
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

std::string &Config::get_string_at(const std::vector<size_t> &indexes)
{
    ...
}

std::string Config::get_string_at(const std::vector<size_t> &indexes) const
{
    ...
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

bool &Config::get_bool_at(const std::vector<std::string> &complex_key)
{
    ...
}

bool Config::get_bool_at(const std::vector<std::string> &complex_key) const
{
    ...
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

long double &Config::get_number_at(const std::vector<std::string> &complex_key)
{
    ...
}

long double Config::get_number_at(const std::vector<std::string> &complex_key) const
{
    ...
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

std::string &Config::get_string_at(const std::vector<std::string> &complex_key)
{
    ...
}

std::string Config::get_string_at(const std::vector<std::string> &complex_key) const
{
    ...
}

bool &Config::get_back_bool() {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    const Config& config = get_back();
    __CHECK_TYPE_IS_BOOL__(config)

    return dynamic_cast<ElementBool*>(config.m_value)->getValue();
}

bool Config::get_back_bool() const {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    const Config& config = get_back();
    __CHECK_TYPE_IS_BOOL__(config)

    return dynamic_cast<const ElementBool*>(config.m_value)->getValue();
}

long double &Config::get_back_number() {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    const Config& config = get_back();
    __CHECK_TYPE_IS_NUMBER__(config)

    return dynamic_cast<ElementNumber*>(config.m_value)->getValue();
}

long double Config::get_back_number() const {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    const Config& config = get_back();
    __CHECK_TYPE_IS_NUMBER__(config)

    return dynamic_cast<const ElementNumber*>(config.m_value)->getValue();
}

std::string &Config::get_back_string() {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    const Config& config = get_back();
    __CHECK_TYPE_IS_STRING__(config)

    return dynamic_cast<ElementString*>(config.m_value)->getValue();
}

std::string Config::get_back_string() const {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    const Config& config = get_back();
    __CHECK_TYPE_IS_STRING__(config)

    return dynamic_cast<const ElementString*>(config.m_value)->getValue();
}

bool Config::isEqual(const Config &other, const bool compare_comments) const {
    __CHECK_TYPES_IS_EQUAL__((*this), (other))

    if(compare_comments && getComment() != other.getComment())
        return false;

    return m_value->isEqual(other.m_value);
}

bool Config::isEqual(const IElement &other, const bool compare_comments) const
{
    ...
}

bool Config::isEqual(const IElement &other) const {
    __CHECK_TYPES_IS_EQUAL__((*this), (other))

    switch(getType()){
    case ValueType::eNull:      return dynamic_cast<const ElementNull*>(m_value);
    case ValueType::eBool:      return dynamic_cast<const ElementBool*>(m_value);
    case ValueType::eNumber:    return dynamic_cast<const ElementNumber*>(m_value);
    case ValueType::eString:    return dynamic_cast<const ElementString*>(m_value);
    case ValueType::eArray:     return dynamic_cast<const ElementArray*>(m_value);
    case ValueType::eJson:      return dynamic_cast<const ElementJson*>(m_value);
    default:                    break;
    }

    return false;
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

Config &Config::operator=(const Config &other) noexcept                { setValue(other); return *this; }

bool Config::operator>(const Config &other) const
{
    ...
}

bool Config::operator>=(const Config &other) const
{
    ...
}

bool Config::operator<(const Config &other) const
{
    ...
}

bool Config::operator<=(const Config &other) const
{
    ...
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

void *Config::begin() {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    switch(getType()) {
    case ValueType::eArray: return reinterpret_cast<void*>(dynamic_cast<const ElementArray*>(m_value)->begin());
    case ValueType::eJson:  return reinterpret_cast<void*>(dynamic_cast<const ElementJson*>(m_value)->begin());
    default:                break;
    }

    return nullptr;
}

void *Config::cbegin() const {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    switch(getType()) {
    case ValueType::eArray: return reinterpret_cast<void*>(dynamic_cast<const ElementArray*>(m_value)->cbegin());
    case ValueType::eJson:  return reinterpret_cast<void*>(dynamic_cast<const ElementJson*>(m_value)->cbegin());
    default:                break;
    }

    return nullptr;
}

void *Config::end() {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    switch(getType()) {
    case ValueType::eArray: return reinterpret_cast<void*>(dynamic_cast<const ElementArray*>(m_value)->end());
    case ValueType::eJson:  return reinterpret_cast<void*>(dynamic_cast<const ElementJson*>(m_value)->end());
    default:                break;
    }

    return nullptr;
}

void *Config::cend() const {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    switch(getType()) {
    case ValueType::eArray: return reinterpret_cast<void*>(dynamic_cast<const ElementArray*>(m_value)->cend());
    case ValueType::eJson:  return reinterpret_cast<void*>(dynamic_cast<const ElementJson*>(m_value)->cend());
    default:                break;
    }

    return nullptr;
}

VElement::iterator Config::array_begin() {
    __CHECK_TYPE_IS_ARRAY__((*this))

    return dynamic_cast<ElementArray*>(m_value)->begin();
}

VElement::const_iterator Config::array_cbegin() const {
    __CHECK_TYPE_IS_ARRAY__((*this))

    return dynamic_cast<const ElementArray*>(m_value)->cbegin();
}

VElement::iterator Config::array_end() {
    __CHECK_TYPE_IS_ARRAY__((*this))

    return dynamic_cast<ElementArray*>(m_value)->end();
}

VElement::const_iterator Config::array_cend() const {
    __CHECK_TYPE_IS_ARRAY__((*this))

    return dynamic_cast<const ElementArray*>(m_value)->cend();
}

VPairElement::iterator Config::json_begin() {
    __CHECK_TYPE_IS_JSON__((*this))

    return dynamic_cast<ElementJson*>(m_value)->begin();
}

VPairElement::const_iterator Config::json_cbegin() const {
    __CHECK_TYPE_IS_JSON__((*this))

    return dynamic_cast<const ElementJson*>(m_value)->cbegin();
}

VPairElement::iterator Config::json_end() {
    __CHECK_TYPE_IS_JSON__((*this))

    return dynamic_cast<ElementJson*>(m_value)->end();
}

VPairElement::const_iterator Config::json_cend() const {
    __CHECK_TYPE_IS_JSON__((*this))

    return dynamic_cast<const ElementJson*>(m_value)->cend();
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
