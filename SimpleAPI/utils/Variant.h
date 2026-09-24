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
 *  - is_contains_conv_type - есть ли указанный тип среди списка (с учётом конвертации типов)
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
// описатель получения индекса по типу (std::is_same)
// ---------------------------------------------------------------------
// базовое описание структуры для корректности выхода из SFINAE
template <typename FindType, typename... Types>
struct index_of_type_same;
// искомый тип НЕ найден, возвращаем ошибку
template <typename FindType>
struct index_of_type_same<FindType> {
    static constexpr size_t value = static_cast<size_t>(-1);
};
// рекурсивный поиск
template <typename FindType, typename Head, typename... Types>
struct index_of_type_same<FindType, Head, Types...> {
    // базовое сравнение
    static constexpr bool is_match = std::is_same<FindType, Head>::value;

    struct OnTrue  { static constexpr size_t value = 0; };
    struct OnFalse {
        static constexpr size_t next_value = index_of_type_same<FindType, Types...>::value;
        // если есть ошибка, то прокидываем её наверх
        // иначе продолжаем поиск
        static constexpr size_t value = (next_value == static_cast<size_t>(-1)) ? static_cast<size_t>(-1)
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
struct index_of_type_convertible<FindType> {
    static constexpr size_t value = static_cast<size_t>(-1);
};
// рекурсивный поиск
template <typename FindType, typename Head, typename... Types>
struct index_of_type_convertible<FindType, Head, Types...> {
    // базовое сравнение
    static constexpr bool is_match = std::is_convertible<FindType, Head>::value;

    struct OnTrue  { static constexpr size_t value = 0; };
    struct OnFalse {
        static constexpr size_t next_value = index_of_type_convertible<FindType, Types...>::value;
        // если есть ошибка, то прокидываем её наверх
        // иначе продолжаем поиск
        static constexpr size_t value = (next_value == static_cast<size_t>(-1)) ? static_cast<size_t>(-1)
                                                                                : 1 + next_value;
    };

    // на основе диспетчера (аналог тернарного оператора) выбираем дальнейшее действие
    static constexpr size_t value = std::conditional<is_match, OnTrue, OnFalse>::type::value;
};
// ---------------------------------------------------------------------
// описатель получения индекса по типу (std::is_constructible)
// ---------------------------------------------------------------------
// базовое описание структуры для корректности выхода из SFINAE
template <typename FindType, typename... Types>
struct index_of_type_constructible;
// искомый тип НЕ найден, возвращаем ошибку
template <typename FindType>
struct index_of_type_constructible<FindType> {
    static constexpr size_t value = static_cast<size_t>(-1);
};
// рекурсивный поиск
template <typename FindType, typename Head, typename... Types>
struct index_of_type_constructible<FindType, Head, Types...> {
    // базовое сравнение
    static constexpr bool is_match = std::is_constructible<FindType, Head>::value;

    struct OnTrue  { static constexpr size_t value = 0; };
    struct OnFalse {
        static constexpr size_t next_value = index_of_type_constructible<FindType, Types...>::value;
        // если есть ошибка, то прокидываем её наверх
        // иначе продолжаем поиск
        static constexpr size_t value = (next_value == static_cast<size_t>(-1)) ? static_cast<size_t>(-1)
                                                                                : 1 + next_value;
    };

    // на основе диспетчера (аналог тернарного оператора) выбираем дальнейшее действие
    static constexpr size_t value = std::conditional<is_match, OnTrue, OnFalse>::type::value;
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
    static constexpr size_t same_index = index_of_type_same<FindType, Head, Types...>::value;
    static constexpr size_t conv_index = (same_index == static_cast<size_t>(-1)) ? index_of_type_convertible<FindType, Head, Types...>::value
                                                                                 : same_index; // прокидываем ошибку дальше
    static constexpr size_t value      = (conv_index == static_cast<size_t>(-1)) ? index_of_type_constructible<FindType, Head, Types...>::value
                                                                                 : conv_index; // прокидываем ошибку дальше
    static constexpr bool is_found     = value != static_cast<size_t>(-1);
};
// строгая проверка для std::nullptr_t
template <typename Head, typename... Types>
struct index_of_type<std::nullptr_t, Head, Types...> {
    static constexpr size_t value      = index_of_type_same<std::nullptr_t, Head, Types...>::value;
    static constexpr bool is_found     = value != static_cast<size_t>(-1);
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
struct is_contains_conv_type<> {
    static constexpr bool value = false;
};
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


template <typename... Types>
class Variant {
    static_assert(!tools::is_contains_duplicate<Types...>::value, "SimpleAPI: incorrect types list, found duplicates");

    // шаблон рекурсивного поиска конструктора
    template <ssize_t Index, bool is_in_bounds = (Index < sizeof...(Types))>
    struct Creator {
        struct ConstructorHelper
        {
            // helper: вариант для специализированного шаблона для std::nullptr_t
            template <typename T, typename Type, typename std::enable_if<std::is_same<Type, std::nullptr_t>::value, int>::type = 0>
            static void helper_create(const size_t& type_index, void* ptr, T&& value) {
                // объект std::nullptr_t не требует выделения памяти
            }

            // helper: общий вариант
            template <typename T, typename Type, typename std::enable_if<!std::is_same<Type, std::nullptr_t>::value, int>::type = 0>
            static void helper_create(const size_t& type_index, void* ptr, T&& value) {
                new (ptr) Type(std::forward<T>(value)); // ручной вызов placement new
            }
        };

        // вариант, когда тип совпадает с искомым
        template <typename T,
                 typename Type = typename tools::type_at_index<Index, Types...>::type,
                 typename std::enable_if<std::is_constructible<Type, T>::value, int>::type = 0
                 >
        static void create(const ssize_t& find_index, void* ptr, T&& value) {
            if(Index == find_index) {
                // создание объекта на указанном буфере с указанным типом
                ConstructorHelper::template helper_create<T, Type>(find_index, ptr, std::forward<T>(value));
            } else {
                // продолжение поиска
                Creator<Index + 1>::create(find_index, ptr, std::forward<T>(value));
            }
        }

        // вариант, когда тип НЕ совпадает с искомым
        template <typename T,
                 typename Type = typename tools::type_at_index<Index, Types...>::type,
                 typename std::enable_if<!std::is_constructible<Type, T>::value, int>::type = 0
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
        static void create(const ssize_t& find_index, void* ptr, T&& value) {
            //TODO: static_assert()
        }
    };

    // шаблон рекурсивного поиска деструктора
    template <ssize_t Index, bool is_in_bounds = (Index < sizeof...(Types))>
    struct Destroyer {
        struct DestructorHelper {
            // helper: вариант для специализированного шаблона для std::nullptr_t
            template <typename Type, typename std::enable_if<std::is_same<Type, std::nullptr_t>::value, int>::type = 0>
            static void helper_destroy(void* ptr) {
                // объект std::nullptr_t не требует освобождения памяти
            }

            // helper: общий вариант
            template <typename Type, typename std::enable_if<!std::is_same<Type, std::nullptr_t>::value, int>::type = 0>
            static void helper_destroy(void* ptr) {
                (reinterpret_cast<Type*>(ptr))->~Type(); // ручной вызов деструктора
            }
        };

        static void destroy(const ssize_t& find_index, void* ptr) {
            if(Index == find_index) {
                using Type = typename tools::type_at_index<Index, Types...>::type;
                DestructorHelper::template helper_destroy<Type>(ptr);
            } else {
                // продолжение поиска
                Destroyer<Index + 1>::destroy(find_index, ptr);
            }
        }
    };
    // шаблон для остановки рекурсии (вышли за границы списка типов)
    template <ssize_t Index>
    struct Destroyer<Index, false> {
        static void destroy(const ssize_t& find_index, void* ptr) {
            //TODO: static_assert()
        }
    };

    // шаблон рекурсивного поиска для вычисления типа объекта other
    template <bool is_in_bounds, ssize_t OtherIndex>
    struct UniversalAssigner {
        // вариант, когда тип совпадает с искомым
        template <typename... OtherTypes>
        static void assign(const ssize_t& other_index, Variant<OtherTypes...>&& other, Variant<Types...>& dest_value)
        {
            using OtherVariant = Variant<OtherTypes...>;
            if(OtherIndex == other_index) {
                using OtherType   = typename tools::type_at_index<OtherIndex, OtherTypes...>::type;
                using TargetIndex = typename tools::index_of_type<OtherType, Types...>::value;

                static_assert(TargetIndex::is_found, "SimpleAPI: Variant target object does not support this type");

                // FIXME: пока не очень понимаю смысл этого условия
                // вычислить тип переменной (l-value/r-value)
                using ValueFormat = typename std::conditional<
                    std::is_const<typename std::remove_reference<OtherVariant>::type>::value,
                    const OtherType,
                    OtherType
                    >::type;

                // достать указатель на value
                ValueFormat* other_data = reinterpret_cast<ValueFormat*>(other.m_data);

                // вызвать перегрузку operator= на основе формата переменной
                dest_value = std::forward<std::conditional<std::is_lvalue_reference<OtherVariant>::value,
                                                           const OtherType&,
                                                           OtherType&&
                                                           >::type
                                          >(*other_data);

            } else {
                // продолжение поиска
                static constexpr bool next_in_bounds = (OtherIndex + 1) < sizeof...(OtherTypes);
                UniversalAssigner<next_in_bounds, OtherIndex + 1>::assign(other_index,
                                                                          std::forward<OtherVariant>(other),
                                                                          dest_value);
            }
        }
    };
    // шаблон для остановки рекурсии (вышли за границы списка типов)
    template <ssize_t OtherIndex>
    struct UniversalAssigner<false, OtherIndex> {
        static void assign(const ssize_t& find_index, void* ptr) {
            //TODO: static_assert()
        }
    };

public:
    //-----------------------------------------------------------------------------------------
    // константы класса времени компиляции
    //-----------------------------------------------------------------------------------------
    static constexpr size_t size       = tools::max_size_of_type<Types...>::size;
    static constexpr size_t align_size = tools::max_align_of_type<Types...>::align_size;
    //-----------------------------------------------------------------------------------------

    // по умолчанию проинициализируется первым типом (его значение по умолчанию)
    Variant() noexcept : m_current_type_index(0) {
        new (m_data) typename tools::type_at_index<0, Types...>::type({});
    }

    template <typename std::enable_if<tools::index_of_type<std::nullptr_t, Types...>::is_found, int>::type = 0>
    Variant(std::nullptr_t) {
        using Index  = typename tools::index_of_type<std::nullptr_t, Types...>;
        m_current_type_index = Index::value;
        Creator<0>::create(m_current_type_index, m_data, nullptr);
    }

    template <typename T, typename std::enable_if<tools::index_of_type<typename std::decay<T>::type, Types...>::is_found, int>::type = 0>
    Variant(const T& value) {
        using CleanT = typename std::decay<T>::type;
        using Index  = typename tools::index_of_type<CleanT, Types...>;
        m_current_type_index = Index::value;
        Creator<0>::create(m_current_type_index, m_data, value);
    }

    template <typename T, typename std::enable_if<tools::index_of_type<typename std::decay<T>::type, Types...>::is_found, int>::type = 0>
    Variant(T&& value) {

        using CleanT = typename std::decay<T>::type;
        using Index  = typename tools::index_of_type<CleanT, Types...>;

        static_assert(std::is_constructible<typename tools::type_at_index<Index::value, Types...>::type, T>::value,
                      "SimpleAPI: incorrect type for creating Variant value");

        m_current_type_index = Index::value;
        Creator<0>::create(m_current_type_index, m_data, std::forward<T>(value));
    }

    template <typename... OtherTypes>
    Variant(const Variant<OtherTypes...>& other) {
        // NOTE: проверка if(this != &other) не нужна, т.к. типы заведомо разные по variadic
        UniversalAssigner<true, 0>::assign(m_current_type_index, other, *this);
    }

    template <typename... OtherTypes>
    Variant(Variant<OtherTypes...>&& other) {
        // NOTE: проверка if(this != &other) не нужна, т.к. типы заведомо разные по variadic
        UniversalAssigner<true, 0>::assign(m_current_type_index, other, *this);
    }

    ~Variant() {
        // начинаем поиск деструктора (compile-time) с нулевого индекса
        Destroyer<0>::destroy(m_current_type_index, m_data);
    }

    template <typename std::enable_if<tools::index_of_type<std::nullptr_t, Types...>::is_found, int>::type = 0>
    Variant& operator=(std::nullptr_t) {
        // уничтожение старого объекта
        // начинаем поиск деструктора (compile-time) с нулевого индекса
        Destroyer<0>::destroy(m_current_type_index, m_data);

        using Index  = typename tools::index_of_type<std::nullptr_t, Types...>;
        m_current_type_index = Index::value;
        Creator<0>::create(m_current_type_index, m_data, nullptr);
        return *this;
    }

    template <typename T, typename std::enable_if<tools::index_of_type<typename std::decay<T>::type, Types...>::is_found, int>::type = 0>
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

    template <typename T, typename std::enable_if<tools::index_of_type<typename std::decay<T>::type, Types...>::is_found, int>::type = 0>
    Variant& operator=(T&& value) {
        // уничтожение старого объекта
        // начинаем поиск деструктора (compile-time) с нулевого индекса
        Destroyer<0>::destroy(m_current_type_index, m_data);

        using CleanT = typename std::decay<T>::type;
        using Index  = typename tools::index_of_type<CleanT, Types...>;

        static_assert(std::is_constructible<typename tools::type_at_index<Index::value, Types...>::type, T>::value,
                      "SimpleAPI: incorrect type for creating Variant value");

        m_current_type_index = Index::value;
        Creator<0>::create(m_current_type_index, m_data, std::forward<T>(value));
        return *this;
    }

    // TODO: закончить реализацию
//    template <typename... OtherTypes>
//    Variant& operator=(const Variant<OtherTypes...>& other) {
//        if(this != &other) {
//            // достать тип элемента внутри other
//            using Type = typename tools::type_at_index<other.getIndex(), OtherTypes...>::type;
//            // создать аналог для this
//            using Index = typename tools::index_of_type<Type, Types...>;
//            m_current_type_index = Index::value;
//            Creator<0>::create(m_current_type_index, m_data, other.template get<Type>());
//        }
//        return *this;
//    }

    // TODO: закончить реализацию
//    template <typename... OtherTypes>
//    Variant& operator=(Variant<OtherTypes...>&& other) {
//        if(this != &other) {
//            // достать тип элемента внутри other
//            using Type = typename tools::type_at_index<other.getIndex(), OtherTypes...>::type;
//            // создать аналог для this
//            using Index = typename tools::index_of_type<Type, Types...>;
//            m_current_type_index = Index::value;
//            Creator<0>::create(m_current_type_index, m_data, std::forward<Type>(other.template get<Type>()));
//        }
//        return *this;
//    }

//    template <typename T, typename std::enable_if<tools::is_contains_conv_type<T, Types...>::value, int>::type = 0>
//    void set(const T& other) {
//        /* FIXME */
//    }

//    template <typename T, typename std::enable_if<tools::is_contains_conv_type<T, Types...>::value, int>::type = 0>
//    void set(T&& other) {
//        /* FIXME */
//    }

//    void set(const Variant& other) {
//        /* FIXME */
//    }

//    void set(Variant&& other) {
//        /* FIXME */
//    }

    template <typename std::enable_if<tools::index_of_type<std::nullptr_t, Types...>::is_found, int>::type = 0>
    std::nullptr_t get() {
        return nullptr;
    }

    template <typename T, typename std::enable_if<tools::index_of_type<typename std::decay<T>::type, Types...>::is_found, int>::type = 0>
    T& get() {
        return *(reinterpret_cast<T*>(m_data));
    }

    template <typename T, typename std::enable_if<tools::index_of_type<typename std::decay<T>::type, Types...>::is_found, int>::type = 0>
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
