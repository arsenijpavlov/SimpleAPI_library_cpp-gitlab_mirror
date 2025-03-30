#include "JsonElementBool.h"
#include <algorithm>



std::string ElementBool::to_string(const ConfigFormat format, const bool to_one_line) const noexcept {
    return m_value ? "true" : "false";
}

bool ElementBool::isEqual(const Element &other, const bool compare_comments) const noexcept {
    if(m_value != dynamic_cast<const ElementBool&>(other).m_value)          return false;
    if(compare_comments) {
        if(m_preview_comment == other.getPreviewComment())  return false;
        if(m_suffix_comment == other.getSuffixComment())    return false;
    }

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

bool IsElementBool(const Element &e) noexcept {
    return e.getType() == ValueType::eBool;
}
