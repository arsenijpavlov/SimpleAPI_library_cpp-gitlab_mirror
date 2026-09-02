#pragma once


namespace simpleapi {
namespace tools {

// базовое объявление шаблона (сформирует ошибку компиляции для типов, которые не объявлены)
template<typename T, typename Enable = void>
struct ConfigTypeTraits;

} // namespace simpleapi
} // namespace tools
