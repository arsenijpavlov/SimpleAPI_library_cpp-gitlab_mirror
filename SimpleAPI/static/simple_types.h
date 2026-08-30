#pragma once

// NOTE: "IWYU pragma: keep" спрячет лишнее предупреждение от clangd
#include "base.h"              // IWYU pragma: keep
#include "type_checkers.h"
#include <type_traits>
#include "../config/Config.h"


namespace simpleapi {
namespace tools {

// правило для определения одиночных типов
template<typename T>
struct ConfigTypeTraits<T, typename std::enable_if<!is_config_struct<T>::value
                                                   && !is_container_as_vector<T>::value
                                                   && !is_container_as_set<T>::value
                                                   && !is_container_as_queue<T>::value
                                                   >::type>
{
    static bool load(const Config& config, const std::string& key, T& field)
    {
        std::cout << "[debug] load simple key=\"" << key << "\"" << std::endl;

        field = config[key].get<T>();
        return true;
    }

    template<typename Lambda, typename... Args,
             typename std::enable_if<is_variadic_lambda_callable<Lambda, const T&, Args...>::value, int>::type = 0>
    static bool load(const Config& config, const std::string& key, T& field, Lambda lambda, Args&&... args)
    {
        std::cout << "[debug] load simple key=\"" << key << "\"" << std::endl;

        T temp_value = config[key].get<T>();

        if(lambda(field, std::forward<Args>(args)...))
        {
            field = temp_value;
            return true;
        }
        return false;
    }

    static bool loadWithoutKey(const Config& config, T& field)
    {
        field = config.get<T>();
        return true;
    }

    // комментарии учитываются только при записи
    static void save(Config& config, const std::string& key, const T& field,
                     const std::string& prefix_comment = "",
                     const std::string& suffix_comment = "")
    {
        std::cout << "[debug] save simple key=\"" << key << "\"" << std::endl;

        config[key] = field;
        config[key].setComment(prefix_comment, suffix_comment);
    }

    static void saveWithoutKey(Config& config, const T& field,
                               const std::string& prefix_comment = "",
                               const std::string& suffix_comment = "")
    {
        std::cout << "save simple without key" << std::endl;

        config = field;
        config.setComment(prefix_comment, suffix_comment);
    }
};

} // namespace simpleapi
} // namespace tools
