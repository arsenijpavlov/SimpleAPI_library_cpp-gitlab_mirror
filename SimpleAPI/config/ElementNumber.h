#ifndef ELEMENT_NUMBER_H
#define ELEMENT_NUMBER_H

#include "IElement.h"


class ElementNumber : public IElement {
protected:
    long double m_value;

public:
    ElementNumber()                                 noexcept : m_value(0)               { init(); }
    explicit ElementNumber(const long double& num)  noexcept : m_value(num)             { init(); }
    explicit ElementNumber(long double&& num)       noexcept : m_value(std::move(num))  { init(); }
//TODO: фича под вопросом    ElementNumber(const std::string& num) noexcept
    ~ElementNumber()                                noexcept                            {}

    // Setters =========================================================================================================
private:
    void            init()                          noexcept                            { m_type = ValueType::eNumber; }
public:
    // ========================================================================================================= Setters

    // Getters =========================================================================================================
    long double&    getValue()                                                          { return m_value; }
    long double     getValue()                      const                               { return m_value; }
    // ========================================================================================================= Getters

    // Modify ==========================================================================================================
    void            clear()                         noexcept                override    { m_value = 0; }
    // ========================================================================================================== Modify

    // Info ============================================================================================================
    size_t          size()                          const noexcept          override    { return 1; }
    // ============================================================================================================ Info

    // Operators =======================================================================================================
    // ======================================================================================================= Operators

    // String ==========================================================================================================
    //вывод без комментариев, "tabulation_level == -1" => запись в одну строку
    std::string     toString(const ConfigFormat format = ConfigFormat::eONLY_VALUE,
                         const int8_t tabulation_level = 0,
                         const CommentDesign &design = {}) const noexcept   override;
    // ========================================================================================================== String

    // File ============================================================================================================
    // ============================================================================================================ File

    // Parser ==========================================================================================================
    // ========================================================================================================== Parser

    // Iterators =======================================================================================================
    // ======================================================================================================= Iterators
};

bool IsElementNumber(const std::string& str)    noexcept;
bool IsElementNumber(const IElement& e)         noexcept;
bool IsElementNumber(const Config& cfg)         noexcept;

#endif // ELEMENT_NUMBER_H
