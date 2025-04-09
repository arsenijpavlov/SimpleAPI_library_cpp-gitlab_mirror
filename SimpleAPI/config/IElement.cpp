#include "IElement.h"

#include "../utils/Utils.h"

#include "ElementNull.h"
#include "ElementBool.h"
#include "ElementString.h"
#include "ElementNumber.h"
#include "ElementJson.h"
#include "ElementArray.h"


//(!) Не преремещать в Header, нужно включение JsonElementNull
IElement::IElement() noexcept {
    *this = ElementNull();
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
    default:                    return ElementNull();
    }
}

IElement ParseJson(const std::string &file_path, const bool with_comments,
                  std::string* error_log) {
    //TODO: IElement &ParseJson()
    return ElementNull();
}

IElement ParseIni(const std::string &file_path, const bool with_comments,
                 std::string* error_log) {
    //TODO: IElement &ParseIni()
    return ElementNull();
}

