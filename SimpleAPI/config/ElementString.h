#ifndef ELEMENT_STRING_H
#define ELEMENT_STRING_H

#include "IElement.h"
#include "../utils/Utils.h"


class ElementString : public IElement {
protected:
    std::string m_value;

public:
    ElementString()                     noexcept : m_value()                        { init(); }
    ElementString(const std::string& s) noexcept : m_value(s)                       { init(); }
    ElementString(std::string&& s)      noexcept : m_value(std::move(s))            { init(); }
    ~ElementString()                    noexcept                                    {}

    void            init()                              noexcept                    { m_type = ValueType::eString; }

    // Getters =========================================================================================================
    std::string&    getValue();
    std::string     getValue()                          const;
    // ========================================================================================================= Getters

    // Info ============================================================================================================
    bool            isEqual(const std::string& other)   const noexcept  override    { return m_value == other; }
    size_t          size()                              const noexcept  override    { return utils::GetStringCharCount(m_value); }
    // ============================================================================================================ Info

    // Operators =======================================================================================================
    bool            operator>(const std::string& other) const noexcept              { return utils::GetStringCharCount(m_value) >  utils::GetStringCharCount(other); }
    bool            operator>=(const std::string& other)const noexcept              { return utils::GetStringCharCount(m_value) >= utils::GetStringCharCount(other); }
    bool            operator<(const std::string& other) const noexcept              { return utils::GetStringCharCount(m_value) <  utils::GetStringCharCount(other); }
    bool            operator<=(const std::string& other)const noexcept              { return utils::GetStringCharCount(m_value) <= utils::GetStringCharCount(other); }
    //TODO: operator>>
    // ======================================================================================================= Operators

    // String ==========================================================================================================
    //вывод без комментариев, "tabulation_level == -1" => запись в одну строку
    virtual std::string     toString(const ConfigFormat format = ConfigFormat::eONLY_VALUE,
                                 const int8_t tabulation_level = 0, const CommentDesign &design = {})
                                                        const noexcept  override;
    // ========================================================================================================== String

    // Iterators =======================================================================================================
    //Не предполагается использовать через класс. Использовать getValue().begin() / getValue().cbegin()
    // ======================================================================================================= Iterators
};

//не const, потому что предполагается использовать эту же переменную для возврата прочитанного значения
bool IsElementString(std::string& str, const ConfigFormat format = ConfigFormat::eJSON,
                     std::string* error_log = nullptr)  noexcept;
bool IsElementString(const IElement& e)                 noexcept;
bool IsElementString(const Config& cfg)                 noexcept;


#endif // ELEMENT_STRING_H
