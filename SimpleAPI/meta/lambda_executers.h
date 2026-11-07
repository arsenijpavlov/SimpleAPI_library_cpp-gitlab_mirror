#pragma once

#include <bitset>
#include <deque>
#include <forward_list>
#include <list>
#include <queue>
#include <set>
#include <stack>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include "../config/Config.h"
#include "../utils/Variant.h"


namespace simpleapi {
namespace tools {

// ---------------
// ---------------
// если указатель — разыменовываем его
template <typename PtrT>
static inline typename std::remove_pointer<PtrT>::type&
UnwrapPointer(PtrT ptr, typename std::enable_if<std::is_pointer<PtrT>::value>::type* = 0) {
    return *ptr;
}
// если объект — отдаем как есть
template <typename ObjT>
static inline ObjT&
UnwrapPointer(ObjT& obj, typename std::enable_if<!std::is_pointer<ObjT>::value>::type* = 0) {
    return obj;
}
// ---------------
// ---------------
// описания перегрузок для лямбд/std::function() для вызова как через объект, так и через указатель на него
template <typename... Args>
static bool ExecuteValidator(std::nullptr_t, Args&&...) noexcept
{
    return true;
}
// описание лямбд по уменьшению возможного количества параметров
// 1. Запускается со всеми аргументами сразу
template <typename ValidatorT, typename ... Args,
          typename std::enable_if<!std::is_same<ValidatorT, std::nullptr_t>::value, int>::type = 0,
          typename = decltype(std::declval<ValidatorT>()(std::declval<Args>()...))
         >
static bool ExecuteValidator(ValidatorT validator, Args&&... args)
{
    // если это указатель на функцию/лямбду (или nullptr)
    auto&& not_ptr_validator = UnwrapPointer(validator);
    return not_ptr_validator(std::forward<Args>(args)...);
}
//----------------------------
// 2. Принимает T value и std::string key
template <typename ValidatorT, typename T, typename ... Args,
          typename std::enable_if<!std::is_same<ValidatorT, std::nullptr_t>::value, int>::type = 0,
          typename = decltype(std::declval<ValidatorT>()(std::declval<T>(), std::declval<std::string>()))
         >
static bool ExecuteValidator(ValidatorT validator, T&& t, std::string&& key, Args&&... /*args*/ /*hide unused*/)
{
    // если это указатель на функцию/лямбду (или nullptr)
    auto&& not_ptr_validator = UnwrapPointer(validator);
    return not_ptr_validator(std::forward<T>(t), std::forward<std::string>(key));
}
//----------------------------
// 3. Принимает только T value
template <typename ValidatorT, typename T, typename ... Args,
          typename std::enable_if<!std::is_same<ValidatorT, std::nullptr_t>::value, int>::type = 0,
          typename = decltype(std::declval<ValidatorT>()(std::declval<T>()))
         >
static bool ExecuteValidator(ValidatorT validator, T&& t, Args&&... /*args*/ /*hide unused*/)
{
    // если это указатель на функцию/лямбду (или nullptr)
    auto&& not_ptr_validator = UnwrapPointer(validator);
    return not_ptr_validator(std::forward<T>(t));
}
// ----------------------------------------------------------------------------

} // namespace tools
} // namespace simpleapi
