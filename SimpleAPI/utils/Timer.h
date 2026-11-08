#pragma once

#include <chrono>


namespace simpleapi {
namespace utils {

// NOTE: если на платформе доступен steady_clock, то нужно использовать его!
// тройной выбор по приоритету: steady, high_resolution, system
using ClockType = std::conditional<
    std::chrono::steady_clock::is_steady,
    std::chrono::steady_clock,
    std::conditional<
        std::chrono::high_resolution_clock::is_steady,
        std::chrono::high_resolution_clock,
        std::chrono::system_clock
        >
    >::type;

class Timer {
    ClockType::time_point m_time_of_start;
    // храним в минимально возможном виде для возможности "на ходу" изменять периодичность таймера
    std::chrono::nanoseconds m_timer;
    bool m_is_active;

public:
    template <typename Rep, typename Period>
    explicit Timer(std::chrono::duration<Rep, Period> value)    noexcept : m_timer(value), m_is_active(false) {}
    explicit Timer(const Timer& other)                          noexcept : m_timer(other.m_timer), m_is_active(other.m_is_active) {}
    explicit Timer(Timer&& other)                               noexcept : m_timer(other.m_timer), m_is_active(other.m_is_active) {}
    ~Timer() noexcept {}

    Timer& operator=(const Timer& other)                        noexcept;
    Timer& operator=(Timer&& other)                             noexcept;

    void start()                                                noexcept;
    bool hasExpired()                                           noexcept;
    bool isActive()                                             noexcept { return m_is_active; }

    template <typename Rep, typename Period>
    void setInterval(const std::chrono::duration<Rep, Period>& value) noexcept
    {
        m_timer     = value;
        m_is_active = false;
    }
};

} // namespace utils
} // namespace simpleapi
