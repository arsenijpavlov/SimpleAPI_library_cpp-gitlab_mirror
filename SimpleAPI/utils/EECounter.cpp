#include "EECounter.h"
#include <stdexcept>


namespace simpleapi {

#define __INCOMPATIBLE_SIZE_EXCEPTION__ \
            if(m_max_size != other.m_max_size) \
                throw std::invalid_argument("try using incompatible size format");


EECounter::EECounter(uint64_t size) noexcept {
    m_max_size   = size;
    m_pos        = 0;
    m_global_pos = 0;
}

EECounter::EECounter(const EECounter &other) noexcept {
    m_max_size   = other.m_max_size;
    m_pos        = other.m_pos;
    m_global_pos = other.m_global_pos;
}

// @TEST(UTILS, EndToEndCounter_set_overflowing)
void EECounter::set_pos(uint64_t pos) noexcept {
    while(pos > m_max_size) {
        pos -= m_max_size;
        if(m_global_pos == m_max_size - 1)
            m_global_pos = 0;
        else
            ++m_global_pos;
    }
    m_pos = pos;
}

// @TEST(UTILS, EndToEndCounter_set_overflowing)
void EECounter::set_pos(uint64_t glob_pos, uint64_t pos) noexcept
{
    while(glob_pos > m_max_size)
        glob_pos -= m_max_size;

    m_global_pos = glob_pos;

    while(pos > m_max_size) {
        pos -= m_max_size;
        if(m_global_pos == m_max_size - 1)
            m_global_pos = 0;
        else
            ++m_global_pos;
    }
    m_pos = pos;
}

// @TEST(UTILS, EndToEndCounter_set_overflowing)
void EECounter::set_glob_pos(uint64_t glob_pos) noexcept {
    while(glob_pos > m_max_size)
        glob_pos -= m_max_size;

    m_global_pos = glob_pos;
}

bool EECounter::operator==(const EECounter &other) const {
    __INCOMPATIBLE_SIZE_EXCEPTION__

    if(m_pos == other.m_pos && m_global_pos == other.m_global_pos)  return true;
    else                                                            return false;
}

bool EECounter::operator!=(const EECounter &other) const {
    __INCOMPATIBLE_SIZE_EXCEPTION__

    if(m_pos != other.m_pos || m_global_pos != other.m_global_pos)  return true;
    else                                                            return false;
}

bool EECounter::operator<(const EECounter &other) const {
    __INCOMPATIBLE_SIZE_EXCEPTION__

    if(m_pos < other.m_pos && m_global_pos <= other.m_global_pos)   return true;
    else                                                            return false;
}

bool EECounter::operator>(const EECounter &other) const {
    __INCOMPATIBLE_SIZE_EXCEPTION__

    if(m_pos > other.m_pos && m_global_pos >= other.m_global_pos)   return true;
    else                                                            return false;
}

bool EECounter::operator<=(const EECounter &other) const {
    try {
        if(*this == other)  return true;
        if(*this < other)   return true;
    } catch(std::exception e) {
        throw e;
    }

    return false;
}

bool EECounter::operator>=(const EECounter &other) const {
    try {
        if(*this == other)  return true;
        if(*this > other)   return true;
    } catch(std::exception e) {
        throw e;
    }

    return false;
}

EECounter& EECounter::operator++() noexcept {
    add();
    return *this;
}

EECounter& EECounter::operator--() noexcept {
    sub();
    return *this;
}

EECounter EECounter::operator++(int) noexcept {
    EECounter saved(*this);
    add();
    return saved;
}

EECounter EECounter::operator--(int) noexcept {
    EECounter saved(*this);
    sub();
    return saved;
}

EECounter EECounter::operator+(const uint64_t& step) noexcept {
    EECounter saved(*this);
    saved.add(step);
    return saved;
}

EECounter EECounter::operator-(const uint64_t& step) noexcept {
    EECounter saved(*this);
    saved.sub(step);
    return saved;
}

EECounter &EECounter::operator=(const EECounter &other) noexcept {
    m_max_size      = other.m_max_size;
    m_pos           = other.m_pos;
    m_global_pos    = other.m_global_pos;

    return *this;
}

uint64_t EECounter::get_add() noexcept {
    EECounter saved(*this);
    add();
    return saved.m_pos;
}

uint64_t EECounter::get_next() noexcept {
    EECounter saved(*this);
    return (++saved).get();
}

// TODO: написать тест
EECounter EECounter::operator+(const EECounter& other) {
    __INCOMPATIBLE_SIZE_EXCEPTION__

    EECounter saved(*this);

    saved.m_global_pos += other.get_glob();
    while(saved.m_global_pos > saved.m_max_size)
        saved.m_global_pos -= saved.m_max_size;

    saved.m_pos += other.m_pos;
    while(saved.m_pos > saved.m_max_size)
    {
        saved.m_pos -= saved.m_max_size;
        saved.m_global_pos++;
        if(saved.m_global_pos == saved.m_max_size)
            saved.m_global_pos = 0;
    }

    return saved;
}

// TODO: написать тест
EECounter EECounter::operator-(const EECounter& other) {
    __INCOMPATIBLE_SIZE_EXCEPTION__

    EECounter saved(*this);

    if(saved.m_global_pos < other.m_global_pos)
        saved.m_global_pos = (saved.m_global_pos + saved.m_max_size) - other.m_global_pos;
    else {
        saved.m_global_pos -= other.m_global_pos;
    }

    if(other.m_pos < saved.m_pos) {
        saved.m_pos = (saved.m_pos + saved.m_max_size) - other.m_pos;

        --saved.m_global_pos;
        if(saved.m_global_pos == 0)
            saved.m_global_pos = saved.m_max_size - 1;
    } else
        saved.m_pos -= other.m_pos;

    return saved;
}

// @TEST(UTILS, EndToEndCounter_sub_overflowing)
void EECounter::add(uint64_t step) noexcept {
    while(step >= m_max_size) {
        step -= m_max_size;

        ++m_global_pos;
        if(m_global_pos == m_max_size)
            m_global_pos = 0;
    }
    m_pos += step;
    if(m_pos >= m_max_size)
        m_pos -= m_max_size;
}

// @TEST(UTILS, EndToEndCounter_sub_overflowing)
void EECounter::sub(uint64_t step) noexcept {
    while(step >= m_max_size)
    {
        step -= m_max_size;
        --m_global_pos;
        if(m_global_pos == 0)
            m_global_pos = m_max_size - 1;
    }
    if(m_pos < step)
        m_pos = m_pos + m_max_size - step;
    else
        m_pos -= step;
}

void EECounter::reset() noexcept {
    m_pos        = 0;
    m_global_pos = 0;
}

} // namespace simpleapi

