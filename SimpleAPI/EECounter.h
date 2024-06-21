#ifndef END_TO_END_COUNTER_H
#define END_TO_END_COUNTER_H


#include <cstdint>

class EECounter {
    uint64_t size;
    uint64_t pos;
    uint64_t global_pos;
public:
    EECounter(uint64_t size);
    EECounter(const EECounter& other);

    bool operator==(const EECounter& other);
    bool operator!=(const EECounter& other);
    bool operator<(const EECounter& other);
    bool operator>(const EECounter& other);
    bool operator<=(const EECounter& other);
    bool operator>=(const EECounter& other);

    EECounter& operator++();
    EECounter& operator--();
    EECounter operator++(int);
    EECounter operator--(int);
    EECounter operator+(const uint64_t step);
    EECounter operator-(const uint64_t step);
    EECounter operator+(const EECounter& other);
    EECounter operator-(const EECounter& other);

    uint64_t get();
    uint64_t get_next();
    void add(uint64_t step = 0);
    void sub(uint64_t step = 0);
    void reset();
};

#endif // END_TO_END_COUNTER_H
