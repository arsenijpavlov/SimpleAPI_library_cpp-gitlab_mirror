#ifndef ELEMENT_STRING_H
#define ELEMENT_STRING_H

#include "IElement.h"


class ElementString : public IElement {
protected:
    std::string m_value;

public:
    ElementString() noexcept : m_value()                        { m_type = ValueType::eString; }
    ElementString(const std::string& s) noexcept : m_value(s)   { m_type = ValueType::eString; }
    ~ElementString() noexcept {}

    //PRINTING =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //для рекурсивного вызова, без комментариев, в одну строку
    std::string toString(const ConfigFormat format = ConfigFormat::eJSON,
                          const int8_t tabulation_level = 0) const noexcept override
                                                                { return m_value; }
    std::string toJsonString(const int8_t tabulation_level = 0) const noexcept
                                                                { return m_value; }
    std::string toIniString(const int8_t tabulation_level = 0) const noexcept
                                                                { return m_value; }

    //для рекурсивного вызова, с использованием комментариев
    std::string toString(const ConfigFormat format, const CommentDesign &design,
                         const int8_t tabulation_level = 0) const noexcept override
                                                                { return m_value; }
    std::string toJsonString(const CommentDesign &design,
                             const int8_t tabulation_level = 0) const noexcept
                                                                { return m_value; }
    std::string toIniString(const CommentDesign &design,
                            const int8_t tabulation_level = 0) const noexcept
                                                                { return m_value; }
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= PRINTING

    std::string     getValue() const noexcept                   { return m_value; }
    std::string&    getValue() noexcept                         { return m_value; }

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
