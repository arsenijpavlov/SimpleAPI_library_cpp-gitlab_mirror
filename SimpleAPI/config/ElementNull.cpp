#include "ElementNull.h"

#include "Config.h"
#include "../utils/Utils.h"
#include "../utils/string_utils.h"

#include <algorithm>


namespace simpleapi {
namespace tools {

bool ElementNull::isEqual(const IElement &other, const bool compare_comments,
                          const bool map_sort_important) const noexcept
{
    if(compare_comments)
        return isCommentsEqual(other);
    return true;
}

//----------------------------------------------------------------------------------------------------------------------


bool IsElementNull(const std::string &str) noexcept {
    std::string temp = str;
    utils::RemoveIllegalSpaces(temp);
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

} // namespace tools
} // namespace simpleapi

