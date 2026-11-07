#pragma once

#include <sys/types.h>
#include <type_traits>
#include <utility>


namespace simpleapi {
namespace meta {

// проверка, что тип содержит метод operator==()
template <typename T>
class is_contain_operator_equal {
    // очистка типа от const и ссылок
    using CleanT = typename std::decay<T>::type;

    // метод для SFINAE проверки наличия метода operator==() у класса U
    template <typename U>
    static char test(decltype(std::declval<U>() == std::declval<U>())*);
    // метод для разрешения конфликта для поля value
    template <typename U> static long test(...);
public:
    static const bool value = sizeof(test<CleanT>(0)) == sizeof(char);
};

} // namespace meta
} // namespace simpleapi

