#include "IElementMapContainer.h"


Config &IElementMapContainer::operator[](const std::string &key) noexcept {
    return get_at(key);
}

Config &IElementMapContainer::operator[](const std::string &key) const noexcept {
    return get_at(key);
}
