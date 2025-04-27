#include "IElement.h"

#include "../utils/Utils.h"

#include "ElementBool.h"
#include "ElementNumber.h"
#include "ElementString.h"
#include "ElementArray.h"
#include "ElementJson.h"


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

bool IElement::getBool() const {
    __CHECK_TYPE_IS_BOOL_EXCEPTION__
    return static_cast<const ElementBool*>(this)->getValue();
}

bool& IElement::getBool() {
    __CHECK_TYPE_IS_BOOL_EXCEPTION__
    return static_cast<ElementBool*>(this)->getValue();
}

long double IElement::getNumber() const {
    __CHECK_TYPE_IS_NUMBER_EXCEPTION__
    return static_cast<const ElementNumber*>(this)->getValue();
}

long double& IElement::getNumber() {
    __CHECK_TYPE_IS_NUMBER_EXCEPTION__
    return static_cast<ElementNumber*>(this)->getValue();
}

std::string IElement::getString() const {
    __CHECK_TYPE_IS_STRING_EXCEPTION__
    return static_cast<const ElementString*>(this)->getValue();
}

std::string& IElement::getString() {
    __CHECK_TYPE_IS_STRING_EXCEPTION__
    return static_cast<ElementString*>(this)->getValue();
}

ElementArray IElement::getArray() const {
    __CHECK_TYPE_IS_ARRAY_EXCEPTION__
    return static_cast<const ElementArray&>(*this);
}

ElementArray& IElement::getArray() {
    __CHECK_TYPE_IS_ARRAY_EXCEPTION__
    return static_cast<ElementArray&>(*this);
}

//TODO: ElementJson IElement::getJson() const
//ElementJson IElement::getJson() {
//    __CHECK_TYPE_IS_JSON_EXCEPTION__
//    return static_cast<const ElementJson&>(*this);
//}

//TODO: ElementJson& IElement::getJson()
//ElementJson& IElement::getJson() {
//    __CHECK_TYPE_IS_JSON_EXCEPTION__
//    return static_cast<ElementJson&>(*this);
//}

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

