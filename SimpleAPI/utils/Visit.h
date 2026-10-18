#pragma once

#include "Variant.h"


namespace simpleapi {
namespace tools {

//-----------------------------------------------------------------
// база для выхода из SFINAE
template <typename T>
struct get_first_arg_type : get_first_arg_type<decltype(&T::operator())>{};
//------
// получение типа первого(единственного) аргумента лямбды
template <typename UniqLambdaType, typename RetType, typename ArgType>
struct get_first_arg_type<RetType(UniqLambdaType::*)(ArgType)> {
    using type = ArgType;
};
template <typename UniqLambdaType, typename RetType, typename ArgType>
struct get_first_arg_type<RetType(UniqLambdaType::*)(ArgType) const> {
    using type = ArgType;
};
//------
// частный случай для std::nullptr_t аргумента лямбды
template <typename UniqLambdaType, typename RetType>
struct get_first_arg_type<RetType(UniqLambdaType::*)()> {
    using type = std::nullptr_t;
};
template <typename UniqLambdaType, typename RetType>
struct get_first_arg_type<RetType(UniqLambdaType::*)() const> {
    using type = std::nullptr_t;
};
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
// база для выхода из SFINAE
template <typename InputVariantType, typename ArgType, typename = void>
struct is_callable_simple_lambda : std::false_type {};
//
template <typename InputVariantType, typename ArgType>
struct is_callable_simple_lambda<InputVariantType, ArgType,
                                 typename std::enable_if<true, decltype(std::declval<InputVariantType>()(std::declval<ArgType>()), void())>
                                 > : std::true_type {};
//-----------------------------------------------------------------
// база для выхода из SFINAE
template <typename... Types>
struct check_all_variant_types_callable_in_lambda;
//------
// если Variant<> содержит только один тип, проверяем лямбду для него
template <typename VariantType, typename... LambdaTypes>
struct check_all_variant_types_callable_in_lambda<Variant<VariantType>, LambdaTypes...> {
    // делегируем выбор типа компилятору
    using Selector = decltype(compiler_type_resolver<0, LambdaTypes...>::template match<VariantType>(std::declval<VariantType>()));
    static constexpr ssize_t index_of_lambda = Selector::value;

    // механика std::conditional не позволяет вписать условие напрямую
    struct OnFalse {
        static constexpr bool value = is_callable_simple_lambda<
            VariantType,
            typename type_at_index<static_cast<size_t>(index_of_lambda), LambdaTypes...>::type
            >::value;
    };

    static constexpr bool value = std::conditional<index_of_lambda == -1, std::false_type, OnFalse>::type::value;
};
// проверяем вызываемость лямбд для каждого из внутренних типов Variant<>
template <typename VariantType, typename... VariantTypes, typename... LambdaTypes>
struct check_all_variant_types_callable_in_lambda<Variant<VariantType, VariantTypes...>, LambdaTypes...> {
    static constexpr bool value = check_all_variant_types_callable_in_lambda<Variant<VariantType>, LambdaTypes...>::value
                                  || check_all_variant_types_callable_in_lambda<Variant<VariantTypes...>, LambdaTypes...>::value;
};
//-----------------------------------------------------------------
} // namespace tools

// основной пользовательский интерфейс для механики Variant->Visitor
template <typename... VariantTypes, typename... Lambdas>
/*RetValue*/void Visit(const Variant<VariantTypes...>& variant, Lambdas... lambdas)
{
    // проверка, что описаны лямбды для всех типов внутри Variant<...>
    static_assert(tools::check_all_variant_types_in_list<Variant<VariantTypes...>,
                                                         typename tools::get_first_arg_type<Lambdas>::type...
                                                         >::value,
                  "SimpleAPI: not all cases of Variant<>'s types are described in lambdas");

    // проверка, что описанные лямбды могут быть вызваны для типов внутри Variant
    static_assert(tools::check_all_variant_types_callable_in_lambda<Variant<VariantTypes...>, Lambdas...>::value,
                  "SimpleAPI: not all lambdas are callable");

    // TODO: определить вызываемую лямбду на основе типа
    // TODO: вызывать лямбду
}

} // namespace simpleapi
