#pragma once

#include "Config.h"

namespace simpleapi {
namespace tools {

// проверка: тип одиночный или список значений
//   получим ::value == false для SFINAE, если указанных методов не существует (нельзя вызвать)
template <typename T>
class is_config_struct {
    // метод для SFINAE проверки наличия метода loadConfig у класса U
    template <typename U> static char test(decltype(&U::loadConfig()));
    // метод для разрешения конфликта для поля value
    template <typename U> static long test(...);
public:
    static const bool value = sizeof(test<T>(0)) == sizeof(char);
};

// проверка: является ли тип контейнером (имеет метод begin())
//   получим ::value == false для SFINAE, если указанных методов не существует (нельзя вызвать)
template <typename T>
class is_container {
    // метод для SFINAE проверки наличия метода loadConfig у класса U
    template <typename U> static char test(decltype(std::declval<U>().begin()));
    // метод для разрешения конфликта для поля value
    template <typename U> static long test(...);
public:
    static const bool value = sizeof(test<T>(0)) == sizeof(char);
};

// базовое объявление шаблона (сформирует ошибку компиляции для типов, которые не объявлены)
template<typename T, typename Enable = void>
struct ConfigTypeTraits;

// правило для определения структур
template<typename T>
struct ConfigTypeTraits<T, typename std::enable_if<is_config_struct<T>::value>::type> {
    static void load()
    {
        std::cout << "(load) is struct: " << typeid(T).name() << std::endl;
    }

    static Config save()
    {
        std::cout << "(save) is struct: " << typeid(T).name() << std::endl;
    }
};

// правило для определения типов-контейнеров
template<typename T>
struct ConfigTypeTraits<T, typename std::enable_if<is_container<T>::value>::type> {
    static void load()
    {
        std::cout << "(load) is container: " << typeid(T).name() << std::endl;
    }

    static void save()
    {
        std::cout << "(save) is container: " << typeid(T).name() << std::endl;
    }
};

// правило для определения одиночных типов
template<typename T>
struct ConfigTypeTraits<T, typename std::enable_if<!is_config_struct<T>::value>::type> {
    static void load(const Config& config, const std::string& key, T& field)
    {
        field = config.get<T>(key);
    }

    static void save(Config& config, const std::string& key, const T& field)
    {
        config[key] = field;
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

} // namespace tools
} // namespace simpleapi
