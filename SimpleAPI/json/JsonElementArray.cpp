#include "JsonElementArray.h"



std::string ElementArray::to_string(const ConfigFormat format,
                                    const bool to_one_line) const noexcept {
    //TODO: to_one_line

    return "";
}

bool ElementArray::isEqual(const Element &other, const bool compare_comments) const noexcept {
    if(m_values != dynamic_cast<const ElementArray&>(other).m_values)   return false;
    if(compare_comments && m_comment == other.getComment())             return false;

    return true;
}

void ElementArray::operator=(const std::vector<Element> &other) noexcept {
    m_values = other;
}
