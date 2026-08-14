#include "Stacker.h"


namespace simpleapi {
namespace tools {


// вернёт false, если встречено закрывающее правило, но не было соответствующего открывающего
bool Stacker::autocheck(char ch) noexcept
{
    for(const auto rule : m_rules) {
        //проверить одиночные правила
        if(rule.second == 0 && rule.first == ch) {
            if(!m_stack.empty() && m_stack.back() == ch)
                m_stack.pop_back();
            else
                m_stack.push_back(ch);
            break;
        }
        //если кавычка, то ждём закрывающую кавычку
        else if((rule.first == '\'' || rule.first == '"')
                 && !m_stack.empty()
                 && (m_stack.back() == '\'' || m_stack.back() == '"')
                 && (ch != '\'' && ch != '"')
                 )
        {
            break;
        }
        //проверить правила на закрытие
        else if(rule.second == ch) {
            if(!m_stack.empty() && m_stack.back() == rule.first)
                m_stack.pop_back();
            else
                return false;
            break;
        }
        //проверить правила на открытие
        else if(rule.first == ch) {
            m_stack.push_back(ch);
            break;
        }
    }

    return true;
}

// вернёт false, если указанное правило уже существует (с учётом двойного правила)
bool Stacker::addSimpleRule(char ch) noexcept
{
    for(const auto it : m_rules) {
        if(it.first == ch) return false;
    }

    m_rules.push_back({ch, 0});
    return true;
}

// вернёт false, если указанное правило уже существует (с учётом одинарного правила)
bool Stacker::addDoubleRule(char ch1, char ch2) noexcept
{
    for(const auto it : m_rules) {
        if(it.first == ch1) return false;
    }

    m_rules.push_back({ch1, ch2});
    return true;
}

// вернёт true, если есть непарная кавычка
bool Stacker::inQuotes() const noexcept
{
    //на основе правил определить тип кавычки (при наличии) и проверить её наличие в стеке
    for(const auto& rule : m_rules) {
        if(rule.first == '\'' || rule.first == '"') {
            for(const auto c : m_stack) {
                if(c == rule.first)
                    return true;
            }
        }
    }

    return false;
}


} // namespace tools
} // namespace simpleapi
