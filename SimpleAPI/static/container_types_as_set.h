#pragma once

// NOTE: "IWYU pragma: keep" спрячет лишнее предупреждение от clangd
#include "base.h"              // IWYU pragma: keep
#include "type_checkers.h"
#include <string>
#include <type_traits>
#include "../config/Config.h"


namespace simpleapi {
namespace tools {

// правило для определения типов-контейнеров (vector, queue и т.д.)
// std::string не считается за контейнер и обрабатывается как цельный элемент
template<typename T>
struct ConfigTypeTraits<T, typename std::enable_if<is_container_as_set<T>::value
                                                   || is_container_as_unordered_set<T>::value>::type>
{
    static bool load(const Config& config, const std::string& key, T& field)
    {
        std::cout << "[debug] load container(as set) key=\"" << key << "\"" << std::endl;

        using Type = typename T::value_type;

        const Config& ck = config[key];
        field.clear();
        size_t counter = 0;
        for(const auto& c : ck.getRange()) {
            // рекурсивно вызываем traits(признаки) для каждого из элементов
            Type item_value;
            if(!ConfigTypeTraits<Type>::loadWithoutKey((*c.get()), item_value))
                return false;

            field.insert(item_value);
        }

        return true;
    }

    template<typename Lambda, typename... Args,
             typename std::enable_if<is_variadic_lambda_callable<Lambda, const T&, Args...>::value, int>::type = 0>
    static bool load(const Config& config, const std::string& key, T& field, Lambda lambda, Args&&... args)
    {
        std::cout << "[debug] load container(as set) key=\"" << key << "\"" << std::endl;

        using Type = typename T::value_type;
        T temp_value;

        const Config& ck = config[key];
        field.clear();
        size_t counter = 0;
        for(const auto& c : ck.getRange()) {
            // рекурсивно вызываем traits(признаки) для каждого из элементов
            Type item_value;
            if(!ConfigTypeTraits<Type>::loadWithoutKey((*c.get()), item_value))
                return false;

            temp_value.insert(item_value);
        }

        if(lambda(temp_value, std::forward<Args>(args)...))
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
        std::cout << "[debug] save container(as set) key=\"" << key << "\"" << std::endl;

        using Type = typename T::value_type;

        for(const auto& item : field) {
            // ключ дальше нельзя передать - создаст новую вложенность
            Config temp;
            ConfigTypeTraits<Type>::saveWithoutKey(temp, item);
            config[key].push_back(temp);
        }
        config[key].setComment(prefix_comment, suffix_comment);
    }
};

} // namespace simpleapi
} // namespace tools
