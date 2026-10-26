#pragma once

#include <string>
#include <sstream>

/* следующие решения тестировались на специфичной сборке:
* crosstool-NG 1.24.0
* - linux headers 2.6.39.4
* - binutils 2.26.1
* - glibc 2.14.1
* - gcc 4.9.4 (первые стабильные C++11)
*/

namespace simpleapi {
namespace tools {

    // проверяем, что функция std::to_string уже существует
    template <typename T>
    auto test_to_string(T&& val) -> decltype(std::to_string(std::forward<T>(val)), std::true_type{});

    // выход из SFINAE, если std::to_string() НЕ существует
    std::false_type test_to_string(...);

    template <typename T>
    constexpr bool has_to_string() {
        return decltype(test_to_string(std::declval<T>()))::value;
    }
} // namespace tools
} // namespace simpleapi

namespace std {

template <typename T,
      typename = typename std::enable_if<!simpleapi::tools::has_to_string<T>()>::type>
std::string to_string(T value)
{
    std::ostringstream os;
    os << value;
    return os.str();
}

// следующее условие проверено для gcc 4.9.4 и gcc 11; clangd от IDE QtCreator всё равно отметит как ошибку
#if (defined(__GNUC__) && !defined(__clang__) && !defined(__INTEL_COMPILER) && __GNUC__ < 5) || \
    (defined(__clang__) && !defined(__cpp_lib_is_trivially_copyable) && defined(__GNUC__) && __GNUC__ < 5)
template <typename T>
struct is_trivially_copyable: public std::integral_constant<bool, __has_trivial_copy(T)>
{};
#endif

// Дополнительно: если в C++11 коде SimpleAPI используется std::stoi
/*
inline int stoi(const std::string& str) {
    std::istringstream is(str);
    int val;
    is >> val;
    return val;
}
*/

} // namespace std


