#include "IElementNonMapContainer.h"

#include "Config.h"



void IElementNonMapContainer::insert_front(const VElement &elements) noexcept {
    for(size_t i = 0; i < elements.size(); i++)
        m_values.insert(m_values.begin() + i, std::make_shared<Config>(elements[i]));
}

void IElementNonMapContainer::insert_front(VElement &&elements) noexcept {
    for(size_t i = 0; i < elements.size(); i++)
        m_values.insert(m_values.begin() + i, std::make_shared<Config>(std::move(elements[i])));
}

void IElementNonMapContainer::insert_at(const size_t index, const VElement &elements) noexcept {
    if(m_values.size() < index - 1) {
        append_null(m_values.size() - index - 1);   // если не хватает элементов - создать пустые
    }
    for(size_t i = 0; i < elements.size(); i++)
        m_values.insert(m_values.begin() + index + i, std::make_shared<Config>(elements[i]));
}

void IElementNonMapContainer::insert_at(const size_t index, VElement &&elements) noexcept {
    if(m_values.size() < index - 1) {
        append_null(m_values.size() - index - 1);   // если не хватает элементов - создать пустые
    }
    for(size_t i = 0; i < elements.size(); i++)
        m_values.insert(m_values.begin() + index + i, std::make_shared<Config>(std::move(elements[i])));
}

void IElementNonMapContainer::insert_back(const VElement &elements) noexcept {
    for(size_t i = 0; i < elements.size(); i++)
        insert_back(std::make_shared<Config>(elements[i]));
}

void IElementNonMapContainer::insert_back(VElement &&elements) noexcept {
    for(size_t i = 0; i < elements.size(); i++)
        insert_back(std::make_shared<Config>(std::move(elements[i])));
}

void IElementNonMapContainer::insert_at(const size_t index, const Config &value) noexcept {
    if(m_values.size() < index - 1) {
        // если не хватает элементов - создать пустые
        append_null(m_values.size() - index - 1);
        m_values.push_back(std::make_shared<Config>(value));
    } else {
        m_values.insert(m_values.begin() + index, std::make_shared<Config>(value));
    }
}

void IElementNonMapContainer::insert_at(const size_t index, Config &&value) noexcept {
    if(m_values.size() < index - 1) {
        // если не хватает элементов - создать пустые
        append_null(m_values.size() - index - 1);
        m_values.push_back(std::make_shared<Config>(std::move(value)));
    } else {
        m_values.insert(m_values.begin() + index, std::make_shared<Config>(std::move(value)));
    }
}

void IElementNonMapContainer::append_null(size_t size) noexcept {
    for(size_t i = size; i > 0; --i)
        m_values.push_back(std::make_shared<Config>());
}

Config &IElementNonMapContainer::operator[](const size_t index) {
    return *m_values[index];
}

Config IElementNonMapContainer::operator[](const size_t index) const {
    return *m_values[index];
}
