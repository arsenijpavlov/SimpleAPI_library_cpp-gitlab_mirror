#ifndef ELEMENT_JSON_H
#define ELEMENT_JSON_H

#include "ConfigDefines.h"
#include "ElementArray.h"


class ElementJson : public IElementContainer {
protected:
    shared_VPairElement m_values;

public:
    ElementJson()                                                               noexcept        { init(); }
    explicit ElementJson(const ElementJson& json) noexcept {
        init();
        m_values = json.m_values;
    }
    explicit ElementJson(const JPair& pair)                                     noexcept;
    //разнесено для решения конфликта, не изменять следующие два конструктора!
    explicit ElementJson(const std::string& input_string) noexcept {
        init();
        parseJson(input_string);
    }
    explicit ElementJson(const std::string& input_string, const ConfigFormat config_format,
                const bool enable_comment = false) noexcept {
        init();
        parse(input_string, config_format, enable_comment);
    }
    __ONLY_ALLOWED_TYPES__(T)
    explicit ElementJson(const std::string& key, const T& value) noexcept {
        init();
        put(key, value);
    }
    explicit ElementJson(const VPairElement& vec) noexcept {
        init();
        m_values = vec;
    }
    explicit ElementJson(const IElement& element)                               noexcept        { init(); }
    ~ElementJson()                                                              noexcept        {}

    // Comment =========================================================================================================
    void    add_comment(const size_t index, const Comment &content)                             override;
    void    add_comment(const size_t index, const std::string &content_before,
                     const std::string &content_after)                                          override;
    void    add_prefix_comment(const size_t index, const std::string &content)                  override;
    void    add_suffix_comment(const size_t index, const std::string &content)                  override;
    Comment&        get_comment(const size_t index)                                             override;
    Comment         get_comment(const size_t index)                             const           override;
    std::string&    get_prefix_comment(const size_t index)                                      override;
    std::string     get_prefix_comment(const size_t index)                      const           override;
    std::string&    get_suffix_comment(const size_t index)                                      override;
    std::string     get_suffix_comment(const size_t index)                      const           override;
    void    clear_comment(const size_t index)                                                   override;
    void    clear_prefix_comment(const size_t index)                                            override;
    void    clear_suffix_comment(const size_t index)                                            override;
    void    delete_comment(const size_t index)                                                  override;
    void    delete_prefix_comment(const size_t index)                                           override;
    void    delete_suffix_comment(const size_t index)                                           override;

    void    add_comment(const std::string& key, const Comment &content);
    void    add_comment(const std::string& key, const std::string &content_before,
                     const std::string &content_after);
    void    add_prefix_comment(const std::string& key, const std::string &content);
    void    add_suffix_comment(const std::string& key, const std::string &content);
    Comment&        get_comment(const std::string& key);
    Comment         get_comment(const std::string& key)                         const;
    std::string&    get_prefix_comment(const std::string& key);
    std::string     get_prefix_comment(const std::string& key)                  const;
    std::string&    get_suffix_comment(const std::string& key);
    std::string     get_suffix_comment(const std::string& key)                  const;
    void    clear_comment(const std::string& key);
    void    clear_prefix_comment(const std::string& key);
    void    clear_suffix_comment(const std::string& key);
    void    delete_comment(const std::string& key);
    void    delete_prefix_comment(const std::string& key);
    void    delete_suffix_comment(const std::string& key);
    // ========================================================================================================= Comment

    // Setters =========================================================================================================
private:
    void    init()                                                              noexcept        { m_type = ValueType::eArray; }
public:
    void    set(const std::string& key, const Config& new_value,
             bool update = true)                                                noexcept;
    void    set(const std::string& key, Config&& new_value,
             bool update = true)                                                noexcept;
    void    set_if_null(const std::string& key, const Config& new_value)        noexcept;
    void    set_if_null(const std::string& key, Config&& new_value)             noexcept;
    // ========================================================================================================= Setters

    // Getters =========================================================================================================
    Config& get_front()                                                                         override;
    Config  get_front()                                                         const           override;
    Config& get_at(const size_t index)                                                          override;
    Config  get_at(const size_t index)                                          const           override;
    Config& get_back()                                                                          override;
    Config  get_back()                                                          const           override;
    //если элемента с таким ключом не существует - создать пустой и вернуть его
    Config& get_at(const std::string& key)                                      noexcept;
    Config  get_at(const std::string& key)                                      const noexcept;
    Config& get_or_default_at(const std::string& key, Config&& default_value)   noexcept;
    Config  get_or_default_at(const std::string& key, Config&& default_value)   const noexcept;
    // ========================================================================================================= Getters

    // Modify ==========================================================================================================
    void    clear()                                                             noexcept        override;
    // ========================================================================================================== Modify

    // Adding ==========================================================================================================
    //одиночные элементы
    void    insert_front(const std::string& key, const Config& value)           noexcept;
    void    insert_front(const std::string& key, Config&& value)                noexcept;
    //@return - был ли изменён индекс вниз при перезаписи ключа
    bool    insert_at(const size_t index, const std::string& key,
                   const Config& value)                                         noexcept;
    //@return - был ли изменён индекс вниз при перезаписи ключа
    bool    insert_at(const size_t index, const std::string& key,
                   Config&& value)                                              noexcept;
    void    insert_at(VPairElement::iterator iterator, const std::string& key,
                   const Config& value);
    void    insert_at(VPairElement::iterator iterator, const std::string& key,
                   Config&& value);
    void    insert_back(const std::string& key, const Config& value)            noexcept;
    void    insert_back(const std::string& key, Config&& value)                 noexcept;

    //если нужного элемента нет - положить в конец списка
    void    insert_at(const std::string& key, const Config& value)              noexcept;
    void    insert_at(const std::string& key, Config&& value)                   noexcept;
    void    insert_before(const std::string &position_key, const std::string& key,
                       const Config& value);
    void    insert_before(const std::string &position_key, const std::string& key,
                       Config&& value);
    void    insert_after(const std::string &position_key, const std::string& key,
                      const Config& value);
    void    insert_after(const std::string &position_key, const std::string& key,
                      Config&& value);

    //группы элементов
    void    insert_front(const VPairElement& elements)                          noexcept;
    void    insert_front(VPairElement&& elements)                               noexcept;
    void    insert_at(const size_t index, const VPairElement& elements)         noexcept;
    void    insert_at(const size_t index, VPairElement&& elements)              noexcept;
    void    insert_back(const VPairElement& elements)                           noexcept;
    void    insert_back(VPairElement&& elements)                                noexcept;

    //неизвестное количество элементов
            template<typename ... Value>
    void    insert_front(std::pair<std::string, Value>&& ... pairs) noexcept {
                VPairElement vpe;
                if(vpe.capacity() < sizeof...(pairs))
                    vpe.reserve(sizeof...(pairs));
                (void)std::initializer_list<int>{(vpe.push_back(std::forward<std::pair<std::string, Value>>(pairs)), 0)...};
                insert_front(std::move(vpe));
            }
            template<typename ... Value>
    void    insert_at(const size_t index, std::pair<std::string, Value>&& ... pairs) noexcept {
                VPairElement vpe;
                if(vpe.capacity() < sizeof...(pairs))
                    vpe.reserve(sizeof...(pairs));
                (void)std::initializer_list<int>{(vpe.push_back(index, std::forward<std::pair<std::string, Value>>(pairs)), 0)...};
                insert_at(index, std::move(vpe));
            }
            template<typename ... Value>
    void    insert_back(std::pair<std::string, Value>&& ... pairs) noexcept {
                (void)std::initializer_list<int>{(insert_back(std::forward<std::pair<std::string, Value>>(pairs)), 0)...};
            }

    //другое имя для тех же действий
    void    push_front(const std::string& key, const Config& value)             noexcept        { insert_front(key, value); }
    void    push_front(const std::string& key, Config&& value)                  noexcept        { insert_front(key, std::move(value)); }
    void    push_back(const std::string& key, const Config& value)              noexcept        { insert_back(key, value); }
    void    push_back(const std::string& key, Config&& value)                   noexcept        { insert_back(key, std::move(value)); }
    // ========================================================================================================== Adding

    // Removing ========================================================================================================
    void    pop_front()                                                                         override;
    void    pop_at(const size_t index)                                                          override;
    void    pop_back()                                                                          override;
    Config  get_and_pop_front()                                                                 override;
    Config  get_and_pop_at(const size_t index)                                                  override;
    Config  get_and_pop_back()                                                                  override;
    void    erase_front()                                                                       override;
    void    erase_at(const size_t index)                                                        override;
    void    erase_back()                                                                        override;

    void    pop_at(const std::string& key)                                      noexcept;
    Config  get_and_pop_at(const std::string& key)                              noexcept;
    void    erase_at(const std::string& key)                                    noexcept;
    // ======================================================================================================== Removing

    // Info ============================================================================================================
    bool    isContainer()                                                       const noexcept  override    { return true; }
    bool    isMapContainer()                                                    const noexcept  override    { return true; }
    size_t  size()                                                              const noexcept  override    { return m_values.size(); }
    bool    empty()                                                             const noexcept  override    { return m_values.empty(); }
    bool    contains(const std::string& key)                                    const noexcept;
    size_t  get_index_from(const std::string& key)                              const;
    // ============================================================================================================ Info

    // Operators =======================================================================================================
    Config& operator[](const size_t index)                                                      override;
    Config  operator[](const size_t index)                                      const           override;
    Config& operator[](const std::string& key)                                  noexcept;
    Config  operator[](const std::string& key)                                  const noexcept;
    // ======================================================================================================= Operators

    // Iterators =======================================================================================================
    shared_VPairElement::iterator       begin()                                 noexcept        { return m_values.begin(); }
    shared_VPairElement::iterator       end()                                   noexcept        { return m_values.end(); }
    shared_VPairElement::const_iterator cbegin()                                const noexcept  { return m_values.cbegin(); }
    shared_VPairElement::const_iterator cend()                                  const noexcept  { return m_values.cend(); }
    // ======================================================================================================= Iterators

    // String ==========================================================================================================
    //вывод без комментариев, "tabulation_level == -1" => запись в одну строку
    std::string toString(const ConfigFormat format = ConfigFormat::eJSON,
                         const int8_t tabulation_level = -1,
                         const CommentDesign &design = {})                      const noexcept  override;
    std::string toJsonString(const int8_t tabulation_level = -1, const CommentDesign &design = {})
                                                                                const noexcept  override;
    std::string toIniString(const int8_t tabulation_level = -1, const CommentDesign &design = {})
                                                                                const noexcept  override;
    std::string toYamlString(const int8_t tabulation_level = -1, const CommentDesign &design = {})
                                                                                const noexcept  override;
    std::string toXmlString(const int8_t tabulation_level = -1, const CommentDesign &design = {})
                                                                                const noexcept  override;
    // ========================================================================================================== String

    // File ============================================================================================================
    // ============================================================================================================ File

    // Parser ==========================================================================================================
    void    parse(std::string&& input_string, const ConfigFormat format = ConfigFormat::eJSON,
               bool parse_comments = true)                                                      override;
    // ========================================================================================================== Parser
};

//TODO: bool IsElementJson(const std::string& str)                                      noexcept;
//TODO: bool IsElementJson(const IElement& e)                                           noexcept;
//TODO: bool IsElementJson(const Config& cfg)                                           noexcept;

//TODO: ElementJson ParseJson(std::string&& input_string, const ConfigFormat format = ConfigFormat::eJSON, bool parse_comments = true)

#endif // ELEMENT_JSON_H
