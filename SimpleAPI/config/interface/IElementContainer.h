#ifndef I_ELEMENT_CONTAINER_H
#define I_ELEMENT_CONTAINER_H

#include "IElement.h"


//абстрактный класс для управления списками элементов
class IElementContainer : public IElement {
    // Comment =========================================================================================================
protected:
//    CommentDesign m_comment_design;
public:
    virtual void    set_comment(const size_t index, const Comment &content)                 = 0;
    virtual void    set_comment(const size_t index, const std::string &content_before,
                             const std::string &content_after)                              = 0;
    virtual void    set_prefix_comment(const size_t index, const std::string &content)      = 0;
    virtual void    set_suffix_comment(const size_t index, const std::string &content)      = 0;
    virtual Comment&        get_comment(const size_t index)                                 = 0;
    virtual Comment         get_comment(const size_t index)                 const           = 0;
    virtual std::string     get_prefix_comment(const size_t index)          const           = 0;
    virtual std::string     get_suffix_comment(const size_t index)          const           = 0;
    //NOTE: доступы к внутренним значениям строго по set() и get() запросам
//    virtual std::string&    get_prefix_comment(const size_t index)                          = 0;
//    virtual std::string&    get_suffix_comment(const size_t index)                          = 0;
    virtual void    clear_comment(const size_t index)                                       = 0;
    virtual void    clear_prefix_comment(const size_t index)                                = 0;
    virtual void    clear_suffix_comment(const size_t index)                                = 0;
    virtual void    delete_comment(const size_t index)                                      = 0;
    virtual void    delete_prefix_comment(const size_t index)                               = 0;
    virtual void    delete_suffix_comment(const size_t index)                               = 0;
    // ========================================================================================================= Comment

    // Setters =========================================================================================================
    // ========================================================================================================= Setters

    // Getters =========================================================================================================
    virtual Config& get_front()                                                             = 0;
    virtual Config  get_front()                                             const           = 0;
    virtual Config& get_at(const size_t index)                                              = 0;
    virtual Config  get_at(const size_t index)                              const           = 0;
    virtual Config& get_back()                                                              = 0;
    virtual Config  get_back()                                              const           = 0;
    // ========================================================================================================= Getters

    // Modify ==========================================================================================================
    // ========================================================================================================== Modify

    // Adding ==========================================================================================================
    // ========================================================================================================== Adding

    // Removing ========================================================================================================
    virtual void    pop_front()                                                             = 0;
    virtual void    pop_at(const size_t index)                                              = 0;
    virtual void    pop_back()                                                              = 0;
    virtual Config  get_and_pop_front()                                                     = 0;
    virtual Config  get_and_pop_at(const size_t index)                                      = 0;
    virtual Config  get_and_pop_back()                                                      = 0;
    virtual void    erase_front()                                                           = 0;
    virtual void    erase_at(const size_t index)                                            = 0;
    virtual void    erase_back()                                                            = 0;

    void            remove_front()                                                          { erase_front(); }
    void            remove_at(const size_t index)                                           { erase_at(index); }
    void            remove_back()                                                           { erase_back(); }
    // ======================================================================================================== Removing

    // Info ============================================================================================================
    // ============================================================================================================ Info

    // Operators =======================================================================================================
    virtual Config& operator[](const size_t index)                                          = 0;
    virtual Config  operator[](const size_t index)                          const           = 0;
    // ======================================================================================================= Operators

    // Iterators =======================================================================================================
    // ======================================================================================================= Iterators

    // String ==========================================================================================================
    // ========================================================================================================== String

    // File ============================================================================================================
    // ============================================================================================================ File

    // Parser ==========================================================================================================
    virtual void    parse(const std::string& input_string, CommentDesign& design,
                        const ConfigFormat format = ConfigFormat::eJSON,
                        const int8_t tabulation_level = 0)                                      = 0;
    virtual void    parse(const std::string& input_string,
                        const ConfigFormat format = ConfigFormat::eJSON,
                        const bool parse_comments = true, const int8_t tabulation_level = 0)    = 0;

    virtual void    parse(std::string&& input_string, CommentDesign& design,
                        const ConfigFormat format = ConfigFormat::eJSON,
                        const int8_t tabulation_level = 0)                                      = 0;
    virtual void    parse(std::string&& input_string,
                        const ConfigFormat format = ConfigFormat::eJSON,
                        const bool parse_comments = true, const int8_t tabulation_level = 0)    = 0;

    virtual void    parseJson(const std::string& input_string, CommentDesign& design,
                        const int8_t tabulation_level = 0)                                      = 0;
    virtual void    parseJson(const std::string& input_string,
                        const bool parse_comments = true, const int8_t tabulation_level = 0)    = 0;

    virtual void    parseJson(std::string&& input_string, CommentDesign& design,
                        const int8_t tabulation_level = 0)                                      = 0;
    virtual void    parseJson(std::string&& input_string,
                        const bool parse_comments = true, const int8_t tabulation_level = 0)    = 0;

    virtual void    parseIni(const std::string& input_string, CommentDesign& design,
                        const int8_t tabulation_level = 0)                                      = 0;
    virtual void    parseIni(const std::string& input_string,
                        const bool parse_comments = true, const int8_t tabulation_level = 0)    = 0;

    virtual void    parseIni(std::string&& input_string, CommentDesign& design,
                        const int8_t tabulation_level = 0)                                      = 0;
    virtual void    parseIni(std::string&& input_string,
                        const bool parse_comments = true, const int8_t tabulation_level = 0)    = 0;

    virtual void    parseYaml(const std::string& input_string, CommentDesign& design,
                        const int8_t tabulation_level = 0)                                      = 0;
    virtual void    parseYaml(const std::string& input_string,
                        const bool parse_comments = true, const int8_t tabulation_level = 0)    = 0;

    virtual void    parseYaml(std::string&& input_string, CommentDesign& design,
                        const int8_t tabulation_level = 0)                                      = 0;
    virtual void    parseYaml(std::string&& input_string,
                        const bool parse_comments = true, const int8_t tabulation_level = 0)    = 0;

    virtual void    parseXml(const std::string& input_string, CommentDesign& design,
                        const int8_t tabulation_level = 0)                                      = 0;
    virtual void    parseXml(const std::string& input_string,
                        const bool parse_comments = true, const int8_t tabulation_level = 0)    = 0;

    virtual void    parseXml(std::string&& input_string, CommentDesign& design,
                        const int8_t tabulation_level = 0)                                      = 0;
    virtual void    parseXml(std::string&& input_string,
                        const bool parse_comments = true, const int8_t tabulation_level = 0)    = 0;

    // ========================================================================================================== Parser
};

#endif // I_ELEMENT_CONTAINER_H
