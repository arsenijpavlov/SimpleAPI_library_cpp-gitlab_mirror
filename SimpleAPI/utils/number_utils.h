#pragma once

#include <cmath>
#include <iomanip>
#include <limits>
#include <sstream>

namespace simpleapi {
namespace utils {

template <typename T,
         typename std::enable_if<std::is_floating_point<T>::value,
                                 int>::type = 0>
static std::string ToString(const T& value) noexcept
{
    if(std::isnan(value)) return "NaN";
    if(std::isinf(value)) return (value > 0) ? "inf" : "-inf";

    std::ostringstream oss;
    oss.imbue(std::locale::classic()); //запрещаем любой разделитель, кроме точки

    // рассчитываем минимальное количество символов для дробной части
    // (рассчитывается обратное преобразование в число)
    for(uint8_t i = 0; i <= std::numeric_limits<T>::max_digits10; i++) {
        oss.str(""); // сбрасываем результирующую строку
        oss.clear(); // сбрасываем флаги

        oss << std::fixed << std::setprecision(i) << value;

        try {
            if(value == static_cast<T>(std::stold(oss.str())))
                break;
        } catch (...) {
            // ошибки в парсинге приводят к пустой строке
            oss.str(""); // сбрасываем результирующую строку
        }
    }

    return oss.str();
}

template <typename T,
         typename std::enable_if<std::is_integral<T>::value
                                     && !std::is_same<T, bool>::value,
                                 int>::type = 0>
static std::string ToString(const T& value) noexcept
{
    std::ostringstream oss;
    oss.imbue(std::locale::classic()); //запрещаем любой разделитель, кроме точки
    oss << std::dec << +value;

    return oss.str();
}

bool        CreateBoolFromString(const std::string& input);
long long   CreateLLongFromString(const std::string& input);
long double CreateLDoubleFromString(const std::string& input);

} // namespace utils
} // namespace simpleapi
