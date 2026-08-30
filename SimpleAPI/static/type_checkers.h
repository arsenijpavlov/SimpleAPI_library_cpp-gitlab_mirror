#pragma once

#include <type_traits>
#include "../config/Config.h"


namespace simpleapi {
namespace tools {

// проверка: тип одиночный или список значений
//   получим ::value == false для SFINAE, если указанных методов не существует (нельзя вызвать)
template <typename T>
class is_config_struct {
    // очистка типа от const и ссылок
    using CleanT = typename std::decay<T>::type;

    // метод для SFINAE проверки наличия метода loadConfig у класса U
    template <typename U>
    static char test(
        decltype(
            std::declval<U>().loadConfig(
                std::declval<const Config&>()
            )
        )*
    );
    // метод для разрешения конфликта для поля value
    template <typename U> static long test(...);
public:
    static const bool value = sizeof(test<CleanT>(0)) == sizeof(char);
};
// ----------------------------------------------------------------------------

// проверка: является ли тип STL контейнером определённого формата
//   получим ::value == false для SFINAE, если указанных методов не существует (нельзя вызвать)
template <typename T>
class is_container {
    // очистка типа от const и ссылок
    using CleanT = typename std::decay<T>::type;

    // метод для SFINAE проверки
    template <typename Dummy = CleanT,
              typename = typename std::enable_if<
                 std::is_same<Dummy,
                              std::vector<typename Dummy::value_type, typename Dummy::allocator_type>
                              >::value>::type>
    static char test(int);
    // метод для разрешения конфликта для поля value
    template <typename U>
    static long test(...);
public:
    static const bool value = (sizeof(test<CleanT>(0)) == sizeof(char));
};
// ----------------------------------------------------------------------------

// проверка: является ли лямбда валидной
//   получим ::value == false для SFINAE, если указанных методов не существует (нельзя вызвать)
template <typename T, typename... Args>
class is_variadic_lambda_callable {
    // очистка типа от const и ссылок
    using CleanT = typename std::decay<T>::type;

    // метод для SFINAE проверки наличия валидной лямбы у класса U
    template <typename U>
    static char test(typename std::enable_if<
                     std::is_convertible<decltype(std::declval<U>()(std::declval<Args>()...)), bool>::value>
                     ::type*);
    // метод для разрешения конфликта для поля value
    template <typename U> static long test(...);
public:
    static const bool value = (sizeof(test<CleanT>(0)) == sizeof(char));
};
// ----------------------------------------------------------------------------

} // namespace tools
} // namespace simpleapi
