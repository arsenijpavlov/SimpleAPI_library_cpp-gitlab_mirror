#ifndef STACKER_H
#define STACKER_H

#include <cstddef>
#include <vector>


namespace simpleapi {
namespace tools {

class Stacker {
    std::vector<char> m_stack;
public:
    void push(char ch)          noexcept;
    bool equal_at_last(char ch) const noexcept;
    void pop()                  noexcept;

    void autocheck(char ch)     noexcept;
    size_t size()               const noexcept      { return m_stack.size(); }
    bool empty()                const noexcept      { return m_stack.empty(); }

    bool lastIsQuotes()         const noexcept;
};

} // namespace tools
} // namespace simpleapi

#endif // STACKER_H
