#include "EECounter.h"
#include <stdexcept>


namespace simpleapi {

#define __INCOMPATIBLE_SIZE_EXCEPTION__ \
            if(m_max_size != other.m_max_size) \
                throw std::invalid_argument("try using incompatible size format");


EECounter::EECounter(uint64_t size) noexcept {
    m_max_size   = size - 1; // чтобы счёт был от 0 до N-1
    m_pos        = 0;
    m_global_pos = 0;
}

EECounter::EECounter(const EECounter &other) noexcept {
    m_max_size   = other.m_max_size;
    m_pos        = other.m_pos;
    m_global_pos = other.m_global_pos;
}

// TODO: нужна защита от переполнения
void EECounter::set_pos(const uint64_t &pos) noexcept {
    m_pos = pos;
}

// TODO: нужна защита от переполнения
void EECounter::set_glob_pos(const uint64_t &glob_m_pos) noexcept {
    m_global_pos = glob_m_pos;
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

EECounter EECounter::operator+(const EECounter& other) {
    __INCOMPATIBLE_SIZE_EXCEPTION__

    EECounter saved(*this);
    saved.m_global_pos += other.m_pos;
    saved.m_pos += other.m_pos;
    if(saved.m_pos > saved.m_max_size) {
        saved.m_global_pos++;
        saved.m_pos = saved.m_pos - saved.m_max_size;
    }

    return saved;
}

EECounter EECounter::operator-(const EECounter& other) {
    __INCOMPATIBLE_SIZE_EXCEPTION__

    EECounter saved(*this);
    saved.m_global_pos -= other.m_pos;
    if(other.m_pos >= saved.m_pos) {
        saved.m_global_pos--;
        saved.m_pos -= other.m_pos - saved.m_max_size;
    } else
        saved.m_pos -= other.m_pos;

    return saved;
}

void EECounter::add(uint64_t step) noexcept {
    while(step > m_max_size) {
        step -= m_max_size;
        if(m_global_pos == m_max_size - 1)
            m_global_pos = 0;
        else
            ++m_global_pos;
    }
    m_pos += step;
    if(m_pos > m_max_size)
        m_pos -= m_max_size;
}

void EECounter::sub(uint64_t step) noexcept {
    while(step > m_max_size)
    {
        step -= m_max_size;
        if(m_global_pos == 0)
            m_global_pos = m_max_size - 1;
        else
            --m_global_pos;
    }
    if(m_pos < step)
        m_pos = m_pos + m_max_size + 1 - step;
    else
        m_pos -= step;
}

void EECounter::reset() noexcept {
    m_pos        = 0;
    m_global_pos = 0;
}

} // namespace simpleapi

