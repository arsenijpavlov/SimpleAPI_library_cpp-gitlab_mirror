#ifndef ELEMENT_BOOL_H
#define ELEMENT_BOOL_H

#include "IElement.h"


class ElementBool : public IElement {
protected:
    bool m_value;

public:
    ElementBool()               noexcept : m_value(false)   {}
    ElementBool(const bool b)   noexcept : m_value(b)       {}
    ~ElementBool()              noexcept                    {}

    // Getters =========================================================================================================
    bool&           getBool()                                           { return m_value; }
    bool            getBool()                           const           { return m_value; }
    // ========================================================================================================= Getters

    // Info ============================================================================================================
    bool            isEqual(const bool other)           const noexcept  override    { return m_value; }
    size_t          size()                              const noexcept  override    { return 1; }
    // ============================================================================================================ Info

    // Operators =======================================================================================================

    // ======================================================================================================= Operators

    // String ==========================================================================================================
    //вывод без комментариев, "tabulation_level == -1" => запись в одну строку
    std::string     toString(const ConfigFormat format = ConfigFormat::eJSON,
                        const int8_t tabulation_level = 0, const CommentDesign &design = {})
                                                        const noexcept  override;
    // ========================================================================================================== String
};

bool IsElementBool(const std::string& str)              noexcept;
bool IsElementBool(const IElement& e)                   noexcept;

#endif // ELEMENT_BOOL_H
