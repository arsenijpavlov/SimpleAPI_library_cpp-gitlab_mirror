#ifndef I_ELEMENT_NON_MAP_CONTAINER_H
#define I_ELEMENT_NON_MAP_CONTAINER_H

#include "IElementContainer.h"


class IElementNonMapContainer : public IElementContainer {
protected:
    VElement m_values;

public:
    // Getters =========================================================================================================
    // ========================================================================================================= Getters

    // Modify ==========================================================================================================
    // ========================================================================================================== Modify

    // Adding ==========================================================================================================
    //одиночные элементы
    virtual void    insert_front(const Config& value)                                   noexcept    = 0;
    virtual void    insert_front(Config&& value)                                        noexcept    = 0;
    virtual void    insert_at(const size_t index, const Config& value)                  noexcept    = 0;
    virtual void    insert_at(const size_t index, Config&& value)                       noexcept    = 0;
    virtual void    insert_at(VElement::iterator iterator, const Config& value)                     = 0;
    virtual void    insert_at(VElement::iterator iterator, Config&& value)                          = 0;
    virtual void    insert_back(const Config& value)                                    noexcept    = 0;
    virtual void    insert_back(Config&& value)                                         noexcept    = 0;

    //группы элементов
    void            insert_front(const VElement& elements)                              noexcept;
    void            insert_front(VElement&& elements)                                   noexcept;
    void            insert_at(const size_t index, const VElement& elements)             noexcept;
    void            insert_at(const size_t index, VElement&& elements)                  noexcept;
    void            insert_back(const VElement& elements)                               noexcept;
    void            insert_back(VElement&& elements)                                    noexcept;

    //неизвестное количество элементов
                    template<typename ... Args>
    void            insert_front(Args&& ... args) noexcept {
                        VElement ve;
                        (void)std::initializer_list<int>{(ve.push_back(std::forward<Args>(args)), 0)...};
                        insert_front(std::move(ve));
                    }
                    template<typename ... Args>
    void            insert_at(const size_t index, Args&& ... args) noexcept {
                        VElement ve;
                        (void)std::initializer_list<int>{(insert_back(std::forward<Args>(args)), 0)...};
                        insert_at(index, std::move(ve));
                    }
                    template<typename ... Args>
    void            insert_back(Args&& ... args) noexcept {
                        (void)std::initializer_list<int>{(insert_back(std::forward<Args>(args)), 0)...};
                    }

    //другое имя для тех же действий
    void            push_front(const Config& value)                                     noexcept    { insert_front(value); }
    void            push_front(Config&& value)                                          noexcept    { insert_front(std::move(value)); }
    void            push_back(const Config& value)                                      noexcept    { insert_back(value); }
    void            push_back(Config&& value)                                           noexcept    { insert_back(std::move(value)); }

                    template<typename ... Args>
    void            push_front(Args&& ... args)                                         noexcept    { insert_front((args)...); }
                    template<typename ... Args>
    void            push_back(Args&& ... args)                                          noexcept    { (void)std::initializer_list<int>{(insert_back(std::forward<Args>(args)), 0)...}; }
private:
    void            append_null(size_t size)                                            noexcept;
public:
    // ========================================================================================================== Adding

    // Removing ========================================================================================================
    // ======================================================================================================== Removing

    // Info ============================================================================================================
    size_t          size()                                                              const noexcept  override    { return m_values.size(); }
    // ============================================================================================================ Info

    // Operators =======================================================================================================
    Config&         operator[](const size_t index)                                                      override;
    Config          operator[](const size_t index)                                      const           override;
    // ======================================================================================================= Operators

    // Iterators =======================================================================================================
    VElement::iterator          begin()                                                 noexcept                    { return m_values.begin(); }
    VElement::iterator          end()                                                   noexcept                    { return m_values.end(); }
    VElement::const_iterator    cbegin()                                                const noexcept              { return m_values.cbegin(); }
    VElement::const_iterator    cend()                                                  const noexcept              { return m_values.cend(); }
    // ======================================================================================================= Iterators
};

#endif // I_ELEMENT_NON_MAP_CONTAINER_H
