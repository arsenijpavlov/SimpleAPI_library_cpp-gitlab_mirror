#include "JsonBaseElement.h"

#include "../utils/Utils.h"

#include "JsonElementNull.h"
#include "JsonElementBool.h"
#include "JsonElementString.h"
#include "JsonElementNumber.h"
#include "JsonElementJson.h"
#include "JsonElementArray.h"


bool Element::writeFile(const std::string &file_path, const ConfigFormat format,
                        const bool with_comments) noexcept {
    return CreateEmptyFile(file_path,
                           with_comments ? m_comment.prefix : "",
                           with_comments ? m_comment.suffix : "");
}

bool Element::writeFileJson(const std::string &file_path, const bool with_comments) noexcept {
    return CreateEmptyFile(file_path,
                           with_comments ? m_comment.prefix : "",
                           with_comments ? m_comment.suffix : "");
}

bool Element::writeFileIni(const std::string &file_path, const bool with_comments) noexcept {
    return CreateEmptyFile(file_path,
                           with_comments ? m_comment.prefix : "",
                           with_comments ? m_comment.suffix : "");
}

void Element::addComment(const Comment &content) noexcept {
    m_comment = content;
}

void Element::clearComment() noexcept {
    m_comment.clear();
}


//----------------------------------------------------------------------------------------------------------------------


Element ReadFile(const std::string &file_path, const ConfigFormat format,
                  const bool with_comments, std::string* error_log) noexcept {
    std::string dest;
    GetAllStringsFromFile(file_path, dest, error_log);
    return Parse(dest, format, with_comments, error_log);
}

Element ReadFileJson(const std::string &file_path, const bool with_comments,
                     std::string* error_log) noexcept {
    std::string dest;
    GetAllStringsFromFile(file_path, dest, error_log);
    return ParseJson(dest, with_comments, error_log);
}

Element ReadFileIni(const std::string &file_path, const bool with_comments,
                    std::string* error_log) noexcept {
    std::string dest;
    GetAllStringsFromFile(file_path, dest, error_log);
    return ParseIni(dest, with_comments, error_log);
}

Element Parse(const std::string &content, const ConfigFormat format,
              const bool with_comments, std::string* error_log) noexcept {
    switch(format) {
    case ConfigFormat::eJSON:   return ParseJson(content, with_comments, error_log);
    case ConfigFormat::eINI:    return ParseIni(content, with_comments, error_log);
    default:                    return ElementNull();
    }
}

Element ParseJson(const std::string &file_path, const bool with_comments,
                  std::string* error_log) noexcept {
    //TODO: Element &ParseJson()
    return ElementNull();
}

Element ParseIni(const std::string &file_path, const bool with_comments,
                 std::string* error_log) noexcept {
    //TODO: Element &ParseIni()
    return ElementNull();
}



