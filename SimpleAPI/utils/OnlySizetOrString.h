#ifndef ONLYSIZETORSTRING_H
#define ONLYSIZETORSTRING_H

#include <stdexcept>
#include <string>
#include "TypeDefines.h"

namespace simpleapi {

class OnlySizetOrString {
public:
    enum class Type {
        type_string,
        type_sizet,
    };
private:
    std::string m_string;
    size_t      m_sizet;
    Type        m_type;

public:
    OnlySizetOrString() = delete;

    __ONLY_STRING_TYPES__(T)
    OnlySizetOrString(const T& str) noexcept
        : m_string{std::string(str)}, m_type{Type::type_string} {};

    __ONLY_NUMBER_TYPES__(T)
    OnlySizetOrString(const T& index) noexcept
        : m_sizet{static_cast<size_t>(index)}, m_type{Type::type_sizet} {};

    std::string toString() const noexcept {
        return m_type == Type::type_string ? "\"" + m_string + "\"" : std::to_string(m_sizet);
    }

    Type getType() const noexcept {
        return m_type;
    }
    size_t getIndexValue() const {
        if(m_type != Type::type_sizet)
            throw std::invalid_argument("value is not a size_t");
        return m_sizet;
    }
    std::string getStringValue() const {
        if(m_type != Type::type_string)
            throw std::invalid_argument("value is not a string");
        return m_string;
    }
};

} // namespace simpleapi

#endif // ONLYSIZETORSTRING_H
