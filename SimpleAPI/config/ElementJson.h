#ifndef ELEMENT_JSON_H
#define ELEMENT_JSON_H

#include "ConfigDefines.h"
#include "ElementArray.h"
#include "interface/IElementMapContainer.h"


class ElementJson : public IElementMapContainer {
protected:

public:
    ElementJson()                                                           noexcept                    { init(); }
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
    explicit ElementJson(const IElement& element)                           noexcept                    { init(); }
    ~ElementJson()                                                          noexcept                    {}

    // Setters =========================================================================================================
private:
    void        init()                                                      noexcept                    { m_type = ValueType::eArray; }
public:
    void    set(const std::string& key, const Config& new_value,
                     bool update = true)                                    noexcept        override    { ... }
    void    set(const std::string& key, Config&& new_value,
                     bool update = true)                                    noexcept        override    { ... }
    // ========================================================================================================= Setters

    // Getters =========================================================================================================
    Config& get_front()                                                                     override    { ... }
    Config  get_front()                                                     const           override    { ... }
    Config& get_at(const size_t index)                                                      override    { ... }
    Config  get_at(const size_t index)                                      const           override    { ... }
    Config& get_back()                                                                      override    { ... }
    Config  get_back()                                                      const           override    { ... }
    //если элемента с таким ключом не существует - создать пустой и вернуть его
    Config& get_at(const std::string& key)                                  noexcept        override    { ... }
    Config  get_at(const std::string& key)                                  const noexcept  override    { ... }
    // ========================================================================================================= Getters

    // Modify ==========================================================================================================
    // ========================================================================================================== Modify

    // Adding ==========================================================================================================
    //одиночные элементы
    void    insert_front(const std::string& key, const Config& value)       noexcept        override    { ... }
    void    insert_front(const std::string& key, Config&& value)            noexcept        override    { ... }
    void    insert_at(const size_t index, const std::string& key,
                   const Config& value)                                     noexcept        override    { ... }
    void    insert_at(const size_t index, const std::string& key,
                   Config&& value)                                          noexcept        override    { ... }
    void    insert_at(VElement::iterator iterator, const std::string& key,
                   const Config& value)                                                     override    { ... }
    void    insert_at(VElement::iterator iterator, const std::string& key,
                   Config&& value)                                                          override    { ... }
    void    insert_back(const std::string& key, const Config& value)        noexcept        override    { ... }
    void    insert_back(const std::string& key, Config&& value)             noexcept        override    { ... }

    void    insert_at(const std::string& key, const Config& value)          noexcept        override    { ... }
    void    insert_at(const std::string& key, Config&& value)               noexcept        override    { ... }
    //если нужного элемента нет - создать пустым и положить этот после
    void    insert_after(const std::string& key, const Config& value)       noexcept        override    { ... }
    void    insert_after(const std::string& key, Config&& value)            noexcept        override    { ... }
    void    insert_before(const std::string& key, const Config& value)      noexcept        override    { ... }
    void    insert_before(const std::string& key, Config&& value)           noexcept        override    { ... }
    // ========================================================================================================== Adding

    // Removing ========================================================================================================
    void    pop_front()                                                                     override    { ... }
    void    pop_at(const size_t index)                                                      override    { ... }
    void    pop_back()                                                                      override    { ... }
    Config  get_and_pop_front()                                             noexcept        override    { ... }
    Config  get_and_pop_at(const size_t index)                              noexcept        override    { ... }
    Config  get_and_pop_back()                                              noexcept        override    { ... }
    void    erase_front()                                                                   override    { ... }
    void    erase_at(const size_t index)                                                    override    { ... }
    void    erase_back()                                                                    override    { ... }

    Config  get_and_pop_first()                                             noexcept        override    { ... }
    Config  get_and_pop_at(const std::string& key)                          noexcept        override    { ... }
    void    erase_at(const std::string& key)                                noexcept        override    { ... }
    // ======================================================================================================== Removing

    // Info ============================================================================================================
    // ============================================================================================================ Info

    // Operators =======================================================================================================
    // ======================================================================================================= Operators

    // String ==========================================================================================================
    //вывод без комментариев, "tabulation_level == -1" => запись в одну строку
    std::string toString(const ConfigFormat format = ConfigFormat::eONLY_VALUE,
                         const int8_t tabulation_level = 0,
                         const CommentDesign &design = {})                  const noexcept  override;
    // ========================================================================================================== String

    // File ============================================================================================================
    // ============================================================================================================ File

    // Parser ==========================================================================================================
    // ========================================================================================================== Parser

    // Iterators =======================================================================================================
    // ======================================================================================================= Iterators
};

bool IsElementJson(const std::string& str)  noexcept;
bool IsElementJson(const IElement& e)       noexcept;
bool IsElementJson(const Config& cfg)       noexcept;

#endif // ELEMENT_JSON_H
