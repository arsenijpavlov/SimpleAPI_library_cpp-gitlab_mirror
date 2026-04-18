#include "ElementBool.h"

#include "Config.h"
#include <algorithm>


namespace simpleapi {

void ElementBool::clear() noexcept {
    clearComment();
    m_value = false;
}

bool ElementBool::isEqual(const IElement &other, const bool compare_comments,
                          const bool map_sort_important) const noexcept
{
    bool b1 = !compare_comments || isCommentsEqual(other);
    bool b2 = m_value == reinterpret_cast<const ElementBool&>(other).getValue();

    return b1 && b2;
}

std::string ElementBool::toString(bool value) noexcept {
    return (value ? "true" : "false");
}

std::string ElementBool::toString(const ConfigFormat format, const CommentDesign &design,
                                  const int8_t custom_tabulation_level) const noexcept
{
    return toString(m_value);
}

std::string ElementBool::toJsonString(const CommentDesign &design, const int8_t custom_tabulation_level) const noexcept
{
    return toString(m_value);
}

std::string ElementBool::toIniString(const CommentDesign &design, const int8_t custom_tabulation_level) const noexcept
{
    return toString(m_value);
}

std::string ElementBool::toYamlString(const CommentDesign &design, const int8_t custom_tabulation_level) const noexcept
{
    return toString(m_value);
}

std::string ElementBool::toXmlString(const CommentDesign &design, const int8_t custom_tabulation_level) const noexcept
{
    return toString(m_value);
}


//----------------------------------------------------------------------------------------------------------------------


bool IsElementBool(const std::string &str) noexcept {
    std::string temp = str;

    //удалить пробелы в начале и конце строки
    RemoveIllegalSpaces(temp);
    std::transform(temp.begin(), temp.end(), temp.begin(),
                   [](unsigned char c){ return std::tolower(c); });

    if(temp == "true" || temp == "false") return true;
    return false;
}

bool IsElementBool(const IElement &e) noexcept {
    return e.getType() == ValueType::eBool;
}

bool IsElementBool(const Config &cfg) noexcept {
    return cfg.isBool();
}

} // namespace simpleapi

