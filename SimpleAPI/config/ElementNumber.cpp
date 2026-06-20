#include "ElementNumber.h"

#include "Config.h"
#include <cmath>
#include <iomanip>
#include <limits>
#include <regex>


namespace simpleapi {
namespace tools {

ElementNumber::ElementNumber(long double &&num) noexcept   {
    init();
    m_value = std::move(num);
}

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

std::string ElementNumber::toString() const noexcept
{
    if(std::isnan(m_value)) return "NaN";
    if(std::isinf(m_value)) return (m_value > 0) ? "inf" : "-inf";

    std::ostringstream oss;
    oss.imbue(std::locale::classic()); //запрещаем любой разделитель, кроме точки

    // рассчитываем минимальное количество символов для дробной части
    // (рассчитывается обратное преобразование в число)
    for(uint8_t i = 0; i <= std::numeric_limits<long double>::max_digits10; i++) {
        oss.str(""); // сбрасываем результирующую строку
        oss.clear(); // сбрасываем флаги

        oss << std::fixed << std::setprecision(i) << m_value;

        try {
        if(m_value == std::stold(oss.str()))
            break;
        } catch (...) {
            // ошибки в парсинге приводят к пустой строке
            oss.str(""); // сбрасываем результирующую строку
        }
    }

    return oss.str();
}

//----------------------------------------------------------------------------------------------------------------------


//NOTE: сейчас дублируется код из папки utils (в utils:: выполнит stod() во второй аргумент)
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

} // namespace tools
} // namespace simpleapi

