#include "IElementMapContainer.h"
#include "Config.h"
#include <algorithm>

Config &IElementMapContainer::operator[](const std::string &key) noexcept {
    return get_at(key);
}

Config IElementMapContainer::operator[](const std::string &key) const noexcept {
    return get_at(key);
}

void IElementMapContainer::set_if_null(const std::string &key, const Config &new_value) noexcept {
    if(!contains(key))
        set(key, new_value);
}

void IElementMapContainer::set_if_null(const std::string &key, Config &&new_value) noexcept {
    if(!contains(key))
        set(key, std::move(new_value));
}

Config &IElementMapContainer::get_at(const std::string &key) noexcept {
    auto cfg_it = std::find_if(cbegin(), cend(), [&](JPair pair){ return pair.first == key; });
    if(cfg_it != m_values.end())
        return *cfg_it->second;

    m_values.push_back(std::make_pair(key, std::move(std::make_shared<Config>())));
    return *m_values.back().second;
}

Config IElementMapContainer::get_at(const std::string &key) const noexcept {
    auto cfg_it = std::find_if(cbegin(), cend(), [&](JPair pair){ return pair.first == key; });
    if(cfg_it != m_values.end())
        return *cfg_it->second;
    return Config();
}

bool IElementMapContainer::contains(const std::string& key) const noexcept {
    return std::any_of(cbegin(), cend(), [&](JPair pair){ return pair.first == key; });
}
