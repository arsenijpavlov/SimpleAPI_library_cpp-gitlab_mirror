#ifndef ELEMENT_NUMBER_H
#define ELEMENT_NUMBER_H

#include "IElement.h"


class ElementNumber : public IElement {
protected:
    long double m_value;

public:
    ElementNumber() noexcept : m_value(0)       { m_type = ValueType::eNumber; }
    ElementNumber(const long double& num) noexcept : m_value(num)
                                                { m_type = ValueType::eNumber; }
//TODO: фича под вопросом    ElementNumber(const std::string& num) noexcept
    ~ElementNumber() noexcept {}

    //PRINTING =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //для рекурсивного вызова, без комментариев, в одну строку
    std::string toString(const ConfigFormat format = ConfigFormat::eJSON,
                          const int8_t tabulation_level = 0) const noexcept override
                                                { return std::to_string(m_value); }
    //для рекурсивного вызова, с использованием комментариев
    std::string toString(const ConfigFormat format, const CommentDesign &design,
                          const int8_t tabulation_level = 0) const noexcept override
                                                { return std::to_string(m_value); }
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= PRINTING

    long double     getValue() const noexcept   { return m_value; }
    long double&    getValue() noexcept         { return m_value; }

    bool        isEqual(const IElement& other, const bool compare_comments = false) const noexcept override;

    //OPERATORS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    void operator=(const long double& other)    { m_value = other; }
    bool operator==(const long double& other)   { return m_value == other; }
    bool operator>(const long double& other)    { return m_value > other; }
    bool operator<(const long double& other)    { return m_value < other; }
    bool operator>=(const long double& other)   { return m_value >= other; }
    bool operator<=(const long double& other)   { return m_value <= other; }
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= OPERATORS
};

bool IsElementNumber(const std::string& str) noexcept;
bool IsElementNumber(const IElement& e) noexcept;

#endif // ELEMENT_NUMBER_H
