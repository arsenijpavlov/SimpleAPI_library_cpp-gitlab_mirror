#pragma once

#include "../Comment.h"


namespace simpleapi {
namespace tools {

class IComment {
protected:
    Comment     m_comment;
public:
    virtual ~IComment() {}

    //COMMENTS =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    void            setComment(const Comment& content)          noexcept            { m_comment = content; }
    void            setComment(const std::string &content_before, const std::string &content_after)
                                                                noexcept            { m_comment = Comment(content_before, content_after); }
    void            setPrefixComment(const std::string& content)noexcept            { m_comment.setPrefix(content); }
    void            setSuffixComment(const std::string& content)noexcept            { m_comment.setSuffix(content); }

    Comment&        getComment()                                noexcept            { return m_comment; }
    Comment         getComment()                                const noexcept      { return m_comment; }
    std::string     getPrefixComment()                          const noexcept      { return m_comment.prefix(); }
    std::string     getSuffixComment()                          const noexcept      { return m_comment.suffix(); }

    //NOTE: доступы к внутренним значениям строго по set() и get() запросам
    //std::string&    getPrefixComment()                          noexcept            { return m_comment.prefix(); }
    //std::string&    getSuffixComment()                          noexcept            { return m_comment.suffix(); }

    void            clearComment()                              noexcept            { m_comment.clear(); }
    void            clearPrefixComment()                        noexcept            { m_comment.clearPrefix(); }
    void            clearSuffixComment()                        noexcept            { m_comment.clearSuffix(); }
    void            deleteComment()                             noexcept            { m_comment.del(); }
    void            deletePrefixComment()                       noexcept            { m_comment.delPrefix(); }
    void            deleteSuffixComment()                       noexcept            { m_comment.delSuffix(); }

    CommentDesign&  getCommentDesign()                          noexcept            { return m_comment.commentDesign(); }
    CommentDesign   getCommentDesign()                          const noexcept      { return m_comment.commentDesign(); }
    void            setCommentDesign(const CommentDesign &design)
                                                                noexcept            { m_comment.setDesign(design); }
    void            clearCommentDesign()                        noexcept            { m_comment.clearDesign(); }
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= COMMENT
};

} // namespace tools
} // namespace simpleapi
