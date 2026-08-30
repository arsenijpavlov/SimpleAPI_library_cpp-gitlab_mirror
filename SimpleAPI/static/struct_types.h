#pragma once

// NOTE: "IWYU pragma: keep" спрячет лишнее предупреждение от clangd
#include "base.h"           // IWYU pragma: keep
#include "type_checkers.h"
#include <type_traits>


namespace simpleapi {
namespace tools {

// правило для определения структур
template<typename T>
struct ConfigTypeTraits<T, typename std::enable_if<is_config_struct<T>::value
                                                   && !is_container_as_vector<T>::value
                                                   && !is_container_as_set<T>::value
                                                   && !is_container_as_unordered_set<T>::value
                                                   && !is_container_as_array<T>::value
                                                   && !is_container_as_queue<T>::value
                                                   && !is_container_as_priority_queue<T>::value
                                                   && !is_container_as_stack<T>::value
                                                   >::type>
{
    static bool load(const Config& config, const std::string& key, T& field)
    {
        std::cout << "[debug] load structure key=\"" << key << "\"" << std::endl;

        field.loadConfig(config[key]);
        return true;
    }

    template<typename Lambda, typename... Args,
             typename std::enable_if<is_variadic_lambda_callable<Lambda, const T&, Args...>::value, int>::type = 0>
    static bool load(const Config& config, const std::string& key, T& field, Lambda lambda, Args&&... args)
    {
        std::cout << "[debug] load structure key=\"" << key << "\"" << std::endl;

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
        std::cout << "[debug] save structure key=\"" << key << "\"" << std::endl;

        if(config.isMapContainer())
        {
            config[key] = field.saveConfig();
            config[key].setComment(prefix_comment, suffix_comment);
        } else {
            config = field.saveConfig();
            config.setComment(prefix_comment, suffix_comment);
        }
    }
};

} // namespace simpleapi
} // namespace tools
