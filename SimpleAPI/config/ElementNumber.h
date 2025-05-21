#ifndef ELEMENT_NUMBER_H
#define ELEMENT_NUMBER_H

#include "IElement.h"


class ElementNumber : public IElement {
protected:
    long double m_value;

public:
    ElementNumber()                             noexcept : m_value(0)               { init(); }
    ElementNumber(const long double& num)       noexcept : m_value(num)             { init(); }
    ElementNumber(long double&& num)            noexcept : m_value(std::move(num))  { init(); }
//TODO: фича под вопросом    ElementNumber(const std::string& num) noexcept
    ~ElementNumber()                            noexcept                            {}

    void init()                                 noexcept                            { m_type = ValueType::eNumber; }

    // Getters =========================================================================================================
    long double&    getValue();
    long double     getValue()                  const;
    // ========================================================================================================= Getters

    // Info ============================================================================================================
    bool            isEqual(const bool other)   const noexcept  override            { return m_value == other; }
    size_t          size()                      const noexcept  override            { return 1; }
    // ============================================================================================================ Info

    // Operators =======================================================================================================
    void operator=(const long double& other)    noexcept                            { m_value = other; }
    bool operator==(const long double& other)   const noexcept                      { return m_value == other; }
    bool operator>(const long double& other)    const noexcept                      { return m_value > other; }
    bool operator<(const long double& other)    const noexcept                      { return m_value < other; }
    bool operator>=(const long double& other)   const noexcept                      { return m_value >= other; }
    bool operator<=(const long double& other)   const noexcept                      { return m_value <= other; }
    // ======================================================================================================= Operators
};

bool IsElementNumber(const std::string& str)    noexcept;
bool IsElementNumber(const IElement& e)         noexcept;
bool IsElementNumber(const Config& cfg)         noexcept;

#endif // ELEMENT_NUMBER_H
