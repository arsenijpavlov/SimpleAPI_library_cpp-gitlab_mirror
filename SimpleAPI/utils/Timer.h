#pragma once

#include <chrono>


namespace simpleapi {
namespace utils {

class Timer {
    std::chrono::system_clock::time_point m_time_of_start;
    // храним в минимально возможном виде для возможности "на ходу" изменять периодичность таймера
    std::chrono::nanoseconds m_timer;
    bool m_is_active;

public:
    template <typename Rep, typename Period>
    explicit Timer(std::chrono::duration<Rep, Period> value) noexcept
        : m_timer(value), m_is_active(false) {}

    explicit Timer(const Timer& other)      noexcept : m_timer(other.m_timer), m_is_active(false) {}
    explicit Timer(Timer&& other)           noexcept : m_timer(other.m_timer), m_is_active(false) {}
    ~Timer() noexcept {}

    Timer& operator=(const Timer& other)    noexcept
    {
        m_timer     = other.m_timer;
        m_is_active = false;
        return *this;
    }
    Timer& operator=(Timer&& other) noexcept
    {
        m_timer     = other.m_timer;
        m_is_active = false;
        return *this;
    }

    void start() noexcept
    {
        m_time_of_start = std::chrono::system_clock::now();
        m_is_active     = true;
    }
    template <typename Rep, typename Period>
    void setInterval(const std::chrono::duration<Rep, Period>& value) noexcept
    {
        m_timer     = value;
        m_is_active = false;
    }
    bool hasExpired() noexcept
    {
        bool expired = std::chrono::system_clock::now() - m_time_of_start > m_timer;
        if(expired)
            m_is_active = false;
        return expired;
    }
    bool isActive() noexcept
    { return m_is_active; }
};

} // namespace utils
} // namespace simpleapi
