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
    ElementNull()       { m_type = ValueType::eNull; }
    ~ElementNull()      {}

    //PRINTING =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//TODO:    std::string to_string(const ConfigFormat format = ConfigFormat::eJSON) const noexcept override;
//TODO:    std::string to_string(const ConfigFormat format, const CommentDesign &design) const noexcept override;
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= PRINTING

    //OPERATORS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    bool        isEqual(const IElement& other, const bool compare_comments = false) const noexcept override;
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= OPERATORS
};

bool IsElementNull(const std::string& str) noexcept;
bool IsElementNull(const IElement& e) noexcept;

#endif // ELEMENT_NULL_H
