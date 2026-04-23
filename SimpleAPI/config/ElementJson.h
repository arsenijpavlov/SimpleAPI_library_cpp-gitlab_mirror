#ifndef ELEMENT_JSON_H
#define ELEMENT_JSON_H

#include "../utils/TypeDefines.h"
#include "ElementArray.h"
#include <iostream>


namespace simpleapi {
namespace tools {

class ElementJson : public IElementContainer {
protected:
    shared_VPairElement m_values;

public:
    ElementJson()                                                               noexcept        { init(); }
    explicit ElementJson(const ElementJson& other)                              noexcept;
    explicit ElementJson(const JPair& pair)                                     noexcept;
    explicit ElementJson(const std::string& input_string,
                         const ConfigFormat config_format = ConfigFormat::eJSON,
                         const CommentDesign design = {},
                         std::string* error_str = nullptr)                      noexcept;

    __ONLY_ALLOWED_TYPES__(T)
    explicit ElementJson(const std::string& key, const T& value) noexcept {
        init();
        put(key, value);
    }
    explicit ElementJson(const VPairElement& vec)                               noexcept;
    ~ElementJson()                                                              noexcept        {}

    // Comment =========================================================================================================
    void    set_comment(const size_t& index, const Comment &content)                            override;
    void    set_comment(const size_t& index, const std::string &content_before,
                        const std::string &content_after)                                       override;
    void    set_prefix_comment(const size_t& index, const std::string &content)                 override;
    void    set_suffix_comment(const size_t& index, const std::string &content)                 override;
    Comment&        get_comment(const size_t& index)                                            override;
    Comment         get_comment(const size_t& index)                            const           override;
    std::string     get_prefix_comment(const size_t& index)                     const           override;
    std::string     get_suffix_comment(const size_t& index)                     const           override;
    //NOTE: доступы к внутренним значениям строго по set() и get() запросам
//    std::string&    get_prefix_comment(const size_t& index)                                      override;
//    std::string&    get_suffix_comment(const size_t& index)                                      override;
    void    clear_comment(const size_t& index)                                                  override;
    void    clear_prefix_comment(const size_t& index)                                           override;
    void    clear_suffix_comment(const size_t& index)                                           override;
    void    delete_comment(const size_t& index)                                                 override;
    void    delete_prefix_comment(const size_t& index)                                          override;
    void    delete_suffix_comment(const size_t& index)                                          override;

    void    set_comment(const std::string& key, const Comment &content);
    void    set_comment(const std::string& key, const std::string &content_before,
                     const std::string &content_after);
    void    set_prefix_comment(const std::string& key, const std::string &content);
    void    set_suffix_comment(const std::string& key, const std::string &content);
    Comment&        get_comment(const std::string& key);
    Comment         get_comment(const std::string& key)                         const;
    std::string     get_prefix_comment(const std::string& key)                  const;
    std::string     get_suffix_comment(const std::string& key)                  const;

    //NOTE: доступы к внутренним значениям строго по set() и get() запросам
//    std::string&    get_prefix_comment(const std::string& key);
//    std::string&    get_suffix_comment(const std::string& key);
    void    clear_comment(const std::string& key);
    void    clear_prefix_comment(const std::string& key);
    void    clear_suffix_comment(const std::string& key);
    void    delete_comment(const std::string& key);
    void    delete_prefix_comment(const std::string& key);
    void    delete_suffix_comment(const std::string& key);
    // ========================================================================================================= Comment

    // Setters =========================================================================================================
private:
    void    init()                                                              noexcept        { m_type = ValueType::eJson; }
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
    Config& get_at(const size_t& index)                                                         override;
    Config  get_at(const size_t& index)                                         const           override;
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
    bool    insert_at(const size_t& index, const std::string& key,
                   const Config& value)                                         noexcept;
    //@return - был ли изменён индекс вниз при перезаписи ключа
    bool    insert_at(const size_t& index, const std::string& key,
                   Config&& value)                                              noexcept;
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

    //NOTE(Json): вернёт iterator на первый из вставленных элементов
    shared_VPairElement::iterator insert_at(shared_VPairElement::iterator iterator, const std::string& key,
                   const Config& value)                                         noexcept;
    shared_VPairElement::iterator insert_at(shared_VPairElement::iterator iterator, const std::string& key,
                   Config&& value)                                              noexcept;

    //группы элементов
    void    insert_front(const VPairElement& elements)                          noexcept;
    void    insert_front(VPairElement&& elements)                               noexcept;
    void    insert_at(const size_t& index, const VPairElement& elements)        noexcept;
    void    insert_at(const size_t& index, VPairElement&& elements)             noexcept;
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
    void    insert_at(const size_t& index, std::pair<std::string, Value>&& ... pairs) noexcept {
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

    void    append(const ElementJson& other)                                    noexcept;
    void    append(ElementJson&& other)                                         noexcept;

    //другое имя для тех же действий
    void    push_front(const std::string& key, const Config& value)             noexcept        { insert_front(key, value); }
    void    push_front(const std::string& key, Config&& value)                  noexcept        { insert_front(key, std::move(value)); }
    void    push_back(const std::string& key, const Config& value)              noexcept        { insert_back(key, value); }
    void    push_back(const std::string& key, Config&& value)                   noexcept        { insert_back(key, std::move(value)); }
    // ========================================================================================================== Adding

    // Removing ========================================================================================================
    //если индекса/ключа не существовало - проигнорировать
    void    pop_front()                                                         noexcept        override;
    void    pop_at(const size_t& index)                                         noexcept        override;
    void    pop_back()                                                          noexcept        override;


    Config  get_and_pop_front()                                                                 override;
    Config  get_and_pop_at(const size_t& index)                                                 override;
    Config  get_and_pop_back()                                                                  override;

    //WARNING: обязательно для области видимости из Config::
    using IElementContainer::erase_front;
    using IElementContainer::erase_at;
    using IElementContainer::erase_back;
    using IElementContainer::remove_front;
    using IElementContainer::remove_at;
    using IElementContainer::remove_back;

    void    pop_at(const std::string& key)                                      noexcept;
    Config  get_and_pop_at(const std::string& key)                              noexcept;
    void    erase_at(const std::string& key)                                    noexcept;

    //NOTE(JsonArray): вернёт iterator, следующий после удаляемого
    shared_VPairElement::iterator pop_at(const shared_VPairElement::iterator iterator);
    shared_VPairElement::iterator erase_at(const shared_VPairElement::iterator iterator)        { return pop_at(iterator); }
    // ======================================================================================================== Removing

    // Info ============================================================================================================
    bool    isContainer()                                                       const noexcept  override    { return true; }
    bool    isMapContainer()                                                    const noexcept  override    { return true; }
    bool    isEqual(const IElement &other, const bool compare_comments,
                    const bool map_sort_important)                              const noexcept  override;
    size_t  size()                                                              const noexcept  override    { return m_values.size(); }
    bool    empty()                                                             const noexcept  override    { return m_values.empty(); }
    bool    contains(const std::string& key)                                    const noexcept;
    size_t  get_index_from(const std::string& key)                              const;
    // ============================================================================================================ Info


    // Operators =======================================================================================================
    //NOTE: вроде не обойтись без такого костыля
    Config  operator[](const size_t& index)                                     const           override;
    Config& operator[](const std::string& key)                                  noexcept;
    Config  operator[](const std::string& key)                                  const noexcept;
    bool    operator==(const Config& config)                                    const           override;
    // ======================================================================================================= Operators

    // Iterators =======================================================================================================
    shared_VPairElement::iterator       begin()                                 noexcept        { return m_values.begin(); }
    shared_VPairElement::iterator       end()                                   noexcept        { return m_values.end(); }
    shared_VPairElement::const_iterator cbegin()                                const noexcept  { return m_values.cbegin(); }
    shared_VPairElement::const_iterator cend()                                  const noexcept  { return m_values.cend(); }
    // ======================================================================================================= Iterators

    // String ==========================================================================================================
    //вывод без комментариев, "tabulation_level == -1" => запись в одну строку
    //вывод без комментариев, ConfigFormat::eONLY_VALUE => запись в одну строку без комментариев
    std::string toString(const ConfigFormat format = ConfigFormat::eONLY_VALUE,
                         const CommentDesign &design = {},
                         const int8_t custom_tabulation_level = -1)             const noexcept  override;
    std::string toJsonString(const CommentDesign &design = {}, const int8_t custom_tabulation_level = -1)
                                                                                const noexcept  override;
    std::string toIniString(const CommentDesign &design = {}, const int8_t custom_tabulation_level = -1)
                                                                                const noexcept  override;
    std::string toYamlString(const CommentDesign &design = {}, const int8_t custom_tabulation_level = -1)
                                                                                const noexcept  override;
    std::string toXmlString(const CommentDesign &design = {}, const int8_t custom_tabulation_level = -1)
                                                                                const noexcept  override;
    // ========================================================================================================== String

    // File ============================================================================================================
    // ============================================================================================================ File

    // Parser ==========================================================================================================
private:
    //NOTE: всё это можно вынести из класса
    enum class ParseState {
        eJSON_START,
        eJSON_KEY,
        eJSON_KEY_VALUE_SEPARATOR,
        eJSON_VALUE,
        eJSON_SEPARATOR,
        eJSON_FINISH,
        eJSON_ERROR_STATE
    };
    std::string to_string(const ParseState state)                               const noexcept;
    void    UpdateState(ParseState& state, const ParseState new_state)          const noexcept;
public:
    std::string parse(const std::string& input_string, CommentDesign &design,
                      const ConfigFormat format = ConfigFormat::eJSON,
                      const int8_t tabulation_level = 0)                                        noexcept    override;

    std::string parse(std::string&& input_string, CommentDesign &design,
                      const ConfigFormat format = ConfigFormat::eJSON,
                      const int8_t tabulation_level = 0)                                        noexcept    override;

    std::string parseJson(const std::string& input_string, CommentDesign &design)               noexcept    override;

    std::string parseJson(std::string&& input_string, CommentDesign &design)                    noexcept    override;

    std::string parseYaml(const std::string& input_string, CommentDesign &design,
                          const int8_t tabulation_level = 0)                                    noexcept    override;

    std::string parseYaml(std::string&& input_string, CommentDesign &design,
                          const int8_t tabulation_level = 0)                                    noexcept    override;

    std::string parseXml(const std::string& input_string, CommentDesign &design)                noexcept    override;

    std::string parseXml(std::string&& input_string, CommentDesign &design)                     noexcept    override;
    // ========================================================================================================== Parser
};

bool IsElementJson(const IElement& e)                                           noexcept;
bool IsElementJson(const Config& cfg)                                           noexcept;

} // namespace tools
} // namespace simpleapi

#endif // ELEMENT_JSON_H
