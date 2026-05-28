#include "ElementString.h"

#include "Config.h"
#include "ConfigDefines.h"

#include "../utils/Utils.h"
#include <algorithm>


namespace simpleapi {
namespace tools {

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



//----------------------------------------------------------------------------------------------------------------------

bool IsElementString(const IElement &e) noexcept {
    return e.getType() == ValueType::eString;
}

bool IsElementString(const Config &cfg) noexcept {
    return cfg.isString();
}

} // namespace tools
} // namespace simpleapi
