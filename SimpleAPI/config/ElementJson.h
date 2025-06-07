#ifndef ELEMENT_JSON_H
#define ELEMENT_JSON_H

#include "ConfigDefines.h"
#include "IElementMapContainer.h"
#include "ElementArray.h"


class ElementJson : public IElementMapContainer {
protected:
    VPairElement m_values;

public:
    ElementJson() noexcept                                  { init(); }
    explicit ElementJson(const ElementJson& json) noexcept {
        init();
        m_values = json.m_values;
    }
    explicit ElementJson(const JPair& pair) noexcept {
        init();
        push_back(pair.first, pair.second);
    }
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
    explicit ElementJson(const IElement& element) noexcept           { init(); }
    ~ElementJson() noexcept                                 {}

    // Setters =========================================================================================================
private:
    void        init()                                              noexcept                    { m_type = ValueType::eArray; }
public:
    // ========================================================================================================= Setters

    // Getters =========================================================================================================
    Config&     get_front()                                                         override;
    Config      get_front()                                         const           override;
    Config&     get_at(const size_t index)                                          override;
    Config      get_at(const size_t index)                          const           override;
    Config&     get_back()                                                          override;
    Config      get_back()                                          const           override;
    Config&     get_at(const std::string& key)                      noexcept        override;
    Config      get_at(const std::string& key)                      const noexcept  override;
    // ========================================================================================================= Getters

    // Modify ==========================================================================================================
    void        clear()                                             noexcept        override    { m_values.clear(); }
    void        update_front(const Config& new_value)               noexcept        override;
    void        update_front(Config&& new_value)                    noexcept        override;
    //если нужного индекса не существует, то будет добавлено N пустых элементов до необходимого индекса
    void        update_at(const size_t index, const Config& new_value)
                                                                    noexcept        override;
    void        update_at(const size_t index, Config&& new_value)   noexcept        override;
    void        update_back(const Config& new_value)                noexcept        override;
    void        update_back(Config&& new_value)                     noexcept        override;
    void        update_at(const std::string& key, const Config& new_value)
                                                                    noexcept        override;
    void        update_at(const std::string& key, Config&& new_value)
                                                                    noexcept        override;
    // ========================================================================================================== Modify

    // Adding ==========================================================================================================
    void        insert_front(const Config& value)                   noexcept        override;
    void        insert_front(Config&& value)                        noexcept        override;
//TODO: когда-нибудь, void insert_at(iterator, const Config& value);
//TODO: когда-нибудь, void insert_at(iterator, Config&& value);
    void        insert_at(const size_t index, const Config& value)  noexcept        override;
    void        insert_at(const size_t index, Config&& value)       noexcept        override;
    void        insert_back(const Config& value)                    noexcept        override;
    void        insert_back(Config&& value)                         noexcept        override;
    void        insert_at(const std::string& key, const Config& value)
                                                                    noexcept        override;
    void        insert_at(const std::string& key, Config&& value)   noexcept        override;
    //если нужного элемента нет - создать пустым и положить этот после
    void        insert_after(const std::string& key, const Config& value)
                                                                    noexcept        override;
    void        insert_after(const std::string& key, Config&& value)noexcept        override;
    // ========================================================================================================== Adding

    // Removing ========================================================================================================
    Config      get_and_pop_front()                                                 override;
    Config      get_and_pop_at(const size_t index)                                  override;
    Config      get_and_pop_back()                                                  override;
    Config      get_and_pop_at(const std::string& key)                              override;
    void        erase_front()                                                       override;
//TODO: когда-нибудь, void erase_at(iterator)
    void        erase_at(const size_t index)                                        override;
    void        erase_back()                                                        override;
    void        erase_at(const std::string& key)                                    override;
    // ======================================================================================================== Removing

    // Info ============================================================================================================
    bool        isContainer()                                       const noexcept  override    { return true; }
    size_t      size()                                              const noexcept  override    { return m_values.size(); }
    // ============================================================================================================ Info

    // Operators =======================================================================================================
    // ======================================================================================================= Operators

    // String ==========================================================================================================
    //вывод без комментариев, "tabulation_level == -1" => запись в одну строку
    std::string toString(const ConfigFormat format = ConfigFormat::eONLY_VALUE,
                         const int8_t tabulation_level = 0,
                         const CommentDesign &design = {})  const noexcept  override;
    // ========================================================================================================== String

    // File ============================================================================================================
    // ============================================================================================================ File

    // Parser ==========================================================================================================
    // ========================================================================================================== Parser

    // Iterators =======================================================================================================
    VPairElement::iterator          begin()                         noexcept                    { return m_values.begin(); }
    VPairElement::iterator          end()                           noexcept                    { return m_values.end(); }
    VPairElement::const_iterator    cbegin()                        const noexcept              { return m_values.cbegin(); }
    VPairElement::const_iterator    cend()                          const noexcept              { return m_values.cend(); }
    // ======================================================================================================= Iterators
};

bool IsElementJson(const std::string& str)  noexcept;
bool IsElementJson(const IElement& e)       noexcept;
bool IsElementJson(const Config& cfg)       noexcept;

#endif // ELEMENT_JSON_H
