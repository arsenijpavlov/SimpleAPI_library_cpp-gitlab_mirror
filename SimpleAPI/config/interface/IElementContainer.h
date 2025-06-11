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
    //индивидуально для потомков (разные условия хранения)
    // ========================================================================================================== Modify

    // Adding ==========================================================================================================
    //индивидуально для потомков (разные условия хранения)
    // ========================================================================================================== Adding

    // Removing ========================================================================================================
    virtual void    pop_front()                                                             = 0;
    virtual void    pop_at(const size_t index)                                              = 0;
    virtual void    pop_back()                                                              = 0;
    virtual Config  get_and_pop_front()                                                     = 0;
    virtual Config  get_and_pop_at(const size_t index)                                      = 0;
    virtual Config  get_and_pop_back()                                                      = 0;
    virtual void    erase_front()                                                           = 0;
    virtual void    erase_at(const size_t index)                                            = 0;
    virtual void    erase_back()                                                            = 0;

    void            remove_front()                                                          { erase_front(); }
    void            remove_at(const size_t index)                                           { erase_at(index); }
    void            remove_back()                                                           { erase_back(); }
    // ======================================================================================================== Removing

    // Operators =======================================================================================================
    virtual Config& operator[](const size_t index)                                          = 0;
    virtual Config  operator[](const size_t index)                          const           = 0;
    // ======================================================================================================= Operators

    // Iterators =======================================================================================================
    //индивидуально для потомков
    // ======================================================================================================= Iterators
};

#endif // I_ELEMENT_CONTAINER_H
