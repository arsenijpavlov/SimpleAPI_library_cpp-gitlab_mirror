#include "JsonElementBool.h"
#include <algorithm>



std::string ElementBool::to_string(const ConfigFormat format, const bool to_one_line) const noexcept {
    return m_value ? "true" : "false";
}

bool ElementBool::isEqual(const Element &other) const noexcept {
    return m_value == dynamic_cast<const ElementBool&>(other).m_value;
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

bool IsElementBool(const Element &e) noexcept {
    return e.getType() == ValueType::eBool;
}
