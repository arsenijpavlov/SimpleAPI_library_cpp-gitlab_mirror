#include "Timer.h"


namespace simpleapi {

utils::Timer &utils::Timer::operator=(const Timer &other) noexcept
{
    m_timer     = other.m_timer;
    m_is_active = false;
    return *this;
}

void utils::Timer::start() noexcept
{
    m_time_of_start = std::chrono::system_clock::now();
    m_is_active     = true;
}

bool utils::Timer::hasExpired() noexcept
{
    bool expired = std::chrono::system_clock::now() - m_time_of_start > m_timer;
    if(expired)
        m_is_active = false;
    return expired;
}

utils::Timer &utils::Timer::operator=(Timer &&other) noexcept
{
    m_timer     = other.m_timer;
    m_is_active = false;
    return *this;
}

} // namespace simpleapi
