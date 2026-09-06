#include "ElementChar.h"
#include "ElementString.h"


namespace simpleapi {
namespace tools {

void ElementChar::clear() noexcept {
    clearComment();
    m_value = 0;
}

// @TEST(ELEMENT, char_compare_string)
bool ElementChar::isEqual(const IElement &other, const bool compare_comments,
                            const bool map_sort_important) const noexcept
{
    bool b1 = !compare_comments || isCommentsEqual(other);
    bool b2;

    // тип other может быть либо строкой, состоящей из одного символа, либо самостоятельным символом
    switch(other.getType()){
    case ValueType::eChar:
        b2 = m_value == reinterpret_cast<const ElementChar&>(other).getValue();
        break;
    case ValueType::eString:
        b2 = (other.size() == 1) && m_value == reinterpret_cast<const ElementString&>(other).getValue().at(0);
        break;
    default:
        return false;
    }

    return b1 && b2;
}



//----------------------------------------------------------------------------------------------------------------------


} // namespace tools
} // namespace simpleapi
