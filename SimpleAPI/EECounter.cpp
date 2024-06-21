#include "EECounter.h"

//TODO: _INCOMPATIBLE_EXCEPTION_
#define _INCOMPATIBLE_EXCEPTION_     if(this->size != other.size) return false;


EECounter::EECounter(uint64_t size) {
    this->size = size;
    this->pos = 0;
    this->global_pos = 0;
}

EECounter::EECounter(const EECounter &other) {
    this->size = other.size;
    this->pos = other.pos;
    this->global_pos = other.global_pos;
}

bool EECounter::operator==(const EECounter &other)
{
    _INCOMPATIBLE_EXCEPTION_

    if(this->pos == other.pos && this->global_pos == other.global_pos)  return true;
    else                                                                return false;
}

bool EECounter::operator!=(const EECounter &other)
{
    _INCOMPATIBLE_EXCEPTION_

    if(this->pos != other.pos || this->global_pos != other.global_pos)  return true;
    else                                                                return false;
}

bool EECounter::operator<(const EECounter &other)
{
    _INCOMPATIBLE_EXCEPTION_

    if(this->pos < other.pos && this->global_pos < other.global_pos)    return true;
    else                                                                return false;
}

bool EECounter::operator>(const EECounter &other)
{
    _INCOMPATIBLE_EXCEPTION_

    if(this->pos > other.pos && this->global_pos > other.global_pos)    return true;
    else                                                                return false;
}

bool EECounter::operator<=(const EECounter &other)
{
    if(*this == other)  return true;
    if(*this < other)   return true;

    return false;
}

bool EECounter::operator>=(const EECounter &other)
{
    if(*this == other)  return true;
    if(*this > other)   return true;

    return false;
}

EECounter& EECounter::operator++()
{
    this->add();
    return *this;
}

EECounter& EECounter::operator--()
{
    this->sub();
    return *this;
}

EECounter EECounter::operator++(int)
{
    EECounter saved(*this);
    this->add();
    return saved;
}

EECounter EECounter::operator--(int)
{
    EECounter saved(*this);
    this->sub();
    return saved;
}

EECounter EECounter::operator+(uint64_t step) {
    EECounter saved(*this);
    saved.add(step);
    return saved;
}

EECounter EECounter::operator-(uint64_t step)
{
    EECounter saved(*this);
    saved.sub(step);
    return saved;
}

uint64_t EECounter::get()
{
    return pos;
}

uint64_t EECounter::get_next()
{
    EECounter saved(*this);
    return (++saved).get();
}

EECounter EECounter::operator+(const EECounter& other) {
    _INCOMPATIBLE_EXCEPTION_

    EECounter saved(*this);
    saved.global_pos += other.pos;
    saved.pos += other.pos;
    if(saved.pos > saved.size) {
        saved.global_pos++;
        saved.pos = saved.pos - saved.size;
    }

    return saved;
}

EECounter EECounter::operator-(const EECounter& other) {
    _INCOMPATIBLE_EXCEPTION_

    EECounter saved(*this);
    saved.global_pos -= other.pos;
    if(other.pos >= saved.pos) {
        saved.global_pos--;
        saved.pos -= other.pos - saved.size;
    } else
        saved.pos -= other.pos;

    return saved;
}

void EECounter::add(uint64_t step)
{
    pos += step;
    if(pos > size) {
        pos = size - pos;
        global_pos++;
    }
}

void EECounter::sub(uint64_t step)
{
    pos += step;
    if(pos > size) pos = size - pos;
}

void EECounter::reset()
{
    pos = 0;
    global_pos = 0;
}
