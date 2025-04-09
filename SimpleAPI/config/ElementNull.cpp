#include "ElementNull.h"

#include "../utils/Utils.h"
#include <algorithm>


bool ElementNull::isEqual(const IElement &other, const bool compare_comments) const noexcept {
    if(compare_comments && m_comment == other.getComment()) return false;

    return true; //предполагаем, что проверка типа проведена уже в базовом классе
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

