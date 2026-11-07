#pragma once

// NOTE: "IWYU pragma: keep" спрячет лишнее предупреждение от clangd
#include "base.h"          // IWYU pragma: keep
#include "../meta/meta_lambda_validators.h"
#include "../meta/lambda_executers.h"
#include <type_traits>
#include "../config/Config.h"


namespace simpleapi {
namespace tools {

// правило для определения одиночных типов
template<typename T>
struct ConfigTypeTraits<T, typename std::enable_if<std::is_same<T, std::wstring>::value>::type>
{
    static bool load(const Config& config, const std::string& key, T& field)
    {
        // std::cout << "[debug] load wstring(utf-8) key=\"" << key << "\"" << std::endl;

        if(config.isMapContainer() && config.containsKey(key)) {
            if(!config.is_can_get<std::string>(key))
                return false;
            field = utils::Utf8ToWstring(config.get<std::string>(key));
        }

        return true;
    }

    template<typename Lambda, typename... Args,
             typename std::enable_if<
                 meta::is_variadic_lambda_callable<Lambda, const T&, const std::string&>::value, int>::type = 0>
    static bool load(const Config& config, const std::string& key, T& field, Lambda lambda, Args&&... args)
    {
        // std::cout << "[debug] load wstring(utf-8) key=\"" << key << "\"" << std::endl;

        if(config.isMapContainer() && config.containsKey(key)) {
            if(!config.is_can_get<std::string>(key))
                return false;
            T temp_value = utils::Utf8ToWstring(config.get<std::string>(key));

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
        // std::cout << "[debug] save wstring(utf-8) key=\"" << key << "\"" << std::endl;

        config[key] = utils::WstringToUtf8(field);
        config[key].setComment(prefix_comment, suffix_comment);
    }

    static bool compare(const T& field, const T& other)
    {
        return utils::CompareIfPossible(field, other);
    }
};

} // namespace tools
} // namespace simpleapi
