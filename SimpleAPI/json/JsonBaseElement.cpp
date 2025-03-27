#include "JsonBaseElement.h"

#include "JsonElementNull.h"
#include "JsonElementBool.h"
#include "JsonElementString.h"
#include "JsonElementNumber.h"
#include "JsonElementJson.h"
#include "JsonElementArray.h"


bool Element::writeFile(const std::string &file_path, const ConfigFormat format,
                        const bool with_comments) noexcept {
    switch(format) {
    case ConfigFormat::eJSON:   return writeFileJson(file_path);
    case ConfigFormat::eYAML:   return writeFileYaml(file_path);
    case ConfigFormat::eINI:    return writeFileIni(file_path);
    default:                    return false;
    }
}


//----------------------------------------------------------------------------------------------------------------------


Element &ReadFile(const std::string &file_path, const ConfigFormat format,
                  const bool with_comments) {
    switch(format) {
    case ConfigFormat::eJSON:   return ReadFileJson(file_path, with_comments);
    case ConfigFormat::eYAML:   return ReadFileYaml(file_path, with_comments);
    case ConfigFormat::eINI:    return ReadFileIni(file_path, with_comments);
    default:                    return ElementNull();
    }
}

Element &ReadFileJson(const std::string &file_path, const bool with_comments) {
    //TODO: Element &ReadFileJson();
    return ElementNull();
}

Element &ReadFileYaml(const std::string &file_path, const bool with_comments) {
    //TODO: Element &ReadFileYaml();
    return ElementNull();
}

Element &ReadFileIni(const std::string &file_path, const bool with_comments) {
    //TODO: Element &ReadFileIni();
    return ElementNull();
}

Element &ReadFileXml(const std::string &file_path, const bool with_comments) {
    //TODO: Element &ReadFileXml();
    return ElementNull();
}


Element &Parse(const std::string &content, const ConfigFormat format, const bool with_comments) {
    //TODO: Element &Parse()
    return ElementNull();
}

Element &ParseJson(const std::string &file_path, const bool with_comments) {
    //TODO: Element &ParseJson()
    return ElementNull();
}

Element &ParseYaml(const std::string &file_path, const bool with_comments) {
    //TODO: Element &ParseYaml()
    return ElementNull();
}

Element &ParseIni(const std::string &file_path, const bool with_comments) {
    //TODO: Element &ParseIni()
    return ElementNull();
}

Element &ParseXml(const std::string &file_path, const bool with_comments) {
    //TODO: Element &ParseXml()
    return ElementNull();
}
