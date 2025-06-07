#ifndef I_ELEMENT_MAP_CONTAINER_H
#define I_ELEMENT_MAP_CONTAINER_H

#include "IElementContainer.h"


class IElementMapContainer : public IElementContainer {
    // Getters =========================================================================================================
    virtual Config& get_at(const std::string& key)                              noexcept        = 0;
    virtual Config  get_at(const std::string& key)                              const noexcept  = 0;
    // ========================================================================================================= Getters

    // Modify ==========================================================================================================
    //если нужного ключа не существует, то будет добавлено N пустых элементов до необходимого ключа
    virtual void    update_at(const std::string& key, const Config& new_value)  noexcept        = 0;
    virtual void    update_at(const std::string& key, Config&& new_value)       noexcept        = 0;
    // ========================================================================================================== Modify

    // Adding ==========================================================================================================
    virtual void    insert_at(const std::string& key, const Config& value)      noexcept        = 0;
    virtual void    insert_at(const std::string& key, Config&& value)           noexcept        = 0;
    //если нужного элемента нет - создать пустым и положить этот после
    virtual void    insert_after(const std::string& key, const Config& value)   noexcept        = 0;
    virtual void    insert_after(const std::string& key, Config&& value)        noexcept        = 0;
    // ========================================================================================================== Adding

    // Removing ========================================================================================================
    virtual Config  get_and_pop_at(const std::string& key)                                      = 0;
    virtual void    erase_at(const std::string& key)                                            = 0;
    // ======================================================================================================== Removing

    // Operators =======================================================================================================
    Config&         operator[](const std::string& key)                          noexcept;
    Config          operator[](const std::string& key)                          const noexcept;
    // ======================================================================================================= Operators
};

#endif // I_ELEMENT_MAP_CONTAINER_H
