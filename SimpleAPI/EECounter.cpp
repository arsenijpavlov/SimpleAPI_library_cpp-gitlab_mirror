#include "EECounter.h"


EECounter::EECounter(uint16_t size) {
    this->size = size;
    this->current_pos = 0;
    this->current_global_pos = 0;
}

EECounter::EECounter(EECounter &other) {
    this->size = other.size;
    this->current_pos = other.current_pos;
    this->current_global_pos = other.current_global_pos;
}

bool EECounter::operator==(EECounter &other)
{
    if(this->size == other.size
        && this->current_pos == other.current_pos
        && this->current_global_pos == other.current_global_pos)
        return true;
    else
        return false;
}

bool EECounter::operator!=(EECounter &other)
{
    if(this->size != other.size
        || this->current_pos != other.current_pos
        || this->current_global_pos != other.current_global_pos)
        return true;
    else
        return false;
}

bool EECounter::operator<(EECounter &other)
{
    if(this->size != other.size)
        return false;

    if(this->current_pos < other.current_pos
        && this->current_global_pos < other.current_global_pos)
        return true;
    else
        return false;
}

bool EECounter::operator>(EECounter &other)
{
    if(this->size != other.size)
        return false;

    if(this->current_pos > other.current_pos
        && this->current_global_pos > other.current_global_pos)
        return true;
    else
        return false;
}

bool EECounter::operator<=(EECounter &other)
{
    //TODO: EECounter::operator<=
    //    if(this == &other)
    //        return true;

    //    if(this->current_pos < other.current_pos
    //        && this->current_global_pos < other.current_global_pos)
    //        return true;
    //    else
    return false;
}

bool EECounter::operator>=(EECounter &other)
{
    //TODO: EECounter::operator>=
    //    if(this == &other)
    //        return true;

    //    if(this->current_pos < other.current_pos
    //        && this->current_global_pos < other.current_global_pos)
    //        return true;
    //    else
    return false;
}
