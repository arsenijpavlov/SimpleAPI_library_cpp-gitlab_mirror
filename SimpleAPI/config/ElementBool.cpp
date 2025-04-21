#include "ElementBool.h"
#include <algorithm>


bool ElementBool::is_equal(const IElement &other, const bool compare_comments) const noexcept {
    if(m_value != dynamic_cast<const ElementBool&>(other).m_value)  return false;
    if(compare_comments && m_comment == other.get_comment())         return false;

    return true;
}

void ElementBool::operator=(const bool other) noexcept {
    m_value = other;
}


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
