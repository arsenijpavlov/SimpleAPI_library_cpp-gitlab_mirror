#pragma once

#include "../utils/Variant.h"


namespace simpleapi {
namespace meta {

//-----------------------------------------------------------------
// проверки того, что в лямбдах описаны все подтипы Variant
//-----------------------------------------------------------------
// база для выхода из SFINAE
template <typename... Types>
struct check_all_variant_types_in_list;
//------
// если Variant<> содержит только один тип, проверяем его наличие в аргументе списка лямбд
template <typename VariantType, typename... LambdaTypes>
struct check_all_variant_types_in_list<Variant<VariantType>, LambdaTypes...> {
    static constexpr bool value = is_contains_type<VariantType, typename std::decay<LambdaTypes>::type...>::value;
};
// проверяем наличие каждого внутреннего типа из Variant<> среди аргументов списка лямбд
template <typename VariantType, typename... VariantTypes, typename... LambdaTypes>
struct check_all_variant_types_in_list<Variant<VariantType, VariantTypes...>, LambdaTypes...> {
    static constexpr bool value = is_contains_type<VariantType, typename std::decay<LambdaTypes>::type...>::value
                                  && check_all_variant_types_in_list<Variant<VariantTypes...>, LambdaTypes...>::value;
};
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// проверки на вызываемость CallableObject(InputVariantType), где CallableObject - лямбда/структура или функция
//-----------------------------------------------------------------
// база для выхода из SFINAE
template <typename InputVariantType, typename... CallableObjects>
struct is_callable_object : std::false_type {};
// проверка с одиночной функцией
template <typename InputVariantType, typename CallableObject>
struct is_callable_object<InputVariantType, CallableObject>
{
    template <typename A, typename B>
    static char test(decltype(std::declval<A>()(std::declval<B>()))*);

    template <typename A, typename B>
    static long test(...);

    static constexpr bool value = sizeof(test<CallableObject, InputVariantType>(0) == sizeof(char));
};
// проверка со списком функций
template <typename InputVariantType, typename CallableObject, typename... CallableObjects>
struct is_callable_object<InputVariantType, CallableObject, CallableObjects...>
{
    static constexpr bool value = is_callable_object<InputVariantType, CallableObject>::value
                                  || is_callable_object<InputVariantType, CallableObjects...>::value;
};
//-----------------------------------------------------------------
// база для выхода из SFINAE
template <typename... Types>
struct check_all_variant_types_callable_in_lambda;
//------
// если Variant<> содержит только один тип, проверяем лямбду для него
template <typename VariantType, typename... Lambdas>
struct check_all_variant_types_callable_in_lambda<Variant<VariantType>, Lambdas...> {
    static constexpr bool value = is_callable_object<VariantType, Lambdas...>::value;
};
// проверяем вызываемость лямбд для каждого из внутренних типов Variant<>
template <typename VariantType, typename... VariantTypes, typename... Lambdas>
struct check_all_variant_types_callable_in_lambda<Variant<VariantType, VariantTypes...>, Lambdas...> {
    static constexpr bool value = check_all_variant_types_callable_in_lambda<Variant<VariantType>, Lambdas...>::value
                                  || check_all_variant_types_callable_in_lambda<Variant<VariantTypes...>, Lambdas...>::value;
};
//-----------------------------------------------------------------

} // namespace meta
} // namespace simpleapi
