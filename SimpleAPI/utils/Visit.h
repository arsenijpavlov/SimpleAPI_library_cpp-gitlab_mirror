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
//------
// получение типа первого(единственного) аргумента функции
template <typename RetType, typename ArgType>
struct lambda_settings<RetType(*)(ArgType)> {
    using arg_type = ArgType;
    using ret_type = RetType;
};
template <typename RetType, typename ArgType>
struct lambda_settings<RetType(ArgType)> {
    using arg_type = ArgType;
    using ret_type = RetType;
};
template <typename RetType, typename ArgType>
struct lambda_settings<RetType(ArgType) const> {
    using arg_type = ArgType;
    using ret_type = RetType;
};
//------
// частный случай для std::nullptr_t аргумента лямбды
template <typename RetType>
struct lambda_settings<RetType(*)()> {
    using arg_type = std::nullptr_t;
    using ret_type = RetType;
};
template <typename RetType>
struct lambda_settings<RetType()> {
    using arg_type = std::nullptr_t;
    using ret_type = RetType;
};
template <typename RetType>
struct lambda_settings<RetType() const> {
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

//-----------------------------------------------------------------
// трейт для получения runtime-объекта из пака по индексу, CurrentIndex = 0
//-----------------------------------------------------------------
template <ssize_t FindIndex, ssize_t CurrentIndex>
struct object_at_index {
    template <typename Head, typename... Types>
    static auto Get(Head&& head, Types&&... types)
        -> typename type_at_index<FindIndex - 1, Types...>::type // <-- мы ведь здесь не можем указывать Head
    {
       return object_at_index<FindIndex, CurrentIndex + 1>
            ::Get(std::forward<Types>(types)...);
    }
};
//------------
template <ssize_t FindIndex>
struct object_at_index<FindIndex, FindIndex> {
    template <typename Head, typename... Types>
    static Head&& Get(Head&& head, Types&&... types) {
        return std::forward<Head>(head);
    }
};
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// трейт для описания runtime-вызова
//-----------------------------------------------------------------
// база для SFINAE
template <typename... Args>
struct VisitContext;
//------------
template <typename... VariantTypes, typename... Lambdas>
struct VisitContext<Variant<VariantTypes...>, Lambdas...> {
    using RetType = typename lambda_settings<typename first_of_pack<Lambdas...>::type>::ret_type;

    // шаблон рекурсивного поиска, где Index - текущий индекс типа Variant<>
    // NOTE: внешний код всегда вызывает начальный индекс <0>
    template <ssize_t Index, bool is_in_bounds = (Index >= 0 && Index < sizeof...(VariantTypes))>
    struct IndexContext
    {
        static RetType Call(const Variant<VariantTypes...>& variant, Lambdas... lambdas) {
            if(Index == variant.index()) {
                using InnerVariantType = typename tools::type_at_index<Index, VariantTypes...>::type;

                static constexpr ssize_t lambda_index =
                    tools::index_of_type<InnerVariantType, typename tools::lambda_settings<Lambdas>::arg_type...>::value;

                auto selected_lambda = tools::object_at_index<lambda_index, 0>::Get(lambdas...);
                return selected_lambda(variant.template get<InnerVariantType>());
            } else {
                return IndexContext<Index+1>::Call(variant, lambdas...);
            }
        }

        static RetType Call(Variant<VariantTypes...>&& variant, Lambdas... lambdas) {
            if(Index == variant.index()) {
                using InnerVariantType = typename tools::type_at_index<Index, VariantTypes...>::type;

                static constexpr ssize_t lambda_index =
                    tools::index_of_type<InnerVariantType, typename tools::lambda_settings<Lambdas>::arg_type...>::value;

                auto selected_lambda = tools::object_at_index<lambda_index, 0>::Get(lambdas...);
                return selected_lambda(variant.template get<InnerVariantType>());
            } else {
                return IndexContext<Index+1>::Call(std::forward<Variant<VariantTypes...>>(variant), lambdas...);
            }
        }
    };
    // шаблон для остановки рекурсии (вышли за границы списка типов)
    template <ssize_t Index>
    struct IndexContext<Index, false>
    {
        static RetType Call(const Variant<VariantTypes...> variant, Lambdas... lambdas) {
            (void)variant; // hide warning
            throw std::runtime_error("SimpleAPI: critical error, Variant index out of bounds");
            // следующая инструкция никогда не будет выполнена, к тому же UB
            return *static_cast<RetType*>(nullptr);
        }
        static RetType Call(Variant<VariantTypes...>&& variant, Lambdas... lambdas) {
            (void)variant; // hide warning
            throw std::runtime_error("SimpleAPI: critical error, Variant index out of bounds");
            // следующая инструкция никогда не будет выполнена, к тому же UB
            return *static_cast<typename std::remove_reference<RetType>::type*>(nullptr);
        }

        // FIXME: нужна отдельная реализация для типа void
    };
};
//-----------------------------------------------------------------
} // namespace tools


// основной пользовательский интерфейс для механики Variant->Visitor (const&)
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

    // проверка, что описанные лямбды могут быть вызваны для типов внутри Variant
    static_assert(tools::check_all_variant_types_callable_in_lambda<Variant<VariantTypes...>, Lambdas...>::value,
                  "SimpleAPI: not all lambdas are callable");

    // проверка, что лямбды имеют одинаковый возвращаемый тип
    using TemplateRetType = typename tools::lambda_settings<typename tools::first_of_pack<Lambdas...>::type>::ret_type;
    static_assert(tools::are_types_equal<TemplateRetType, typename tools::lambda_settings<Lambdas>::ret_type...>::value,
                  "SimpleAPI: return types of lambdas does not equal");

    //-----------------------
    // TODO: определить и вызвать лямбду/функцию на основе текущего типа Variant<>
    return tools::VisitContext<Variant<VariantTypes...>, Lambdas...>
        ::template IndexContext<0>
        ::Call(variant, lambdas...);
}

// основной пользовательский интерфейс для механики Variant->Visitor (&&)
template <typename... VariantTypes, typename... Lambdas>
auto Visit(Variant<VariantTypes...>&& variant, Lambdas... lambdas)
    -> typename tools::lambda_settings<
        typename tools::first_of_pack<Lambdas...>::type
        >::ret_type
{
    // проверка, что описаны лямбды для всех типов внутри Variant<...>
    static_assert(tools::check_all_variant_types_in_list<
                      Variant<VariantTypes...>,typename tools::lambda_settings<Lambdas>::arg_type...>::value,
                  "SimpleAPI: not all cases of Variant<>'s types are described in lambdas");

    // проверка, что описанные лямбды могут быть вызваны для типов внутри Variant
    static_assert(tools::check_all_variant_types_callable_in_lambda<Variant<VariantTypes...>, Lambdas...>::value,
                  "SimpleAPI: not all lambdas are callable");

    // проверка, что лямбды имеют одинаковый возвращаемый тип
    using TemplateRetType = typename tools::lambda_settings<typename tools::first_of_pack<Lambdas...>::type>::ret_type;
    static_assert(tools::are_types_equal<TemplateRetType, typename tools::lambda_settings<Lambdas>::ret_type...>::value,
                  "SimpleAPI: return types of lambdas does not equal");

    //-----------------------
    // TODO: определить и вызвать лямбду/функцию на основе текущего типа Variant<>
    return tools::VisitContext<Variant<VariantTypes...>, Lambdas...>
        ::template IndexContext<0>
        ::Call(std::forward<Variant<VariantTypes...>>(variant), lambdas...);
}

} // namespace simpleapi
