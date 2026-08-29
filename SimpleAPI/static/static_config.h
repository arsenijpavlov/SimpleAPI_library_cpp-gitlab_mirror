#pragma once

/** @file static_config.h
 * @if LANG_RU
 *      @details Хотя в данном файле есть только описание макросов, блок include всё равно остаётся, для удобства подключения
 *      этого файла в итоговой програамме, использующей SimpleAPI
 * @endif
 * @if LANG_EN
 *      @details Although this file contains only macro definitions, the include block is retained to make it easier to
 *      include the file in the final program using SimpleAPI
 * @endif
*/

// NOTE: "IWYU pragma: keep" спрячет лишнее предупреждение от clangd
#include "../config/Config.h"      // IWYU pragma: keep
#include "type_checkers.h"         // IWYU pragma: keep
#include "simple_types.h"          // IWYU pragma: keep
#include "struct_types.h"          // IWYU pragma: keep
#include "index_container_types.h" // IWYU pragma: keep
#include "map_container_types.h"   // IWYU pragma: keep
#include <functional>


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
    if(load_conf.isMapContainer() && load_conf.containsKey(#name)) {                         \
        if(!simpleapi::tools::ConfigTypeTraits<type>::load(load_conf, #name, name))          \
            return false;                                                                    \
    }
#define SAPI_LOAD_FIELD_4(type, name, default_value, lambda)                                 \
    if(load_conf.isMapContainer() && load_conf.containsKey(#name)) {                         \
        if(!simpleapi::tools::ConfigTypeTraits<type>::load(load_conf, #name, name, lambda))  \
            return false;                                                                    \
    }
#define SAPI_LOAD_FIELD_5(type, name, default_value, lambda, prefix_comment)                 \
    if(load_conf.isMapContainer() && load_conf.containsKey(#name)) {                         \
        if(!simpleapi::tools::ConfigTypeTraits<type>::load(load_conf, #name, name, lambda))  \
            return false;                                                                    \
    }
#define SAPI_LOAD_FIELD_6(type, name, default_value, lambda, prefix_comment, suffix_comment) \
    if(load_conf.isMapContainer() && load_conf.containsKey(#name)) {                         \
        if(!simpleapi::tools::ConfigTypeTraits<type>::load(load_conf, #name, name, lambda))  \
            return false;                                                                    \
    }
// обёртка
#define SAPI_LOAD_FIELD(...) \
    SAPI_GETTER_MACRO(__VA_ARGS__, SAPI_LOAD_FIELD_6, SAPI_LOAD_FIELD_5, SAPI_LOAD_FIELD_4, SAPI_LOAD_FIELD_3)(__VA_ARGS__)

// записыватели
#define SAPI_SAVE_FIELD_3(type, name, default_value)                                                                    \
    std::cout << "save() config type " << ToString(save_conf.getType()) << ", key: \"" << #name << "\"" << std::endl;   \
    simpleapi::tools::ConfigTypeTraits<type>::save(save_conf, #name, name);
#define SAPI_SAVE_FIELD_4(type, name, default_value, lambda)                                                            \
    std::cout << "save() config type " << ToString(save_conf.getType()) << ", key: \"" << #name << "\"" << std::endl;   \
    simpleapi::tools::ConfigTypeTraits<type>::save(save_conf, #name, name);
#define SAPI_SAVE_FIELD_5(type, name, default_value, lambda, prefix_comment)                                            \
    std::cout << "save() config type " << ToString(save_conf.getType()) << ", key: \"" << #name << "\"" << std::endl;   \
    simpleapi::tools::ConfigTypeTraits<type>::save(save_conf, #name, name, prefix_comment);
#define SAPI_SAVE_FIELD_6(type, name, default_value, lambda, prefix_comment, suffix_comment)                            \
    std::cout << "save() config type " << ToString(save_conf.getType()) << ", key: \"" << #name << "\"" << std::endl;   \
    simpleapi::tools::ConfigTypeTraits<type>::save(save_conf, #name, name, prefix_comment, suffix_comment);
// обёртка
#define SAPI_SAVE_FIELD(...) \
    SAPI_GETTER_MACRO(__VA_ARGS__, SAPI_SAVE_FIELD_6, SAPI_SAVE_FIELD_5, SAPI_SAVE_FIELD_4, SAPI_SAVE_FIELD_3)(__VA_ARGS__)

/* Ограничения:
 * - базовая структура не может состоять из примитива Config,
 *      то есть пользователю нужно использовать стандартное "val = config;" без преобразований
 * - Config верхнего уровня обязан состоять из объекта Config, который сам по себе является ассоциативным массивом
*/
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
