#pragma once

// NOTE: "IWYU pragma: keep" спрячет лишнее предупреждение от clangd
#include "base.h"          // IWYU pragma: keep
#include <type_traits>
#include "../config/Config.h"


namespace simpleapi {
namespace tools {

// правило для определения одиночных типов
template<typename T>
struct ConfigTypeTraits<T, typename std::enable_if<std::is_same<T, std::nullptr_t>::value>::type>
{
    static bool load(const Config& config, const std::string& key, std::nullptr_t&)
    {
        // std::cout << "[debug] load std::nullptr_t key=\"" << key << "\"" << std::endl;

        if(config.isMapContainer() && config.containsKey(key)) {
            return config[key].isNull();
        }

        return true;
    }

    template<typename Lambda, typename... Args>
    static bool load(const Config& config, const std::string& key, std::nullptr_t&, Lambda lambda, Args&&... args)
    {
        // std::cout << "[debug] load std::nullptr_t key=\"" << key << "\"" << std::endl;

        if(config.isMapContainer() && config.containsKey(key)) {
            return config[key].isNull();
        }

        return true;
    }

    // комментарии учитываются только при записи
    static void save(Config& config, const std::string& key, const T& field,
                     const std::string& prefix_comment = "",
                     const std::string& suffix_comment = "")
    {
        // std::cout << "[debug] save std::nullptr_t key=\"" << key << "\"" << std::endl;

        config[key] = nullptr;
        config[key].setComment(prefix_comment, suffix_comment);
    }

    static bool compare(const T& field, const T& other)
    {
        return utils::CompareIfPossible(field, other);
    }
};

} // namespace tools
} // namespace simpleapi
