#pragma once

#include <cstddef>
#include <sys/types.h>
#include <type_traits>


namespace simpleapi {
namespace tools {

/* Макросы времени компиляции:
 *  - max_size_of_type            - расчёт максмального размера типа среди указанных
 *  - max_align_of_type           - расчёт максмального размера выравнивания типа
 *  - type_at_index               - получить тип на основе индекса
 *  - index_of_type_same          - поиск подходящего типа из списка по точному совпадению
 *  - index_of_type_convertible   - поиск подходящего типа из списка по проверке преобразования
 *  - index_of_type_constructible - поиск подходящего типа из списка по проверке конструктора
 *  - compiler_type_resolver      - даём компилятору выбрать подходящую перегрузку функции и получаем её индекс
 *  - index_of_type               - получить индекс на основе типа (выбирает компилятор)
 *  - is_contains_type            - есть ли указанный тип среди списка
 *  - is_contains_conv_type       - есть ли указанный тип среди списка (с учётом конвертации типов)
 *  - is_contains_duplicate       - запрет создания Variant с дубликатами типов
 */

// ---------------------------------------------------------------------
// описатель рекурсивного поиска масксимального размера
// ---------------------------------------------------------------------
// базовое описание структуры для корректности выхода из SFINAE
template <typename... Types>
struct max_size_of_type;
// базовый случай, конец списка
template <>
struct max_size_of_type<> { static constexpr size_t size = 0; };
// рекурсивное извлечение максимального размера
template <typename Head, typename... Tail>
struct max_size_of_type<Head, Tail...> {
    static constexpr size_t size = sizeof(Head) > max_size_of_type<Tail...>::size ? sizeof(Head)
                                                                                  : max_size_of_type<Tail...>::size;
};
// ---------------------------------------------------------------------

// ---------------------------------------------------------------------
// описатель рекурсивного поиска максимальной границы выравнивания
// ---------------------------------------------------------------------
// базовое описание структуры для корректности выхода из SFINAE
template <typename... Types>
struct max_align_of_type;
// базовый случай, конец списка
template <>
struct max_align_of_type<> { static constexpr size_t align_size = 0; };
// рекурсивное извлечение максимального размера выравнивания
template <typename Head, typename... Tail>
struct max_align_of_type<Head, Tail...> {
    static constexpr size_t align_size = alignof(Head) > max_align_of_type<Tail...>::align_size ? alignof(Head)
                                                                                                : max_align_of_type<Tail...>::align_size;
};
// ---------------------------------------------------------------------

// ---------------------------------------------------------------------
// описатель получения типа по индексу
// ---------------------------------------------------------------------
// базовое описание структуры для корректности выхода из SFINAE
template <size_t Index, typename... Types>
struct type_at_index;
// дошли до конца списка
template <typename Head, typename... Tail>
struct type_at_index<0, Head, Tail...> {
    using type = Head; // возвращаем тип
};
// рекурсивное извлечение типа по итерации индекса
template <size_t Index, typename Head, typename... Tail>
struct type_at_index<Index, Head, Tail...> {
    using type = typename type_at_index<Index - 1, Tail...>::type;
};
// ---------------------------------------------------------------------

// ---------------------------------------------------------------------
// описатель получения индекса по типу (std::is_same)
// ---------------------------------------------------------------------
// базовое описание структуры для корректности выхода из SFINAE
template <typename FindType, typename... Types>
struct index_of_type_same;
// искомый тип НЕ найден, возвращаем ошибку
template <typename FindType>
struct index_of_type_same<FindType> { static constexpr ssize_t value = -1; };
// рекурсивный поиск
template <typename FindType, typename Head, typename... Types>
struct index_of_type_same<FindType, Head, Types...> {
    // базовое сравнение
    static constexpr bool is_match = std::is_same<FindType, Head>::value;

    struct OnTrue  { static constexpr size_t value = 0; };
    struct OnFalse {
        static constexpr ssize_t next_value = index_of_type_same<FindType, Types...>::value;
        // если есть ошибка, то прокидываем её наверх
        // иначе продолжаем поиск
        static constexpr ssize_t value = (next_value == -1) ? -1
                                                            : 1 + next_value;
    };

    // на основе диспетчера (аналог тернарного оператора) выбираем дальнейшее действие
    static constexpr size_t value = std::conditional<is_match, OnTrue, OnFalse>::type::value;
};
// ---------------------------------------------------------------------
// описатель получения индекса по типу (std::is_convertible)
// ---------------------------------------------------------------------
// базовое описание структуры для корректности выхода из SFINAE
template <typename FindType, typename... Types>
struct index_of_type_convertible;
// искомый тип НЕ найден, возвращаем ошибку
template <typename FindType>
struct index_of_type_convertible<FindType> { static constexpr ssize_t value = -1; };
// рекурсивный поиск
template <typename FindType, typename Head, typename... Types>
struct index_of_type_convertible<FindType, Head, Types...> {
    // базовое сравнение
    static constexpr bool is_match = std::is_convertible<FindType, Head>::value;

    struct OnTrue  { static constexpr ssize_t value = 0; };
    struct OnFalse {
        static constexpr ssize_t next_value = index_of_type_convertible<FindType, Types...>::value;
        // если есть ошибка, то прокидываем её наверх
        // иначе продолжаем поиск
        static constexpr ssize_t value = (next_value == -1) ? -1
                                                            : 1 + next_value;
    };

    // на основе диспетчера (аналог тернарного оператора) выбираем дальнейшее действие
    static constexpr ssize_t value = std::conditional<is_match, OnTrue, OnFalse>::type::value;
};
// ---------------------------------------------------------------------
// описатель получения индекса по типу (std::is_constructible)
// ---------------------------------------------------------------------
// базовое описание структуры для корректности выхода из SFINAE
template <typename FindType, typename... Types>
struct index_of_type_constructible;
// искомый тип НЕ найден, возвращаем ошибку
template <typename FindType>
struct index_of_type_constructible<FindType> { static constexpr ssize_t value = -1; };
// рекурсивный поиск
template <typename FindType, typename Head, typename... Types>
struct index_of_type_constructible<FindType, Head, Types...> {
    // базовое сравнение
    static constexpr bool is_match = std::is_constructible<FindType, Head>::value;

    struct OnTrue  { static constexpr ssize_t value = 0; };
    struct OnFalse {
        static constexpr ssize_t next_value = index_of_type_constructible<FindType, Types...>::value;
        // если есть ошибка, то прокидываем её наверх
        // иначе продолжаем поиск
        static constexpr ssize_t value = (next_value == -1) ? -1
                                                            : 1 + next_value;
    };

    // на основе диспетчера (аналог тернарного оператора) выбираем дальнейшее действие
    static constexpr ssize_t value = std::conditional<is_match, OnTrue, OnFalse>::type::value;
};

// ---------------------------------------------------------------------
// разворачиваем список типов -> компилятор выберет наиболее подходящую перегрузку
// проверка вернёт тип void, если нет подходящего кандидата
// ---------------------------------------------------------------------
// базовое описание структуры для корректности выхода из SFINAE
template <ssize_t Index, typename... Types>
struct compiler_type_resolver;
// рекурсивный поиск
template <ssize_t Index, typename Head, typename... Types>
struct compiler_type_resolver<Index, Head, Types...> : compiler_type_resolver<Index + 1, Types...>
{
    // делаем метод предка явно видимым
    using compiler_type_resolver<Index + 1, Types...>::match;

    template <typename T>
    using CleanT      = typename std::remove_reference<T>::type;
    template <typename T>
    using BaseElement = typename std::remove_all_extents<CleanT<T>>::type;
    // std::integral_constant<> - создаёт тип-связку
    template <typename T,
             typename std::enable_if<std::is_constructible<Head, T>::value
                                         && !(std::is_array<CleanT<T>>::value
                                             && !std::is_same<typename std::decay<BaseElement<T>>::type, char>::value
                                             && !std::is_same<typename std::decay<BaseElement<T>>::type, wchar_t>::value)
                                         && !(std::is_same<Head, bool>::value
                                              && (std::is_array<CleanT<T>>::value
                                                  || std::is_pointer<typename std::decay<T>::type>::value))
                                     , int>::type = 0>
    static std::integral_constant<ssize_t, Index> match(Head);
};
// остановка рекурсии (пустой список)
template <ssize_t Index>
struct compiler_type_resolver<Index> {
    template <typename T>
    static std::integral_constant<ssize_t, -1> match(...); // заглушка
};
// ---------------------------------------------------------------------
// описатель получения индекса по типу (ОБЩИЙ)
// ---------------------------------------------------------------------
// базовое описание структуры для корректности выхода из SFINAE
template <typename FindType, typename... Types>
struct index_of_type;
// рекурсивный поиск
template <typename FindType, typename Head, typename... Types>
struct index_of_type<FindType, Head, Types...> {
    // делегируем выбор типа компилятору
    using Selector = decltype(compiler_type_resolver<0, Head, Types...>::template match<FindType>(std::declval<FindType>()));

    static constexpr ssize_t value = Selector::value;
    static constexpr bool is_found = value != -1;
};
// строгая проверка для std::nullptr_t
template <typename Head, typename... Types>
struct index_of_type<std::nullptr_t, Head, Types...> {
    static constexpr ssize_t value = index_of_type_same<std::nullptr_t, Head, Types...>::value;
    static constexpr bool is_found = value != -1;
};
// ---------------------------------------------------------------------

// ---------------------------------------------------------------------
// описатель проверки наличия типа в списке
// ---------------------------------------------------------------------
// базовое описание структуры для корректности выхода из SFINAE
template <typename... Types>
struct is_contains_type;
// дошли до конца списка
template <>
struct is_contains_type<> { static constexpr bool value = false; };
// список состоит из одного элемента
template <typename TemplateType, typename T>
struct is_contains_type<TemplateType, T> {
    static constexpr bool value = std::is_same<TemplateType, T>::value;
};
// рекурсивное сравнение типов из списка с искомым
template <typename TemplateType, typename Head, typename... Tail>
struct is_contains_type<TemplateType, Head, Tail...> {
    static constexpr bool value = std::is_same<TemplateType, Head>::value
                                  || is_contains_type<TemplateType, Tail...>::value;
};
// ---------------------------------------------------------------------
// описатель проверки наличия типа в списке (с учётом конвертации)
// ---------------------------------------------------------------------
// базовое описание структуры для корректности выхода из SFINAE
template <typename... Types>
struct is_contains_conv_type;
// дошли до конца списка
template <>
struct is_contains_conv_type<> { static constexpr bool value = false; };
// список состоит из одного элемента
template <typename TemplateType, typename T>
struct is_contains_conv_type<TemplateType, T> {
    static constexpr bool value = std::is_same<TemplateType, T>::value
                                  || std::is_constructible<TemplateType, T>::value
                                  || std::is_convertible<TemplateType, T>::value;
};
// рекурсивное сравнение типов из списка с искомым
template <typename TemplateType, typename Head, typename... Tail>
struct is_contains_conv_type<TemplateType, Head, Tail...> {
    static constexpr bool value = std::is_same<TemplateType, Head>::value
                                  || std::is_constructible<TemplateType, Head>::value
                                  || std::is_convertible<TemplateType, Head>::value
                                  || is_contains_conv_type<TemplateType, Tail...>::value;
};
// ---------------------------------------------------------------------

// ---------------------------------------------------------------------
// описатель проверки наличия дубликатов в списке типов (квалификатор const не создаёт состояние дубликата)
// ---------------------------------------------------------------------
// базовое описание структуры для корректности выхода из SFINAE
template <typename... Types>
struct is_contains_duplicate;
template <>
struct is_contains_duplicate<>      { static constexpr bool value = false; };
template <typename Head>
struct is_contains_duplicate<Head>  { static constexpr bool value = false; };
// рекурсивный поиск повторений наличия в списке
template <typename Head, typename... Tail>
struct is_contains_duplicate<Head, Tail...> {
    static constexpr bool value = is_contains_type<Head, Tail...>::value || is_contains_duplicate<Tail...>::value;
};
// ---------------------------------------------------------------------

} // namespace tools
} // namespace simpleapi
