#include "ElementNull.h"

#include "Config.h"
#include "../utils/Utils.h"

#include <algorithm>


std::string ElementNull::toString(const ConfigFormat format, const int8_t tabulation_level,
                                  const CommentDesign &design) const noexcept
{
    //TODO: ElementNull::toString()
    return "";
}

std::string ElementNull::toJsonString(const int8_t tabulation_level, const CommentDesign &design) const noexcept
{
    //TODO: ElementNull::toJsonString()
    return "";
}

std::string ElementNull::toIniString(const int8_t tabulation_level, const CommentDesign &design) const noexcept
{
    //TODO: ElementNull::toIniString()
    return "";
}

std::string ElementNull::toYamlString(const int8_t tabulation_level, const CommentDesign &design) const noexcept
{
    //TODO: ElementNull::toYamlString()
    return "";
}

std::string ElementNull::toXmlString(const int8_t tabulation_level, const CommentDesign &design) const noexcept
{
    //TODO: ElementNull::toXmlString()
    return "";
}


//----------------------------------------------------------------------------------------------------------------------


bool IsElementNull(const std::string &str) noexcept {
    std::string temp = str;
    RemoveIllegalSpaces(temp);
    std::transform(temp.begin(), temp.end(), temp.begin(),
                   [](unsigned char c){ return std::tolower(c); });

    if(temp.empty() || utils::OnlySpaces(temp)) return true;
    if(temp.size() == 1)                        return str == "~"; //уникальное поведение для YAML
    if(temp == "null")                          return true;

    return false;
}

bool IsElementNull(const IElement &e) noexcept {
    return e.getType() == ValueType::eNull;
}

bool IsElementNull(const Config &cfg) noexcept {
    return cfg.isNull();
}

