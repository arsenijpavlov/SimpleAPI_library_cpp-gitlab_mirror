#ifndef JSON_ELEMENT_NULL_H
#define JSON_ELEMENT_NULL_H

#include "JsonBaseElement.h"


class ElementNull : public Element {
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
    ElementNull()       { m_type = ValueType::eNull; }
    ~ElementNull()      {}

    //PRINTING =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//TODO:    std::string to_string(const ConfigFormat format = ConfigFormat::eJSON) const noexcept override;
//TODO:    std::string to_string(const ConfigFormat format, const CommentDesign &design) const noexcept override;
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= PRINTING

    //OPERATORS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    bool        isEqual(const Element& other, const bool compare_comments = false) const noexcept override;
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= OPERATORS
};

bool IsElementNull(const std::string& str) noexcept;
bool IsElementNull(const Element& e) noexcept;

#endif // JSON_ELEMENT_NULL_H
