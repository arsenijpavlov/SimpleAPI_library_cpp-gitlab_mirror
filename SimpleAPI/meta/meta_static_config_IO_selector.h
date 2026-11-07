#pragma once

#include <utility>
#include "meta_same_types_validators.h"


namespace simpleapi {
namespace tools {

// разделение чтения переменной "value=get<T>()"/"value=T::loadConfig(cfg)"
// если T имеет метод loadConfig()
template <typename T>
static bool Loader(const Config& config, T& output_value,
                   typename std::enable_if<meta::is_config_struct<T>::value>::type* = 0)
{
    if(!output_value.loadConfig(config))
        return false;
    return true;
}
// если T не имеет метода loadConfig()
template <typename T>
static bool Loader(const Config& config, T& output_value,
                   typename std::enable_if<!meta::is_config_struct<T>::value>::type* = 0)
{
    output_value = config.get<T>();
    return true;
}
// ----------------------------------------------------------------------------

// разделение чтения переменной "value=get<T>()"/"value=T::loadConfig(cfg)"
// если T имеет метод saveConfig()
template <typename T>
static Config Saver(const T& input_value, const std::string prefix_comment, const std::string suffix_comment,
                    typename std::enable_if<meta::is_config_struct<T>::value>::type* = 0)
{
    return input_value.saveConfig();
}
// если T не имеет метода saveConfig()
template <typename T>
static Config Saver(const T& input_value, const std::string prefix_comment, const std::string suffix_comment,
                   typename std::enable_if<!meta::is_config_struct<T>::value>::type* = 0)
{
    return Config(input_value);
}

} // namespace tools
} // namespace simpleapi
