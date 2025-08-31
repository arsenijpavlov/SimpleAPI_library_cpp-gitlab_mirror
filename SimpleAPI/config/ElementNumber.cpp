#include "ElementNumber.h"

#include "Config.h"
#include <regex>


void ElementNumber::clear() noexcept {
    clearComment();
    m_value = 0;
}

bool ElementNumber::isEqual(const IElement &other, const bool compare_comments,
                            const bool map_sort_important) const noexcept
{
    bool b1 = !compare_comments || isCommentsEqual(other);
    bool b2 = m_value == reinterpret_cast<const ElementNumber&>(other).getValue();

    return b1 && b2;
}

std::string ElementNumber::toString(const ConfigFormat format, const int8_t tabulation_level,
                                    const CommentDesign &design) const noexcept
{
    //TODO: ElementNumber::toString()
    return "";
}

std::string ElementNumber::toJsonString(const int8_t tabulation_level, const CommentDesign &design) const noexcept
{
    //TODO: std::string ElementNumber::toJsonString()
    return "";
}

std::string ElementNumber::toIniString(const int8_t tabulation_level, const CommentDesign &design) const noexcept
{
    //TODO: std::string ElementNumber::toIniString()
    return "";
}

std::string ElementNumber::toYamlString(const int8_t tabulation_level, const CommentDesign &design) const noexcept
{
    //TODO: std::string ElementNumber::toYamlString()
    return "";
}

std::string ElementNumber::toXmlString(const int8_t tabulation_level, const CommentDesign &design) const noexcept
{
    //TODO: std::string ElementNumber::toXmlString()
    return "";
}


//----------------------------------------------------------------------------------------------------------------------


//FIXME: сейчас дублируется код из папки utils
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

bool IsElementNumber(const Config &cfg) noexcept {
    return cfg.isNumber();
}
