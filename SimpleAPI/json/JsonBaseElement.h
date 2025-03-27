#ifndef JSON_BASE_ELEMENT_H
#define JSON_BASE_ELEMENT_H

#include "Comment.h"
#include "JsonCommon.h"

#include <map>


//абстрактный класс
class Element {
protected:
    ValueType m_type;
    //            m_value;

public:
    Element(){}
    virtual ~Element() noexcept = 0;

    //PARSING -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //return - получившийся распаршенный корневой элемент, NullElement если не удалось чтение
    Element& readFile(const std::string& file_path, const ConfigFormat format) noexcept {
        switch(format) {
        case ConfigFormat::eJSON:   return readFileJson(file_path);
        case ConfigFormat::eYAML:   return readFileYaml(file_path);
        case ConfigFormat::eINI:    return readFileIni(file_path);
        case ConfigFormat::eXML:    return readFileXml(file_path);
        }
    }
    virtual Element&    readFileJson(const std::string& file_path) noexcept     = 0;
    virtual Element&    readFileYaml(const std::string& file_path) noexcept     = 0;
    virtual Element&    readFileIni(const std::string& file_path) noexcept      = 0;
    virtual Element&    readFileXml(const std::string& file_path) noexcept      = 0;
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= PARSING

    //WRITING -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //return - удалось записать файл или нет
    bool writeFile(const std::string& file_path, const ConfigFormat format) noexcept {
        switch(format) {
        case ConfigFormat::eJSON:   return writeFileJson(file_path);
        case ConfigFormat::eYAML:   return writeFileYaml(file_path);
        case ConfigFormat::eINI:    return writeFileIni(file_path);
        case ConfigFormat::eXML:    return writeFileXml(file_path);
        }
    }
    virtual bool        writeFileJson(const std::string& file_path) noexcept    = 0;
    virtual bool        writeFileYaml(const std::string& file_path) noexcept    = 0;
    virtual bool        writeFileIni(const std::string& file_path) noexcept     = 0;
    virtual bool        writeFileXml(const std::string& file_path) noexcept     = 0;
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= WRITING

    //PRINTING =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //to_one_line - следует ли попытаться вывести всё в одну строку (комментарии будут проигнорированы, \
                    а многострочные значения так же станут занимать несколько строк)
    virtual std::string to_string(const ConfigFormat format = ConfigFormat::eJSON, const bool to_one_line = false) noexcept = 0;
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= PRINTING

    //COMMENTS =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
protected:
    struct CommentPair {
        std::string key_name;
        Comment     comment;
    };
    std::string                 m_preview_comment;
    std::string                 m_suffix_comment;
    std::map<size_t, Comment>   m_map_comments; //TODO: при insert() сместить и комментарии
public:
    void addPreviewComment(const std::string& content) noexcept { m_preview_comment = content; }
    void addSuffixComment(const std::string& content) noexcept  { m_suffix_comment = content; }
    //TODO: addComment(index, content): Json, JArray
    //TODO: addComment(key, content):   Json

    std::string& getPreviewComment() noexcept                   { return m_preview_comment; }
    std::string& getSuffixComment() noexcept                    { return m_suffix_comment; }
    //TODO: getComment(index): Json, JArray
    //TODO: getComment(key):   Json

    void clearPreviewComment() noexcept                         { m_preview_comment.clear(); }
    void clearSuffixComment() noexcept                          { m_suffix_comment.clear(); }
    //TODO: clearComment(index):    Json, JArray
    //TODO: clearComment(key):      Json
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= COMMENTS

    //OPERATORS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //TODO: operators
private:
    //<в зависимости от типа> getValue()        { return m_value; }
    //<в зависимости от типа> getValue(index)   { return m_value[index]; }
    //<в зависимости от типа> getValue(key)     { return m_value[key]; }
public:
    //Element&    operator=(const Element& other) noexcept;
    virtual bool operator==(const Element& other) const noexcept = 0;
    bool operator!=(const Element& other) const noexcept        { return !(*this == other); }
    //Element&    operator[](const size_t index):       Json, JArray
    //Element&    operator[](const std::string& key):   Json
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= OPERATORS
};

//TODO for all sons:
//  Is<TYPE>(std::string)


#endif // JSON_BASE_ELEMENT_H
