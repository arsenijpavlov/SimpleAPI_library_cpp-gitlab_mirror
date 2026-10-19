#pragma once

#include "Variant.h"


namespace simpleapi {
namespace tools {

//-----------------------------------------------------------------
// трейт для получения первого аргумента лямбды (либо пустой аргумент)
//-----------------------------------------------------------------
// база для выхода из SFINAE
template <typename T>
struct lambda_settings : lambda_settings<decltype(&T::operator())>{};
//------
// получение типа первого(единственного) аргумента лямбды
template <typename UniqLambdaType, typename RetType, typename ArgType>
struct lambda_settings<RetType(UniqLambdaType::*)(ArgType)> {
    using arg_type = ArgType;
    using ret_type = RetType;
};
template <typename UniqLambdaType, typename RetType, typename ArgType>
struct lambda_settings<RetType(UniqLambdaType::*)(ArgType) const> {
    using arg_type = ArgType;
    using ret_type = RetType;
};
//------
// частный случай для std::nullptr_t аргумента лямбды
template <typename UniqLambdaType, typename RetType>
struct lambda_settings<RetType(UniqLambdaType::*)()> {
    using arg_type = std::nullptr_t;
    using ret_type = RetType;
};
template <typename UniqLambdaType, typename RetType>
struct lambda_settings<RetType(UniqLambdaType::*)() const> {
    using arg_type = std::nullptr_t;
    using ret_type = RetType;
};

//-----------------------------------------------------------------
// получение первого типа из списка
//-----------------------------------------------------------------
// база для выхода из SFINAE
template <typename... PackTypes>
struct first_of_pack;
// основа выбора
template <typename FirstOfPack, typename... PackTypes>
struct first_of_pack<FirstOfPack, PackTypes...> {
    using type = FirstOfPack;
};
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// проверка, что все переданные типы совпадают
//-----------------------------------------------------------------
// база для выхода из SFINAE
template <typename... PackTypes>
struct are_types_equal;
// список пуст
template <typename TemplateType>
struct are_types_equal<TemplateType> {
    static constexpr bool value = true;
};
// основа проверки
template <typename TemplateType, typename FirstOfPack, typename... PackTypes>
struct are_types_equal<TemplateType, FirstOfPack, PackTypes...> {
    static constexpr bool value = std::is_same<TemplateType, FirstOfPack>::value
                                  && are_types_equal<TemplateType, PackTypes...>::value;
};
//-----------------------------------------------------------------

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
// база для выхода из SFINAE
template <typename InputVariantType, typename ArgType, typename = void>
struct is_callable_simple_lambda : std::false_type {};
//
template <typename InputVariantType, typename ArgType>
struct is_callable_simple_lambda<
    InputVariantType, ArgType,
    typename std::enable_if<true, decltype(std::declval<InputVariantType>()(std::declval<ArgType>()), void())>::type
    > : std::true_type {};
//-----------------------------------------------------------------
// база для выхода из SFINAE
template <typename... Types>
struct check_all_variant_types_callable_in_lambda;
//------
// если Variant<> содержит только один тип, проверяем лямбду для него
template <typename VariantType, typename... Lambdas>
struct check_all_variant_types_callable_in_lambda<Variant<VariantType>, Lambdas...> {
    // делегируем выбор типа компилятору
    using Selector = decltype(compiler_type_resolver<0, Lambdas...>::template match<VariantType>(std::declval<VariantType>()));
    static constexpr ssize_t index_of_lambda = Selector::value;

    // механика std::conditional не позволяет вписать условие напрямую
    struct OnFalse {
        static constexpr bool value = is_callable_simple_lambda<
            VariantType,
            typename type_at_index<static_cast<size_t>(index_of_lambda), Lambdas...>::type
            >::value;
    };

    static constexpr bool value = std::conditional<index_of_lambda == -1, std::false_type, OnFalse>::type::value;
};
// проверяем вызываемость лямбд для каждого из внутренних типов Variant<>
template <typename VariantType, typename... VariantTypes, typename... Lambdas>
struct check_all_variant_types_callable_in_lambda<Variant<VariantType, VariantTypes...>, Lambdas...> {
    static constexpr bool value = check_all_variant_types_callable_in_lambda<Variant<VariantType>, Lambdas...>::value
                                  || check_all_variant_types_callable_in_lambda<Variant<VariantTypes...>, Lambdas...>::value;
};
//-----------------------------------------------------------------
} // namespace tools

// основной пользовательский интерфейс для механики Variant->Visitor
template <typename... VariantTypes, typename... Lambdas>
auto Visit(const Variant<VariantTypes...>& variant, Lambdas... lambdas)
    -> typename tools::lambda_settings<
        typename tools::first_of_pack<Lambdas...>::type
        >::ret_type
{
    // проверка, что описаны лямбды для всех типов внутри Variant<...>
    static_assert(tools::check_all_variant_types_in_list<Variant<VariantTypes...>,
                                                         typename tools::lambda_settings<Lambdas>::arg_type...
                                                         >::value,
                  "SimpleAPI: not all cases of Variant<>'s types are described in lambdas");

    // FIXME: проверка, что описанные лямбды могут быть вызваны для типов внутри Variant
//    static_assert(tools::check_all_variant_types_callable_in_lambda<Variant<VariantTypes...>, Lambdas...>::value,
//                  "SimpleAPI: not all lambdas are callable");

    // проверка, что лямбды имеют одинаковый возвращаемый тип
    using TemplateRetType = typename tools::lambda_settings<typename tools::first_of_pack<Lambdas...>::type>::ret_type;
    static_assert(tools::are_types_equal<TemplateRetType, typename tools::lambda_settings<Lambdas>::ret_type...>::value,
                  "SimpleAPI: return types of lambdas does not equal");

    // TODO: определить вызываемую лямбду на основе типа
    // TODO: вызывать лямбду
}

} // namespace simpleapi
