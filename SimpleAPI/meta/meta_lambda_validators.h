#pragma once

#include <type_traits>
#include <tuple>


namespace simpleapi {
namespace meta {

// проверка: является ли лямбда валидной для вызова
//   получим ::value == false для SFINAE, если указанных методов не существует (нельзя вызвать)
template <typename T, typename... Args>
class is_variadic_lambda_callable {
    // очистка типа от const и ссылок
    using CleanT = typename std::decay<T>::type;

    // если это указатель, снимаем указатель
    // иначе оставляем как есть
    using CallableT = typename std::conditional<
        std::is_pointer<CleanT>::value,
        typename std::remove_pointer<CleanT>::type,
        CleanT
        >::type;

    // метод для SFINAE проверки наличия лямбы как nullptr
    template <typename U>
    static typename std::enable_if<std::is_same<U, std::nullptr_t>::value, char>::type
        test(int);

    // метод для SFINAE проверки наличия валидной лямбы у класса U
    template <typename U>
    static auto test(int) -> decltype(
        std::declval<U>()(std::declval<Args>()...),
        char()
    );

    // Если аргументов предложено много, но Перегрузка 2 не подошла,
    // мы проверяем, умеет ли лямбда принимать ТОЛЬКО ПЕРВЫЙ аргумент
    template <typename U, typename Dummy = void>
    static auto test(long) -> decltype(
        std::declval<U>()(std::declval<typename std::tuple_element<0, std::tuple<Args...>>::type>()),
        char()
        );

    // метод для разрешения конфликта для поля value
    template <typename U> static long test(...);
public:
    static const bool value = (sizeof(test<CallableT>(0)) == sizeof(char));
};

} // namespace meta
} // namespace simpleapi

