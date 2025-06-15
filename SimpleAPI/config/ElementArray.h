#ifndef ELEMENT_ARRAY_H
#define ELEMENT_ARRAY_H

#include "interface/IElementNonMapContainer.h"


class ElementArray : public IElementNonMapContainer {
protected:

public:
    ElementArray()                                                          noexcept                    { init(); }
    template<typename ... Types>
    explicit ElementArray(Types&&... args) noexcept {
        init();
        (void)std::initializer_list<int>{(push_back(std::forward<Types>(args)), 0)...};
    }
    ElementArray(const std::string& string, const ConfigFormat format,
                 const bool enable_comments = false)                        noexcept;
    ~ElementArray()                                                         noexcept                    {}

    // Setters =========================================================================================================
private:
    void        init()                                                      noexcept                    { m_type = ValueType::eArray; }
public:
    // ========================================================================================================= Setters

    // Getters =========================================================================================================
    Config&     get_front()                                                                 override;
    Config      get_front()                                                 const           override;
    Config&     get_at(const size_t index)                                                  override;
    Config      get_at(const size_t index)                                  const           override;
    Config&     get_back()                                                                  override;
    Config      get_back()                                                  const           override;
    // ========================================================================================================= Getters

    // Modify ==========================================================================================================
    // ========================================================================================================== Modify

    // Adding ==========================================================================================================
    //одиночные элементы
    void        insert_front(const Config& value)                           noexcept        override    { ... }
    void        insert_front(Config&& value)                                noexcept        override    { ... }
    void        insert_at(const size_t index, const Config& value)          noexcept        override    { ... }
    void        insert_at(const size_t index, Config&& value)               noexcept        override    { ... }
    void        insert_at(VElement::iterator iterator, const Config& value)                 override    { ... }
    void        insert_at(VElement::iterator iterator, Config&& value)                      override    { ... }
    void        insert_back(const Config& value)                            noexcept        override    { ... }
    void        insert_back(Config&& value)                                 noexcept        override    { ... }
    // ========================================================================================================== Adding

    // Removing ========================================================================================================
    void        pop_front()                                                                 override    { ... }
    void        pop_at(const size_t index)                                                  override    { ... }
    void        pop_back()                                                                  override    { ... }
    Config      get_and_pop_front()                                         noexcept        override    { ... }
    Config      get_and_pop_at(const size_t index)                          noexcept        override    { ... }
    Config      get_and_pop_back()                                          noexcept        override    { ... }
    void        erase_front()                                                               override    { ... }
    void        erase_at(const size_t index)                                                override    { ... }
    void        erase_back()                                                                override    { ... }
    // ======================================================================================================== Removing

    // Info ============================================================================================================
    // ============================================================================================================ Info

    // Operators =======================================================================================================
    // ======================================================================================================= Operators

    // String ==========================================================================================================
    //вывод без комментариев, "tabulation_level == -1" => запись в одну строку
    std::string toString(const ConfigFormat format = ConfigFormat::eONLY_VALUE,
                                 const int8_t tabulation_level = 0,
                                 const CommentDesign &design = {})          const noexcept  override;
    // ========================================================================================================== String

    // File ============================================================================================================
    // ============================================================================================================ File

    // Parser ==========================================================================================================
    // ========================================================================================================== Parser

    // Iterators =======================================================================================================
    // ======================================================================================================= Iterators
};

bool IsElementArray(const std::string& str, const ConfigFormat format = ConfigFormat::eJSON) noexcept;
bool IsElementJsonArray(const std::string& str) noexcept;
bool IsElementIniArray(const std::string& str) noexcept;
bool IsElementArray(const IElement& e) noexcept;

#endif // ELEMENT_ARRAY_H
