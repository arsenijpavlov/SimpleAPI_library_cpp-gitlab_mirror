#ifndef I_ELEMENT_NON_MAP_CONTAINER_H
#define I_ELEMENT_NON_MAP_CONTAINER_H

#include "IElementContainer.h"


class IElementNonMapContainer : public IElementContainer {
protected:
    VElement m_values;

public:
    // Getters =========================================================================================================
    Config& get_front()                                                             override;
    Config  get_front()                                                 const       override;
    Config& get_at(const size_t index)                                              override;
    Config  get_at(const size_t index)                                  const       override;
    Config& get_back()                                                              override;
    Config  get_back()                                                  const       override;
    // ========================================================================================================= Getters

    // Modify ==========================================================================================================
//    void    update_front(const Config& new_value)                                   noexcept;
//    void    update_front(Config&& new_value)                                        noexcept;
//    //если нужного индекса не существует, то будет добавлено N пустых элементов до необходимого индекса
//    void    update_at(const size_t index, const Config& new_value)                  noexcept;
//    void    update_at(const size_t index, Config&& new_value)                       noexcept;
//    void    update_at(const VElement::iterator iterator, const Config& new_value)   noexcept;
//    void    update_at(const VElement::iterator iterator, Config&& new_value)        noexcept;
//    void    update_back(const Config& new_value)                                    noexcept;
//    void    update_back(Config&& new_value)                                         noexcept;
    // ========================================================================================================== Modify

    // Adding ==========================================================================================================
    void    insert_front(const Config& value)                           noexcept;
    void    insert_front(Config&& value)                                noexcept;
    void    insert_at(const size_t index, const Config& value)          noexcept;
    void    insert_at(const size_t index, Config&& value)               noexcept;
    void    insert_at(VElement::iterator iterator, const Config& value) /*may throw except*/        { m_values.insert(iterator, std::make_shared<Config>(value)); }
    void    insert_at(VElement::iterator iterator, Config&& value)      /*may throw except*/        { m_values.insert(iterator, std::make_shared<Config>(std::move(value))); }
    void    insert_back(const Config& value)                            noexcept                    { m_values.push_back(std::make_shared<Config>(value)); }
    void    insert_back(Config&& value)                                 noexcept                    { m_values.push_back(std::make_shared<Config>(std::move(value))); }

    void    push_front(const Config& value)                             noexcept                    { insert_front(value); }
    void    push_front(Config&& value)                                  noexcept                    { insert_front(std::move(value)); }
    void    push_back(const Config& value)                              noexcept                    { insert_back(value); }
    void    push_back(Config&& value)                                   noexcept                    { insert_back(std::move(value)); }
private:
    void    append_null(size_t size)                                    noexcept;
public:
    // ========================================================================================================== Adding

    // Removing ========================================================================================================
    void    pop_at(const VElement::iterator iterator)                                               { m_values.erase(iterator); }
    Config  get_and_pop_at(const VElement::iterator iterator);
    void    erase_at(const VElement::iterator iterator)                                             { m_values.erase(iterator); }

    void    remove_at(const VElement::iterator iterator)                                            { erase_at(iterator); }
    // ======================================================================================================== Removing

    // Info ============================================================================================================
    size_t  size()                                                      const noexcept  override    { return m_values.size(); }
    // ============================================================================================================ Info

    // Operators =======================================================================================================
    Config& operator[](const size_t index);
    Config  operator[](const size_t index)                              const;
    // ======================================================================================================= Operators

    // Iterators =======================================================================================================
    VElement::iterator          begin()                                 noexcept                    { return m_values.begin(); }
    VElement::iterator          end()                                   noexcept                    { return m_values.end(); }
    VElement::const_iterator    cbegin()                                const noexcept              { return m_values.cbegin(); }
    VElement::const_iterator    cend()                                  const noexcept              { return m_values.cend(); }
    // ======================================================================================================= Iterators
};

#endif // I_ELEMENT_NON_MAP_CONTAINER_H
