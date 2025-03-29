#ifndef JSON_ELEMENT_BOOL_H
#define JSON_ELEMENT_BOOL_H

#include "JsonBaseElement.h"


class ElementBool : public Element {
protected:
    bool m_value;

public:
    ElementBool() noexcept : m_value(false)         { m_type = ValueType::eBool; }
    ElementBool(const bool b) noexcept : m_value(b) { m_type = ValueType::eBool; }
    ~ElementBool() noexcept;

    //PRINTING =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    std::string to_string(const ConfigFormat format = ConfigFormat::eJSON,
                          const bool to_one_line = false) const noexcept override;
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= PRINTING

    bool        getValue() const noexcept           { return m_value; }
    bool        isEqual(const Element& other) const noexcept override;

    //OPERATORS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    void operator=(const bool other) noexcept;
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= OPERATORS
};

bool IsElementBool(const std::string& str) noexcept;
bool IsElementBool(const Element& e) noexcept;

#endif // JSON_ELEMENT_BOOL_H
