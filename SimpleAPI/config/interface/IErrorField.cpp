#include "IErrorField.h"


namespace simpleapi {

bool IErrorField::error() const noexcept
{
    return m_error_str.isValid();
}

std::string IErrorField::getError() const noexcept
{
    return (m_error_str.isValid() ? m_error_str.value() : "");
}

void IErrorField::setError() noexcept
{
    m_error_str.unset();
}

void IErrorField::setError(const std::string &error_string) noexcept
{
    if(error_string.empty())
        m_error_str.unset();
    else
        m_error_str = error_string;
}

void IErrorField::setError(std::string &&error_string) noexcept
{
    if(error_string.empty())
        m_error_str.unset();
    else
        m_error_str = std::move(error_string);
}

} // namespace simpleapi

