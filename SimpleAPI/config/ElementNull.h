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

private:

public:
    ElementNull()                                       noexcept        {}
    ~ElementNull()                                      noexcept        {}

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
};

bool IsElementNull(const std::string& str)              noexcept;
bool IsElementNull(const IElement& e)                   noexcept;
bool IsElementNull(const Config& cfg)                   noexcept;

#endif // ELEMENT_NULL_H
