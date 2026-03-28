#ifndef ELEMENT_ARRAY_H
#define ELEMENT_ARRAY_H

#include "interface/IElementContainer.h"
#include <iostream>
#include "../utils/TypeDefines.h"

class ElementArray : public IElementContainer {
protected:
    shared_VElement m_values;

public:
    ElementArray()                                                      noexcept                    { init(); }
    template<typename... Types,
             typename std::enable_if<
                 all_true<(!std::is_same<typename std::decay<Types>::type, ConfigFormat>::value)...>
                 ::value>
             ::type* = nullptr>
    ElementArray(Types&&... args) noexcept {
        init();
        (void)std::initializer_list<int>{(push_back(std::forward<Types>(args)), 0)...};
    }
    ElementArray(const std::string& input_string, const ConfigFormat config_format,
                 const bool enable_comments = false, std::string* error_str = nullptr)  noexcept;
    ~ElementArray()                                                     noexcept                    {}

    // Comment =========================================================================================================
    void    set_comment(const size_t& index, const Comment &content)                    override;
    void    set_comment(const size_t& index, const std::string &content_before,
                        const std::string &content_after)                               override;
    void    set_prefix_comment(const size_t& index, const std::string &content)         override;
    void    set_suffix_comment(const size_t& index, const std::string &content)         override;
    Comment&        get_comment(const size_t& index)                                    override;
    Comment         get_comment(const size_t& index)                    const           override;
    std::string     get_prefix_comment(const size_t& index)             const           override;
    std::string     get_suffix_comment(const size_t& index)             const           override;
    //NOTE: доступы к внутренним значениям строго по set() и get() запросам
//    std::string&    get_prefix_comment(const size_t& index)                              override;
//    std::string&    get_suffix_comment(const size_t& index)                              override;
    void    clear_comment(const size_t& index)                                          override;
    void    clear_prefix_comment(const size_t& index)                                   override;
    void    clear_suffix_comment(const size_t& index)                                   override;
    void    delete_comment(const size_t& index)                                         override;
    void    delete_prefix_comment(const size_t& index)                                  override;
    void    delete_suffix_comment(const size_t& index)                                  override;
    // ========================================================================================================= Comment

    // Setters =========================================================================================================
private:
    void    init()                                                      noexcept                    { m_type = ValueType::eArray; }
public:
    void    append_if_not_contains(const Config& config)                noexcept;
    void    append_if_not_contains(Config&& config)                     noexcept;
    // ========================================================================================================= Setters

    // Getters =========================================================================================================
    Config& get_front()                                                                 override;
    Config  get_front()                                                 const           override;
    Config& get_at(const size_t& index)                                                 override;
    Config  get_at(const size_t& index)                                 const           override;
    Config& get_back()                                                                  override;
    Config  get_back()                                                  const           override;
    // ========================================================================================================= Getters

    // Modify ==========================================================================================================
    void    clear()                                                     noexcept        override;
    // ========================================================================================================== Modify

    // Adding ==========================================================================================================
            template<typename ... Args>
    void    insert_front(Args&& ... args) noexcept {
        (void)std::initializer_list<int>{(m_values.insert(cbegin(), std::make_shared<Config>(std::move(args))), 0)...};
    }
            template<typename ... Args>
    void    insert_at(const size_t& index, Args&& ... args) noexcept {
        VElement ve;
        if(ve.capacity() < sizeof...(args))
            ve.reserve(sizeof...(args));
        (void)std::initializer_list<int>{(ve.push_back(std::forward<Args>(args)), 0)...};
        insert_at(index, std::move(ve));
    }
            template<typename ... Args>
    void    insert_back(Args&& ... args) noexcept {
        (void)std::initializer_list<int>{(m_values.insert(cend(), std::make_shared<Config>(std::forward<Args>(args))), 0)...};
    }

    //группы элементов
    void    insert_front(const VElement& elements)                      noexcept;
    void    insert_front(VElement&& elements)                           noexcept;
    void    insert_at(const size_t& index, const VElement& elements)    noexcept;
    void    insert_at(const size_t& index, VElement&& elements)         noexcept;
    void    insert_back(const VElement& elements)                       noexcept;
    void    insert_back(VElement&& elements)                            noexcept;

    //NOTE(JsonArray): вернёт iterator на первый из вставленных элементов
    shared_VElement::iterator insert_at(const shared_VElement::iterator iterator, const Config& value);
    shared_VElement::iterator insert_at(const shared_VElement::iterator iterator, Config&& value);

    void    append(const ElementArray& other)                           noexcept;
    void    append(ElementArray&& other)                                noexcept;

    //другое имя для тех же действий
    void    push_front(const Config& value)                             noexcept                    { insert_front(value); }
    void    push_front(Config&& value)                                  noexcept                    { insert_front(std::move(value)); }
    void    push_back(const Config& value)                              noexcept                    { insert_back(value); }
    void    push_back(Config&& value)                                   noexcept                    { insert_back(std::move(value)); }

            template<typename ... Args>
    void    push_front(Args&& ... args)                                 noexcept                    { insert_front((args)...); }
            template<typename ... Args>
    void    push_back(Args&& ... args)                                  noexcept                    { (void)std::initializer_list<int>{(insert_back(std::forward<Args>(args)), 0)...}; }
private:
    void    append_null(size_t size)                                    noexcept;
public:
    // ========================================================================================================== Adding

    // Removing ========================================================================================================
    // если индекса не существовало - проигнорировать
    void    pop_front()                                                 noexcept        override;
    void    pop_at(const size_t& index)                                 noexcept        override;
    void    pop_back()                                                  noexcept        override;

    Config  get_and_pop_front()                                                         override;
    Config  get_and_pop_at(const size_t& index)                                         override;
    Config  get_and_pop_back()                                                          override;

    //WARNING: обязательно для области видимости из Config::
    using IElementContainer::erase_front;
    using IElementContainer::erase_at;
    using IElementContainer::erase_back;
    using IElementContainer::remove_front;
    using IElementContainer::remove_at;
    using IElementContainer::remove_back;

    //NOTE(JsonArray): вернёт iterator, следующий после удаляемого
    shared_VElement::iterator pop_at(const shared_VElement::iterator iterator);
    shared_VElement::iterator erase_at(const shared_VElement::iterator iterator)                    { return pop_at(iterator); }
    // ======================================================================================================== Removing

    // Info ============================================================================================================
    bool    isContainer()                                               const noexcept  override    { return true; }
    bool    isIndexContainer()                                          const noexcept  override    { return true; }
    bool    isEqual(const IElement& other, const bool compare_comments = false,
                 const bool map_sort_important = false)                 const noexcept  override;
    size_t  size()                                                      const noexcept  override    { return m_values.size(); }
    bool    empty()                                                     const noexcept  override    { return m_values.empty(); }
    bool    contains(const Config& config)                              const noexcept;
    // ============================================================================================================ Info

    // Operators =======================================================================================================
    //NOTE: вроде не обойтись без такого костыля
    Config  operator[](const size_t& index)                             const           override;
    bool    operator==(const Config& config)                            const           override;
    // ======================================================================================================= Operators

    // Iterators =======================================================================================================
    shared_VElement::iterator       begin()                             noexcept                    { return m_values.begin(); }
    shared_VElement::iterator       end()                               noexcept                    { return m_values.end(); }
    shared_VElement::const_iterator cbegin()                            const noexcept              { return m_values.cbegin(); }
    shared_VElement::const_iterator cend()                              const noexcept              { return m_values.cend(); }
    // ======================================================================================================= Iterators

    // String ==========================================================================================================
    //вывод без комментариев, "tabulation_level == -1" => запись в одну строку
    std::string toString(const ConfigFormat format = ConfigFormat::eJSON,
                         const CommentDesign &design = {},
                         const int8_t custom_tabulation_level = -1)     const noexcept  override;
    std::string toJsonString(const CommentDesign &design = {},
                             const int8_t custom_tabulation_level = -1) const noexcept  override;
    //NOTE: метод в данном контексте не функционален
    std::string toIniString(const CommentDesign &design = {},
                            const int8_t custom_tabulation_level = -1)  const noexcept  override;
    std::string toYamlString(const CommentDesign &design = {},
                             const int8_t custom_tabulation_level = -1) const noexcept  override;
    std::string toXmlString(const CommentDesign &design = {},
                            const int8_t custom_tabulation_level = -1)  const noexcept  override;
    // ========================================================================================================== String

    // File ============================================================================================================
    // ============================================================================================================ File

    // Parser ==========================================================================================================
private:
    //NOTE: всё это можно вынести из класса
    enum class ParseState {
        eARRAY_START,
        eARRAY_VALUE,
        eARRAY_SEPARATOR,
        eARRAY_FINISH,
        eARRAY_ERROR_STATE
    };
    std::string to_string(const ParseState state)                       const noexcept;
    void    UpdateState(ParseState& state, const ParseState new_state)  const noexcept;
public:
    std::string parse(const std::string& input_string, CommentDesign& design,
                      const ConfigFormat format = ConfigFormat::eJSON,
                      const int8_t tabulation_level = 0)                                        noexcept    override;
    std::string parse(const std::string& input_string,
                      const ConfigFormat format = ConfigFormat::eJSON,
                      const bool parse_comments = true, const int8_t tabulation_level = 0)      noexcept    override;

    std::string parse(std::string&& input_string, CommentDesign& design,
                      const ConfigFormat format = ConfigFormat::eJSON,
                      const int8_t tabulation_level = 0)                                        noexcept    override;
    std::string parse(std::string&& input_string,
                      const ConfigFormat format = ConfigFormat::eJSON,
                      const bool parse_comments = true, const int8_t tabulation_level = 0)      noexcept    override;

    std::string parseJson(const std::string& input_string, CommentDesign& design)               noexcept    override;
    std::string parseJson(const std::string& input_string, const bool parse_comments = true)    noexcept    override;

    std::string parseJson(std::string&& input_string, CommentDesign& design)                    noexcept    override;
    std::string parseJson(std::string&& input_string, const bool parse_comments = true)         noexcept    override;

    //NOTE: в данном контексте не функциональны
    std::string parseIni(const std::string& input_string, CommentDesign& design)                noexcept    override;
    std::string parseIni(const std::string& input_string, const bool parse_comments = true)     noexcept    override;

    //NOTE: в данном контексте не функциональны
    std::string parseIni(std::string&& input_string, CommentDesign& design)                     noexcept    override;
    std::string parseIni(std::string&& input_string, const bool parse_comments = true)          noexcept    override;

    std::string parseYaml(const std::string& input_string, CommentDesign& design,
                          const int8_t tabulation_level = 0)                                    noexcept    override;
    std::string parseYaml(const std::string& input_string, const bool parse_comments = true,
                          const int8_t tabulation_level = 0)                                    noexcept    override;

    std::string parseYaml(std::string&& input_string, CommentDesign& design,
                          const int8_t tabulation_level = 0)                                    noexcept    override;
    std::string parseYaml(std::string&& input_string, const bool parse_comments = true,
                          const int8_t tabulation_level = 0)                                    noexcept    override;

    std::string parseXml(const std::string& input_string, CommentDesign& design)                noexcept    override;
    std::string parseXml(const std::string& input_string, const bool parse_comments = true)     noexcept    override;

    std::string parseXml(std::string&& input_string, CommentDesign& design)                     noexcept    override;
    std::string parseXml(std::string&& input_string, const bool parse_comments = true)          noexcept    override;
    // ========================================================================================================== Parser
};

bool IsElementArray(const IElement &e)                                          noexcept;
bool IsElementArray(const Config &cfg)                                          noexcept;

#endif // ELEMENT_ARRAY_H
