#ifndef STACKER_H
#define STACKER_H

#include <cstddef>
#include <vector>


namespace simpleapi {
namespace tools {

class Stacker {
    std::vector<char> m_stack;


    struct Rule {
        char first;
        char second;
    };

    std::vector<Rule> m_rules;

public:
    bool autocheck(char ch)                 noexcept;
    size_t size()                           const noexcept  { return m_stack.size(); }
    bool empty()                            const noexcept  { return m_stack.empty(); }

    //result - включено ли правило в список (без дубликатов)
    //символ включает и исклюает из стека сам себя
    bool addSimpleRule(char ch)             noexcept;
    //ch1 включает в стек, ch2 исклюает из стека
    bool addDoubleRule(char ch1, char ch2)  noexcept;
    void clearAllRules()                    noexcept        { m_rules.clear(); }

    bool inQuotes()                         const noexcept;
};

} // namespace tools
} // namespace simpleapi

#endif // STACKER_H
