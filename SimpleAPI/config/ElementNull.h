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
    bool&           getBool()                                           override;
    bool            getBool()                           const           override;
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

    // Operators =======================================================================================================
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

bool IsElementNull(const std::string& str)              noexcept;
bool IsElementNull(const IElement& e)                   noexcept;

#endif // ELEMENT_NULL_H
