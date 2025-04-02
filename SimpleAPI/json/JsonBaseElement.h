#ifndef JSON_BASE_ELEMENT_H
#define JSON_BASE_ELEMENT_H

#include "Comment.h"
#include "JsonCommon.h"

#include <map>


//базовый класс
class Element {
protected:
    ValueType m_type;
    //m_value;

public:
    Element() noexcept { *this = ElementNull(); }
    virtual ~Element() noexcept;

    ValueType getType() const noexcept                          { return m_type; }

    //WRITING -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //return - удалось записать файл или нет
    bool                writeFile(const std::string& file_path, const ConfigFormat format,
                                const bool with_comments = 0) noexcept;
    virtual bool        writeFileJson(const std::string& file_path, const bool with_comments = 0) noexcept;
    virtual bool        writeFileIni(const std::string& file_path, const bool with_comments = 0) noexcept;
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= WRITING

    //PRINTING =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //to_one_line - следует ли попытаться вывести всё в одну строку (комментарии будут проигнорированы, \
                    а многострочные значения так же станут занимать несколько строк)
    virtual std::string to_string(const ConfigFormat format = ConfigFormat::eJSON,
                                  const bool to_one_line = false) const noexcept
                                                                { return ""; }
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= PRINTING

    //COMMENTS =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
protected:
    Comment m_comment;
public:
    void addComment(const Comment& content) noexcept;
    void addPreviewComment(const std::string& content) noexcept { m_comment.prefix = content; }
    void addSuffixComment(const std::string& content) noexcept  { m_comment.suffix = content; }
    //TODO: addComment(index, content): Json, JArray
    //TODO: addComment(key, content):   Json

    Comment&        getComment() noexcept                       { return m_comment; }
    Comment         getComment() const noexcept                 { return m_comment; }
    std::string&    getPreviewComment() noexcept                { return m_comment.prefix; }
    std::string     getPreviewComment() const noexcept          { return m_comment.prefix; }
    std::string&    getSuffixComment() noexcept                 { return m_comment.suffix; }
    std::string     getSuffixComment() const noexcept           { return m_comment.suffix; }
    //TODO: getComment(index): Json, JArray
    //TODO: getComment(key):   Json

    void clearComment() noexcept;
    void clearPreviewComment() noexcept                         { m_comment.prefix.clear(); }
    void clearSuffixComment() noexcept                          { m_comment.suffix.clear(); }
    //TODO: clearComment(index):    Json, JArray
    //TODO: clearComment(key):      Json
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= COMMENTS

private:
    //<в зависимости от типа> getValue()        { return m_value; }
    //<в зависимости от типа> getValue(index)   { return m_value[index]; }
    //<в зависимости от типа> getValue(key)     { return m_value[key]; }
public:
    //OPERATORS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //TODO: operators
    //TODO: Element&    operator=(const Element& other) noexcept; //включая комментарии

    virtual bool isEqual(const Element& other, const bool compare_comments = false) const noexcept
                                                                { return false; }
    //NOTE: комментарии при сравнении не учитываются
    bool operator==(const Element& other) const noexcept
    {
        if(m_type != other.m_type)  return false;
        return isEqual(other);
    }
    bool operator!=(const Element& other) const noexcept        { return !(*this == other); }
    //Element&    operator[](const size_t index):       Json, JArray
    //Element&    operator[](const std::string& key):   Json
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= OPERATORS
};


//----------------------------------------------------------------------------------------------------------------------

//TODO: m_comment_sym(0) при любом парсинге

//PARSING -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //NOTE: не 'noexcept', потому что надо вернуть std::exception при ошибке парсинга
//return - получившийся распаршенный корневой элемент, NullElement если не удалось чтение
Element ReadFile(const std::string& file_path, const ConfigFormat format,
                  const bool with_comments = false, std::string* error_log = nullptr) noexcept;
Element ReadFileJson(const std::string& file_path, const bool with_comments = 0,
                     std::string* error_log = nullptr) noexcept;
Element ReadFileIni(const std::string& file_path, const bool with_comments = 0,
                    std::string* error_log = nullptr) noexcept;

//Element ReadFileYaml(const std::string& file_path, const bool with_comments = 0,
//                     std::string* error_log = nullptr) noexcept;
//Element ReadFileXml(const std::string& file_path, const bool with_comments = 0,
//                     std::string* error_log = nullptr) noexcept;

Element Parse(const std::string& content, const ConfigFormat format,
               const bool with_comments = false, std::string* error_log = nullptr) noexcept;
Element ParseJson(const std::string& file_path, const bool with_comments = 0,
                  std::string* error_log = nullptr) noexcept;
Element ParseIni(const std::string& file_path, const bool with_comments = 0,
                 std::string* error_log = nullptr) noexcept;

//Element ParseYaml(const std::string& file_path, const bool with_comments = 0,
//                  std::string* error_log = nullptr) noexcept;
//Element ParseXml(const std::string& file_path, const bool with_comments = 0,
//                 std::string* error_log = nullptr) noexcept;

//TODO: перенести в отдельный класс virtual bool        writeFileYaml(const std::string& file_path, const bool with_comments = 0) noexcept  { return false; }
//TODO: перенести в отдельный класс virtual bool        writeFileXml(const std::string& file_path, const bool with_comments = 0) noexcept     = 0;
//TODO: перенести в отдельный класс case ConfigFormat::eXML:    return writeFileXml(file_path);
//TODO: перенести в отдельный класс case ConfigFormat::eXML:    return ReadFileXml(file_path, with_comments);

//TODO: перенести в отдельный класс Element& ReadFileXml(const std::string& file_path, const bool with_comments = 0);
//TODO: перенести в отдельный класс Element& ParseXml(const std::string& file_path, const bool with_comments = 0);
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= PARSING


#endif // JSON_BASE_ELEMENT_H
