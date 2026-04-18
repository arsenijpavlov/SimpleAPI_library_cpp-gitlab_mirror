#ifndef ELEMENT_BOOL_H
#define ELEMENT_BOOL_H

#include "interface/IElement.h"


namespace simpleapi {

class ElementBool : public IElement {
protected:
    bool m_value;

public:
    ElementBool()                       noexcept : m_value(false)                       { init(); }
    explicit ElementBool(const bool b)  noexcept : m_value(b)                           { init(); }
    ~ElementBool()                      noexcept                                        {}

    // Setters =========================================================================================================
private:
    void        init()                                      noexcept                    { m_type = ValueType::eBool; }
public:
    // ========================================================================================================= Setters

    // Getters =========================================================================================================
    bool&       getValue()                                                              { return m_value; }
    bool        getValue()                                  const                       { return m_value; }
    // ========================================================================================================= Getters

    // Modify ==========================================================================================================
    void        clear()                                     noexcept        override;
    // ========================================================================================================== Modify

    // Info ============================================================================================================
    bool        isEqual(const IElement &other, const bool compare_comments,
                        const bool map_sort_important)      const noexcept  override;
    size_t      size()                                      const noexcept  override    { return 1; }
    // ============================================================================================================ Info

    // Operators =======================================================================================================
    // ======================================================================================================= Operators

    // String ==========================================================================================================
    //вывод без комментариев, "tabulation_level == -1" => запись в одну строку
    static std::string toString(bool value)                             noexcept;
    std::string toString(const ConfigFormat format = ConfigFormat::eONLY_VALUE,
                         const CommentDesign &design = {},
                         const int8_t custom_tabulation_level = -1)     const noexcept  override;
    std::string toJsonString(const CommentDesign &design = {},
                             const int8_t custom_tabulation_level = -1) const noexcept  override;
    std::string toIniString(const CommentDesign &design = {},
                            const int8_t custom_tabulation_level = -1)  const noexcept  override;
    std::string toYamlString(const CommentDesign &design = {},
                             const int8_t custom_tabulation_level = -1) const noexcept  override;
    std::string toXmlString(const CommentDesign &design = {},
                            const int8_t custom_tabulation_level = -1)  const noexcept  override;
    // ========================================================================================================== String

    // File ============================================================================================================
    // ============================================================================================================ File

    // Parser ==========================================================================================================
    // ========================================================================================================== Parser

    // Iterators =======================================================================================================
    // ======================================================================================================= Iterators
};

bool IsElementBool(const std::string& str)              noexcept;
bool IsElementBool(const IElement& e)                   noexcept;
bool IsElementBool(const Config& cfg)                   noexcept;

} // namespace simpleapi

#endif // ELEMENT_BOOL_H
