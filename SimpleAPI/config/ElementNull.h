#ifndef ELEMENT_NULL_H
#define ELEMENT_NULL_H

#include "IElement.h"


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
    ElementNull()                                       noexcept                    { init(); }
    ~ElementNull()                                      noexcept                    {}

    void            init()                              noexcept                    { m_type = ValueType::eNull; }

    // Getters =========================================================================================================
    // ========================================================================================================= Getters

    // Info ============================================================================================================
    // ============================================================================================================ Info

    // Operators =======================================================================================================
    // ======================================================================================================= Operators

    // String ==========================================================================================================
    //вывод без комментариев, "tabulation_level == -1" => запись в одну строку
    std::string     toString(const ConfigFormat format = ConfigFormat::eJSON,
                        const int8_t tabulation_level = 0, const CommentDesign &design = {})
                                                        const noexcept  override;
    // ========================================================================================================== String

    // Iterators =======================================================================================================
    // ======================================================================================================= Iterators
};

bool IsElementNull(const std::string& str)              noexcept;
bool IsElementNull(const IElement& e)                   noexcept;        //{ return e.getType() == ValueType::eNull; }
bool IsElementNull(const Config& cfg)                   noexcept;        //{ return cfg.isNull(); }

#endif // ELEMENT_NULL_H
