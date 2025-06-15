#include "IElementMapContainer.h"
#include "Config.h"
#include <algorithm>


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

void IElementMapContainer::insert_front(const VPairElement &elements) noexcept {
    for(size_t i = 0; i < elements.size(); i++)
        m_values.insert(m_values.cbegin() + i, elements[i]);
}

void IElementMapContainer::insert_front(VPairElement &&elements) noexcept {
    for(size_t i = 0; i < elements.size(); i++)
        m_values.insert(m_values.cbegin() + i, std::move(elements[i]));
}

//если индекса не существует - добавится в конец списка
void IElementMapContainer::insert_at(const size_t index, const VPairElement &elements) noexcept {
    for(size_t i = 0; i < elements.size(); i++)
        if(m_values.size() < index + 1)
            m_values.push_back(elements[i]);
        else
            m_values.insert(m_values.cbegin() + index + i, elements[i]);
}

//если индекса не существует - добавится в конец списка
void IElementMapContainer::insert_at(const size_t index, VPairElement &&elements) noexcept {
    for(size_t i = 0; i < elements.size(); i++)
        if(m_values.size() < index + 1)
            m_values.push_back(std::move(elements[i]));
        else
            m_values.insert(m_values.cbegin() + index + i, std::move(elements[i]));
}

void IElementMapContainer::insert_back(const VPairElement &elements) noexcept {
    for(size_t i = 0; i < elements.size(); i++)
        m_values.push_back(elements[i]);
}

void IElementMapContainer::insert_back(VPairElement &&elements) noexcept {
    for(size_t i = 0; i < elements.size(); i++)
        m_values.push_back(std::move(elements[i]));
}

bool IElementMapContainer::contains(const std::string& key) const noexcept {
    return std::any_of(cbegin(), cend(), [&](JPair pair){ return pair.first == key; });
}

Config &IElementMapContainer::operator[](const std::string &key) noexcept {
    return get_at(key);
}

Config IElementMapContainer::operator[](const std::string &key) const noexcept {
    return get_at(key);
}
