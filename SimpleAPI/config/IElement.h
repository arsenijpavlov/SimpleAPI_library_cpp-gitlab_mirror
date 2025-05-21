#ifndef I_ELEMENT_H
#define I_ELEMENT_H

#include "Comment.h"
#include "ConfigCommon.h"

#include <map>
#include <vector>


//предобъявление
class Config;
class IElement;

using JPair         = std::pair<std::string, std::shared_ptr<IElement>>;
using VPairElement  = std::vector<JPair>;
using VElement      = std::vector<std::shared_ptr<IElement>>;

//базовый класс, в идеале используется только для ссылки на производный
class IElement {
protected:
    ValueType   m_type;
    Comment     m_comment;
    IElement()                                                  noexcept            {}
public:
    virtual ~IElement()                                         noexcept            {}

    // Getters =========================================================================================================
//    virtual bool&           getBool();
//    virtual bool            getBool()                           const;
//    virtual long double&    getNumber();
//    virtual long double     getNumber()                         const;
//    virtual std::string&    getString();
//    virtual std::string     getString()                         const;

    // вложенные контейнеры
//    virtual Config&         get_front();
//    virtual Config          get_front()                         const;
//    virtual Config&         get_at(const size_t index);
//    virtual Config          get_at(const size_t index)          const;
//    virtual Config&         get_at(const std::string& key);
//    virtual Config          get_at(const std::string& key)      const;
//    virtual Config&         get_back();
//    virtual Config          get_back()                          const;
    // ========================================================================================================= Getters

    // Info ============================================================================================================
    ValueType               getType()                           const noexcept      { return m_type; }
    virtual bool            isContainer()                       const noexcept      { return false; }
    virtual bool            isEqual(const Config& other)        const noexcept      { return false; }
    virtual bool            isEqual(const bool other)           const noexcept      { return false; }
    virtual bool            isEqual(const long double& other)   const noexcept      { return false; }
    virtual bool            isEqual(const std::string& other)   const noexcept      { return false; }
    virtual size_t          size()                              const noexcept      { return 0; }
    // ============================================================================================================ Info

    // Operators =======================================================================================================
//    bool                    operator==(const Config& other)     const noexcept      { return isEqual(other); }
//    bool                    operator==(const bool other)        const noexcept      { return isEqual(other); }
//    bool                    operator==(const long double& other)const noexcept      { return isEqual(other); }
//    bool                    operator==(const std::string& other)const noexcept      { return isEqual(other); }

//    bool                    operator!=(const Config& other)     const noexcept      { return !isEqual(other); }
//    bool                    operator!=(const bool other)        const noexcept      { return !isEqual(other); }
//    bool                    operator!=(const long double& other)const noexcept      { return !isEqual(other); }
//    bool                    operator!=(const std::string& other)const noexcept      { return !isEqual(other); }

    //TODO: operator<<

    //числа, контейнеры(размер), строки(длина в видимых символах)
//    virtual bool            operator>(const Config& other)      const noexcept      { return false; }
//    virtual bool            operator>=(const Config& other)     const noexcept      { return false; }
//    virtual bool            operator<(const Config& other)      const noexcept      { return false; }
//    virtual bool            operator<=(const Config& other)     const noexcept      { return false; }
//    virtual Config&         operator[](const size_t index)      noexcept;
//    virtual Config          operator[](const size_t index)      const noexcept;
//    virtual Config&         operator[](const std::string& key)  noexcept;
//    virtual Config          operator[](const std::string& key)  const noexcept;
    // ======================================================================================================= Operators

    //COMMENTS =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    void            addComment(const Comment& content)          noexcept            { m_comment = content; }
    void            addComment(const std::string &content_before, const std::string &content_after)
                                                                noexcept            { m_comment = Comment(content_before, content_after); }
    void            addPrefixComment(const std::string& content) noexcept           { m_comment.setPrefix(content); }
    void            addSuffixComment(const std::string& content) noexcept           { m_comment.setSuffix(content); }

    Comment&        getComment()                                noexcept            { return m_comment; }
    Comment         getComment()                                const noexcept      { return m_comment; }
    std::string&    getPrefixComment()                          noexcept            { return m_comment.prefix(); }
    std::string     getPrefixComment()                          const noexcept      { return m_comment.prefix(); }
    std::string&    getSuffixComment()                          noexcept            { return m_comment.suffix(); }
    std::string     getSuffixComment()                          const noexcept      { return m_comment.suffix(); }

    void            clearComment()                              noexcept            { m_comment.clear(); }
    void            clearPrefixComment()                        noexcept            { m_comment.clearPrefix(); }
    void            clearSuffixComment()                        noexcept            { m_comment.clearSuffix(); }
    void            deleteComment()                             noexcept            { m_comment.del(); }
    void            deletePrefixComment()                       noexcept            { m_comment.delPrefix(); }
    void            deleteSuffixComment()                       noexcept            { m_comment.delSuffix(); }

    CommentDesign&  getCommentDesign()                          noexcept            { return m_comment.commentDesign(); }
    CommentDesign   getCommentDesign()                          const noexcept      { return m_comment.commentDesign(); }
    void            setCommentDesign(const CommentDesign& design)
                                                                noexcept            { m_comment.setDesign(design); }
    void            clearCommentDesign()                        noexcept            { m_comment.clearDesign(); }
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= COMMENTS

    // String ==========================================================================================================
    //вывод без комментариев, "tabulation_level == -1" => запись в одну строку
    virtual std::string     toString(const ConfigFormat format = ConfigFormat::eONLY_VALUE,
                                const int8_t tabulation_level = 0, const CommentDesign &design = {})
                                                                const noexcept;
    // ========================================================================================================== String

    // Iterators =======================================================================================================
    // (!) Для foreach итераторы необходимо кастить вручную к нужному типу; за основу брать типы ниже
//    /* (!) */ void*                 begin();
//    /* (!) */ void*                 end();
//    /* (!) */ void*                 cbegin()                    const;
//    /* (!) */ void*                 cend()                      const;
    // (!) Для foreach итераторы необходимо кастить вручную к нужному типу; за основу брать типы ниже
    //Array
//    VElement::iterator              array_begin();
//    VElement::iterator              array_end();
//    VElement::const_iterator        array_cbegin()              const;
//    VElement::const_iterator        array_cend()                const;
    //Json
//    VPairElement::iterator          json_begin();
//    VPairElement::iterator          json_end();
//    VPairElement::const_iterator    json_cbegin()               const;
//    VPairElement::const_iterator    json_cend()                 const;
    // ======================================================================================================= Iterators
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

#endif // I_ELEMENT_H
