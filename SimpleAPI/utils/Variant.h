#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <sys/types.h>
#include <type_traits>
#include <utility>


namespace simpleapi {

namespace tools {

/* Макросы времени компиляции:
 *  - max_size_of_type      - расчёт максмального размера типа среди указанных
 *  - max_align_of_type     - расчёт максмального размера выравнивания типа
 *  - type_at_index         - получить тип на основе индекса
 *  - index_of_type         - получить индекс на основе типа
 *  - is_contains_type      - есть ли указанный тип среди списка
 *  - is_contains_duplicate - запрет создания Variant с дубликатами типов
 */

// ---------------------------------------------------------------------
// описатель рекурсивного поиска масксимального размера
// ---------------------------------------------------------------------
// базовое описание структуры для корректности выхода из SFINAE
template <typename... Types>
struct max_size_of_type;
// базовый случай, конец списка
template <>
struct max_size_of_type<> {
    static constexpr size_t size = 0;
};
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
struct max_align_of_type<> {
    static constexpr size_t align_size = 0;
};
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
// helper: база
template <bool IsMatch, typename FindType, typename... Types>
struct index_of_type_dispatcher;
// helper: если тип найден - возвращаем индекс
template <typename FindType, typename... Types>
struct index_of_type_dispatcher<true, FindType, Types...> {
    static constexpr size_t value = 0;
};
// helper: если тип НЕ найден - возвращаем ошибку
template <typename FindType, typename... Types>
struct index_of_type_dispatcher<false, FindType, Types...> {
    static constexpr size_t value = 111;
};
// ---------------------------------------------------------------------
// описатель получения индекса по типу
// ---------------------------------------------------------------------
// базовое описание структуры для корректности выхода из SFINAE
template <typename FindType, typename... Types>
struct index_of_type;
// искомый тип НЕ найден, возвращаем ошибку
template <typename FindType>
struct index_of_type<FindType> {
    static constexpr size_t value = static_cast<size_t>(-1);
};
// рекурсивный поиск
template <typename FindType, typename Head, typename... Types>
struct index_of_type<FindType, Head, Types...> {
    // базовое сравнение
    static constexpr bool is_match = std::is_same<FindType, Head>::value
                                     || std::is_constructible<FindType, Head>::value
                                     || std::is_convertible<FindType, Head>::value;

    // на основе диспетчера (аналог тернарного оператора) выбираем дальнейшее действие
    static constexpr size_t value = index_of_type_dispatcher<is_match, FindType, Head, Types...>::value;
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
struct is_contains_type<> {
    static constexpr bool value = false;
};
// список состоит из одного элемента
template <typename TemplateType, typename T>
struct is_contains_type<TemplateType, T> {
    static constexpr bool value = std::is_same<TemplateType, T>::value
                                  || std::is_constructible<TemplateType, T>::value
                                  || std::is_convertible<TemplateType, T>::value;
};
// рекурсивное сравнение типов из списка с искомым
template <typename TemplateType, typename Head, typename... Tail>
struct is_contains_type<TemplateType, Head, Tail...> {
    static constexpr bool value = std::is_same<TemplateType, Head>::value
                                  || std::is_constructible<TemplateType, Head>::value
                                  || std::is_convertible<TemplateType, Head>::value
                                  || is_contains_type<TemplateType, Tail...>::value;
};
// ---------------------------------------------------------------------

// ---------------------------------------------------------------------
// описатель проверки наличия дубликатов в списке типов
// ---------------------------------------------------------------------
// базовое описание структуры для корректности выхода из SFINAE
template <typename... Types>
struct is_contains_duplicate;
template <>
struct is_contains_duplicate<> {
    static constexpr bool value = false;
};
template <typename Head>
struct is_contains_duplicate<Head> {
    static constexpr bool value = false;
};
// рекурсивный поиск повторений наличия в списке
template <typename Head, typename... Tail>
struct is_contains_duplicate<Head, Tail...> {
    static constexpr bool value = is_contains_type<Head, Tail...>::value || is_contains_duplicate<Tail...>::value;
};
// ---------------------------------------------------------------------

} // namespace tools


// TODO: сделать уникальные правила для nullptr
template <typename... Types>
class Variant {
    static_assert(!tools::is_contains_duplicate<Types...>::value, "SimpleAPI: incorrect types list, found duplicates");

    // шаблон рекурсивного поиска конструктора
    template <ssize_t Index, bool is_in_bounds = (Index < sizeof...(Types))>
    struct Creator {
        // вариант, когда тип совпадает с искомым
        template <typename T,
                 typename Type = typename tools::type_at_index<Index, Types...>::type,
                 typename std::enable_if<std::is_constructible<Type, T&&>::value
                                         || std::is_convertible<T&&, std::string>::value
                                         , int>::type = 0
                 >
        static void create(const ssize_t& find_index, void* ptr, T&& value) {
            if(Index == find_index) {
                // создание объекта на указанном буфере с указанным типом
                new (ptr) Type(std::forward<T>(value)); // ручной вызов placement new
            } else {
                // продолжение поиска
                Creator<Index + 1>::create(find_index, ptr, std::forward<T>(value));
            }
        }
        // вариант, когда тип НЕ совпадает с искомым
        template <typename T,
                 typename Type = typename tools::type_at_index<Index, Types...>::type,
                 typename std::enable_if<!std::is_constructible<Type, T&&>::value
                                         || std::is_convertible<T&&, std::string>::value
                                         , int>::type = 0
                 >
        static void create(const ssize_t& find_index, void* ptr, T&& value) {
            if(Index == find_index) {
                // тип не совпал, нельзя присваивать (не скомпилируется) -> заглушка
            } else {
                // продолжение поиска
                Creator<Index + 1>::create(find_index, ptr, std::forward<T>(value));
            }
        }
    };

    // шаблон для остановки рекурсии (вышли за границы списка типов)
    template <ssize_t Index>
    struct Creator<Index, false> {
        template <typename T>
        static void create(const ssize_t& find_index, void* ptr, T&& value) {}
    };

    // шаблон рекурсивного поиска деструктора
    template <ssize_t Index, bool is_in_bounds = (Index < sizeof...(Types))>
    struct Destroyer {
        static void destroy(const ssize_t& find_index, void* ptr) {
            if(Index == find_index) {
                using Type = typename tools::type_at_index<Index, Types...>::type;
                (reinterpret_cast<Type*>(ptr))->~Type(); // ручной вызов деструктора
            } else {
                // продолжение поиска
                Destroyer<Index + 1>::destroy(find_index, ptr);
            }
        }
    };

    // шаблон для остановки рекурсии (вышли за границы списка типов)
    template <ssize_t Index>
    struct Destroyer<Index, false> {
        static void destroy(const ssize_t& find_index, void* ptr) {}
    };

public:
    static constexpr size_t size       = tools::max_size_of_type<Types...>::size;
    static constexpr size_t align_size = tools::max_align_of_type<Types...>::align_size;


    // по умолчанию проинициализируется первым типом (его значение по умолчанию)
    Variant() noexcept : m_current_type_index(0) {
        new (m_data) typename tools::type_at_index<0, Types...>::type(0);
    }

    template <typename T, typename std::enable_if<tools::is_contains_type<typename std::decay<T>::type, Types...>::value, int>::type = 0>
    Variant(const T& value) {
        using CleanT = typename std::decay<T>::type;
        using Index  = typename tools::index_of_type<CleanT, Types...>;
        m_current_type_index = Index::value;
        Creator<0>::create(m_current_type_index, m_data, value);
    }

    template <typename T, typename std::enable_if<tools::is_contains_type<typename std::decay<T>::type, Types...>::value, int>::type = 0>
    Variant(T&& value) {
        using CleanT = typename std::decay<T>::type;
        using Index  = typename tools::index_of_type<CleanT, Types...>;
        m_current_type_index = Index::value;
        Creator<0>::create(m_current_type_index, m_data, std::forward<CleanT>(value));
    }

//    Variant(const Variant& value) {
//        // m_current_type_index будет обновлён внутри
//        //Creator<tools::index_of_type<T, Types...>::value>::create(m_current_type_index, m_data, value);
//    }

//    Variant(Variant&& value) {
//        // m_current_type_index будет обновлён внутри
//        //Creator<tools::index_of_type<T, Types...>::value>::create(m_current_type_index, m_data, std::move(value));
//    }

    ~Variant() {
        // начинаем поиск деструктора (compile-time) с нулевого индекса
        Destroyer<0>::destroy(m_current_type_index, m_data);
    }

    template <typename T, typename std::enable_if<tools::is_contains_type<typename std::decay<T>::type, Types...>::value, int>::type = 0>
    Variant& operator=(const T& value) {
        // уничтожение старого объекта
        // начинаем поиск деструктора (compile-time) с нулевого индекса
        Destroyer<0>::destroy(m_current_type_index, m_data);

        using CleanT = typename std::decay<T>::type;
        using Index  = typename tools::index_of_type<CleanT, Types...>;
        m_current_type_index = Index::value;
        Creator<0>::create(m_current_type_index, m_data, value);
        return *this;
    }

    template <typename T, typename std::enable_if<tools::is_contains_type<typename std::decay<T>::type, Types...>::value, int>::type = 0>
    Variant& operator=(T&& value) {
        // уничтожение старого объекта
        // начинаем поиск деструктора (compile-time) с нулевого индекса
        Destroyer<0>::destroy(m_current_type_index, m_data);

        using CleanT = typename std::decay<T>::type;
        using Index  = typename tools::index_of_type<CleanT, Types...>;
        m_current_type_index = Index::value;
        Creator<0>::create(m_current_type_index, m_data, std::forward<T>(value));
        return *this;
    }

//    Variant& operator=(const Variant& other) {
//        /* FIXME */
//        return {};
//    }

//    Variant& operator=(Variant&& other) {
//        /* FIXME */
//        return {};
//    }

//    template <typename T, typename std::enable_if<tools::is_contains_type<T, Types...>::value, int>::type = 0>
//    void set(const T& other) {
//        /* FIXME */
//    }

//    template <typename T, typename std::enable_if<tools::is_contains_type<T, Types...>::value, int>::type = 0>
//    void set(T&& other) {
//        /* FIXME */
//    }

//    void set(const Variant& other) {
//        /* FIXME */
//    }

//    void set(Variant&& other) {
//        /* FIXME */
//    }

    template <typename T, typename std::enable_if<tools::is_contains_type<typename std::decay<T>::type, Types...>::value, int>::type = 0>
    T& get() {
        return *(reinterpret_cast<T*>(m_data));
    }

    template <typename T, typename std::enable_if<tools::is_contains_type<typename std::decay<T>::type, Types...>::value, int>::type = 0>
    T get() const {
        return *(reinterpret_cast<T*>(m_data));
    }

    /**
     * @brief index
     * @return Возвращает текущий индекс типа.
     */
    ssize_t index() const noexcept { return m_current_type_index; }

private:
    ssize_t m_current_type_index;
    // хранилище значения; аналог union, но через placement new
    alignas(align_size) uint8_t m_data[size];
};

} // namespace simpleapi
