#ifndef I_ELEMENT_H
#define I_ELEMENT_H

#include "Comment.h"
#include "ConfigCommon.h"
#include "ConfigDefines.h"
//#include "ConfigDefines.h"

#include <map>
#include <vector>


//предобъявление
class ElementArray;
class ElementJson;
class IElement;

using JPair         = std::pair<std::string, std::shared_ptr<IElement>>;
using VPairElement  = std::vector<JPair>;
using VElement      = std::vector<std::shared_ptr<IElement>>;

//базовый класс
//NOTE: равносилен ELEMENT_NULL
class IElement {
protected:
    ValueType m_type;

public:
    IElement()                      noexcept    { init(); }
    IElement(const IElement& value) noexcept    { setValue(value); }
    IElement(IElement&& value)      noexcept    { setValue(value); }

    IElement(const bool value)      noexcept    { setValue(value); }
    __ONLY_NUMBER_TYPES__(T)
    IElement(T&& value)             noexcept    { setValue(static_cast<long double&>(value)); }
    __ONLY_STRING_TYPES__(T)
    IElement(T&& value)             noexcept    { setValue(std::string(value)); }
    IElement(ElementArray&& value)  noexcept    { setValue(value); }
    IElement(ElementJson&& value)   noexcept    { setValue(value); }

    virtual ~IElement()             noexcept    {}

private:
    //создание ПУСТОГО(NULL) элемента
    void init()                     noexcept    { setValue(); }

public:
    // Operators =======================================================================================================
    virtual IElement&   operator=(const IElement& other)  noexcept;
    virtual IElement&   operator=(IElement&& other)       noexcept;

    //WARNING: комментарии не учитываются!
    virtual bool        operator==(const IElement& other) const noexcept    { return isEqual(other); }
    virtual bool        operator!=(const IElement& other) const noexcept    { return !isEqual(other); }

    //числа, контейнеры(размер), строки(длина в видимых символах)
    virtual bool        operator>(const IElement& other)  const noexcept;
    virtual bool        operator>=(const IElement& other) const noexcept;
    virtual bool        operator<(const IElement& other)  const noexcept;
    virtual bool        operator<=(const IElement& other) const noexcept;

    //контейнеры
    //FIXME: на подумать: https://en.cppreference.com/w/cpp/language/operators
//    IElement& operator<<(const IElement& other)   noexcept;       //аналог push_back()
//    IElement& operator<<(const IElement& other)   noexcept;       //аналог push_back()
//    IElement  operator>>()                        noexcept;       //аналог pop_front()
    virtual IElement&   operator[](const size_t index)    noexcept;       //(ARRAY, JSON)
    virtual IElement    operator[](const size_t index)    const noexcept; //(ARRAY, JSON)
    virtual IElement&   operator[](const std::string& key) noexcept;       //(JSON)
    virtual IElement    operator[](const std::string& key) const noexcept; //(JSON)
    // ======================================================================================================= Operators

    // Info ============================================================================================================
    ValueType       getType()       const noexcept  { return m_type; }
    bool            isNull()        const noexcept  { return getType() == ValueType::eNull; }
    bool            isBool()        const noexcept  { return getType() == ValueType::eBool; }
    bool            isNumber()      const noexcept  { return getType() == ValueType::eNumber; }
    bool            isString()      const noexcept  { return getType() == ValueType::eString; }
    bool            isArray()       const noexcept  { return getType() == ValueType::eArray; }
    bool            isJson()        const noexcept  { return getType() == ValueType::eJson; }
//TODO: bool isYaml() const noexcept
//TODO: bool isXml() const noexcept
    bool            isContainer()   const noexcept;

    virtual size_t  size()          const noexcept;
    virtual bool    isEqual(const IElement& other, const bool compare_comments = false) const noexcept;
    // ============================================================================================================ Info

    // Getters =========================================================================================================
    bool&           getBool();
    bool            getBool()       const;
    long double&    getNumber();
    long double     getNumber()     const;
    std::string&    getString();
    std::string     getString()     const;
    ElementArray&   getArray();
    ElementArray    getArray()      const;
    ElementJson&    getJson();
    ElementJson     getJson()       const;

    // вложенные контейнеры
    IElement&       get_front();
    IElement        get_front()                     const;
    IElement&       get_at(const size_t index);
    IElement        get_at(const size_t index)      const;
    IElement&       get_at(const std::string& key);
    IElement        get_at(const std::string& key)  const;
    IElement&       get_back();
    IElement        get_back()                      const;
    // ========================================================================================================= Getters

    // Setters =========================================================================================================
    IElement&       setValue()                              noexcept;
    IElement&       setValue(const IElement& value)         noexcept;
    IElement&       setValue(const IElement&& value)        noexcept;
    IElement&       setValue(const bool value)              noexcept;
    IElement&       setValue(const long double& value)      noexcept;
    IElement&       setValue(const std::string& value)      noexcept;
    IElement&       setValue(const ElementArray& value)     noexcept;
    IElement&       setValue(const ElementJson& value)      noexcept;
    // ========================================================================================================= Setters

    //COMMENTS =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
protected:
    Comment m_comment;
public:
    IElement&       addComment(const Comment& content)      noexcept                { m_comment = content; }
    IElement&       addComment(const std::string &content_before, const std::string &content_after) noexcept
    																				{ m_comment = Comment(content_before, content_after); }
    IElement&       addPrefixComment(const std::string& content) noexcept           { m_comment.setPrefix(content); }
    IElement&       addSuffixComment(const std::string& content) noexcept           { m_comment.setSuffix(content); }

    Comment&        getComment()                            noexcept                { return m_comment; }
    Comment         getComment()                            const noexcept          { return m_comment; }
    std::string&    getPrefixComment()                      noexcept                { return m_comment.prefix(); }
    std::string     getPrefixComment()                      const noexcept          { return m_comment.prefix(); }
    std::string&    getSuffixComment()                      noexcept                { return m_comment.suffix(); }
    std::string     getSuffixComment()                      const noexcept          { return m_comment.suffix(); }

    IElement&       clearComment()                          noexcept                { m_comment.clear(); }
    IElement&       clearPrefixComment()                    noexcept                { m_comment.clearPrefix(); }
    IElement&       clearSuffixComment()                    noexcept                { m_comment.clearSuffix(); }
    IElement&       deleteComment()                         noexcept                { m_comment.del(); }
    IElement&       deletePrefixComment()                   noexcept                { m_comment.delPrefix(); }
    IElement&       deleteSuffixComment()                   noexcept                { m_comment.delSuffix(); }

    CommentDesign&  getCommentDesign()                      noexcept                { return m_comment.commentDesign(); }
    CommentDesign   getCommentDesign()                      const noexcept          { return m_comment.commentDesign(); }
    IElement&       setCommentDesign(const CommentDesign& design)
        													noexcept                { m_comment.setDesign(design); }
    IElement&       clearCommentDesign()                    noexcept                { m_comment.clearDesign(); }
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= COMMENTS

    // String ==========================================================================================================
//TODO:    std::string toStringWithComments();
    virtual std::string toString(const ConfigFormat format, const CommentDesign &design,
                                 const int8_t tabulation_level = 0) const noexcept;
    //вывод без комментариев, "tabulation_level == -1" => запись в одну строку
    virtual std::string toString(const ConfigFormat format = ConfigFormat::eJSON, const int8_t tabulation_level = 0) const noexcept;
    // ========================================================================================================== String

    // File ============================================================================================================
    //return - получившийся распаршенный корневой элемент, ElementNull если не удалось чтение
    IElement&       readFile(const std::string& file_path, const ConfigFormat format,
                        const bool with_comments = false, std::string* error_log = nullptr);
    IElement&       readFileJson(const std::string& file_path, const bool with_comments = 0,
                        std::string* error_log = nullptr);
    IElement&       readFileIni(const std::string& file_path, const bool with_comments = 0,
                        std::string* error_log = nullptr);

    //return - удалось записать файл или нет
    bool            writeFile(const std::string& file_path, const ConfigFormat format,
                        const bool with_comments = 0)                                           noexcept;
    bool            writeFileJson(const std::string& file_path, const bool with_comments = 0)   noexcept;
    bool            writeFileIni(const std::string& file_path, const bool with_comments = 0)    noexcept;
    // ============================================================================================================ File

    // Parser ==========================================================================================================
    IElement&       parse(const std::string& content, const ConfigFormat format,
                        const bool with_comments = false, std::string* error_log = nullptr);
    IElement&       parseJson(const std::string& content, const bool with_comments = 0,
                        std::string* error_log = nullptr);
    IElement&       parseIni(const std::string& content, const bool with_comments = 0,
                        std::string* error_log = nullptr);
    // ========================================================================================================== Parser

    // Iterators =======================================================================================================
    // (!) Для foreach итераторы необходимо кастить вручную к нужному типу; за основу брать типы ниже
    /* (!) */ void*                 begin();
    /* (!) */ void*                 end();
    /* (!) */ void*                 cbegin()        const;
    /* (!) */ void*                 cend()          const;
    // (!) Для foreach итераторы необходимо кастить вручную к нужному типу; за основу брать типы ниже
    //Array
    VElement::iterator              array_begin();
    VElement::iterator              array_end();
    VElement::const_iterator        array_cbegin()  const;
    VElement::const_iterator        array_cend()    const;
    //Json
    VPairElement::iterator          json_begin();
    VPairElement::iterator          json_end();
    VPairElement::const_iterator    json_cbegin()   const;
    VPairElement::const_iterator    json_cend()     const;
    // ======================================================================================================= Iterators
};

//STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC
//return - получившийся распаршенный корневой элемент, ElementNull если не удалось чтение
IElement    ReadFile(const std::string& file_path, const ConfigFormat format,
                const bool with_comments = false, std::string* error_log = nullptr);
IElement    ReadFileJson(const std::string& file_path, const bool with_comments = 0,
                std::string* error_log = nullptr);
IElement    ReadFileIni(const std::string& file_path, const bool with_comments = 0,
                std::string* error_log = nullptr);

//return - удалось записать файл или нет
bool        WriteFile(const IElement& config, const std::string& file_path,
                const ConfigFormat format, const bool with_comments = 0)    noexcept;
bool        WriteFileJson(const IElement& config, const std::string& file_path,
                const bool with_comments = 0)                               noexcept;
bool        WriteFileIni(const IElement& config, const std::string& file_path,
                const bool with_comments = 0)                               noexcept;

IElement    Parse(const std::string& content, const ConfigFormat format,
                const bool with_comments = false, std::string* error_log = nullptr);
IElement    ParseJson(const std::string& content, const bool with_comments = 0,
                std::string* error_log = nullptr);
IElement    ParseIni(const std::string& content, const bool with_comments = 0,
                std::string* error_log = nullptr);
//STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC


//    //PRINTING =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//    //для рекурсивного вызова, без комментариев, в одну строку
//    virtual std::string toString(const ConfigFormat format = ConfigFormat::eJSON,
//                                  const int8_t tabulation_level = 0) const noexcept
//                                                                { return ""; }
//    //для рекурсивного вызова, с использованием комментариев
//    virtual std::string toString(const ConfigFormat format, const CommentDesign &design,
//                                  const int8_t tabulation_level = 0) const noexcept
//                                                                { return ""; }
//    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= PRINTING



//    virtual bool isEqual(const IElement& other)                 const noexcept { return false; }
//    virtual bool isEqualWithComments(const IElement& other)     const noexcept { return false; }

//    //OPERATORS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//    //NOTE: комментарии при присваивании копируются
////    IElement& operator=(const IElement& other) noexcept;
//    //NOTE: комментарии при сравнении не учитываются
//    bool operator==(const IElement& other) const noexcept       { return (m_type == other.m_type ? isEqual(other) : false); }
//    bool operator!=(const IElement& other) const noexcept       { return !(*this == other); }
//    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= OPERATORS
//};


//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

#endif // I_ELEMENT_H
