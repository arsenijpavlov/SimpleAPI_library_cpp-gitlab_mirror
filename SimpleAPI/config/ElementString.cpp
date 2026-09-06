#include "ElementString.h"

#include "Config.h"
#include <algorithm>


namespace simpleapi {
namespace tools {

ElementString::ElementString(std::string &&s) noexcept    {
    init();
    m_value = std::move(s);
}

void ElementString::clear() noexcept {
    clearComment();
    m_value.clear();
}

// @TEST(ELEMENT, char_compare_string)
bool ElementString::isEqual(const IElement &other, const bool compare_comments,
                            const bool map_sort_important) const noexcept
{
    bool b1 = !compare_comments || isCommentsEqual(other);
    bool b2;

    // тип other может быть либо строкой, либо самостоятельным символом
    switch(other.getType()){
    case ValueType::eChar:
        b2 = m_value == reinterpret_cast<const ElementChar&>(other).toString();
        break;
    case ValueType::eString:
        b2 = (other.size() == size()) && m_value == reinterpret_cast<const ElementString&>(other).getValue();
        break;
    default:
        return false;
    }

    return b1 && b2;
}



//----------------------------------------------------------------------------------------------------------------------

bool IsElementString(const IElement &e) noexcept {
    return e.getType() == ValueType::eString;
}

bool IsElementString(const Config &cfg) noexcept {
    return cfg.isString();
}

} // namespace tools
} // namespace simpleapi
