#include "ParserSymbolCounter.h"


namespace simpleapi {

void ParserSymbolCounter::check(const size_t current_iterator, const char current_ch) noexcept
{
    if(current_iterator > m_last_iterator)
    {
        m_last_iterator = current_iterator;
        if(current_ch == '\n')
        {
            m_symbol_counter = 0;
            m_line_counter++;
        } else {
            if(current_ch != 0)
                m_symbol_counter++;
        }
    }
}

size_t ParserSymbolCounter::getLastLineCounter() const noexcept
{
    return m_line_counter;
}

size_t ParserSymbolCounter::getLastSymbolCounter() const noexcept
{
    return m_symbol_counter;
}

size_t ParserSymbolCounter::getLastIterator() const noexcept
{
    return m_last_iterator;
}

} // namespace simpleapi

