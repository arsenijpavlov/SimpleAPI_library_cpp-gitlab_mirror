#include "IElement.h"

#include "../utils/Utils.h"

#include "ElementBool.h"
#include "ElementString.h"
#include "ElementNumber.h"
#include "ElementJson.h"
#include "ElementArray.h"


IElement::IElement(const IElement &other) noexcept {
    if(this != &other)
        *this = other;
}

void IElement::setValue(const long double &value) noexcept {
    *this = static_cast<IElement>(ElementNumber(value));
}

void IElement::setValue(const bool &value) noexcept {
    *this = static_cast<IElement>(ElementBool(value));
}

void IElement::setValue(const std::string &value) noexcept {
    *this = static_cast<IElement>(ElementString(value));
}

bool IElement::writeFile(const std::string &file_path, const ConfigFormat format,
                         const bool with_comments) noexcept {
    return CreateEmptyFile(file_path, m_comment.prefix(), m_comment.suffix());
}

bool IElement::writeFileJson(const std::string &file_path, const bool with_comments) noexcept {
    return CreateEmptyFile(file_path, m_comment.prefix(), m_comment.suffix());
}

bool IElement::writeFileIni(const std::string &file_path, const bool with_comments) noexcept {
    return CreateEmptyFile(file_path, m_comment.prefix(), m_comment.suffix());
}

//IElement &IElement::operator=(const IElement &other) noexcept {

//}


//----------------------------------------------------------------------------------------------------------------------


IElement ReadFile(const std::string &file_path, const ConfigFormat format,
                  const bool with_comments, std::string* error_log) {
    std::string dest;
    GetAllStringsFromFile(file_path, dest, error_log);
    return Parse(dest, format, with_comments, error_log);
}

IElement ReadFileJson(const std::string &file_path, const bool with_comments,
                     std::string* error_log) {
    std::string dest;
    GetAllStringsFromFile(file_path, dest, error_log);
    return ParseJson(dest, with_comments, error_log);
}

IElement ReadFileIni(const std::string &file_path, const bool with_comments,
                    std::string* error_log) {
    std::string dest;
    GetAllStringsFromFile(file_path, dest, error_log);
    return ParseIni(dest, with_comments, error_log);
}

IElement Parse(const std::string &content, const ConfigFormat format,
              const bool with_comments, std::string* error_log) {
    switch(format) {
    case ConfigFormat::eJSON:   return ParseJson(content, with_comments, error_log);
    case ConfigFormat::eINI:    return ParseIni(content, with_comments, error_log);
    default:                    return IElement();
    }
}

IElement ParseJson(const std::string &file_path, const bool with_comments,
                  std::string* error_log) {
    //TODO: IElement &ParseJson()
    return IElement();
}

IElement ParseIni(const std::string &file_path, const bool with_comments,
                 std::string* error_log) {
    //TODO: IElement &ParseIni()
    return IElement();
}

