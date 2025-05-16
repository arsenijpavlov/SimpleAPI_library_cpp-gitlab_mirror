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
    bool&           getBool()                                           override    { return m_value; }
    bool            getBool()                           const           override    { return m_value; }
    long double&    getNumber()                                         override;
    long double     getNumber()                         const           override;
    std::string&    getString()                                         override;
    std::string     getString()                         const           override;

    // вложенные контейнеры
    Config&         get_front()                                         override;
    Config          get_front()                         const           override;
    Config&         get_at(const size_t index)                          override;
    Config          get_at(const size_t index)          const           override;
    Config&         get_at(const std::string& key)                      override;
    Config          get_at(const std::string& key)      const           override;
    Config&         get_back()                                          override;
    Config          get_back()                          const           override;
    // ========================================================================================================= Getters

    // Info ============================================================================================================
    bool            isEqual(const bool other)           const noexcept  override    { return m_value; }
    size_t          size()                              const noexcept  override    { return 1; }
    // ============================================================================================================ Info

    // Operators =======================================================================================================
    //числа, контейнеры(размер), строки(длина в видимых символах)
    Config&         operator[](const size_t index)      noexcept        override;
    Config          operator[](const size_t index)      const noexcept  override;
    Config&         operator[](const std::string& key)  noexcept        override;
    Config          operator[](const std::string& key)  const noexcept  override;
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
