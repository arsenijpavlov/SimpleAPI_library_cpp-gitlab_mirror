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
template <typename InputVariantType, typename CallableObject, typename = void>
struct is_callable_object : std::false_type {};
//
template <typename InputVariantType, typename CallableObject>
struct is_callable_object<
    InputVariantType, CallableObject,
    typename std::enable_if<true,
                            decltype(std::declval<CallableObject>()(std::declval<InputVariantType>()), void())>::type
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
        static constexpr bool value = is_callable_object<
            VariantType, typename type_at_index<static_cast<size_t>(index_of_lambda), Lambdas...>::type>::value;
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

//-----------------------------------------------------------------
// разворачиваем список типов -> компилятор выберет наиболее подходящую перегрузку
// проверка вернёт тип void, если нет подходящего кандидата
//-----------------------------------------------------------------
// базовое описание структуры для корректности выхода из SFINAE
template <ssize_t Index, typename... Lambdas>
struct compiler_lambda_resolver;
// рекурсивный поиск
template <ssize_t Index, typename Head, typename... Types>
struct compiler_lambda_resolver<Index, Head, Types...> : compiler_type_resolver<Index + 1, Types...>
{
    // делаем метод предка явно видимым
    using compiler_type_resolver<Index + 1, Types...>::match;

    template <typename T>
    using CleanT      = typename std::remove_reference<T>::type;
    template <typename T>
    using BaseElement = typename std::remove_all_extents<CleanT<T>>::type;
    // std::integral_constant<> - создаёт тип-связку
    template <typename T,
             typename std::enable_if<std::is_constructible<Head, T>::value
                                         && !(std::is_array<CleanT<T>>::value
                                              && !std::is_same<typename std::decay<BaseElement<T>>::type, char>::value
                                              && !std::is_same<typename std::decay<BaseElement<T>>::type, wchar_t>::value)
                                         && !(std::is_same<Head, bool>::value
                                              && (std::is_array<CleanT<T>>::value
                                                  || std::is_pointer<typename std::decay<T>::type>::value))
                                     , int>::type = 0>
    static std::integral_constant<ssize_t, Index> match(Head);
};
// остановка рекурсии (пустой список)
template <ssize_t Index>
struct compiler_lambda_resolver<Index> {
    template <typename T>
    static std::integral_constant<ssize_t, -1> match(...); // заглушка
};
//-----------------------------------------------------------------
} // namespace tools


// TODO: работа с функциями, указателями на функции

// основной пользовательский интерфейс для механики Variant->Visitor
template <typename... VariantTypes, typename... Lambdas>
auto Visit(const Variant<VariantTypes...>& variant, Lambdas... lambdas)
    -> typename tools::lambda_settings<
        typename tools::first_of_pack<Lambdas...>::type
        >::ret_type
{
    // проверка, что описаны лямбды для всех типов внутри Variant<...>
    static_assert(tools::check_all_variant_types_in_list<
                      Variant<VariantTypes...>,typename tools::lambda_settings<Lambdas>::arg_type...>::value,
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
