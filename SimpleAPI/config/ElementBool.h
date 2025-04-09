#ifndef ELEMENT_BOOL_H
#define ELEMENT_BOOL_H

#include "IElement.h"


class ElementBool : public IElement {
protected:
    bool m_value;

public:
    ElementBool() noexcept : m_value(false)         { m_type = ValueType::eBool; }
    ElementBool(const bool b) noexcept : m_value(b) { m_type = ValueType::eBool; }
    ~ElementBool() noexcept;

    //PRINTING =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//TODO:    std::string to_string(const ConfigFormat format = ConfigFormat::eJSON) const noexcept override;
//TODO:    std::string to_string(const ConfigFormat format, const CommentDesign &design) const noexcept override;
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= PRINTING

    bool        getValue() const noexcept           { return m_value; }
    bool        isEqual(const IElement& other, const bool compare_comments = false) const noexcept override;

    //OPERATORS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    void operator=(const bool other) noexcept;
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= OPERATORS
};

bool IsElementBool(const std::string& str) noexcept;
bool IsElementBool(const IElement& e) noexcept;

#endif // ELEMENT_BOOL_H
