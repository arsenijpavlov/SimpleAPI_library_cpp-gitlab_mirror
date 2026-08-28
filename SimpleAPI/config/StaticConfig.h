#pragma once

#include "Config.h"
#include <functional>

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

// проверка: является ли тип контейнером (имеет метод begin())
//   получим ::value == false для SFINAE, если указанных методов не существует (нельзя вызвать)
template <typename T>
class is_container {
    // очистка типа от const и ссылок
    using CleanT = typename std::decay<T>::type;

    // метод для SFINAE проверки наличия метода begin() у класса U
    template <typename U> static char test(decltype(std::declval<U>().begin())*);
    // метод для разрешения конфликта для поля value
    template <typename U> static long test(...);
public:
    static const bool value = (sizeof(test<CleanT>(0)) == sizeof(char))
                              && !std::is_same<CleanT, std::string>::value;
};

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

// базовое объявление шаблона (сформирует ошибку компиляции для типов, которые не объявлены)
template<typename T, typename Enable = void>
struct ConfigTypeTraits;

// правило для определения структур
template<typename T>
struct ConfigTypeTraits<T, typename std::enable_if<is_config_struct<T>::value
                                                   && !is_container<T>::value>::type>
{
    static bool load(const Config& config, const std::string& key, T& field)
    {
        std::cout << "load struct" << std::endl;
        std::cout << "\tconfig[" << key << "] type " << ToString(config[key].getType()) << std::endl;

        field.loadConfig(config[key]);
        return true;
    }

    template<typename Lambda, typename... Args,
             typename std::enable_if<is_variadic_lambda_callable<Lambda, const T&, Args...>::value, int>::type = 0>
    static bool load(const Config& config, const std::string& key, T& field, Lambda lambda, Args&&... args)
    {
        std::cout << "load struct" << std::endl;
        std::cout << "\tconfig[" << key << "] type " << ToString(config[key].getType()) << std::endl;

        T temp_value;
        bool ret = temp_value.loadConfig(config[key]);
        if(ret && lambda(field, std::forward<Args>(args)...))
        {
            field = temp_value;
            return true;
        }

        return false;
    }

    // комментарии учитываются только при записи
    static void save(Config& config, const std::string& key, const T& field,
                     const std::string& prefix_comment = "",
                     const std::string& suffix_comment = "")
    {
        config[key] = field.saveConfig();
        config[key].setComment(prefix_comment, suffix_comment);
    }
    static void save(Config& config, const T& field,
                     const std::string& prefix_comment = "",
                     const std::string& suffix_comment = "")
    {
        config = field.saveConfig();
        config.setComment(prefix_comment, suffix_comment);
    }
};

// правило для определения типов-контейнеров (vector, queue и т.д.)
// std::string не считается за контейнер и обрабатывается как цельный элемент
template<typename T>
struct ConfigTypeTraits<T, typename std::enable_if<is_container<T>::value>::type>
{
    static bool load(const Config& config, const std::string& key, T& field)
    {
        std::cout << "load container" << std::endl;
        std::cout << "\tconfig1[" << key << "] type " << ToString(config[key].getType()) << std::endl;

        using Type = typename T::value_type;
        field.clear();

        for(const auto& item : config[key].getRange()) {
            // рекурсивно вызываем traits(признаки) для каждого из элементов
            Type item_value;
            if(!ConfigTypeTraits<Type>::load((*item.get()), "", item_value))
                return false;

            field.push_back(item_value);
        }

        return true;
    }

    template<typename Lambda, typename... Args,
             typename std::enable_if<is_variadic_lambda_callable<Lambda, const T&, Args...>::value, int>::type = 0>
    static bool load(const Config& config, const std::string& key, T& field, Lambda lambda, Args&&... args)
    {
        std::cout << "load container" << std::endl;
        std::cout << "\tconfig2[" << key << "] type " << ToString(config[key].getType()) << std::endl;

        using Type = typename T::value_type;
        field.clear();

        T temp_value;
        for(const auto& item : config[key].getRange()) {
            // рекурсивно вызываем traits(признаки) для каждого из элементов
            Type item_value;
            if(!ConfigTypeTraits<Type>::load((*item.get()), "", item_value))
                return false;

            temp_value.push_back(item_value);
        }

        if(lambda(temp_value, std::forward<Args>(args)...))
        {
            field = temp_value;
            return true;
        }
        return false;
    }

    // комментарии учитываются только при записи
    static void save(Config& config, const std::string& key, const T& field,
                     const std::string& prefix_comment = "",
                     const std::string& suffix_comment = "")
    {
        using Type = typename T::value_type;
        Config temp;
        for(const auto& item : field) {
            // ключ дальше нельзя передатть - создаст новую вложенность
            ConfigTypeTraits<Type>::save(temp, item, "", "");
            config[key].push_back(temp);
            config[key].setComment(prefix_comment, suffix_comment);
        }
    }
};

// правило для определения одиночных типов
template<typename T>
struct ConfigTypeTraits<T, typename std::enable_if<!is_config_struct<T>::value
                                                   && !is_container<T>::value>::type>
{
    static bool load(const Config& config, const std::string& key, T& field)
    {
        field = config[key].get<T>();
        return true;
    }

    template<typename Lambda, typename... Args,
             typename std::enable_if<is_variadic_lambda_callable<Lambda, const T&, Args...>::value, int>::type = 0>
    static bool load(const Config& config, const std::string& key, T& field, Lambda lambda, Args&&... args)
    {
        T temp_value = config[key].get<T>();

        if(lambda(field, std::forward<Args>(args)...))
        {
            field = temp_value;
            return true;
        }
        return false;
    }

    // комментарии учитываются только при записи
    static void save(Config& config, const std::string& key, const T& field,
                     const std::string& prefix_comment = "",
                     const std::string& suffix_comment = "")
    {
            config[key] = field;
            config[key].setComment(prefix_comment, suffix_comment);
    }
    static void save(Config& config, const T& field,
                     const std::string& prefix_comment = "",
                     const std::string& suffix_comment = "")
    {
            config = field;
            config.setComment(prefix_comment, suffix_comment);
    }
};


// NOTE: приставка SAPI_ - сокращение от SimpleAPI_ для избежания совпадений с пользовательским пространством

// макрос-счётчик аргументов в других макросах
#define SAPI_GETTER_MACRO(_1, _2, _3, _4, _5, _6, NAME, ...) NAME

// объявления
#define SAPI_DECLARE_FIELD_3(type, name, default_value)                                         \
    type name;
#define SAPI_DECLARE_FIELD_4(type, name, default_value, lambda)                                 \
    type name;
#define SAPI_DECLARE_FIELD_5(type, name, default_value, lambda, prefix_comment)                 \
    type name;
#define SAPI_DECLARE_FIELD_6(type, name, default_value, lambda, prefix_comment, suffix_comment) \
    type name;
// обёртка
#define SAPI_DECLARE_FIELD(...) \
    SAPI_GETTER_MACRO(__VA_ARGS__, SAPI_DECLARE_FIELD_6, SAPI_DECLARE_FIELD_5, SAPI_DECLARE_FIELD_4, SAPI_DECLARE_FIELD_3)(__VA_ARGS__)

// инициализаторы
#define SAPI_INIT_FIELD_3(type, name, default_value)                                         \
    name = default_value;
#define SAPI_INIT_FIELD_4(type, name, default_value, lambda)                                 \
    name = default_value;
#define SAPI_INIT_FIELD_5(type, name, default_value, lambda, prefix_comment)                 \
    name = default_value;
#define SAPI_INIT_FIELD_6(type, name, default_value, lambda, prefix_comment, suffix_comment) \
    name = default_value;
// обёртка
#define SAPI_INIT_FIELD(...) \
    SAPI_GETTER_MACRO(__VA_ARGS__, SAPI_INIT_FIELD_6, SAPI_INIT_FIELD_5, SAPI_INIT_FIELD_4, SAPI_INIT_FIELD_3)(__VA_ARGS__)

// загрузчики
#define SAPI_LOAD_FIELD_3(type, name, default_value)                                         \
    if(load_conf.containsKey(#name)) {                                                       \
        if(!simpleapi::tools::ConfigTypeTraits<type>::load(load_conf, #name, name))          \
            return false;                                                                    \
    } else {                                                                                 \
        std::cout << "not found \"" << #name << "\"" << std::endl;                           \
    }
#define SAPI_LOAD_FIELD_4(type, name, default_value, lambda)                                 \
    if(load_conf.containsKey(#name)) {                                                       \
        if(!simpleapi::tools::ConfigTypeTraits<type>::load(load_conf, #name, name, lambda))  \
            return false;                                                                    \
    } else {                                                                                 \
        std::cout << "not found \"" << #name << "\"" << std::endl;                           \
    }
#define SAPI_LOAD_FIELD_5(type, name, default_value, lambda, prefix_comment)                 \
    if(load_conf.containsKey(#name)) {                                                       \
        if(!simpleapi::tools::ConfigTypeTraits<type>::load(load_conf, #name, name, lambda))  \
            return false;                                                                    \
    } else {                                                                                 \
        std::cout << "not found \"" << #name << "\"" << std::endl;                           \
    }
#define SAPI_LOAD_FIELD_6(type, name, default_value, lambda, prefix_comment, suffix_comment) \
    if(load_conf.containsKey(#name)) {                                                       \
        if(!simpleapi::tools::ConfigTypeTraits<type>::load(load_conf, #name, name, lambda))  \
            return false;                                                                    \
    } else {                                                                                 \
        std::cout << "not found \"" << #name << "\"" << std::endl;                           \
    }
// обёртка
#define SAPI_LOAD_FIELD(...) \
    SAPI_GETTER_MACRO(__VA_ARGS__, SAPI_LOAD_FIELD_6, SAPI_LOAD_FIELD_5, SAPI_LOAD_FIELD_4, SAPI_LOAD_FIELD_3)(__VA_ARGS__)

// записыватели
#define SAPI_SAVE_FIELD_3(type, name, default_value)                                                               \
    simpleapi::tools::ConfigTypeTraits<type>::save(save_conf, #name, name);
#define SAPI_SAVE_FIELD_4(type, name, default_value, lambda)                                                       \
    simpleapi::tools::ConfigTypeTraits<type>::save(save_conf, #name, name);
#define SAPI_SAVE_FIELD_5(type, name, default_value, lambda, prefix_comment)                                       \
    simpleapi::tools::ConfigTypeTraits<type>::save(save_conf, #name, name, prefix_comment);
#define SAPI_SAVE_FIELD_6(type, name, default_value, lambda, prefix_comment, suffix_comment)                       \
    simpleapi::tools::ConfigTypeTraits<type>::save(save_conf, #name, name, prefix_comment, suffix_comment);
// обёртка
#define SAPI_SAVE_FIELD(...) \
    SAPI_GETTER_MACRO(__VA_ARGS__, SAPI_SAVE_FIELD_6, SAPI_SAVE_FIELD_5, SAPI_SAVE_FIELD_4, SAPI_SAVE_FIELD_3)(__VA_ARGS__)

#define SAPI_REGISTER_CONFIG(StructName, SAPI_FIELDS_MACRO)     \
    struct StructName {                                         \
        SAPI_FIELDS_MACRO(SAPI_DECLARE_FIELD)                   \
                                                                \
        StructName() {                                          \
            SAPI_FIELDS_MACRO(SAPI_INIT_FIELD)                  \
        }                                                       \
                                                                \
        bool loadConfig(const simpleapi::Config& load_conf) {   \
            SAPI_FIELDS_MACRO(SAPI_LOAD_FIELD)                  \
            return true;                                        \
        }                                                       \
                                                                \
        simpleapi::Config saveConfig() const {                  \
            simpleapi::Config save_conf;                        \
            SAPI_FIELDS_MACRO(SAPI_SAVE_FIELD)                  \
            return save_conf;                                   \
        }                                                       \
    };

// EXAMPLE OF USAGE -------------------------------------------
//    #define STRUCT_FIELDS(X)       \
//        X(int, integer, 2)         \
//        X(uint8_t, u_integer, 3, []{ return (u_integer != 5); }) \
//        X(std::string, str, "asd")
//
//    SAPI_REGISTER_CONFIG(TestConfig, STRUCT_FIELDS)
// EXAMPLE OF USAGE -------------------------------------------

} // namespace tools
} // namespace simpleapi
