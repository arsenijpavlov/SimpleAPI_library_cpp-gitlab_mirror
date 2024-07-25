#include "EECounter.h"

//TODO: _INCOMPATIBLE_EXCEPTION_
#define _INCOMPATIBLE_EXCEPTION_     if(this->maxSize != other.maxSize) return false;


EECounter::EECounter(uint64_t size) {
    this->maxSize = size;
    this->pos = 0;
    this->global_pos = 0;
}

EECounter::EECounter(const EECounter &other) {
    this->maxSize = other.maxSize;
    this->pos = other.pos;
    this->global_pos = other.global_pos;
}

void EECounter::set_pos(uint64_t pos)
{
    this->pos = pos;
}

void EECounter::set_glob_pos(uint64_t glob_pos)
{
    this->global_pos = glob_pos;
}

bool EECounter::operator==(const EECounter &other) const {
    _INCOMPATIBLE_EXCEPTION_

    if(this->pos == other.pos && this->global_pos == other.global_pos)  return true;
    else                                                                return false;
}

bool EECounter::operator!=(const EECounter &other) const {
    _INCOMPATIBLE_EXCEPTION_

    if(this->pos != other.pos || this->global_pos != other.global_pos)  return true;
    else                                                                return false;
}

bool EECounter::operator<(const EECounter &other) const {
    _INCOMPATIBLE_EXCEPTION_

    if(this->pos < other.pos && this->global_pos <= other.global_pos)   return true;
    else                                                                return false;
}

bool EECounter::operator>(const EECounter &other) const {
    _INCOMPATIBLE_EXCEPTION_

    if(this->pos > other.pos && this->global_pos >= other.global_pos)   return true;
    else                                                                return false;
}

bool EECounter::operator<=(const EECounter &other) const {
    if(*this == other)  return true;
    if(*this < other)   return true;

    return false;
}

bool EECounter::operator>=(const EECounter &other) const {
    if(*this == other)  return true;
    if(*this > other)   return true;

    return false;
}

EECounter& EECounter::operator++() {
    this->add();
    return *this;
}

EECounter& EECounter::operator--() {
    this->sub();
    return *this;
}

EECounter EECounter::operator++(int) {
    EECounter saved(*this);
    this->add();
    return saved;
}

EECounter EECounter::operator--(int) {
    EECounter saved(*this);
    this->sub();
    return saved;
}

EECounter EECounter::operator+(uint64_t step) {
    EECounter saved(*this);
    saved.add(step);
    return saved;
}

EECounter EECounter::operator-(uint64_t step) {
    EECounter saved(*this);
    saved.sub(step);
    return saved;
}

EECounter &EECounter::operator=(const EECounter &other) {
    this->maxSize       = other.maxSize;
    this->pos           = other.pos;
    this->global_pos    = other.global_pos;

    return *this;
}

uint64_t EECounter::get() const {
    return pos;
}

uint64_t EECounter::get_add() {
    EECounter saved(*this);
    this->add();
    return saved.pos;
}

uint64_t EECounter::get_next() {
    EECounter saved(*this);
    return (++saved).get();
}

uint64_t EECounter::get_glob() {
    return this->global_pos;
}

EECounter EECounter::operator+(const EECounter& other) {
    _INCOMPATIBLE_EXCEPTION_

    EECounter saved(*this);
    saved.global_pos += other.pos;
    saved.pos += other.pos;
    if(saved.pos > saved.maxSize) {
        saved.global_pos++;
        saved.pos = saved.pos - saved.maxSize;
    }

    return saved;
}

EECounter EECounter::operator-(const EECounter& other) {
    _INCOMPATIBLE_EXCEPTION_

    EECounter saved(*this);
    saved.global_pos -= other.pos;
    if(other.pos >= saved.pos) {
        saved.global_pos--;
        saved.pos -= other.pos - saved.maxSize;
    } else
        saved.pos -= other.pos;

    return saved;
}

void EECounter::add(uint64_t step) {
    pos += step;
    if(pos > maxSize) {
        pos = maxSize - pos;
        global_pos++;
    }
}

void EECounter::sub(uint64_t step) {
    pos += step;
    if(pos > maxSize) pos = maxSize - pos;
}

void EECounter::reset() {
    pos = 0;
    global_pos = 0;
}
