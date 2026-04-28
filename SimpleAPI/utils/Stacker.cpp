#include "Stacker.h"


namespace simpleapi {
namespace tools {

void Stacker::push(char ch) noexcept
{
    m_stack.push_back(ch);
}

bool Stacker::equal_at_last(char ch) const noexcept
{
    if(m_stack.empty()) return false;

    return ch == m_stack.back();
}

void Stacker::pop() noexcept
{
    if(m_stack.empty())
        return;

    m_stack.pop_back();
}

void Stacker::autocheck(char ch) noexcept
{
    if(m_stack.empty()) {
        m_stack.push_back(ch);
        return;
    }

    if(ch == m_stack.back())
        m_stack.pop_back();
    else
        m_stack.push_back(ch);
}

bool Stacker::lastIsQuotes() const noexcept
{
    return equal_at_last('\'') || equal_at_last('"');
}


} // namespace tools
} // namespace simpleapi
