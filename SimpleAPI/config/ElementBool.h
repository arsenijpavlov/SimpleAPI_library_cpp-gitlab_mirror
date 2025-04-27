#ifndef ELEMENT_BOOL_H
#define ELEMENT_BOOL_H

#include "IElement.h"


class ElementBool : public IElement {
protected:
    bool m_value;

public:
    ElementBool() noexcept : m_value(false)         { m_type = ValueType::eBool; }
    ElementBool(const bool b) noexcept : m_value(b) { m_type = ValueType::eBool; }
    ~ElementBool() noexcept {}

    //PRINTING =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //для рекурсивного вызова, без комментариев, в одну строку
    std::string toString(const ConfigFormat format = ConfigFormat::eJSON,
                         const int8_t tabulation_level = 0) const noexcept override
                                                    { return m_value ? "true" : "false"; }
    //для рекурсивного вызова, с использованием комментариев
    std::string toString(const ConfigFormat format, const CommentDesign &design,
                         const int8_t tabulation_level = 0) const noexcept override
                                                    { return m_value ? "true" : "false"; }
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= PRINTING

    bool        getValue() const noexcept           { return m_value; }
    bool&       getValue() noexcept                 { return m_value; }

    bool        isEqual(const IElement& other, const bool compare_comments = false) const noexcept override;

    //OPERATORS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    void operator=(const bool other) noexcept;
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= OPERATORS
};

bool IsElementBool(const std::string& str) noexcept;
bool IsElementBool(const IElement& e) noexcept;

#endif // ELEMENT_BOOL_H
