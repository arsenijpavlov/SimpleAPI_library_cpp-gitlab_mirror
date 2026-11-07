#pragma once

// NOTE: "IWYU pragma: keep" спрячет лишнее предупреждение от clangd
#include "base.h"                               // IWYU pragma: keep
#include "../meta/meta_same_types_validators.h"
#include "../meta/meta_lambda_validators.h"
#include "../meta/lambda_executers.h"
#include <type_traits>
#include "../config/Config.h"


namespace simpleapi {
namespace tools {

// правило для определения одиночных типов
template<typename T>
struct ConfigTypeTraits<T, typename std::enable_if<!meta::is_config_struct<T>::value
                                                   && !meta::is_container_as_vector<T>::value
                                                   && !meta::is_container_as_set<T>::value
                                                   && !meta::is_container_as_unordered_set<T>::value
                                                   && !meta::is_container_as_array<T>::value
                                                   && !meta::is_container_as_queue<T>::value
                                                   && !meta::is_container_as_priority_queue<T>::value
                                                   && !meta::is_container_as_stack<T>::value
                                                   && !meta::is_container_as_bitset<T>::value
                                                   && !meta::is_container_as_map<T>::value
                                                   && !meta::is_container_as_unordered_map<T>::value
                                                   && !meta::is_container_as_variant<T>::value
                                                   && !std::is_enum<T>::value
                                                   && !meta::is_pair<T>::value
                                                   && !std::is_same<T, Config>::value
                                                   && !meta::is_optional<T>::value
                                                   && !std::is_same<T, std::wstring>::value
                                                   && !std::is_same<T, std::nullptr_t>::value
                                                   >::type>
{
    static bool load(const Config& config, const std::string& key, T& field)
    {
        // std::cout << "[debug] load simple key=\"" << key << "\"" << std::endl;

        if(config.isMapContainer() && config.containsKey(key)) {
            if(!config.is_can_get<T>(key))
                return false;

            field = config.get<T>(key);
        }

        return true;
    }

    template<typename Lambda, typename... Args,
             typename std::enable_if<
                 meta::is_variadic_lambda_callable<Lambda, const T&, const std::string&>::value, int>::type = 0>
    static bool load(const Config& config, const std::string& key, T& field, Lambda lambda, Args&&... args)
    {
        // std::cout << "[debug] load simple key=\"" << key << "\"" << std::endl;

        if(config.isMapContainer() && config.containsKey(key)) {
            T temp_value = config.get<T>(key);

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
        // std::cout << "[debug] save simple key=\"" << key << "\"" << std::endl;

        config[key] = field;
        config[key].setComment(prefix_comment, suffix_comment);
    }

    static bool compare(const T& field, const T& other)
    {
        return utils::CompareIfPossible(field, other);
    }
};

} // namespace tools
} // namespace simpleapi
