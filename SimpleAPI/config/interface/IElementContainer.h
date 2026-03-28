#ifndef I_ELEMENT_CONTAINER_H
#define I_ELEMENT_CONTAINER_H

#include "IElement.h"


//абстрактный класс для управления списками элементов
class IElementContainer : public IElement {
    // Comment =========================================================================================================
protected:
//    CommentDesign m_comment_design;
public:
    virtual void    set_comment(const size_t& index, const Comment &content)                = 0;
    virtual void    set_comment(const size_t& index, const std::string &content_before,
                                const std::string &content_after)                           = 0;
    virtual void    set_prefix_comment(const size_t& index, const std::string &content)     = 0;
    virtual void    set_suffix_comment(const size_t& index, const std::string &content)     = 0;
    virtual Comment&        get_comment(const size_t& index)                                = 0;
    virtual Comment         get_comment(const size_t& index)                 const          = 0;
    virtual std::string     get_prefix_comment(const size_t& index)          const          = 0;
    virtual std::string     get_suffix_comment(const size_t& index)          const          = 0;
    //NOTE: доступы к внутренним значениям строго по set() и get() запросам
//    virtual std::string&    get_prefix_comment(const size_t& index)                          = 0;
//    virtual std::string&    get_suffix_comment(const size_t& index)                          = 0;
    virtual void    clear_comment(const size_t& index)                                      = 0;
    virtual void    clear_prefix_comment(const size_t& index)                               = 0;
    virtual void    clear_suffix_comment(const size_t& index)                               = 0;
    virtual void    delete_comment(const size_t& index)                                     = 0;
    virtual void    delete_prefix_comment(const size_t& index)                              = 0;
    virtual void    delete_suffix_comment(const size_t& index)                              = 0;
    // ========================================================================================================= Comment

    // Setters =========================================================================================================
    // ========================================================================================================= Setters

    // Getters =========================================================================================================
    virtual Config& get_front()                                                             = 0;
    virtual Config  get_front()                                             const           = 0;
    virtual Config& get_at(const size_t& index)                                             = 0;
    virtual Config  get_at(const size_t& index)                             const           = 0;
    virtual Config& get_back()                                                              = 0;
    virtual Config  get_back()                                              const           = 0;
    // ========================================================================================================= Getters

    // Modify ==========================================================================================================
    // ========================================================================================================== Modify

    // Adding ==========================================================================================================
    // ========================================================================================================== Adding

    // Removing ========================================================================================================
    virtual void    pop_front()                                             noexcept        = 0;
    virtual void    pop_at(const size_t& index)                             noexcept        = 0;
    virtual void    pop_back()                                              noexcept        = 0;

    virtual Config  get_and_pop_front()                                                     = 0;
    virtual Config  get_and_pop_at(const size_t& index)                                     = 0;
    virtual Config  get_and_pop_back()                                                      = 0;

    //WARNING: в наследуемых классах нужно явно указать using объявление имён методов
    void            erase_front()                                           noexcept        { return pop_front(); };
    void            erase_at(const size_t& index)                           noexcept        { return pop_at(index); };
    void            erase_back()                                            noexcept        { return pop_back(); };

    void            remove_front()                                                          { return erase_front(); }
    void            remove_at(const size_t& index)                                          { return erase_at(index); }
    void            remove_back()                                                           { return erase_back(); }
    // ======================================================================================================== Removing

    // Info ============================================================================================================
    // ============================================================================================================ Info

    // Operators =======================================================================================================
    Config&         operator[](const size_t& index)                                         { return get_at(index); }
    // функция требует явной реализации в дочерних классах --> { return get_at(index); }
    virtual Config  operator[](const size_t& index)                         const           = 0;
    virtual bool    operator==(const Config& config)                        const           = 0;
    // ======================================================================================================= Operators

    // Iterators =======================================================================================================
    // ======================================================================================================= Iterators

    // String ==========================================================================================================
    // ========================================================================================================== String

    // File ============================================================================================================
    // ============================================================================================================ File

    // Parser ==========================================================================================================
    // NOTE: все методы ниже при ошибке вернут строку, описывающую ошибку
    virtual std::string parse(const std::string& input_string, CommentDesign& design,
                              const ConfigFormat format = ConfigFormat::eJSON,
                              const int8_t tabulation_level = 0)                                        noexcept    = 0;
    virtual std::string parse(const std::string& input_string,
                              const ConfigFormat format = ConfigFormat::eJSON,
                              const bool parse_comments = true,
                              const int8_t tabulation_level = 0)                                        noexcept    = 0;

    virtual std::string parse(std::string&& input_string, CommentDesign& design,
                              const ConfigFormat format = ConfigFormat::eJSON,
                              const int8_t tabulation_level = 0)                                        noexcept    = 0;
    virtual std::string parse(std::string&& input_string,
                              const ConfigFormat format = ConfigFormat::eJSON,
                              const bool parse_comments = true,
                              const int8_t tabulation_level = 0)                                        noexcept    = 0;

    virtual std::string parseJson(const std::string& input_string, CommentDesign& design)               noexcept    = 0;
    virtual std::string parseJson(const std::string& input_string,
                                  const bool parse_comments = true)                                     noexcept    = 0;

    virtual std::string parseJson(std::string&& input_string, CommentDesign& design)                    noexcept    = 0;
    virtual std::string parseJson(std::string&& input_string,
                                  const bool parse_comments = true)                                     noexcept    = 0;

    virtual std::string parseIni(const std::string& input_string, CommentDesign& design)                noexcept    = 0;
    virtual std::string parseIni(const std::string& input_string,
                                 const bool parse_comments = true)                                      noexcept    = 0;

    virtual std::string parseIni(std::string&& input_string, CommentDesign& design)                     noexcept    = 0;
    virtual std::string parseIni(std::string&& input_string, const bool parse_comments = true)          noexcept    = 0;

    virtual std::string parseYaml(const std::string& input_string, CommentDesign& design,
                                  const int8_t tabulation_level = 0)                                    noexcept    = 0;
    virtual std::string parseYaml(const std::string& input_string,
                                  const bool parse_comments = true, const int8_t tabulation_level = 0)  noexcept    = 0;

    virtual std::string parseYaml(std::string&& input_string, CommentDesign& design,
                                  const int8_t tabulation_level = 0)                                    noexcept    = 0;
    virtual std::string parseYaml(std::string&& input_string,
                                  const bool parse_comments = true, const int8_t tabulation_level = 0)  noexcept    = 0;

    virtual std::string parseXml(const std::string& input_string, CommentDesign& design)                noexcept    = 0;
    virtual std::string parseXml(const std::string& input_string,
                                 const bool parse_comments = true)                                      noexcept    = 0;

    virtual std::string parseXml(std::string&& input_string, CommentDesign& design)                     noexcept    = 0;
    virtual std::string parseXml(std::string&& input_string,
                                 const bool parse_comments = true)                                      noexcept    = 0;

    // ========================================================================================================== Parser
};

#endif // I_ELEMENT_CONTAINER_H
