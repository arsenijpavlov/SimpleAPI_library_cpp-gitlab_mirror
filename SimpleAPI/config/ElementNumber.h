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
    ~ElementNumber() noexcept;

    //PRINTING =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//TODO:    std::string to_string(const ConfigFormat format = ConfigFormat::eJSON) const noexcept override;
//TODO:    std::string to_string(const ConfigFormat format, const CommentDesign &design) const noexcept override;
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= PRINTING

    long double getValue() const noexcept       { return m_value; }
//TODO: фича под вопросом    int64_t     toInt() const noexcept          { return m_value; }
//TODO: фича под вопросом    uint64_t    toUInt() const noexcept         { return m_value; }
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
