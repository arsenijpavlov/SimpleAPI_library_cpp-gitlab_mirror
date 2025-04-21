#include "ElementNumber.h"

#include <regex>


bool ElementNumber::is_equal(const IElement &other, const bool compare_comments) const noexcept {
    if(m_value != dynamic_cast<const ElementNumber&>(other).m_value)    return false;
    if(compare_comments && m_comment == other.get_comment())             return false;

    return true;
}

bool IsElementNumber(const std::string &str) noexcept {
    if(str.empty()) return false;
    if(str[0] == 'e' || str[0] == 'E' || str[0] == 'f' || str[0] == 'F')
        return false;

    std::regex reg("^[+-]?[0-9]*[.]?[0-9]*[eE]?[+-]?[0-9]*[fF]?$");
    bool matched = std::regex_match(str, reg);

    bool e_is_last = str[str.length() - 1] == 'e' || str[str.length() - 1] == 'E';
    bool f_is_last = str[str.length() - 1] == 'f' || str[str.length() - 1] == 'F';

    if(f_is_last && str.length() > 1) {
        e_is_last = str[str.length() - 2] == 'e' || str[str.length() - 2] == 'E';
    }

    return matched && !e_is_last;
}

bool IsElementNumber(const IElement &e) noexcept {
    return e.getType() == ValueType::eNumber;
}
