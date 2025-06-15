#ifndef I_ELEMENT_MAP_CONTAINER_H
#define I_ELEMENT_MAP_CONTAINER_H

#include "IElementContainer.h"


class IElementMapContainer : public IElementContainer {
protected:
    VPairElement m_values;
public:
    // Setters =========================================================================================================
    virtual void    set(const std::string& key, const Config& new_value,
                     bool update = true)                                        noexcept                    = 0;
    virtual void    set(const std::string& key, Config&& new_value,
                     bool update = true)                                        noexcept                    = 0;
    void            set_if_null(const std::string& key, const Config& new_value)noexcept;
    void            set_if_null(const std::string& key, Config&& new_value)     noexcept;
    // ========================================================================================================= Setters

    // Getters =========================================================================================================
    //если элемента с таким ключом не существует - создать пустой и вернуть его
    virtual Config& get_at(const std::string& key)                                      noexcept            = 0;
    virtual Config  get_at(const std::string& key)                                      const noexcept      = 0;
    Config&         get_or_default_at(const std::string& key, Config&& default_value)   noexcept;
    Config          get_or_default_at(const std::string& key, Config&& default_value)   const noexcept;
    // ========================================================================================================= Getters

    // Modify ==========================================================================================================
    void            clear()                                                     noexcept    override        { m_values.clear(); }
    // ========================================================================================================== Modify

    // Adding ==========================================================================================================
    //одиночные элементы
    virtual void    insert_front(const std::string& key, const Config& value)   noexcept                    = 0;
    virtual void    insert_front(const std::string& key, Config&& value)        noexcept                    = 0;
    virtual void    insert_at(const size_t index, const std::string& key,
                           const Config& value)                                 noexcept                    = 0;
    virtual void    insert_at(const size_t index, const std::string& key,
                           Config&& value)                                      noexcept                    = 0;
    virtual void    insert_at(VElement::iterator iterator, const std::string& key,
                           const Config& value)                                                             = 0;
    virtual void    insert_at(VElement::iterator iterator, const std::string& key,
                           Config&& value)                                                                  = 0;
    virtual void    insert_back(const std::string& key, const Config& value)    noexcept                    = 0;
    virtual void    insert_back(const std::string& key, Config&& value)         noexcept                    = 0;

    //группы элементов
    void            insert_front(const VPairElement& elements)                  noexcept;
    void            insert_front(VPairElement&& elements)                       noexcept;
    void            insert_at(const size_t index, const VPairElement& elements) noexcept;
    void            insert_at(const size_t index, VPairElement&& elements)      noexcept;
    void            insert_back(const VPairElement& elements)                   noexcept;
    void            insert_back(VPairElement&& elements)                        noexcept;

    //неизвестное количество элементов
    template<typename ... Value>
    void            insert_front(std::pair<std::string, Value>&& ... pairs) noexcept {
                        VPairElement vpe;
                        if(vpe.capacity() < sizeof...(pairs))
                            vpe.reserve(sizeof...(pairs));
                        (void)std::initializer_list<int>{(vpe.push_back(std::forward<std::pair<std::string, Value>>(pairs)), 0)...};
                        insert_front(std::move(vpe));
                    }
                    template<typename ... Value>
    void            insert_at(const size_t index, std::pair<std::string, Value>&& ... pairs) noexcept {
                        VPairElement vpe;
                        (void)std::initializer_list<int>{(insert_back(std::forward<std::pair<std::string, Value>>(pairs)), 0)...};
                        insert_at(index, std::move(vpe));
                    }
                    template<typename ... Value>
    void            insert_back(std::pair<std::string, Value>&& ... pairs) noexcept {
                        (void)std::initializer_list<int>{(insert_back(std::forward<std::pair<std::string, Value>>(pairs)), 0)...};
                    }

    //другое имя для тех же действий
    void            push_front(const std::string& key, const Config& value)     noexcept                    { insert_front(key, value); }
    void            push_front(const std::string& key, Config&& value)          noexcept                    { insert_front(key, std::move(value)); }
    void            push_back(const std::string& key, const Config& value)      noexcept                    { insert_back(key, value); }
    void            push_back(const std::string& key, Config&& value)           noexcept                    { insert_back(key, std::move(value)); }

    virtual void    insert_at(const std::string& key, const Config& value)      noexcept                    = 0;
    virtual void    insert_at(const std::string& key, Config&& value)           noexcept                    = 0;
    //если нужного элемента нет - создать пустым и положить этот после
    virtual void    insert_after(const std::string& key, const Config& value)   noexcept                    = 0;
    virtual void    insert_after(const std::string& key, Config&& value)        noexcept                    = 0;
    virtual void    insert_before(const std::string& key, const Config& value)  noexcept                    = 0;
    virtual void    insert_before(const std::string& key, Config&& value)       noexcept                    = 0;
    // ========================================================================================================== Adding

    // Removing ========================================================================================================
    virtual Config  get_and_pop_first()                                         noexcept                    = 0;
    virtual Config  get_and_pop_at(const std::string& key)                      noexcept                    = 0;
    virtual void    erase_at(const std::string& key)                            noexcept                    = 0;
    // ======================================================================================================== Removing

    // Info ============================================================================================================
    bool            isContainer()                                               const noexcept  override    { return true; }
    bool            isMapContainer()                                            const noexcept  override    { return true; }
    size_t          size()                                                      const noexcept  override    { return m_values.size(); }
    bool            contains(const std::string& key)                            const noexcept;
    // ============================================================================================================ Info

    // Operators =======================================================================================================
    Config&         operator[](const std::string& key)                          noexcept;
    Config          operator[](const std::string& key)                          const noexcept;
    // ======================================================================================================= Operators

    // Iterators =======================================================================================================
    VPairElement::iterator          begin()                                     noexcept                    { return m_values.begin(); }
    VPairElement::iterator          end()                                       noexcept                    { return m_values.end(); }
    VPairElement::const_iterator    cbegin()                                    const noexcept              { return m_values.cbegin(); }
    VPairElement::const_iterator    cend()                                      const noexcept              { return m_values.cend(); }
    // ======================================================================================================= Iterators
};

#endif // I_ELEMENT_MAP_CONTAINER_H
