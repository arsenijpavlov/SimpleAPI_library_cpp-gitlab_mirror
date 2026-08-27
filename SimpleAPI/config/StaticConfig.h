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

    static void save(Config& config, const std::string& key, const T& field)
    {
        config[key] = field.saveConfig();
    }
};

// правило для определения типов-контейнеров (vector, queue и т.д.)
// std::string не считать за контейнер
template<typename T>
struct ConfigTypeTraits<T, typename std::enable_if<is_container<T>::value>::type>
{
    static void load(const Config& config, const std::string& key, T& field)
    {
        field.clear();

        if(config.containsKey(key)) {
            const Config& inner_cfg = config[key];
            for(const auto& item : inner_cfg.getRange()) {
                using Type = typename T::value_type;
                // рекурсивно вызываем traits(признаки) для каждого из элементов
                Type item_value;
                ConfigTypeTraits<Type>::load((*item.get()), "", item_value);

                field.push_back(item_value);
            }
        }
    }

    static void save(Config& config, const std::string& key, const T& field)
    {
        size_t index = 0;
        for(const auto& item : field) {
            using Type = typename T::value_type;

            Config temp;
            ConfigTypeTraits<Type>::save(temp, "", item);
            config[key].push_back(temp);
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
        if(!key.empty())
            field = config[key].get<T>();
        else
            field = config.get<T>();
    }

    static void save(Config& config, const std::string& key, const T& field)
    {
        if(!key.empty())
            config[key] = field;
        else
            config = field;
    }
};

#define DECLARE_FIELD(type, name, default_value) \
    type name;

#define INIT_FIELD(type, name, default_value) \
    this->name = default_value;

#define LOAD_FIELD(type, name, default_value)                                         \
    if(load_conf.containsKey(#name)) {                                                \
        simpleapi::tools::ConfigTypeTraits<type>::load(load_conf, #name, this->name); \
    }

#define SAVE_FIELD(type, name, default_value) \
    simpleapi::tools::ConfigTypeTraits<type>::save(save_conf, #name, this->name);

#define REGISTER_CONFIG(StructName, FIELDS_MACRO)               \
    struct StructName {                                         \
        FIELDS_MACRO(DECLARE_FIELD)                             \
                                                                \
        StructName() {                                          \
            FIELDS_MACRO(INIT_FIELD)                            \
        }                                                       \
                                                                \
        void loadConfig(const simpleapi::Config& load_conf) {   \
            FIELDS_MACRO(LOAD_FIELD)                            \
        }                                                       \
                                                                \
        simpleapi::Config saveConfig() const {                  \
            simpleapi::Config save_conf;                        \
            FIELDS_MACRO(SAVE_FIELD)                            \
            return save_conf;                                   \
        }                                                       \
    };


// EXAMPLE OF USAGE -------------------------------------------
//    #define STRUCT_FIELDS(X)       \
//        X(int, integer, 2)         \
//        X(uint8_t, u_integer, 3)   \
//        X(std::string, str, "asd")
//
//    REGISTER_CONFIG(TestConfig, STRUCT_FIELDS)
// EXAMPLE OF USAGE -------------------------------------------

} // namespace tools
} // namespace simpleapi
