#ifndef OPTIONAL_H
#define OPTIONAL_H

#include <algorithm>

template <typename T>
class Optional {
    T    m_value;
    bool m_status;
public:
    Optional<T>()                                       noexcept
        : m_status(false)
    {}
    Optional<T>(const T& value)                         noexcept
    {
        set(value);
    }
    Optional<T>(T&& value)                              noexcept
    {
        set(std::move(value));
    }
    Optional<T>(const Optional<T>& other)               noexcept
    {
        set(other);
    }
    Optional<T>(Optional<T>&& other)                    noexcept
    {
        set(std::move(other));
    }

    Optional<T>& operator=(const T& value)              noexcept
    {
        set(value);
        return *this;
    }
    Optional<T>& operator=(T&& value)                   noexcept
    {
        set(std::move(value));
        return *this;
    }
    Optional<T>& operator=(const Optional<T>& other)    noexcept
    {
        set(other);
        return *this;
    }
    Optional<T>& operator=(Optional<T>&& other)         noexcept
    {
        set(std::move(other));
        return *this;
    }

    void set(const T& value)                            noexcept
    {
        m_value = value;
        m_status = true;
    }
    void set(T&& value)                                 noexcept
    {
        m_value = std::move(value);
        m_status = true;
    }
    void set(const Optional<T>& other)                  noexcept
    {
        m_value = other.m_value;
        m_status = true;
    }
    void set(Optional<T>&& other)                       noexcept
    {
        m_value = other.m_value;
        m_status = true;
    }

    void    unset()                                     noexcept        { m_status = false; }
    bool    isValid()                                   const noexcept  { return m_status; }
    T&      value()                                     noexcept        { return m_value; }
    T       value()                                     const noexcept  { return m_value; }

    bool operator==(const T& value)                     const noexcept  { return m_status && m_value == value; }
    bool operator!=(T&& value)                          const noexcept  { return !(*this == value); }
    bool operator==(const Optional<T>& other)           const noexcept  { return !(*this != other); }
    bool operator!=(Optional<T>&& other)                const noexcept  { return (m_status != other.m_status) || (m_value != other); }
};

#endif // OPTIONAL_H
