#pragma once

#include <cstddef>


namespace simpleapi {

class ParserSymbolCounter {
    size_t m_last_iterator;
    size_t m_line_counter;
    size_t m_symbol_counter;
    size_t m_logic_line_counter; // итерируется строго вне комментариев

public:
    ParserSymbolCounter() noexcept
        : m_last_iterator{0}
        , m_line_counter{0}
        , m_logic_line_counter{0}
        , m_symbol_counter{0}
    {}
    ParserSymbolCounter(const size_t& line_start_counter, const size_t& symbol_start_counter) noexcept
        : m_last_iterator{symbol_start_counter}
        , m_line_counter{line_start_counter}
        , m_logic_line_counter{line_start_counter}
        , m_symbol_counter{symbol_start_counter}
    {}
    ParserSymbolCounter(const size_t& line_start_counter)                       noexcept
        : m_last_iterator{0}
        , m_line_counter{line_start_counter}
        , m_logic_line_counter{line_start_counter}
        , m_symbol_counter{0}
    {}

    void check(const size_t current_iterator, const char current_ch)            noexcept;
    size_t getLastLineCounter()                                                 const noexcept;
    size_t getLastSymbolCounter()                                               const noexcept;
    size_t getLastIterator()                                                    const noexcept;

    // нужно вызывать строго после .check() для синхронизации m_last_iterator
    void checkLogicLine(const size_t current_iterator, const char current_ch)   noexcept;
    size_t getLastLogicLineCounter()                                            const noexcept;

    //для отладки
    void printCoords()                                                          const noexcept;
};

} // namespace simpleapi
