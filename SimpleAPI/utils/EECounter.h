#pragma once

#include <cstdint>


namespace simpleapi {

// End-to-end counter - счётчик строк и символов в строке
class EECounter {
    uint64_t m_max_size;
    uint64_t m_pos;
    uint64_t m_global_pos;

public:
    EECounter(uint64_t size) noexcept;
    EECounter(const EECounter& other) noexcept;

    void        set_pos(const uint64_t& pos) noexcept;
    void        set_glob_pos(const uint64_t& glob_pos) noexcept;

    bool        operator==(const EECounter& other) const;
    bool        operator!=(const EECounter& other) const;
    bool        operator<(const EECounter& other) const;
    bool        operator>(const EECounter& other) const;
    bool        operator<=(const EECounter& other) const;
    bool        operator>=(const EECounter& other) const;

    EECounter&  operator++() noexcept;
    EECounter&  operator--() noexcept;
    EECounter   operator++(int) noexcept;
    EECounter   operator--(int) noexcept;
    EECounter   operator+(const uint64_t step) noexcept;
    EECounter   operator-(const uint64_t step) noexcept;
    EECounter   operator+(const EECounter& other);
    EECounter   operator-(const EECounter& other);

    EECounter&  operator=(const EECounter& other) noexcept;

    uint64_t    get() const noexcept;
    uint64_t    get_add() noexcept;
    uint64_t    get_next() noexcept;
    uint64_t    get_glob() noexcept;
    void        add(uint64_t step = 1) noexcept;
    void        sub(uint64_t step = 1) noexcept;
    uint64_t    size() noexcept { return m_max_size; }
    void        reset() noexcept;
};

} // namespace simpleapi
