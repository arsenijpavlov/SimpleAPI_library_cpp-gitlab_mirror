#ifndef PARSER_SYMBOL_COUNTER_H
#define PARSER_SYMBOL_COUNTER_H

#include <cstddef>

class ParserSymbolCounter {
    size_t m_last_iterator;
    size_t m_line_counter;
    size_t m_symbol_counter;
public:
    ParserSymbolCounter() noexcept
        : m_last_iterator{0}
        , m_line_counter{0}
        , m_symbol_counter{0}
    {}

    void check(const size_t current_iterator, const char current_ch)    noexcept;
    size_t getLastLineCounter()                                         const noexcept;
    size_t getLastSymbolCounter()                                       const noexcept;
    size_t getLastIterator()                                            const noexcept;
};

#endif // PARSER_SYMBOL_COUNTER_H
