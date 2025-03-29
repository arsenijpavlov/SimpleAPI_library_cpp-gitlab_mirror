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
    //to_one_line - следует ли попытаться вывести всё в одну строку (комментарии будут проигнорированы, \
                    а многострочные значения так же станут занимать несколько строк)
    std::string to_string(const ConfigFormat format = ConfigFormat::eJSON, const bool to_one_line = false) const noexcept override;
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= PRINTING

    //OPERATORS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    bool        isEqual(const Element& other) const noexcept override;
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= OPERATORS
};

bool IsElementNull(const std::string& str);
bool IsElementNull(const Element& e);

#endif // JSON_ELEMENT_NULL_H
