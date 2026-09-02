#pragma once

#include <bitset>
#include <deque>
#include <forward_list>
#include <list>
#include <queue>
#include <set>
#include <stack>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
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
class is_container_as_vector {
    // очистка типа от const и ссылок
    using CleanT = typename std::decay<T>::type;

    // метод для SFINAE проверки
    template <typename Dummy = CleanT,
              typename = typename std::enable_if<
                 std::is_same<Dummy, std::vector<typename Dummy::value_type,
                                                 typename Dummy::allocator_type>>::value
                 || std::is_same<Dummy, std::list<typename Dummy::value_type,
                                                  typename Dummy::allocator_type>>::value
                 || std::is_same<Dummy, std::forward_list<typename Dummy::value_type,
                                                          typename Dummy::allocator_type>>::value
                 || std::is_same<Dummy, std::deque<typename Dummy::value_type,
                                                   typename Dummy::allocator_type>>::value
                 >::type
             >
    static char test(int);
    // метод для разрешения конфликта для поля value
    template <typename U>
    static long test(...);
public:
    static const bool value = (sizeof(test<CleanT>(0)) == sizeof(char));
};
//----------------
template <typename T>
class is_container_as_set {
    // очистка типа от const и ссылок
    using CleanT = typename std::decay<T>::type;

    // метод для SFINAE проверки
    template <typename Dummy = CleanT,
              typename = typename std::enable_if<
                 std::is_same<Dummy, std::set<typename Dummy::value_type,
                                              typename Dummy::key_compare,
                                              typename Dummy::allocator_type>>::value
                 || std::is_same<Dummy, std::multiset<typename Dummy::value_type,
                                                      typename Dummy::key_compare,
                                                      typename Dummy::allocator_type>>::value
                 >::type
             >
    static char test(int);
    // метод для разрешения конфликта для поля value
    template <typename U>
    static long test(...);
public:
    static const bool value = (sizeof(test<CleanT>(0)) == sizeof(char));
};
//----------------
template <typename T>
class is_container_as_unordered_set {
    // очистка типа от const и ссылок
    using CleanT = typename std::decay<T>::type;

    // метод для SFINAE проверки
    template <typename Dummy = CleanT,
              typename = typename std::enable_if<
                 std::is_same<Dummy, std::unordered_set<typename Dummy::value_type,
                                                        typename Dummy::hasher,
                                                        typename Dummy::key_equal,
                                                        typename Dummy::allocator_type>>::value
                 || std::is_same<Dummy, std::unordered_multiset<typename Dummy::value_type,
                                                                typename Dummy::hasher,
                                                                typename Dummy::key_equal,
                                                                typename Dummy::allocator_type>>::value
                 >::type
             >
    static char test(int);
    // метод для разрешения конфликта для поля value
    template <typename U>
    static long test(...);
public:
    static const bool value = (sizeof(test<CleanT>(0)) == sizeof(char));
};
//----------------
template <typename T>
class is_container_as_queue {
    // очистка типа от const и ссылок
    using CleanT = typename std::decay<T>::type;

    // метод для SFINAE проверки
    template <typename Dummy = CleanT,
              typename = typename std::enable_if<
                 std::is_same<Dummy, std::queue<typename Dummy::value_type,
                                                typename Dummy::container_type>>::value
                 >::type
             >
    static char test(int);
    // метод для разрешения конфликта для поля value
    template <typename U>
    static long test(...);
public:
    static const bool value = (sizeof(test<CleanT>(0)) == sizeof(char));
};
//----------------
template <typename T>
class is_container_as_priority_queue {
    // очистка типа от const и ссылок
    using CleanT = typename std::decay<T>::type;

    // метод для SFINAE проверки
    template <typename Dummy = CleanT,
              typename = typename std::enable_if<
                 std::is_same<Dummy, std::priority_queue<typename Dummy::value_type,
                                                         typename Dummy::container_type,
                                                         typename Dummy::value_compare>>::value
                 >::type
             >
    static char test(int);
    // метод для разрешения конфликта для поля value
    template <typename U>
    static long test(...);
public:
    static const bool value = (sizeof(test<CleanT>(0)) == sizeof(char));
};
//----------------
template <typename T>
class is_container_as_stack {
    // очистка типа от const и ссылок
    using CleanT = typename std::decay<T>::type;

    // метод для SFINAE проверки
    template <typename Dummy = CleanT,
             typename = typename std::enable_if<
                 std::is_same<Dummy, std::stack<typename Dummy::value_type,
                                                typename Dummy::container_type>>::value
                 >::type
             >
    static char test(int);
    // метод для разрешения конфликта для поля value
    template <typename U>
    static long test(...);
public:
    static const bool value = (sizeof(test<CleanT>(0)) == sizeof(char));
};
//----------------
template <typename T>
class is_container_as_array {
    // очистка типа от const и ссылок
    using CleanT = typename std::decay<T>::type;

    // метод для SFINAE проверки
    template <typename Dummy = CleanT,
             typename = typename std::enable_if<
                 std::is_same<Dummy, std::array<typename Dummy::value_type,
                                                std::tuple_size<Dummy>::value>>::value
                 >::type
             >
    static char test(int);
    // метод для разрешения конфликта для поля value
    template <typename U>
    static long test(...);
public:
    static const bool value = (sizeof(test<CleanT>(0)) == sizeof(char));
};
//----------------
template <typename T>
class is_container_as_bitset {
    // очистка типа от const и ссылок
    using CleanT = typename std::decay<T>::type;

    // метод для SFINAE проверки
    template <typename Dummy = CleanT,
             typename = typename std::enable_if<
                 std::is_same<Dummy, std::bitset<Dummy().size()>>::value
                 >::type
             >
    static char test(int);
    // метод для разрешения конфликта для поля value
    template <typename U>
    static long test(...);
public:
    static const bool value = (sizeof(test<CleanT>(0)) == sizeof(char));
};
//----------------
template <typename T>
class is_container_as_map {
    // очистка типа от const и ссылок
    using CleanT = typename std::decay<T>::type;

    // метод для SFINAE проверки
    template <typename Dummy = CleanT,
             typename = typename std::enable_if<
                 std::is_same<Dummy, std::map<typename Dummy::key_type,
                                              typename Dummy::mapped_type,
                                              typename Dummy::key_compare,
                                              typename Dummy::allocator_type>>::value
                 || std::is_same<Dummy, std::multimap<typename Dummy::key_type,
                                                      typename Dummy::mapped_type,
                                                      typename Dummy::key_compare,
                                                      typename Dummy::allocator_type>>::value
                 >::type
             >
    static char test(int);
    // метод для разрешения конфликта для поля value
    template <typename U>
    static long test(...);
public:
    static const bool value = (sizeof(test<CleanT>(0)) == sizeof(char));
};
//----------------
template <typename T>
class is_container_as_unordered_map {
    // очистка типа от const и ссылок
    using CleanT = typename std::decay<T>::type;

    // метод для SFINAE проверки
    template <typename Dummy = CleanT,
             typename = typename std::enable_if<
                 std::is_same<Dummy, std::unordered_map<typename Dummy::key_type,
                                                        typename Dummy::mapped_type,
                                                        typename Dummy::hasher,
                                                        typename Dummy::key_equal,
                                                        typename Dummy::allocator_type>>::value
                 || std::is_same<Dummy, std::unordered_multimap<typename Dummy::key_type,
                                                                typename Dummy::mapped_type,
                                                                typename Dummy::hasher,
                                                                typename Dummy::key_equal,
                                                                typename Dummy::allocator_type>>::value
                 >::type
             >
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

    // если это указатель, снимаем указатель
    // иначе оставляем как есть
    using CallableT = typename std::conditional<
        std::is_pointer<CleanT>::value,
        typename std::remove_pointer<CleanT>::type,
        CleanT
        >::type;

    // метод для SFINAE проверки наличия валидной лямбы у класса U
    template <typename U>
    static char test(typename std::enable_if<
                     !std::is_same<U, std::nullptr_t>::value
                     && std::is_convertible<decltype(std::declval<U>()(std::declval<Args>()...)), bool>::value
                     >::type*);
    // метод для разрешения конфликта для поля value
    template <typename U> static long test(...);
public:
    static const bool value = (sizeof(test<CallableT>(0)) == sizeof(char));
};
// ---------------
// описания перегрузок для лямбд/std::function() для вызова как через объект, так и через указатель на него
template <typename T, typename ValidatorT, typename ... Args>
static bool ExecuteValidator(const T& value, ValidatorT validator, Args&&... args,
                             typename std::enable_if<std::is_pointer<ValidatorT>::value>::type* = 0)
{
    // если это указатель на функцию/лямбду (или nullptr)
    if (validator != nullptr) {
        if (!(*validator)(value, std::forward<Args>(args)...)) { // разыменовываем указатель перед вызовом
            return false;
        }
    }
    return true;
}
template <typename T, typename ValidatorT, typename ... Args>
static bool ExecuteValidator(const T& value, const ValidatorT& validator, Args&&... args,
                             typename std::enable_if<!std::is_pointer<ValidatorT>::value>::type* = 0)
{
    // если это прямая лямбда или функтор
    if (!validator(value, std::forward<Args>(args)...)) {
        return false;
    }
    return true;
}
// ----------------------------------------------------------------------------

} // namespace tools
} // namespace simpleapi
