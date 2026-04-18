#include "ElementString.h"

#include "Config.h"
#include "ConfigDefines.h"

#include "../utils/Utils.h"
#include <algorithm>


namespace simpleapi {

void ElementString::clear() noexcept {
    clearComment();
    m_value.clear();
}

bool ElementString::isEqual(const IElement &other, const bool compare_comments,
                            const bool map_sort_important) const noexcept
{
    bool b1 = !compare_comments || isCommentsEqual(other);
    bool b2 = m_value == reinterpret_cast<const ElementString&>(other).getValue();

    return b1 && b2;
}

std::string ElementString::toString(const ConfigFormat format, const CommentDesign &design,
                                    const int8_t custom_tabulation_level) const noexcept
{
    return (format == ConfigFormat::eONLY_VALUE ? m_value : utils::AddQuotes(m_value));
}

std::string ElementString::toJsonString(const CommentDesign &design, const int8_t custom_tabulation_level) const noexcept
{
    return utils::AddQuotes(m_value);
}

std::string ElementString::toIniString(const CommentDesign &design, const int8_t custom_tabulation_level) const noexcept
{
    return utils::AddQuotes(m_value);
}

std::string ElementString::toYamlString(const CommentDesign &design, const int8_t custom_tabulation_level) const noexcept
{
    return utils::AddQuotes(m_value);
}

std::string ElementString::toXmlString(const CommentDesign &design, const int8_t custom_tabulation_level) const noexcept
{
    return utils::AddQuotes(m_value);
}

//----------------------------------------------------------------------------------------------------------------------

bool IsElementString(const IElement &e) noexcept {
    return e.getType() == ValueType::eString;
}

bool IsElementString(const Config &cfg) noexcept {
    return cfg.isString();
}

} // namespace simpleapi
