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
    Element(){}
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
                                  const bool to_one_line = false) const noexcept                            { return ""; }
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
    //TODO: Element&    operator=(const Element& other) noexcept;

    //NOTE: комментарии при сравнении не учитываются
    virtual bool isEqual(const Element& other) const noexcept   { return false; }
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


//удалить пробелы в начале и конце строки
void RemoveIllegalSpaces(std::string& string) noexcept;

//READING -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
bool GetAllStringsFromFile(const std::string& path, std::string& dest_string,
                           std::string* error_log = nullptr) noexcept;
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= READING

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


//WRITING -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
bool CreateEmptyFile(const std::string& file_path, const std::string& start_comment,
                     const std::string& finish_comment, std::string* error_log = nullptr) noexcept;
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= WRITING


#endif // JSON_BASE_ELEMENT_H
