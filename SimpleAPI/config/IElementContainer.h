#ifndef I_ELEMENT_CONTAINER_H
#define I_ELEMENT_CONTAINER_H

#include "IElement.h"


//абстрактный класс для управления списками элементов
class IElementContainer : public IElement {
    // Getters =========================================================================================================
    virtual Config& get_front()                                                         = 0;
    virtual Config  get_front()                                                 const   = 0;
    virtual Config& get_at(const size_t index)                                          = 0;
    virtual Config  get_at(const size_t index)                                  const   = 0;
    virtual Config& get_at(const std::string& key)                                      = 0;
    virtual Config  get_at(const std::string& key)                              const   = 0;
    virtual Config& get_back()                                                          = 0;
    virtual Config  get_back()                                                  const   = 0;
    // ========================================================================================================= Getters

    // Modify ==========================================================================================================
    //NOTE: не путать с remove(), здесь просто сброс комментариев, сброс значения до:
    //контейнеры
    //TODO: erase_front
    //TODO: erase_at(iterator)
    //TODO: erase_at(index)
    //TODO: erase_at(key)
    //TODO: erase_backf

    //TODO: insert_front
    //TODO: insert_at(iterator)
    //TODO: insert_at(index)
    //TODO: insert_at(key)
    //TODO: insert_back
    //TODO: insert_after(key)
    // ========================================================================================================== Modify

    // Operators =======================================================================================================
    //контейнеры (индивидуально для ElementContainer классов)
    virtual Config& operator[](const size_t index)                                      = 0;
    virtual Config  operator[](const size_t index)                              const   = 0;
    virtual Config& operator[](const std::vector<size_t>& indexes)                      = 0;
    virtual Config  operator[](const std::vector<size_t>& indexes)              const   = 0;
    // ======================================================================================================= Operators

    // Iterators =======================================================================================================
    //индивидуально для ElementContainer классов
    // (!) Для foreach итераторы необходимо кастить вручную к нужному типу; за основу брать типы ниже
//    /* (!) */ void*                 begin();
//    /* (!) */ void*                 cbegin()                        const;
//    /* (!) */ void*                 end();
//    /* (!) */ void*                 cend()                          const;
    // (!) Для foreach итераторы необходимо кастить вручную к нужному типу; за основу брать типы ниже
    // ======================================================================================================= Iterators
};

#endif // I_ELEMENT_CONTAINER_H
