#ifndef ELEMENT_STRING_H
#define ELEMENT_STRING_H

#include "IElement.h"


class ElementString : public IElement {
protected:
    std::string m_value;

public:
    ElementString() noexcept : m_value()                        { m_type = ValueType::eString; }
    ElementString(const std::string& s) noexcept : m_value(s)   { m_type = ValueType::eString; }
    ~ElementString() noexcept                                   {}

    //PRINTING =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //для рекурсивного вызова, без комментариев, в одну строку
    std::string to_string(const ConfigFormat format = ConfigFormat::eJSON,
                          const int8_t tabulation_level = 0) const noexcept override;
    std::string to_JSON_string(const int8_t tabulation_level = 0) const noexcept;
    std::string to_INI_string(const int8_t tabulation_level = 0) const noexcept;

    //для рекурсивного вызова, с использованием комментариев
    std::string to_string(const ConfigFormat format, const CommentDesign &design,
                          const int8_t tabulation_level = 0) const noexcept override;
    std::string to_JSON_string(const CommentDesign &design,
                               const int8_t tabulation_level = 0) const noexcept;
    std::string to_INI_string(const CommentDesign &design,
                              const int8_t tabulation_level = 0) const noexcept;
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= PRINTING

    std::string getValue() const noexcept                       { return m_value; }
    bool        isEqual(const IElement& other, const bool compare_comments = false) const noexcept override;

    //OPERATORS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    void operator<<(const std::string& other) noexcept;
    void operator=(const std::string& other) noexcept;
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= OPERATORS
};

//не const, потому что предполагается использовать эту же переменную для возврата прочитанного значения
bool IsElementString(std::string& str, const ConfigFormat format = ConfigFormat::eJSON,
                     std::string* error_log = nullptr) noexcept;
bool IsElementString(const IElement& e) noexcept;


#endif // ELEMENT_STRING_H
