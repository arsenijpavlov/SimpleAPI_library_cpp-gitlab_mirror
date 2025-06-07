#ifndef ELEMENT_ARRAY_H
#define ELEMENT_ARRAY_H

#include "IElementNonMapContainer.h"


class ElementArray : public IElementNonMapContainer {
protected:
    VElement m_values;

public:
    ElementArray() noexcept                                                                     { init(); }
    template<typename ... Types>
    explicit ElementArray(Types&&... args) noexcept {
        init();
        //NOTE: без объявления массива не работает в c++11
        int dummy[] = { (push_back(std::forward<Types>(args)), 0)... };
        (void)dummy; //suppress unused variable warning
    }
    ElementArray(const std::string& string, const ConfigFormat format,
                 const bool enable_comments = false)                noexcept;
    ~ElementArray()                                                 noexcept                    {}

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
    // ========================================================================================================== Adding

    // Removing ========================================================================================================
    Config      get_and_pop_front()                                                 override;
    Config      get_and_pop_at(const size_t index)                                  override;
    Config      get_and_pop_back()                                                  override;
    void        erase_front()                                                       override;
//TODO: когда-нибудь, void erase_at(iterator)
    void        erase_at(const size_t index)                                        override;
    void        erase_back()                                                        override;
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
    VElement::iterator       begin()                                noexcept                    { return m_values.begin(); }
    VElement::iterator       end()                                  noexcept                    { return m_values.end(); }
    VElement::const_iterator cbegin()                               const noexcept              { return m_values.cbegin(); }
    VElement::const_iterator cend()                                 const noexcept              { return m_values.cend(); }
    // ======================================================================================================= Iterators
};

bool IsElementArray(const std::string& str, const ConfigFormat format = ConfigFormat::eJSON) noexcept;
bool IsElementJsonArray(const std::string& str) noexcept;
bool IsElementIniArray(const std::string& str) noexcept;
bool IsElementArray(const IElement& e) noexcept;

#endif // ELEMENT_ARRAY_H
