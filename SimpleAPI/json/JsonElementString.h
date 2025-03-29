#ifndef JSON_ELEMENT_STRING_H
#define JSON_ELEMENT_STRING_H

#include "JsonBaseElement.h"


class ElementString : public Element {
protected:
    std::string m_value;

public:
    ElementString() noexcept : m_value()                        { m_type = ValueType::eString; }
    ElementString(const std::string& s) noexcept : m_value(s)   { m_type = ValueType::eString; }
    ~ElementString() noexcept                                   {}

    //PRINTING =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    std::string to_string(const ConfigFormat format = ConfigFormat::eJSON,
                          const bool to_one_line = false) const noexcept override;
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= PRINTING

    std::string getValue() const noexcept                       { return m_value; }
    bool        isEqual(const Element& other) const noexcept override;

    //OPERATORS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    void operator<<(const std::string& other) noexcept;
    void operator=(const std::string& other) noexcept;
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= OPERATORS
};

//не const, потому что предполагается использовать эту же переменную для возврата прочитанного значения
bool IsElementString(std::string& str, const ConfigFormat format = ConfigFormat::eJSON,
                     std::string* error_log = nullptr) noexcept;
bool IsElementString(const Element& e) noexcept;


#endif // JSON_ELEMENT_STRING_H
