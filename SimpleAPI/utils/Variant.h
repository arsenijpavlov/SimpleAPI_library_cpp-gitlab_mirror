#pragma once

#include "type_finders_from_typelist.h"
#include <cstdint>
#include <utility>


namespace simpleapi {

template <typename... Types>
class Variant {
    static_assert(!tools::is_contains_duplicate<Types...>::value, "SimpleAPI: incorrect types list, found duplicates");

    // шаблон рекурсивного поиска конструктора
    // NOTE: внешний код всегда вызывает начальный индекс <0>
    template <ssize_t Index, bool is_in_bounds = (Index < sizeof...(Types))>
    struct Creator {
        struct ConstructorHelper
        {
            // helper: вариант для специализированного шаблона для std::nullptr_t
            template <typename T, typename Type, typename std::enable_if<std::is_same<Type, std::nullptr_t>::value, int>::type = 0>
            static void helper_create(const ssize_t& type_index, void* ptr, T&& value) {
                // объект std::nullptr_t не требует выделения памяти
            }

            // helper: общий вариант
            template <typename T, typename Type, typename std::enable_if<!std::is_same<Type, std::nullptr_t>::value, int>::type = 0>
            static void helper_create(const ssize_t& type_index, void* ptr, T&& value) {
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
        static void create(const ssize_t& find_index, void* ptr, T&& value) { /*метод-заглушка*/ }
    };

    // шаблон рекурсивного поиска деструктора
    // NOTE: внешний код всегда вызывает начальный индекс <0>
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
            if(find_index == -1)
                return; // память не задействована, деструктор не нужен
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
        static void destroy(const ssize_t& find_index, void* ptr) { /*метод-заглушка*/ }
    };

    //---------------------------------------------------------------------------------
    // helper: тип найден - присваивание (const T&)
    template <bool enable = true, typename SrcType, typename TargetType,
             typename std::enable_if<
                 std::is_assignable<TargetType, SrcType>::value
             , int>::type = 0>
    static void HelperAssign(void* target_ptr, const SrcType& value)
    {
        *reinterpret_cast<TargetType*>(target_ptr) = std::forward<SrcType>(value);
    }
    // helper: тип найден - присваивание (T&&, указатель)
    template <bool enable = true, typename SrcType, typename TargetType,
             typename std::enable_if<
                 std::is_assignable<TargetType, SrcType>::value
                 && std::is_trivially_copyable<SrcType>::value
             , int>::type = 0>
    static void HelperAssign(void* target_ptr, SrcType&& value)
    {
        // placement new
        new (static_cast<void*>(target_ptr)) TargetType(value);
    }
    // helper: тип найден - присваивание (T&&)
    template <bool enable = true, typename SrcType, typename TargetType,
             typename std::enable_if<
                 std::is_assignable<TargetType, SrcType>::value
                 && !std::is_trivially_copyable<SrcType>::value
             , int>::type = 0>
    static void HelperAssign(void* target_ptr, SrcType&& value)
    {
        *reinterpret_cast<TargetType*>(target_ptr) = std::forward<SrcType>(value);
    }
    // helper: тип найден - конструирование (const T&)
    template <bool enable = true, typename SrcType, typename TargetType,
             typename std::enable_if<
                 !std::is_assignable<TargetType, SrcType>::value
                     && std::is_constructible<TargetType, SrcType>::value
                 , int>::type = 0>
    static void HelperAssign(void* target_ptr, const SrcType& value)
    {
        // placement new
        new (static_cast<void*>(target_ptr)) TargetType(value);
    }
    // helper: тип найден - конструирование (T&&)
    template <bool enable = true, typename SrcType, typename TargetType,
             typename std::enable_if<
                 !std::is_assignable<TargetType, SrcType>::value
                     && std::is_constructible<TargetType, SrcType>::value
                 , int>::type = 0>
    static void HelperAssign(void* target_ptr, SrcType&& value)
    {
        // placement new
        new (static_cast<void*>(target_ptr)) TargetType(std::forward<SrcType>(value));
    }
    //------------------------------------------
    // helper: тип НЕ найден (const T&)
    template <bool enable = false, typename SrcType, typename TargetType,
             typename std::enable_if<
                 !std::is_assignable<TargetType, SrcType>::value
                    && !std::is_constructible<TargetType, SrcType>::value
                 , int>::type = 0>
    static void HelperAssign(void* target_ptr, const SrcType&& value) { /* заглушка */ }
    // helper: тип НЕ найден (T&&)
    template <bool enable = false, typename SrcType, typename TargetType,
             typename std::enable_if<
                 !std::is_assignable<TargetType, SrcType>::value
                    && !std::is_constructible<TargetType, SrcType>::value
                 , int>::type = 0>
    static void HelperAssign(void* target_ptr, SrcType&& value) { /* заглушка */ }
    //---------------------------------------------------------------------------------

    // шаблон рекурсивного поиска для вычисления типа объекта other
    // NOTE: внешний код всегда вызывает начальную связку <true, 0>
    template <bool is_in_bounds, ssize_t OtherIndex>
    struct UniversalAssigner {
        // вариант, когда тип совпадает с искомым (const &)
        template <typename... OtherTypes>
        static void assign(const ssize_t& other_index, const Variant<OtherTypes...>& other, Variant<Types...>& dest_value)
        {
            using OtherVariant = Variant<OtherTypes...>;
            if(OtherIndex == other_index) {
                using OtherType   = typename tools::type_at_index<OtherIndex, OtherTypes...>::type;
                using TargetIndex = typename tools::index_of_type<OtherType, Types...>;

                using TargetType = std::conditional<
                    TargetIndex::is_found,
                    typename tools::type_at_index<(TargetIndex::is_found ? TargetIndex::value : 0), Types...>::type,
                    std::nullptr_t // заглушка
                    >;

                HelperAssign<TargetIndex::is_found, OtherType, TargetType>
                    (dest_value.m_data, other.template get<OtherType>());
            } else {
                // продолжение поиска
                static constexpr bool next_in_bounds = (OtherIndex + 1) < sizeof...(OtherTypes);
                UniversalAssigner<next_in_bounds, OtherIndex + 1>::assign(other_index, other, dest_value);
            }
        }

        // вариант, когда тип совпадает с искомым (&&)
        template <typename... OtherTypes>
        static void assign(const ssize_t& other_index, Variant<OtherTypes...>&& other, Variant<Types...>& dest_value)
        {
            using OtherVariant = Variant<OtherTypes...>;
            if(OtherIndex == other_index) {
                using OtherType   = typename tools::type_at_index<OtherIndex, OtherTypes...>::type;
                using TargetIndex = typename tools::index_of_type<OtherType, Types...>;

                using TargetType  = std::conditional<
                    TargetIndex::is_found,
                    typename tools::type_at_index<(TargetIndex::is_found ? TargetIndex::value : 0), Types...>::type,
                    std::nullptr_t // заглушка
                    >;

                HelperAssign<TargetIndex::is_found, OtherType, TargetType>
                    (dest_value.m_data, std::move(other.template get<OtherType>()));
            } else {
                // продолжение поиска
                static constexpr bool next_in_bounds = (OtherIndex + 1) < sizeof...(OtherTypes);
                UniversalAssigner<next_in_bounds, OtherIndex + 1>::assign(other_index, std::move(other), dest_value);
            }
        }
    };
    // шаблон для остановки рекурсии (вышли за границы списка типов)
    template <ssize_t OtherIndex>
    struct UniversalAssigner<false, OtherIndex> {
        template <typename... OtherTypes>
        static void assign(const ssize_t& other_index,
                           const Variant<OtherTypes...>& other,
                           Variant<Types...>& dest_value)
        { /*метод-заглушка*/ }

        template <typename... OtherTypes>
        static void assign(const ssize_t& other_index,
                           Variant<OtherTypes...>&& other,
                           Variant<Types...>& dest_value)
        { /*метод-заглушка*/ }
    };

public:
    //-----------------------------------------------------------------------------------------
    // константы класса времени компиляции
    //-----------------------------------------------------------------------------------------
    static constexpr size_t size       = tools::max_size_of_type<Types...>::size;
    static constexpr size_t align_size = tools::max_align_of_type<Types...>::align_size;
    //-----------------------------------------------------------------------------------------

    // по умолчанию проинициализируется первым типом (его значение по умолчанию)
    Variant() noexcept : m_current_type_index(0)
    {
        new (m_data) typename tools::type_at_index<0, Types...>::type({});
    }

    Variant(const Variant& other)
    {
        if(this != &other)
        {
            m_current_type_index = other.m_current_type_index;
            UniversalAssigner<true, 0>::assign(m_current_type_index, other, *this);
        }
    }

    Variant(Variant&& other)
    {
        if(this != &other)
        {
            m_current_type_index = other.m_current_type_index;
            UniversalAssigner<true, 0>::assign(m_current_type_index, std::move(other), *this);

            // уничтожаем индекс, чтобы Destroyer не делал лишних действий
            other.m_current_type_index = -1;
        }
    }

    template <typename... OtherTypes>
    Variant(const Variant<OtherTypes...>& other)
    {
        // NOTE: проверка if(this != &other) не нужна, т.к. типы заведомо разные по variadic
        UniversalAssigner<true, 0>::assign(m_current_type_index, other, *this);
    }

    template <typename... OtherTypes>
    Variant(Variant<OtherTypes...>&& other)
    {
        // NOTE: проверка if(this != &other) не нужна, т.к. типы заведомо разные по variadic
        UniversalAssigner<true, 0>::assign(m_current_type_index, std::move(other), *this);
        // уничтожаем индекс, чтобы Destroyer не делал лишние действия
        other.m_current_type_index = -1;
    }

    // NOTE: трюк с Dummy= и std::is_same<Dummy,> нужен для переноса проверки с момента создания объекта на момент вызова конкретного метода
    template <typename Dummy = std::nullptr_t,
             typename std::enable_if<std::is_same<Dummy, std::nullptr_t>::value
                                         && tools::index_of_type<std::nullptr_t, Types...>::is_found
                                     , int>::type = 0>
    Variant(std::nullptr_t) noexcept
    {
        using Index = typename tools::index_of_type<std::nullptr_t, Types...>;

        m_current_type_index = Index::value;
        Creator<0>::create(m_current_type_index, m_data, nullptr);
    }

    template <typename T,
             typename std::enable_if<!std::is_same<typename std::decay<T>::type, Variant>::value
                                         && tools::index_of_type<T, Types...>::is_found
                                     , int>::type = 0>
    Variant(const T& value)
    {
        using CleanT = typename std::decay<T>::type;
        using Index  = typename tools::index_of_type<CleanT, Types...>;

        m_current_type_index = Index::value;
        Creator<0>::create(m_current_type_index, m_data, value);
    }

    template <typename T,
             typename std::enable_if<!std::is_same<typename std::decay<T>::type, Variant>::value
                                         && tools::index_of_type<T, Types...>::is_found
                                     , int>::type = 0>
    Variant(T&& value)
    {
        using CleanT = typename std::decay<T>::type;
        using Index  = typename tools::index_of_type<CleanT, Types...>;

        m_current_type_index = Index::value;
        Creator<0>::create(m_current_type_index, m_data, std::forward<T>(value));
    }

    ~Variant()
    {
        Destroyer<0>::destroy(m_current_type_index, m_data);
    }

    Variant& operator=(const Variant& other)
    {
        if(this != &other)
        {
            // уничтожение старого объекта
            Destroyer<0>::destroy(m_current_type_index, m_data);

            m_current_type_index = other.m_current_type_index;
            UniversalAssigner<true, 0>::assign(m_current_type_index, other, *this);
        }

        return *this;
    }

    Variant& operator=(Variant&& other) noexcept
    {
        if(this != &other)
        {
            // уничтожение старого объекта
            Destroyer<0>::destroy(m_current_type_index, m_data);

            m_current_type_index = other.m_current_type_index;
            UniversalAssigner<true, 0>::assign(m_current_type_index, std::move(other), *this);

            // уничтожаем индекс, чтобы Destroyer не делал лишних действий
            other.m_current_type_index = -1;
        }

        return *this;
    }

    template <typename... OtherTypes>
    Variant& operator=(const Variant<OtherTypes...>& other)
    {
        // NOTE: проверка if(this != &other) не нужна, т.к. типы заведомо разные по variadic
        UniversalAssigner<true, 0>::assign(m_current_type_index, other, *this);

        return *this;
    }

    template <typename... OtherTypes>
    Variant& operator=(Variant<OtherTypes...>&& other) noexcept
    {
        // NOTE: проверка if(this != &other) не нужна, т.к. типы заведомо разные по variadic
        UniversalAssigner<true, 0>::assign(m_current_type_index, std::move(other), *this);
        // уничтожаем индекс, чтобы Destroyer не делал лишние действия
        other.m_current_type_index = -1;

        return *this;
    }

    // NOTE: трюк с Dummy= и std::is_same<Dummy,> нужен для переноса проверки с момента создания объекта на момент вызова конкретного метода
    template <typename Dummy = std::nullptr_t,
             typename std::enable_if<std::is_same<Dummy, std::nullptr_t>::value
                                         && tools::index_of_type<std::nullptr_t, Types...>::is_found
                                     , int>::type = 0>
    Variant& operator=(std::nullptr_t)
    {
        // уничтожение старого объекта
        Destroyer<0>::destroy(m_current_type_index, m_data);

        using Index          = typename tools::index_of_type<std::nullptr_t, Types...>;
        m_current_type_index = Index::value;
        Creator<0>::create(m_current_type_index, m_data, nullptr);

        return *this;
    }

    template <typename T,
             typename std::enable_if<!std::is_same<typename std::decay<T>::type, Variant>::value
                                         && tools::index_of_type<T, Types...>::is_found
                                     , int>::type = 0>
    Variant& operator=(const T& value)
    {
        using CleanT                              = typename std::decay<T>::type;
        static constexpr ssize_t input_type_index = tools::index_of_type<CleanT, Types...>::value;

        if(input_type_index == m_current_type_index) {
            // если новый тип совпал с текущим - применить сразу

            // запрещаем перезапись константного типа
            using CurrentType = typename tools::type_at_index<input_type_index, Types...>::type;
            static_assert(!std::is_const<CurrentType>::value,
                          "SimpleAPI: сannot assign a new value(const&) to a const alternative in class Variant<>");

            HelperAssign<true, T, CurrentType>(m_data, value);
        } else {
            // иначе - работа с памятью
            // уничтожение старого объекта
            Destroyer<0>::destroy(m_current_type_index, m_data);

            m_current_type_index = input_type_index;
            Creator<0>::create(m_current_type_index, m_data, value);
        }

        return *this;
    }

    template <typename T,
             typename std::enable_if<!std::is_same<typename std::decay<T>::type, Variant>::value
                                         && tools::index_of_type<T, Types...>::is_found
                                     , int>::type = 0>
    Variant& operator=(T&& value) {
        using CleanT                              = typename std::decay<T>::type;
        static constexpr ssize_t input_type_index = tools::index_of_type<CleanT, Types...>::value;

        if(input_type_index == m_current_type_index) {
            // если новый тип совпал с текущим - применить сразу

            // запрещаем перезапись константного типа
            using CurrentType = typename tools::type_at_index<input_type_index, Types...>::type;
            static_assert(!(std::is_const<CurrentType>::value && !std::is_pointer<CurrentType>::value),
                          "SimpleAPI: сannot assign a new value(&&) to a const alternative in class Variant<>");

            HelperAssign<true, T, CurrentType>(m_data, std::forward<T>(value));
        } else {
            // иначе - работа с памятью
            // уничтожение старого объекта
            Destroyer<0>::destroy(m_current_type_index, m_data);

            m_current_type_index = input_type_index;
            Creator<0>::create(m_current_type_index, m_data, std::forward<T>(value));
        }

        return *this;
    }

    template <typename T,
             typename std::enable_if<!std::is_same<typename std::decay<T>::type, Variant>::value
                                         && tools::index_of_type<T, Types...>::is_found
                                     , int>::type = 0>
    Variant& set(const T& value)
    {
        *this = value;
        return *this;
    }

    template <typename T,
             typename std::enable_if<!std::is_same<typename std::decay<T>::type, Variant>::value
                                         && tools::index_of_type<T, Types...>::is_found
                                     , int>::type = 0>
    Variant& set(T&& value)
    {
        *this = std::forward<T>(value);
        return *this;
    }

    // NOTE: трюк с Dummy= и std::is_same<Dummy,> нужен для переноса проверки с момента создания объекта на момент вызова конкретного метода
    template <typename Dummy = std::nullptr_t,
             typename std::enable_if<std::is_same<Dummy, std::nullptr_t>::value
                                         && tools::index_of_type<std::nullptr_t, Types...>::is_found
                                     , int>::type = 0>
    std::nullptr_t get()
    {
        return nullptr;
    }

    template <typename T,
             typename std::enable_if<tools::index_of_type<T, Types...>::is_found
                                     , int>::type = 0>
    T& get()
    {
        return *(reinterpret_cast<T*>(m_data));
    }

    template <typename T,
             typename std::enable_if<tools::index_of_type<T, Types...>::is_found
                                     , int>::type = 0>
    T get() const
    {
        return *(reinterpret_cast<const T*>(m_data));
    }

    // версия через получение указателя на значение (если тип существует)
    template <typename T,
             typename std::enable_if<tools::index_of_type<T, Types...>::is_found
                                     , int>::type = 0>
    T* get_if() noexcept
    {
        static constexpr ssize_t type_index = tools::index_of_type<T, Types...>::value;
        static constexpr bool    is_nullptr = std::is_same<T, std::nullptr_t>::value;

        if(type_index == m_current_type_index && !is_nullptr) {
            return reinterpret_cast<T*>(m_data);
        }
        return nullptr;
    }

    // версия через получение (const) указателя на значение (если тип существует)
    template <typename T,
             typename std::enable_if<tools::index_of_type<T, Types...>::is_found
                                     , int>::type = 0>
    const T* get_if() const noexcept
    {
        static constexpr ssize_t type_index = tools::index_of_type<T, Types...>::value;
        static constexpr bool    is_nullptr = std::is_same<T, std::nullptr_t>::value;

        if(type_index == m_current_type_index && !is_nullptr) {
            return reinterpret_cast<const T*>(m_data);
        }
        return nullptr;
    }

    /**
     * @brief index
     * @return Возвращает текущий индекс типа. Может быть -1 для не инициализированного значения.
     */
    ssize_t index() const noexcept
    {
        return m_current_type_index;
    }

private:
    ssize_t m_current_type_index;
    // хранилище значения; аналог union, но через placement new
    alignas(align_size) uint8_t m_data[size];
};

} // namespace simpleapi
