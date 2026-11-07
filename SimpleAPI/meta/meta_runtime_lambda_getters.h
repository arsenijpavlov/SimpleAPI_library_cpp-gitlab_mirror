#pragma once


#include <sys/types.h>
#include <utility>
namespace simpleapi {
namespace meta {

//-----------------------------------------------------------------
// трейт для получения runtime-объекта из пака функций по индексу, CurrentIndex = 0
//-----------------------------------------------------------------
template <ssize_t FindIndex>
struct runtime_object_at_index {
    template <typename Head, typename... Types>
    static auto Get(Head&& head, Types&&... types)
        // decltype нужен для сохранения категории ссылки lvalue/rvalue
        -> decltype(runtime_object_at_index<FindIndex - 1>::Get(std::forward<Types>(types)...))
    {
        return runtime_object_at_index<FindIndex - 1>::Get(std::forward<Types>(types)...);
    }
};
//------------
template <>
struct runtime_object_at_index<0> {
    template <typename Head, typename... Types>
    static auto Get(Head&& head, Types&&... types)
        -> decltype(std::forward<Head>(head))
    {
        return std::forward<Head>(head);
    }
};
//-----------------------------------------------------------------

} // namespace meta
} // namespace simpleapi

