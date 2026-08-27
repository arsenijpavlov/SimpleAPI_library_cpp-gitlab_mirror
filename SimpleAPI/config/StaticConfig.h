#pragma once

#include "Config.h"

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

// базовое объявление шаблона (сформирует ошибку компиляции для типов, которые не объявлены)
template<typename T, typename Enable = void>
struct ConfigTypeTraits;

// правило для определения структур
template<typename T>
struct ConfigTypeTraits<T, typename std::enable_if<is_config_struct<T>::value
                                                   && !is_container<T>::value>::type>
{
    static void load(const Config& config, const std::string& key, T& field)
    {
        field.loadConfig(config[key]);
    }

    // комментарии учитываются только при записи
    static void save(Config& config, const std::string& key, const T& field,
                     const std::string& prefix_comment, const std::string& suffix_comment)
    {
        config[key] = field.saveConfig();
        config[key].setComment(prefix_comment, suffix_comment);
    }
    static void save(Config& config, const T& field,
                     const std::string& prefix_comment, const std::string& suffix_comment)
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
    static void load(const Config& config, const std::string& key, T& field)
    {
        using Type = typename T::value_type;
        field.clear();

        if(config.containsKey(key)) {
            const Config& inner_cfg = config[key];
            for(const auto& item : inner_cfg.getRange()) {
                // рекурсивно вызываем traits(признаки) для каждого из элементов
                Type item_value;
                ConfigTypeTraits<Type>::load((*item.get()), "", item_value);

                field.push_back(item_value);
            }
        }
    }

    // комментарии учитываются только при записи
    static void save(Config& config, const std::string& key, const T& field,
                     const std::string& prefix_comment, const std::string& suffix_comment)
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
    static void load(const Config& config, const std::string& key, T& field)
    {
//        if(!key.empty())
            field = config[key].get<T>();
//        else
//            field = config.get<T>();
    }

    // комментарии учитываются только при записи
    static void save(Config& config, const std::string& key, const T& field,
                     const std::string& prefix_comment, const std::string& suffix_comment)
    {
//        if(!key.empty())
//        {
            config[key] = field;
            config[key].setComment(prefix_comment, suffix_comment);
//        }
//        else
//        {
//            config = field;
//            config.setComment(prefix_comment, suffix_comment);
//        }
    }
};


// NOTE: приставка SAPI_ - сокращение от SimpleAPI_ для избежания совпадений с пользовательским пространством

// макрос-счётчик аргументов в других макросах
#define SAPI_GETTER_MACRO(_1, _2, _3, _4, _5, NAME, ...) NAME

// объявления
#define SAPI_DECLARE_FIELD_3(type, name, default_value) \
    type name;
#define SAPI_DECLARE_FIELD_4(type, name, default_value, prefix_comment) \
    type name;
#define SAPI_DECLARE_FIELD_5(type, name, default_value, prefix_comment, suffix_comment) \
    type name;
// обёртка
#define SAPI_DECLARE_FIELD(...) \
    SAPI_GETTER_MACRO(__VA_ARGS__, SAPI_DECLARE_FIELD_5, SAPI_DECLARE_FIELD_4, SAPI_DECLARE_FIELD_3)(__VA_ARGS__)

// инициализаторы
#define SAPI_INIT_FIELD_3(type, name, default_value) \
    name = default_value;
#define SAPI_INIT_FIELD_4(type, name, default_value, prefix_comment) \
    name = default_value;
#define SAPI_INIT_FIELD_5(type, name, default_value, prefix_comment, suffix_comment) \
    name = default_value;
// обёртка
#define SAPI_INIT_FIELD(...) \
    SAPI_GETTER_MACRO(__VA_ARGS__, SAPI_INIT_FIELD_5, SAPI_INIT_FIELD_4, SAPI_INIT_FIELD_3)(__VA_ARGS__)

// загрузчики
#define SAPI_LOAD_FIELD_3(type, name, default_value)                                 \
    if(load_conf.containsKey(#name)) {                                               \
        simpleapi::tools::ConfigTypeTraits<type>::load(load_conf, #name, name);      \
    }
#define SAPI_LOAD_FIELD_4(type, name, default_value, prefix_comment)                 \
    if(load_conf.containsKey(#name)) {                                               \
        simpleapi::tools::ConfigTypeTraits<type>::load(load_conf, #name, name);      \
    }
#define SAPI_LOAD_FIELD_5(type, name, default_value, prefix_comment, suffix_comment) \
    if(load_conf.containsKey(#name)) {                                               \
        simpleapi::tools::ConfigTypeTraits<type>::load(load_conf, #name, name);      \
    }
// обёртка
#define SAPI_LOAD_FIELD(...) \
    SAPI_GETTER_MACRO(__VA_ARGS__, SAPI_LOAD_FIELD_5, SAPI_LOAD_FIELD_4, SAPI_LOAD_FIELD_3)(__VA_ARGS__)

// записыватели
#define SAPI_SAVE_FIELD_3(type, name, default_value)                                                       \
    simpleapi::tools::ConfigTypeTraits<type>::save(save_conf, #name, name, "", "");
#define SAPI_SAVE_FIELD_4(type, name, default_value, prefix_comment)                                       \
    simpleapi::tools::ConfigTypeTraits<type>::save(save_conf, #name, name, prefix_comment, "");
#define SAPI_SAVE_FIELD_5(type, name, default_value, prefix_comment, suffix_comment)                       \
    simpleapi::tools::ConfigTypeTraits<type>::save(save_conf, #name, name, prefix_comment, suffix_comment);
// обёртка
#define SAPI_SAVE_FIELD(...) \
    SAPI_GETTER_MACRO(__VA_ARGS__, SAPI_SAVE_FIELD_5, SAPI_SAVE_FIELD_4, SAPI_SAVE_FIELD_3)(__VA_ARGS__)

#define SAPI_REGISTER_CONFIG(StructName, SAPI_FIELDS_MACRO)     \
    struct StructName {                                         \
        SAPI_FIELDS_MACRO(SAPI_DECLARE_FIELD)                   \
                                                                \
        StructName() {                                          \
            SAPI_FIELDS_MACRO(SAPI_INIT_FIELD)                  \
        }                                                       \
                                                                \
        void loadConfig(const simpleapi::Config& load_conf) {   \
            SAPI_FIELDS_MACRO(SAPI_LOAD_FIELD)                  \
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
//        X(uint8_t, u_integer, 3)   \
//        X(std::string, str, "asd")
//
//    SAPI_REGISTER_CONFIG(TestConfig, STRUCT_FIELDS)
// EXAMPLE OF USAGE -------------------------------------------

} // namespace tools
} // namespace simpleapi
