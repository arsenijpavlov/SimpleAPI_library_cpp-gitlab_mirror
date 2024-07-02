#ifndef END_TO_END_COUNTER_H
#define END_TO_END_COUNTER_H


#include <cstdint>

class EECounter {
    uint64_t maxSize;
    uint64_t pos;
    uint64_t global_pos;

public:
    EECounter(uint64_t size);
    EECounter(const EECounter& other);

    void        set_pos(uint64_t pos);
    void        set_glob_pos(uint64_t glob_pos);

    bool        operator==(const EECounter& other) const;
    bool        operator!=(const EECounter& other) const;
    bool        operator<(const EECounter& other) const;
    bool        operator>(const EECounter& other) const;
    bool        operator<=(const EECounter& other) const;
    bool        operator>=(const EECounter& other) const;

    EECounter&  operator++();
    EECounter&  operator--();
    EECounter   operator++(int);
    EECounter   operator--(int);
    EECounter   operator+(const uint64_t step);
    EECounter   operator-(const uint64_t step);
    EECounter   operator+(const EECounter& other);
    EECounter   operator-(const EECounter& other);

    EECounter&  operator=(const EECounter& other);

    uint64_t    get();
    uint64_t    get_add();
    uint64_t    get_next();
    uint64_t    get_glob();
    void        add(uint64_t step = 1);
    void        sub(uint64_t step = 1);
    uint64_t    size() { return maxSize; }
    void        reset();
};

#endif // END_TO_END_COUNTER_H
