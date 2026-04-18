#ifndef ELEMENT_NULL_H
#define ELEMENT_NULL_H

#include "interface/IElement.h"
#include "interface/IErrorField.h"


namespace simpleapi {

class ElementNull : public IElement, public IErrorField {
    /* "Пустое значение"
     * JSON, наполнение:
     *      - null (во всех регистрах)
     * YAML, наполнение:
     *      - null (во всех регистрах)
     *      - ~
     *      - (ничего)
    */
public:
    ElementNull()                                           noexcept                    { init(); }
    ~ElementNull()                                          noexcept                    {}

    // Setters =========================================================================================================
private:
    void        init()                                      noexcept                    { m_type = ValueType::eNull; }
public:
    // ========================================================================================================= Setters

    // Getters =========================================================================================================
    // ========================================================================================================= Getters

    // Modify ==========================================================================================================
    void        clear()                                     noexcept        override    { clearComment(); }
    // ========================================================================================================== Modify

    // Info ============================================================================================================
    bool        isEqual(const IElement& other, const bool compare_comments = false,
                 const bool map_sort_important = false)     const noexcept  override;
    // ============================================================================================================ Info

    // Operators =======================================================================================================
    // ======================================================================================================= Operators

    // String ==========================================================================================================
    //вывод без комментариев, "tabulation_level == -1" => запись в одну строку
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

bool IsElementNull(const std::string& str)              noexcept;
bool IsElementNull(const IElement& e)                   noexcept;
bool IsElementNull(const Config& cfg)                   noexcept;

} // namespace simpleapi

#endif // ELEMENT_NULL_H
