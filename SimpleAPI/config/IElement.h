#ifndef I_ELEMENT_H
#define I_ELEMENT_H

#include "Comment.h"
#include "ConfigCommon.h"
//#include "ConfigDefines.h"

#include <map>
#include <vector>


//предобъявление
class ElementArray;
class ElementJson;

//базовый класс
//NOTE: равносилен ELEMENT_NULL
class IElement {
protected:
    ValueType m_type;

public:
    IElement() noexcept : m_type(ValueType::eNull)              {}
    IElement(const IElement& other) noexcept;

    //TODO: конструкторы для всех подтипов
//    Element(ValueType type, BaseElement* ptr) noexcept : first(type), second(ptr)
//    {}
//    __ONLY_NUMBER_TYPES__(T)
//    IElement(const T& value) noexcept                           { setValue(static_cast<const long double>(value)); }
//    IElement(const bool value) noexcept                         { setValue(value); }
//    __ONLY_STRING_TYPES__(T)
//    IElement(T&& value) noexcept                                { setValue(std::string(value)); }
//    Element(const Json& value) noexcept;
//    Element(const ElementArray& value) noexcept;
//    Element(const Element& other) noexcept;

    virtual ~IElement() noexcept {};

private:
//    void setValue(const long double& value) noexcept;
//    void setValue(const bool& value) noexcept;
//    void setValue(const std::string& value) noexcept;
public:

    ValueType   getType() const noexcept                        { return m_type; }
    bool        isNull() const noexcept                         { return getType() == ValueType::eNull; }
    bool        isBool() const noexcept                         { return getType() == ValueType::eBool; }
    bool        isNumber() const noexcept                       { return getType() == ValueType::eNumber; }
    bool        isString() const noexcept                       { return getType() == ValueType::eString; }
    bool        isArray() const noexcept                        { return getType() == ValueType::eArray; }
    bool        isJson() const noexcept                         { return getType() == ValueType::eJson; }

    bool            getBool() const;
    bool&           getBool();
    long double     getNumber() const;
    long double&    getNumber();
    std::string     getString() const;
    std::string&    getString();
    ElementArray    getArray() const;
    ElementArray&   getArray();
//TODO:    ElementJson     getJson() const;
//TODO:    ElementJson&    getJson();

    //WRITING -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //return - удалось записать файл или нет
    bool                writeFile(const std::string& file_path, const ConfigFormat format,
                                const bool with_comments = 0) noexcept;
    virtual bool        writeFileJson(const std::string& file_path, const bool with_comments = 0) noexcept;
    virtual bool        writeFileIni(const std::string& file_path, const bool with_comments = 0) noexcept;
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= WRITING

    //PRINTING =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //для рекурсивного вызова, без комментариев, в одну строку
    virtual std::string toString(const ConfigFormat format = ConfigFormat::eJSON,
                                  const int8_t tabulation_level = 0) const noexcept
                                                                { return ""; }
    //для рекурсивного вызова, с использованием комментариев
    virtual std::string toString(const ConfigFormat format, const CommentDesign &design,
                                  const int8_t tabulation_level = 0) const noexcept
                                                                { return ""; }
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= PRINTING

    //COMMENTS =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
protected:
    Comment m_comment;
public:
    void addComment(const Comment& content) noexcept            { m_comment = content; }
    void addComment(const std::string &content_before, const std::string &content_after) noexcept
                                                                { m_comment = Comment(content_before, content_after); }
    void addPrefixComment(const std::string& content) noexcept  { m_comment.setPrefix(content); }
    void addSuffixComment(const std::string& content) noexcept  { m_comment.setSuffix(content); }

    Comment&        getComment() noexcept                       { return m_comment; }
    Comment         getComment() const noexcept                 { return m_comment; }
    std::string&    getPrefixComment() noexcept                 { return m_comment.prefix(); }
    std::string     getPrefixComment() const noexcept           { return m_comment.prefix(); }
    std::string&    getSuffixComment() noexcept                 { return m_comment.suffix(); }
    std::string     getSuffixComment() const noexcept           { return m_comment.suffix(); }

    void clearComment() noexcept                                { m_comment.clear(); }
    void clearPrefixComment() noexcept                          { m_comment.clearPrefix(); }
    void clearSuffixComment() noexcept                          { m_comment.clearSuffix(); }
    void deleteComment() noexcept                               { m_comment.del(); }
    void deletePrefixComment() noexcept                         { m_comment.delPrefix(); }
    void deleteSuffixComment() noexcept                         { m_comment.delSuffix(); }

    CommentDesign&  getCommentDesign() noexcept                 { return m_comment.commentDesign(); }
    CommentDesign   getCommentDesign() const noexcept           { return m_comment.commentDesign(); }
    void    setCommentDesign(const CommentDesign& design) noexcept
                                                                { m_comment.setDesign(design); }
    void    clearCommentDesign() noexcept                       { m_comment.clearDesign(); }
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= COMMENTS

    virtual bool isEqual(const IElement& other)                 const noexcept { return false; }
    virtual bool isEqualWithComments(const IElement& other)     const noexcept { return false; }

    //OPERATORS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //NOTE: комментарии при присваивании копируются
//    IElement& operator=(const IElement& other) noexcept;
    //NOTE: комментарии при сравнении не учитываются
    bool operator==(const IElement& other) const noexcept       { return (m_type == other.m_type ? isEqual(other) : false); }
    bool operator!=(const IElement& other) const noexcept       { return !(*this == other); }
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= OPERATORS
};


//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

using JPair         = std::pair<std::string, IElement>;
using VPairElement  = std::vector<JPair>;
using VElement      = std::vector<IElement>;

#endif // I_ELEMENT_H
