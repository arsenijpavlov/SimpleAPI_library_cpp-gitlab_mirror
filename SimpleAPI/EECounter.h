#ifndef END_TO_END_COUNTER_H
#define END_TO_END_COUNTER_H


#include <cstdint>

struct EEC_element{

};

class EECounter {
    uint16_t size;
    uint16_t current_pos;
    uint16_t current_global_pos;
public:
    EECounter(uint16_t size);
    EECounter(EECounter& other);

    bool operator==(EECounter& other);
    bool operator!=(EECounter& other);
    bool operator<(EECounter& other);
    bool operator>(EECounter& other);
    bool operator<=(EECounter& other);
    bool operator>=(EECounter& other);

//TODO: (EECounter)
    void operator++();
    void operator--();
    EECounter operator+(uint16_t step);
    EECounter operator-(uint16_t step);
    EECounter operator+(EECounter& other);
    EECounter operator-(EECounter& other);

    void add();
    void sub();
    void reset();
};

#endif // END_TO_END_COUNTER_H
