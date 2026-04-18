#ifndef ELEMENT_STRING_H
#define ELEMENT_STRING_H

#include "interface/IElement.h"
#include "../utils/Utils.h"


namespace simpleapi {

class ElementString : public IElement {
protected:
    std::string m_value;

public:
    ElementString()                                 noexcept : m_value()                { init(); }
    explicit ElementString(const std::string& s)    noexcept : m_value(s)               { init(); }
    explicit ElementString(std::string&& s)         noexcept : m_value(std::move(s))    { init(); }
    ~ElementString()                                noexcept                            {}

    // Setters =========================================================================================================
private:
    void            init()                                  noexcept                    { m_type = ValueType::eString; }
public:
    // ========================================================================================================= Setters

    // Getters =========================================================================================================
    std::string&    getValue()                                                          { return m_value; }
    std::string     getValue()                              const                       { return m_value; }
    // ========================================================================================================= Getters

    // Modify ==========================================================================================================
    void            clear()                                 noexcept        override;
    // ========================================================================================================== Modify

    // Info ============================================================================================================
    //длина в видимых символах
    bool            isEqual(const IElement &other, const bool compare_comments,
                            const bool map_sort_important)  const noexcept  override;
    size_t          size()                                  const noexcept  override    { return utils::GetStringCharCount(m_value); }
    // ============================================================================================================ Info

    // Operators =======================================================================================================
    // ======================================================================================================= Operators

    // String ==========================================================================================================
    //вывод без комментариев, "tabulation_level == -1" => запись в одну строку
    std::string     toString(const ConfigFormat format = ConfigFormat::eONLY_VALUE,
                         const CommentDesign &design = {},
                         const int8_t custom_tabulation_level = -1)     const noexcept  override;
    std::string     toJsonString(const CommentDesign &design = {},
                             const int8_t custom_tabulation_level = -1) const noexcept  override;
    std::string     toIniString(const CommentDesign &design = {},
                            const int8_t custom_tabulation_level = -1)  const noexcept  override;
    std::string     toYamlString(const CommentDesign &design = {},
                             const int8_t custom_tabulation_level = -1) const noexcept  override;
    std::string     toXmlString(const CommentDesign &design = {},
                            const int8_t custom_tabulation_level = -1)  const noexcept  override;
    // ========================================================================================================== String

    // File ============================================================================================================
    // ============================================================================================================ File

    // Parser ==========================================================================================================
    // ========================================================================================================== Parser

    // Iterators =======================================================================================================
    // ======================================================================================================= Iterators
};

bool IsElementString(const IElement& e)                 noexcept;
bool IsElementString(const Config& cfg)                 noexcept;

} // namespace simpleapi

#endif // ELEMENT_STRING_H
