#pragma once

// NOTE: "IWYU pragma: keep" спрячет лишнее предупреждение от clangd
#include "base.h"          // IWYU pragma: keep
#include "../meta/meta_same_types_validators.h"
#include "../meta/meta_lambda_validators.h"
#include "../meta/lambda_executers.h"
#include <string>
#include <type_traits>
#include "../config/Config.h"


namespace simpleapi {
namespace tools {


// правило для определения мульти-типа - Variant<Types...>
template<typename T>
struct ConfigTypeTraits<T, typename std::enable_if<meta::is_container_as_variant<T>::value>::type>
{
    // предобъявление для SFINAE
    template <typename... VariantTypes>
    struct Helper;
    // основной класс хелпера
    template <typename... VariantTypes>
    struct Helper<Variant<VariantTypes...>> {
        template <ssize_t Index, bool is_in_bounds = (Index >= 0 && Index < sizeof...(VariantTypes))>
        struct Runtime {
            //-------------------------------------------------------------------------------
            // Читатель
            //-------------------------------------------------------------------------------
            static bool Load(const Config& config, const std::string& key, Variant<VariantTypes...>& field) {
                using Type  = typename meta::type_at_index<Index, VariantTypes...>::type;
                Type temp;
                bool result = ConfigTypeTraits<Type>::load(config, key, temp);
                if(result) {
                    // если успешно прочитали значение, то выходим из рекурсии
                    field = temp;
                    return true;
                }

                // TODO: если все false, то нужно накапливать лог ошибки - вернуть первое вхождение
                return Runtime<Index + 1>::Load(config, key, field);
            }

            //-------------------------------------------------------------------------------
            // Записыватель
            //-------------------------------------------------------------------------------
            static void Save(Config& config, const std::string& key, const Variant<VariantTypes...>& field) {
                if(Index == field.index()) {
                    using Type = typename meta::type_at_index<Index, VariantTypes...>::type;
                    ConfigTypeTraits<Type>::save(config, key, field.template get<Type>());
                } else {
                    Runtime<Index + 1>::Save(config, key, field);
                }
            }
            //-------------------------------------------------------------------------------
        };

        //-----------------------------------------
        template <ssize_t Index>
        struct Runtime<Index, false> {
            static bool Load(const Config& config, const std::string& key, Variant<VariantTypes...>& field)
            { /*заглушка*/ return true; }
            static void Save(Config& config, const std::string& key, const Variant<VariantTypes...>& field)
            { /*заглушка*/ }
        };
    };
    //-------------------------------------------------------------------------------

    static bool load(const Config& config, const std::string& key, T& field)
    {
        // std::cout << "[debug] load container(as variant) key=\"" << key << "\"" << std::endl;

        // сам вызовет загрузчик для чтения любого из типов Variant<>
        return Helper<T>::template Runtime<0>::Load(config, key, field);
    }

    template<typename Lambda, typename... Args,
             typename std::enable_if<meta::is_variadic_lambda_callable<Lambda, T&&, std::string&&>::value, int>::type = 0>
    static bool load(const Config& config, const std::string& key, T& field, Lambda lambda, Args&&... args)
    {
        // std::cout << "[debug] load container(as variant) key=\"" << key << "\"" << std::endl;

        if(config.isMapContainer() && config.containsKey(key)) {
            T temp_value;
            // сам вызовет загрузчик для чтения любого из типов Variant<>
            if(!Helper<T>::template Runtime<0>::Load(config, key, temp_value))
                return false;

            if(!ExecuteValidator(lambda, temp_value, key)) {
                static_config_error_str = "validate for [" + key + "] failed\n";
                return false;
            }

            field = temp_value;
        }

        return true;
    }

    // комментарии учитываются только при записи
    static void save(Config& config, const std::string& key, const T& field,
                     const std::string& prefix_comment = "",
                     const std::string& suffix_comment = "")
    {
        // std::cout << "[debug] save container(as variant) key=\"" << key << "\"" << std::endl;

        // сам вызовет сохранения по текущему типу Variant<>
        Helper<T>::template Runtime<0>::Save(config, key, field);
        config[key].setComment(prefix_comment, suffix_comment);
    }

    static bool compare(const T& field, const T& other)
    {
        return utils::CompareIfPossible(field, other);
    }
};

} // namespace tools
} // namespace simpleapi
