#ifndef ONLYSIZETORSTRING_H
#define ONLYSIZETORSTRING_H

#include <stdexcept>
#include <string>
class OnlySizetOrString {
private:
    std::string m_string;
    size_t      m_sizet;
    enum class Type {
        type_string,
        type_sizet,
    } m_type;

public:
    OnlySizetOrString(std::string str) noexcept
        : m_string{str}, m_type{Type::type_string} {};
    OnlySizetOrString(const char* str) noexcept
        : m_string{std::string(str)}, m_type{Type::type_string} {};
    OnlySizetOrString(size_t index) noexcept
        : m_sizet{index}, m_type{Type::type_sizet} {};

    std::string toString() const noexcept {
        return m_type == Type::type_string ? "\"" + m_string + "\"" : std::to_string(m_sizet);
    }

    size_t getIndexValue() const {
        if(m_type != Type::type_sizet)
            throw std::invalid_argument("value is not a size_t");
        return m_sizet;
    }
    size_t getStringValue() const {
        if(m_type != Type::type_string)
            throw std::invalid_argument("value is not a string");
        return m_sizet;
    }
};

#endif // ONLYSIZETORSTRING_H
