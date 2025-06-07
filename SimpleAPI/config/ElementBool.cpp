#include "ElementBool.h"

#include "Config.h"
#include <algorithm>


std::string ElementBool::toString(const ConfigFormat format, const int8_t tabulation_level,
                                  const CommentDesign &design) const noexcept
{
//TODO: ElementBool::toString()
    return "";
}


//----------------------------------------------------------------------------------------------------------------------


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

bool IsElementBool(const Config &cfg) noexcept {
    return cfg.isBool();
}
