#ifndef I_ELEMENT_H
#define I_ELEMENT_H

#include "Comment.h"
#include "ConfigCommon.h"

#include <map>
#include <vector>


//базовый класс
class IElement {
protected:
    ValueType m_type;

public:
    IElement() noexcept;
    virtual ~IElement() noexcept                                {}

    ValueType getType() const noexcept                          { return m_type; }

    //WRITING -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //return - удалось записать файл или нет
    bool                writeFile(const std::string& file_path, const ConfigFormat format,
                                const bool with_comments = 0) noexcept;
    virtual bool        writeFileJson(const std::string& file_path, const bool with_comments = 0) noexcept;
    virtual bool        writeFileIni(const std::string& file_path, const bool with_comments = 0) noexcept;
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= WRITING

    //PRINTING =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //для рекурсивного вызова, без комментариев, в одну строку
    virtual std::string to_string(const ConfigFormat format = ConfigFormat::eJSON,
                                  const int8_t tabulation_level = 0) const noexcept
                                                                { return ""; }
    //для рекурсивного вызова, с использованием комментариев
    virtual std::string to_string(const ConfigFormat format, const CommentDesign &design,
                                  const int8_t tabulation_level = 0) const noexcept
                                                                { return ""; }
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= PRINTING

    //COMMENTS =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
protected:
    Comment m_comment;
public:
    void add_comment(const Comment& content) noexcept            { m_comment = content; }
    void add_comment(const std::string &content_before, const std::string &content_after) noexcept
                                                                { m_comment = Comment(content_before, content_after); }
    void add_prefix_comment(const std::string& content) noexcept  { m_comment.setPrefix(content); }
    void add_suffix_comment(const std::string& content) noexcept  { m_comment.setSuffix(content); }

    Comment&        get_comment() noexcept                       { return m_comment; }
    Comment         get_comment() const noexcept                 { return m_comment; }
    std::string&    get_prefix_comment() noexcept                 { return m_comment.prefix(); }
    std::string     get_prefix_comment() const noexcept           { return m_comment.prefix(); }
    std::string&    get_suffix_comment() noexcept                 { return m_comment.suffix(); }
    std::string     get_suffix_comment() const noexcept           { return m_comment.suffix(); }

    void clear_comment() noexcept                                { m_comment.clear(); }
    void clear_prefix_comment() noexcept                          { m_comment.clearPrefix(); }
    void clear_suffix_comment() noexcept                          { m_comment.clearSuffix(); }
    void delete_comment() noexcept                               { m_comment.del(); }
    void delete_prefix_comment() noexcept                         { m_comment.delPrefix(); }
    void delete_suffix_comment() noexcept                         { m_comment.delSuffix(); }

    CommentDesign&  get_commentDesign() noexcept                 { return m_comment.commentDesign(); }
    CommentDesign   get_commentDesign() const noexcept           { return m_comment.commentDesign(); }
    void    setCommentDesign(const CommentDesign& design) noexcept
                                                                { m_comment.setDesign(design); }
    void    clear_commentDesign() noexcept                       { m_comment.clearDesign(); }
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= COMMENTS

    virtual bool is_equal(const IElement& other, const bool compare_comments = false) const noexcept
                                                                { return false; }

    //OPERATORS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //NOTE: комментарии при присваивании копируются
    IElement& operator=(const IElement& other) noexcept;
    //NOTE: комментарии при сравнении не учитываются
    bool operator==(const IElement& other) const noexcept {
        if(m_type != other.m_type)  return false;
        return is_equal(other);
    }
    bool operator!=(const IElement& other) const noexcept       { return !(*this == other); }
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= OPERATORS
};


//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

using JPair         = std::pair<std::string, IElement>;
using VPairElement  = std::vector<JPair>;
using VElement      = std::vector<IElement>;

//PARSING -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//NOTE: не 'noexcept', потому что надо вернуть std::exception при ошибке парсинга
    //return - получившийся распаршенный корневой элемент, ElementNull если не удалось чтение
IElement ReadFile(const std::string& file_path, const ConfigFormat format,
                 const bool with_comments = false, std::string* error_log = nullptr);
IElement ReadFileJson(const std::string& file_path, const bool with_comments = 0,
                     std::string* error_log = nullptr);
IElement ReadFileIni(const std::string& file_path, const bool with_comments = 0,
                    std::string* error_log = nullptr);

//Element ReadFileYaml(const std::string& file_path, const bool with_comments = 0,
//                     std::string* error_log = nullptr) noexcept;
//Element ReadFileXml(const std::string& file_path, const bool with_comments = 0,
//                     std::string* error_log = nullptr) noexcept;

IElement Parse(const std::string& content, const ConfigFormat format,
              const bool with_comments = false, std::string* error_log = nullptr);
IElement ParseJson(const std::string& file_path, const bool with_comments = 0,
                  std::string* error_log = nullptr);
IElement ParseIni(const std::string& file_path, const bool with_comments = 0,
                 std::string* error_log = nullptr);

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


#endif // I_ELEMENT_H
