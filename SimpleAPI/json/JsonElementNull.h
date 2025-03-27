#ifndef JSON_ELEMENT_NULL_H
#define JSON_ELEMENT_NULL_H

#include "JsonBaseElement.h"


//TODO: class ElementNull
class ElementNull : Element {
private:

public:
    ElementNull()       { m_type = ValueType::eNull; }
    ~ElementNull()      {}

    //PARSING -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //return - получившийся распаршенный корневой элемент, NullElement если не удалось чтение
    Element&    readFileJson(const std::string& file_path) noexcept;
    Element&    readFileYaml(const std::string& file_path) noexcept;
    Element&    readFileIni(const std::string& file_path) noexcept;
    Element&    readFileXml(const std::string& file_path) noexcept;
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= PARSING

    //WRITING -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //return - удалось записать файл или нет
    bool        writeFileJson(const std::string& file_path) noexcept;
    bool        writeFileYaml(const std::string& file_path) noexcept;
    bool        writeFileIni(const std::string& file_path) noexcept;
    bool        writeFileXml(const std::string& file_path) noexcept;
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= WRITING

    //PRINTING =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //to_one_line - следует ли попытаться вывести всё в одну строку (комментарии будут проигнорированы, \
                    а многострочные значения так же станут занимать несколько строк)
    std::string to_string(const ConfigFormat format = ConfigFormat::eJSON, const bool to_one_line = false) noexcept;
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= PRINTING

    //COMMENTS =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //как в базовом классе
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= COMMENTS

    //OPERATORS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //TODO: operators
private:
         //<в зависимости от типа> getValue()        { return m_value; }
         //<в зависимости от типа> getValue(index)   { return m_value[index]; }
         //<в зависимости от типа> getValue(key)     { return m_value[key]; }
public:
        //Element&    operator=(const Element& other) noexcept;
        //bool        operator==(const Element& other) const noexcept;
        //bool        operator!=(const Element& other) const noexcept { return !(*this == other); }
        //Element&    operator[](const size_t index):       Json, JArray
        //Element&    operator[](const std::string& key):   Json
        //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= OPERATORS
};

bool IsElementNull(const std::string& str);

#endif // JSON_ELEMENT_NULL_H
