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
    IElement()                                                  noexcept            { m_type = ValueType::eNull; }
public:
    virtual ~IElement()                                         noexcept            {}

    //общее
    //COMMENTS =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    void            addComment(const Comment& content)          noexcept            { m_comment = content; }
    void            addComment(const std::string &content_before, const std::string &content_after)
                                                                noexcept            { m_comment = Comment(content_before, content_after); }
    void            addPrefixComment(const std::string& content)noexcept            { m_comment.setPrefix(content); }
    void            addSuffixComment(const std::string& content)noexcept            { m_comment.setSuffix(content); }

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
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= COMMENT

    //индивидуальное

    // Setters =========================================================================================================
        // заполнение строго через конструкторы или operator=
    // ========================================================================================================= Setters

    // Getters =========================================================================================================
        // внутри у каждого потомка должна быть реализация getValue()
    // ========================================================================================================= Getters

    // Modify ==========================================================================================================
    //NOTE: не путать с remove(), здесь просто сброс комментариев, сброс значения до:
    virtual void    clear()                                     noexcept            = 0;
    // ========================================================================================================== Modify

    // Info ============================================================================================================
    ValueType       getType()                                   const noexcept      { return m_type; }
    virtual bool    isContainer()                               const noexcept      { return false; }
    virtual bool    isMapContainer()                            const noexcept      { return false; }

    bool            isEqual(const IElement& other, const bool compare_comments = false) const;

    //числа, контейнеры(размер), строки(длина в видимых символах)
    virtual size_t  size()                                      const noexcept      { return 0; }
    // ============================================================================================================ Info

    // Operators =======================================================================================================
        // operator=() не реализован для потомков
        // operator==() не реализован для потомков
        // operator!=() не реализован для потомков
        // operator>() не реализован для потомков
        // operator>=() не реализован для потомков
        // operator<() не реализован для потомков
        // operator<=() не реализован для потомков
    // ======================================================================================================= Operators

    // String ==========================================================================================================
    //вывод без комментариев, "tabulation_level == -1" => запись в одну строку
    virtual std::string toString(const ConfigFormat format = ConfigFormat::eONLY_VALUE,
                                 const int8_t tabulation_level = 0,
                                 const CommentDesign &design = {})  const noexcept  = 0;
    // NOTE: работа с std::cout реализована в Config
    // ========================================================================================================== String

    // File ============================================================================================================
        // работа с файлами и парсингом не предусмотрена для этого класса
    // ============================================================================================================ File

    // Parser ==========================================================================================================
        // работа с файлами и парсингом не предусмотрена для этого класса
    // ========================================================================================================== Parser

    // Iterators =======================================================================================================
        //индивидуально для ElementContainer классов
    // ======================================================================================================= Iterators
};

//----------------------------------------------------------------------------------------------------------------------
//TODO: добавить СЮДА Is<TYPE>(std::string) парсинги?
//----------------------------------------------------------------------------------------------------------------------

#endif // I_ELEMENT_H
