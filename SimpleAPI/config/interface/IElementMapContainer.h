#ifndef I_ELEMENT_MAP_CONTAINER_H
#define I_ELEMENT_MAP_CONTAINER_H

#include "IElementContainer.h"


class IElementMapContainer : public IElementContainer {
protected:
    VPairElement m_values;
public:
    // Setters =========================================================================================================
    void    set(const std::string& key, const Config& new_value,
             bool update = true)                                        noexcept                    {}
    void    set(const std::string& key, Config&& new_value,
             bool update = true)                                        noexcept                    {}
    void    set_if_null(const std::string& key, const Config& new_value)noexcept                    {}
    void    set_if_null(const std::string& key, Config&& new_value)     noexcept                    {}
    // ========================================================================================================= Setters

    // Getters =========================================================================================================
    Config& get_front()                                                                 override    {}
    Config  get_front()                                                 const           override    {}
    Config& get_at(const size_t index)                                                  override    {}
    Config  get_at(const size_t index)                                  const           override    {}
    Config& get_back()                                                                  override    {}
    Config  get_back()                                                  const           override    {}
    Config& get_at(const std::string& key)                              noexcept                    {}
    Config  get_at(const std::string& key)                              const noexcept              {}
    // ========================================================================================================= Getters

    // Modify ==========================================================================================================
    void    clear()                                                     noexcept        override    { m_values.clear(); }
    //если нужного ключа не существует, то будет добавлено N пустых элементов до необходимого ключа
    //TODO: set_if_null(const std::string& key, const Config& new_value)        noexcept;
    //TODO: set_if_null(const std::string& key, Config&& new_value)             noexcept;
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
