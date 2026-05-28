#ifndef I_ERROR_FIELD_H
#define I_ERROR_FIELD_H

#include "../../utils/Optional.h"
#include <string>

namespace simpleapi {
namespace tools {

class IErrorField {
    Optional<std::string> m_error_str;
public:
    // Parser ==========================================================================================================
    //будут вызваны, если парсеры Config/IElementContainer не смогут прочесть значения
    bool                error()                                                                         const noexcept;
    std::string         getError()                                                                      const noexcept;
    void                setError()                                                                      noexcept;
    void                setError(const std::string &error_string)                                       noexcept;
    void                setError(std::string &&error_string)                                            noexcept;
    // ========================================================================================================== Parser
};

} // namespace tools
} // namespace simpleapi

#endif // I_ERROR_FIELD_H
