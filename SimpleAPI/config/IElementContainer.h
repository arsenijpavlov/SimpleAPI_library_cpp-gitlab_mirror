#ifndef I_ELEMENT_CONTAINER_H
#define I_ELEMENT_CONTAINER_H

#include "IElement.h"


//абстрактный класс для управления списками элементов
class IElementContainer : public IElement {
    // Getters =========================================================================================================
    virtual Config& get_front()                                                             = 0;
    virtual Config  get_front()                                             const           = 0;
    virtual Config& get_at(const size_t index)                                              = 0;
    virtual Config  get_at(const size_t index)                              const           = 0;
    virtual Config& get_back()                                                              = 0;
    virtual Config  get_back()                                              const           = 0;
    // ========================================================================================================= Getters

    // Modify ==========================================================================================================
    virtual void    update_front(const Config& new_value)                   noexcept        = 0;
    virtual void    update_front(Config&& new_value)                        noexcept        = 0;
    //если нужного индекса не существует, то будет добавлено N пустых элементов до необходимого индекса
    virtual void    update_at(const size_t index, const Config& new_value)  noexcept        = 0;
    virtual void    update_at(const size_t index, Config&& new_value)       noexcept        = 0;
    virtual void    update_back(const Config& new_value)                    noexcept        = 0;
    virtual void    update_back(Config&& new_value)                         noexcept        = 0;
    // ========================================================================================================== Modify

    // Adding ==========================================================================================================
    virtual void    insert_front(const Config& value)                       noexcept        = 0;
    virtual void    insert_front(Config&& value)                            noexcept        = 0;
//TODO: когда-нибудь, virtual void insert_at(iterator, const Config& value);
//TODO: когда-нибудь, virtual void insert_at(iterator, Config&& value);
    virtual void    insert_at(const size_t index, const Config& value)      noexcept        = 0;
    virtual void    insert_at(const size_t index, Config&& value)           noexcept        = 0;
    virtual void    insert_back(const Config& value)                        noexcept        = 0;
    virtual void    insert_back(Config&& value)                             noexcept        = 0;

    virtual void    push_front(const Config& value)                         noexcept        { insert_front(value); }
    virtual void    push_front(Config&& value)                              noexcept        { insert_front(std::move(value)); }
    virtual void    push_back(const Config& value)                          noexcept        { insert_back(value); }
    virtual void    push_back(Config&& value)                               noexcept        { insert_back(std::move(value)); }
    // ========================================================================================================== Adding

    // Removing ========================================================================================================
    virtual Config  get_and_pop_front()                                                     = 0;
    virtual Config  get_and_pop_at(const size_t index)                                      = 0;
    virtual Config  get_and_pop_back()                                                      = 0;
    virtual void    erase_front()                                                           = 0;
//TODO: когда-нибудь, virtual void erase_at(iterator)
    virtual void    erase_at(const size_t index)                                            = 0;
    virtual void    erase_back()                                                            = 0;
    // ======================================================================================================== Removing

    // Operators =======================================================================================================
    Config&         operator[](const size_t index);
    Config          operator[](const size_t index)                          const;
    // ======================================================================================================= Operators

    // Iterators =======================================================================================================
    //индивидуально для потомков
    //<TYPE>::iterator begin();
    //<TYPE>::iterator cbegin() const;
    //<TYPE>::iterator end();
    //<TYPE>::iterator cend() const;
    // ======================================================================================================= Iterators
};

#endif // I_ELEMENT_CONTAINER_H
