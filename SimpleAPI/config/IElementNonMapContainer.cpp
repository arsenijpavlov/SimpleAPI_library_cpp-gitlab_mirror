#include "IElementNonMapContainer.h"

#include "Config.h"


Config &IElementNonMapContainer::get_front() {
    return *m_values.front();
}

Config IElementNonMapContainer::get_front() const {
    return *m_values.front();
}

Config &IElementNonMapContainer::get_at(const size_t index) {
    return *m_values[index];
}

Config IElementNonMapContainer::get_at(const size_t index) const {
    return *m_values[index];
}

Config &IElementNonMapContainer::get_back() {
    return *m_values.back();
}

Config IElementNonMapContainer::get_back() const {
    return *m_values.back();
}

void IElementNonMapContainer::insert_front(const Config &value) noexcept {
    m_values.insert(m_values.begin(), std::make_shared<Config>(value));
}

void IElementNonMapContainer::insert_front(Config &&value) noexcept {
    m_values.insert(m_values.begin(), std::make_shared<Config>(std::move(value)));
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

Config IElementNonMapContainer::get_and_pop_at(const VElement::iterator iterator) {
    Config cfg = *iterator->get();
    pop_at(iterator);
    return cfg;
}

Config &IElementNonMapContainer::operator[](const size_t index) {
    return *m_values[index];
}

Config IElementNonMapContainer::operator[](const size_t index) const {
    return *m_values[index];
}
