#pragma once

// NOTE: "IWYU pragma: keep" спрячет лишнее предупреждение от clangd
#include "base.h"          // IWYU pragma: keep
#include "type_checkers.h" // IWYU pragma: keep
#include <string>
#include <type_traits>
#include "../config/Config.h"


namespace simpleapi {
namespace tools {

// правило для определения мульти-типа - Variant<Types...>
template<typename T>
struct ConfigTypeTraits<T, typename std::enable_if<is_container_as_variant<T>::value>::type>
{
    template <typename... VariantTypes>
    struct Helper {

        //-------------------------------------------------------------------------------
        // Записыватель
        //-------------------------------------------------------------------------------
        template <ssize_t Index, bool is_in_bounds = (Index >= 0 && Index < sizeof...(VariantTypes))>
        struct ForVariant {
            static void Save(Config& config, const std::string& key, const Variant<VariantTypes...>& field) {
                if(Index == field.index()) {
                    using Type = typename type_at_index<Index, VariantTypes...>::type;
//                    int i = field.template get<Type>(); // <-- почему-то не срабатывает для Variant<int, std::string>, выбирается весь Variant
                    ConfigTypeTraits<Type>::save(config, key, field.template get<Type>());
                } else {
                    ForVariant<Index + 1>::Save(config, key, field);
                }
            }
        };
        //-------------------
        template <ssize_t Index>
        struct ForVariant<Index, false> {
            static void Save(Config& config, const std::string& key, const Variant<VariantTypes...>& field)
            { /*заглушка*/ }
        };
        //-------------------------------------------------------------------------------
    };
    //-----------------------------------------

    static bool load(const Config& config, const std::string& key, T& field)
    {
        // std::cout << "[debug] load container(as variant) key=\"" << key << "\"" << std::endl;

        // FIXME:
//        using Type = typename T::value_type;

//        if(config.isMapContainer() && config.containsKey(key)) {
//            const Config& ck = config[key];
//            field.clear();
//            size_t counter = 0;
//            for(const auto& c : ck.getRange()) {
//                // рекурсивно вызываем traits(признаки) для каждого из элементов
//                Type item_temp_value;

//                if(!Loader((*c.get()), item_temp_value)) {
//                    static_config_error_str += "inner loader for [" + key + "] failed\n";
//                    return false;
//                }

//                field.insert(item_temp_value);
//            }
//        }

        return true;
    }

    template<typename Lambda, typename... Args,
             typename std::enable_if<is_variadic_lambda_callable<Lambda, T&&, std::string&&>::value, int>::type = 0>
    static bool load(const Config& config, const std::string& key, T& field, Lambda lambda, Args&&... args)
    {
        // std::cout << "[debug] load container(as variant) key=\"" << key << "\"" << std::endl;

        // FIXME:
//        using Type = typename T::value_type;
//        T temp_value;

//        if(config.isMapContainer() && config.containsKey(key)) {
//            const Config& ck = config[key];
//            field.clear();
//            size_t counter = 0;
//            for(const auto& c : ck.getRange()) {
//                // рекурсивно вызываем traits(признаки) для каждого из элементов
//                Type item_temp_value;

//                if(!Loader((*c.get()), item_temp_value)) {
//                    static_config_error_str += "inner loader for [" + key + "] failed\n";
//                    return false;
//                }

//                temp_value.insert(item_temp_value);
//            }

//            if(!ExecuteValidator(lambda, temp_value, key)) {
//                static_config_error_str = "validate for [" + key + "] failed\n";
//                return false;
//            }

//            field = temp_value;
//        }

        return true;
    }

    // комментарии учитываются только при записи
    static void save(Config& config, const std::string& key, const T& field,
                     const std::string& prefix_comment = "",
                     const std::string& suffix_comment = "")
    {
        // std::cout << "[debug] save container(as variant) key=\"" << key << "\"" << std::endl;

        // сам вызовет сохранения по текущему типу Variant<>
        Helper<T>::template ForVariant<0>::Save(config, key, field);
        config[key].setComment(prefix_comment, suffix_comment);
    }

    static bool compare(const T& field, const T& other)
    {
        return utils::CompareIfPossible(field, other);
    }
};

} // namespace tools
} // namespace simpleapi
