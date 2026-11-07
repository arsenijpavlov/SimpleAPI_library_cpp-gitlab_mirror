#pragma once



namespace simpleapi {
namespace meta {

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

} // namespace meta
} // namespace simpleapi

