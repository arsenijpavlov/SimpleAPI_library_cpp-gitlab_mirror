#ifndef ELEMENT_ARRAY_H
#define ELEMENT_ARRAY_H

#include "interface/IElementContainer.h"
#include <iostream>

class ElementArray : public IElementContainer {
protected:
    shared_VElement m_values;

public:
    ElementArray()                                                      noexcept                    { init(); }
    template<typename ... Types>
    explicit ElementArray(Types&&... args) noexcept {
        init();
        (void)std::initializer_list<int>{(push_back(std::forward<Types>(args)), 0)...};
    }
    ElementArray(const std::string& input_string, const ConfigFormat config_format,
                 const bool enable_comments = false)                    noexcept {
        init();
        ElementArray temp;
        try {
            temp.parse(input_string, config_format, enable_comments);
        } catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
        *this = temp;
    }
    ~ElementArray()                                                     noexcept                    {}

    // Comment =========================================================================================================
    void    add_comment(const size_t index, const Comment &content)                     override;
    void    add_comment(const size_t index, const std::string &content_before,
                     const std::string &content_after)                                  override;
    void    add_prefix_comment(const size_t index, const std::string &content)          override;
    void    add_suffix_comment(const size_t index, const std::string &content)          override;
    Comment&        get_comment(const size_t index)                                     override;
    Comment         get_comment(const size_t index)                     const           override;
    std::string&    get_prefix_comment(const size_t index)                              override;
    std::string     get_prefix_comment(const size_t index)              const           override;
    std::string&    get_suffix_comment(const size_t index)                              override;
    std::string     get_suffix_comment(const size_t index)              const           override;
    void    clear_comment(const size_t index)                                           override;
    void    clear_prefix_comment(const size_t index)                                    override;
    void    clear_suffix_comment(const size_t index)                                    override;
    void    delete_comment(const size_t index)                                          override;
    void    delete_prefix_comment(const size_t index)                                   override;
    void    delete_suffix_comment(const size_t index)                                   override;
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
    Config& get_at(const size_t index)                                                  override;
    Config  get_at(const size_t index)                                  const           override;
    Config& get_back()                                                                  override;
    Config  get_back()                                                  const           override;
    // ========================================================================================================= Getters

    // Modify ==========================================================================================================
    void    clear()                                                     noexcept        override;
    // ========================================================================================================== Modify

    // Adding ==========================================================================================================
    //одиночные элементы
    void    insert_front(const Config& value)                           noexcept;
    void    insert_front(Config&& value)                                noexcept;
    void    insert_at(const size_t index, const Config& value)          noexcept;
    void    insert_at(const size_t index, Config&& value)               noexcept;
    void    insert_at(shared_VElement::iterator iterator, const Config& value);
    void    insert_at(shared_VElement::iterator iterator, Config&& value);
    void    insert_back(const Config& value)                            noexcept;
    void    insert_back(Config&& value)                                 noexcept;

    //группы элементов
    void    insert_front(const VElement& elements)                      noexcept;
    void    insert_front(VElement&& elements)                           noexcept;
    void    insert_at(const size_t index, const VElement& elements)     noexcept;
    void    insert_at(const size_t index, VElement&& elements)          noexcept;
    void    insert_back(const VElement& elements)                       noexcept;
    void    insert_back(VElement&& elements)                            noexcept;

    //неизвестное количество элементов
            template<typename ... Args>
    void    insert_front(Args&& ... args) noexcept {
                VElement ve;
                if(ve.capacity() < sizeof...(args))
                    ve.reserve(sizeof...(args));
                (void)std::initializer_list<int>{(ve.push_back(std::forward<Args>(args)), 0)...};
                insert_front(std::move(ve));
            }
            template<typename ... Args>
    void    insert_at(const size_t index, Args&& ... args) noexcept {
                VElement ve;
                if(ve.capacity() < sizeof...(args))
                    ve.reserve(sizeof...(args));
                (void)std::initializer_list<int>{(ve.push_back(std::forward<Args>(args)), 0)...};
                insert_at(index, std::move(ve));
            }
            template<typename ... Args>
    void    insert_back(Args&& ... args) noexcept {
                (void)std::initializer_list<int>{(insert_back(std::forward<Args>(args)), 0)...};
            }

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
    void    pop_front()                                                                 override;
    void    pop_at(const size_t index)                                                  override;
    void    pop_back()                                                                  override;
    Config  get_and_pop_front()                                                         override;
    Config  get_and_pop_at(const size_t index)                                          override;
    Config  get_and_pop_back()                                                          override;
    void    erase_front()                                                               override;
    void    erase_at(const size_t index)                                                override;
    void    erase_back()                                                                override;
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
    Config& operator[](const size_t index)                                              override;
    Config  operator[](const size_t index)                              const           override;
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
                         const int8_t tabulation_level = 0,
                         const CommentDesign &design = {})              const noexcept  override;
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
private:
    //NOTE: всё это можно вынести из класса
    enum class ParseState {
        eARRAY_START,
        eARRAY_VALUE,
        eARRAY_SEPARATOR,
        eARRAY_COMMENT,
        eARRAY_FINISH,
        eARRAY_ERROR_STATE
    };
    std::string to_string(const ParseState state)                       const noexcept;
    void    UpdateState(ParseState& state, const ParseState new_state)  const noexcept;
public:
    void    parse(const std::string& input_string,
               CommentDesign& design,
               const ConfigFormat format = ConfigFormat::eJSON,
               const bool parse_comments = true, const int8_t tabulation_level = 0)                                        override;
    void    parse(const std::string& input_string,
               const ConfigFormat format = ConfigFormat::eJSON,
               const bool parse_comments = true, const int8_t tabulation_level = 0)                                        override;

    void    parse(std::string&& input_string,
               CommentDesign& design,
               const ConfigFormat format = ConfigFormat::eJSON,
               const bool parse_comments = true, const int8_t tabulation_level = 0)                                        override;
    void    parse(std::string&& input_string,
               const ConfigFormat format = ConfigFormat::eJSON,
               const bool parse_comments = true, const int8_t tabulation_level = 0)                                        override;

    void    parseJson(const std::string& input_string,
                   CommentDesign& design,
                   const bool parse_comments = true, const int8_t tabulation_level = 0)                                    override;
    void    parseJson(const std::string& input_string,
                   const bool parse_comments = true, const int8_t tabulation_level = 0)                                    override;

    void    parseJson(std::string&& input_string,
                   CommentDesign& design,
                   const bool parse_comments = true, const int8_t tabulation_level = 0)                                    override;
    void    parseJson(std::string&& input_string,
                   const bool parse_comments = true, const int8_t tabulation_level = 0)                                    override;

    void    parseIni(const std::string& input_string,
                  CommentDesign& design,
                  const bool parse_comments = true, const int8_t tabulation_level = 0)                                     override;
    void    parseIni(const std::string& input_string,
                  const bool parse_comments = true, const int8_t tabulation_level = 0)                                     override;

    void    parseIni(std::string&& input_string,
                  CommentDesign& design,
                  const bool parse_comments = true, const int8_t tabulation_level = 0)                                     override;
    void    parseIni(std::string&& input_string,
                  const bool parse_comments = true, const int8_t tabulation_level = 0)                                     override;

    void    parseYaml(const std::string& input_string,
                   CommentDesign& design,
                   const bool parse_comments = true, const int8_t tabulation_level = 0)                                    override;
    void    parseYaml(const std::string& input_string,
                   const bool parse_comments = true, const int8_t tabulation_level = 0)                                    override;

    void    parseYaml(std::string&& input_string,
                   CommentDesign& design,
                   const bool parse_comments = true, const int8_t tabulation_level = 0)                                    override;
    void    parseYaml(std::string&& input_string,
                   const bool parse_comments = true, const int8_t tabulation_level = 0)                                    override;

    void    parseXml(const std::string& input_string,
                  CommentDesign& design,
                  const bool parse_comments = true, const int8_t tabulation_level = 0)                                     override;
    void    parseXml(const std::string& input_string,
                  const bool parse_comments = true, const int8_t tabulation_level = 0)                                     override;

    void    parseXml(std::string&& input_string,
                  CommentDesign& design,
                  const bool parse_comments = true, const int8_t tabulation_level = 0)                                     override;
    void    parseXml(std::string&& input_string,
                  const bool parse_comments = true, const int8_t tabulation_level = 0)                                     override;
    // ========================================================================================================== Parser
};

//TODO: bool IsElementArray(const std::string& str, const ConfigFormat format = ConfigFormat::eJSON) noexcept;
//TODO: bool IsElementJsonArray(const std::string& str)                         noexcept;
//TODO: bool IsElementIniArray(const std::string& str)                          noexcept;
//TODO: bool IsElementArray(const IElement& e)                                  noexcept;

//TODO: ElementArray ParseArray(std::string&& input_string, const ConfigFormat format = ConfigFormat::eJSON, const bool parse_comments = true, const int8_t tabulation_level = 0)

#endif // ELEMENT_ARRAY_H
