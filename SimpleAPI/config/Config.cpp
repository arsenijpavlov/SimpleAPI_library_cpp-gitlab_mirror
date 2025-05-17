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
    if(m_value != nullptr)
        delete m_value;

    m_type  = ValueType::eNull;
    m_value = dynamic_cast<IElement*>(new ElementNull());
    return *this;
}

Config &Config::setValue(const Config &value) noexcept {
    if(m_value != nullptr)
        delete m_value;

    switch(value.getType()) {
    case ValueType::eNull:      setValue();
    case ValueType::eBool:      setValue(value.getBool());
    case ValueType::eNumber:    setValue(value.getNumber());
    case ValueType::eString:    setValue(value.getString());
    case ValueType::eArray:     setValue(value.getArray());
    case ValueType::eJson:      setValue(value.getJson());
    default: break;
    }

    return *this;
}

Config &Config::setValue(Config &&value) noexcept {
    if(m_value != nullptr)
        delete m_value;

    switch(value.getType()) {
    case ValueType::eNull:      setValue();
    case ValueType::eBool:      setValue(value.getBool());
    case ValueType::eNumber:    setValue(std::move(value.getNumber()));
    case ValueType::eString:    setValue(std::move(value.getString()));
    case ValueType::eArray:     setValue(std::move(value.getArray()));
    case ValueType::eJson:      setValue(std::move(value.getJson()));
    default:                    break;
    }

    value.init();
    return *this;
}

Config &Config::setValue(const bool value) noexcept {
    if(m_value != nullptr)
        delete m_value;

    m_type  = ValueType::eBool;
    m_value = dynamic_cast<IElement*>(new ElementBool(value));
    return *this;
}

Config &Config::setValue(const long double &value) noexcept {
    if(m_value != nullptr)
        delete m_value;

    m_type  = ValueType::eNumber;
    m_value = dynamic_cast<IElement*>(new ElementNumber(value));
    return *this;
}

Config &Config::setValue(long double &&value) noexcept {
    if(m_value != nullptr)
        delete m_value;

    m_type  = ValueType::eNumber;
    m_value = dynamic_cast<IElement*>(new ElementNumber(std::move(value)));
    return *this;
}

Config &Config::setValue(const std::string &value) noexcept {
    if(m_value != nullptr)
        delete m_value;

    m_type  = ValueType::eString;
    m_value = dynamic_cast<IElement*>(new ElementString(value));
    return *this;
}

Config &Config::setValue(std::string &&value) noexcept {
    if(m_value != nullptr)
        delete m_value;

    m_type  = ValueType::eString;
    m_value = dynamic_cast<IElement*>(new ElementString(std::move(value)));
    return *this;
}

Config &Config::setValue(const ElementArray &value) noexcept {
    if(m_value != nullptr)
        delete m_value;

    m_type  = ValueType::eArray;
    m_value = dynamic_cast<IElement*>(new ElementArray(value));
    return *this;
}

Config &Config::setValue(ElementArray &&value) noexcept {
    if(m_value != nullptr)
        delete m_value;

    m_type  = ValueType::eArray;
    m_value = dynamic_cast<IElement*>(new ElementArray(std::move(value)));
    return *this;
}

Config &Config::setValue(const ElementJson &value) noexcept {
    if(m_value != nullptr)
        delete m_value;

    m_type  = ValueType::eJson;
    m_value = dynamic_cast<IElement*>(new ElementJson(value));
    return *this;
}

Config &Config::setValue(ElementJson &&value) noexcept {
    if(m_value != nullptr)
        delete m_value;

    m_type  = ValueType::eJson;
    m_value = dynamic_cast<IElement*>(new ElementJson(std::move(value)));
    return *this;
}

bool Config::isEqual(const Config &other, const bool compare_comments) const noexcept {
    if(m_type != other.getType())
        return false;
    if(compare_comments && m_comment != other.m_comment)
        return false;

    return m_value->isEqual(other.m_value);
}

Config &Config::operator=(const Config &other) noexcept {
    return setValue(other);
}

Config &Config::operator=(Config &&other) noexcept {
    return setValue(std::move(other));
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
