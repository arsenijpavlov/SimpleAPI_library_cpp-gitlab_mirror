#pragma once

#include "Variant.h"
#include "../meta/meta_pack_traits.h"
#include "../meta/meta_lambda_settings.h"
#include "../meta/meta_runtime_lambda_getters.h"
#include "../meta/meta_variant_traits.h"


namespace simpleapi {

namespace tools {
//-----------------------------------------------------------------
// трейт для описания runtime-вызова
//-----------------------------------------------------------------
// база для SFINAE
template <typename... Args>
struct VisitContext;
//------------
template <typename... VariantTypes, typename... Lambdas>
struct VisitContext<Variant<VariantTypes...>, Lambdas...> {
    using RetType = typename meta::lambda_settings<typename meta::first_of_pack<Lambdas...>::type>::ret_type;

    // шаблон рекурсивного поиска, где Index - текущий индекс типа Variant<>
    // NOTE: внешний код всегда вызывает начальный индекс <0>
    template <ssize_t Index, bool is_in_bounds = (Index >= 0 && Index < sizeof...(VariantTypes))>
    struct IndexContext
    {
        static RetType Call(const Variant<VariantTypes...>& variant, Lambdas... lambdas) {
            if(Index == variant.index()) {
                using InnerVariantType = typename meta::type_at_index<Index, VariantTypes...>::type;

                static constexpr ssize_t lambda_index =
                    meta::index_of_type<InnerVariantType, typename meta::lambda_settings<Lambdas>::arg_type...>::value;

                auto selected_lambda = meta::runtime_object_at_index<lambda_index>::Get(lambdas...);
                return selected_lambda(variant.template get<InnerVariantType>());
            } else {
                return IndexContext<Index+1>::Call(variant, lambdas...);
            }
        }

        static RetType Call(Variant<VariantTypes...>&& variant, Lambdas... lambdas) {
            if(Index == variant.index()) {
                using InnerVariantType = typename meta::type_at_index<Index, VariantTypes...>::type;

                static constexpr ssize_t lambda_index =
                    meta::index_of_type<InnerVariantType, typename meta::lambda_settings<Lambdas>::arg_type...>::value;

                auto selected_lambda = meta::runtime_object_at_index<lambda_index>::Get(lambdas...);
                return selected_lambda(std::move(variant.template get<InnerVariantType>()));
            } else {
                return IndexContext<Index+1>::Call(std::move(variant), lambdas...);
            }
        }
    };
    // шаблон для остановки рекурсии (вышли за границы списка типов)
    template <ssize_t Index>
    struct IndexContext<Index, false>
    {
        // NOTE: описываются только const Variant<> для избегания ошибки ambigous Call()

        template <typename Dummy = RetType,
                 typename std::enable_if<!std::is_same<Dummy, void>::value, int>::type = 0>
        static RetType Call(const Variant<VariantTypes...> variant, Lambdas... lambdas) {
            (void)variant; // hide warning
            throw std::runtime_error("SimpleAPI: critical error, Variant(const&) index out of bounds");
            // следующая инструкция никогда не будет выполнена, к тому же UB, но возвращаемое значение явно обозначаем
            return *static_cast<RetType*>(nullptr);
        }

        template <typename Dummy = RetType,
                 typename std::enable_if<std::is_same<Dummy, void>::value, int>::type = 0>
        static RetType Call(const Variant<VariantTypes...> variant, Lambdas... lambdas) {
            (void)variant; // hide warning
            throw std::runtime_error("SimpleAPI: critical error, Variant(const&) index out of bounds");
            return;
        }
    };
};
} // namespace tools


// основной пользовательский интерфейс для механики Variant->Visitor (const&)
template <typename... VariantTypes, typename... Lambdas>
auto Visit(const Variant<VariantTypes...>& variant, Lambdas... lambdas)
    -> typename meta::lambda_settings<
        typename meta::first_of_pack<Lambdas...>::type
        >::ret_type
{
    // проверка, что описаны лямбды для всех типов внутри Variant<...>
    static_assert(meta::check_all_variant_types_in_list<
                      Variant<VariantTypes...>,typename meta::lambda_settings<Lambdas>::arg_type...>::value,
                  "SimpleAPI: not all cases of Variant<>'s types are described in lambdas");

    // проверка, что описанные лямбды могут быть вызваны для типов внутри Variant
    static_assert(meta::check_all_variant_types_callable_in_lambda<Variant<VariantTypes...>, Lambdas...>::value,
                  "SimpleAPI: not all lambdas are callable");

    // проверка, что лямбды имеют одинаковый возвращаемый тип
    using TemplateRetType = typename meta::lambda_settings<typename meta::first_of_pack<Lambdas...>::type>::ret_type;
    static_assert(meta::are_types_equal<TemplateRetType, typename meta::lambda_settings<Lambdas>::ret_type...>::value,
                  "SimpleAPI: return types of lambdas does not equal");

    //-----------------------
    // определить и вызвать лямбду/функцию на основе текущего типа Variant<>
    return tools::VisitContext<Variant<VariantTypes...>, Lambdas...>
        ::template IndexContext<0>
        ::Call(variant, lambdas...);
}

// основной пользовательский интерфейс для механики Variant->Visitor (&&)
template <typename... VariantTypes, typename... Lambdas>
auto Visit(Variant<VariantTypes...>&& variant, Lambdas... lambdas)
    -> typename meta::lambda_settings<
        typename meta::first_of_pack<Lambdas...>::type
        >::ret_type
{
    // проверка, что описаны лямбды для всех типов внутри Variant<...>
    static_assert(meta::check_all_variant_types_in_list<
                      Variant<VariantTypes...>,typename meta::lambda_settings<Lambdas>::arg_type...>::value,
                  "SimpleAPI: not all cases of Variant<>'s types are described in lambdas");

    // проверка, что описанные лямбды могут быть вызваны для типов внутри Variant
    static_assert(meta::check_all_variant_types_callable_in_lambda<Variant<VariantTypes...>, Lambdas...>::value,
                  "SimpleAPI: not all lambdas are callable");

    // проверка, что лямбды имеют одинаковый возвращаемый тип
    using TemplateRetType = typename meta::lambda_settings<typename meta::first_of_pack<Lambdas...>::type>::ret_type;
    static_assert(meta::are_types_equal<TemplateRetType, typename meta::lambda_settings<Lambdas>::ret_type...>::value,
                  "SimpleAPI: return types of lambdas does not equal");

    //-----------------------
    // определить и вызвать лямбду/функцию на основе текущего типа Variant<>
    return tools::VisitContext<Variant<VariantTypes...>, Lambdas...>
        ::template IndexContext<0>
        ::Call(std::forward<Variant<VariantTypes...>>(variant), lambdas...);
}

} // namespace simpleapi
