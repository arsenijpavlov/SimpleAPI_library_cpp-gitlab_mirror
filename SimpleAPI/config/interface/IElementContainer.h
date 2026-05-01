#ifndef I_ELEMENT_CONTAINER_H
#define I_ELEMENT_CONTAINER_H

#include "IElement.h"

#include "IErrorField.h"


namespace simpleapi {
namespace tools {

//абстрактный класс для управления списками элементов
class IElementContainer : public IElement, public IErrorField {
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
    virtual void    clearContainer()                                        noexcept        = 0;
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
    //вывод без комментариев, "tabulation_level == -1" => запись в одну строку
    //вывод без комментариев, ConfigFormat::eONLY_VALUE => запись в одну JSON-строку без комментариев
    virtual std::string toString(const ConfigFormat format = ConfigFormat::eONLY_VALUE,
                                 const CommentDesign &design = {},
                                 const int8_t custom_tabulation_level = -1)     const noexcept  = 0;
    virtual std::string toJsonString(const CommentDesign &design = {},
                                     const int8_t custom_tabulation_level = -1) const noexcept  = 0;
    virtual std::string toIniString(const CommentDesign &design = {},
                                    const int8_t custom_tabulation_level = -1)  const noexcept  = 0;
    virtual std::string toYamlString(const CommentDesign &design = {},
                                     const int8_t custom_tabulation_level = -1) const noexcept  = 0;
    virtual std::string toXmlString(const CommentDesign &design = {},
                                    const int8_t custom_tabulation_level = -1)  const noexcept  = 0;
    // ========================================================================================================== String

    // File ============================================================================================================
    // ============================================================================================================ File

    // Parser ==========================================================================================================
    // парсерами управляет исключительно Config
    // ========================================================================================================== Parser

protected:
    struct KeysBase {
        virtual ~KeysBase()                                 noexcept {}
    };
    struct KeysComments : KeysBase {
        const std::string* m_ptr_comment_str; //указатель для экономии памяти, всё равно объект статичный

        KeysComments(const std::string* p_to_comment)       noexcept : m_ptr_comment_str(p_to_comment)  {}
        KeysComments(const KeysComments& other)             noexcept;
        KeysComments(KeysComments&& other)                  noexcept;
        ~KeysComments()                                     noexcept override                           { m_ptr_comment_str = nullptr; }
        KeysComments& operator=(const KeysComments& other)  noexcept;
        KeysComments& operator=(KeysComments&& other)       noexcept;
    };
    struct KeysValues : KeysBase {
        std::string m_key;
        const Config* m_ptr_remote_cfg;

        KeysValues(std::string key, const Config* p_to_cfg) noexcept : m_key(key), m_ptr_remote_cfg(p_to_cfg) {}
        KeysValues(const KeysValues& other)                 noexcept;
        KeysValues(KeysValues&& other)                      noexcept;
        ~KeysValues()                                       noexcept override                           { m_ptr_remote_cfg = nullptr; }
        KeysValues& operator=(const KeysValues& other)      noexcept;
        KeysValues& operator=(KeysValues&& other)           noexcept;
    };

    static std::vector<std::unique_ptr<KeysBase>> CollectKeys(Config& cfg, std::string prefix = "")     noexcept;
    static std::vector<std::unique_ptr<KeysBase>> CollectKeys(Config& cfg, VString prefixes)            noexcept;
};

} // namespace tools
} // namespace simpleapi

#endif // I_ELEMENT_CONTAINER_H
