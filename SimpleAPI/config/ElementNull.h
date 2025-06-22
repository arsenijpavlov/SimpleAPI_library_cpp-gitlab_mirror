#ifndef ELEMENT_NULL_H
#define ELEMENT_NULL_H

#include "interface/IElement.h"


class ElementNull : public IElement {
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
    // ============================================================================================================ Info

    // Operators =======================================================================================================
    // ======================================================================================================= Operators

    // String ==========================================================================================================
    //вывод без комментариев, "tabulation_level == -1" => запись в одну строку
    std::string toString(const ConfigFormat format = ConfigFormat::eONLY_VALUE,
                         const int8_t tabulation_level = 0,
                         const CommentDesign &design = {})  const noexcept  override;
    std::string toJsonString(const int8_t tabulation_level = -1, const CommentDesign &design = {})
                                                            const noexcept  override;
    std::string toIniString(const int8_t tabulation_level = -1, const CommentDesign &design = {})
                                                            const noexcept  override;
    std::string toYamlString(const int8_t tabulation_level = -1, const CommentDesign &design = {})
                                                            const noexcept  override;
    std::string toXmlString(const int8_t tabulation_level = -1, const CommentDesign &design = {})
                                                            const noexcept  override;
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

#endif // ELEMENT_NULL_H
