#ifndef TYPE_DEFINES_H
#define TYPE_DEFINES_H

#include <type_traits>


namespace simpleapi {

#define __ONLY_ALLOWED_TYPES__(ARG) \
template<typename ARG, \
                       typename std::enable_if< \
                                                std::is_same<           typename std::remove_cv<typename std::remove_reference<ARG>::type>::type, ElementJson>::value \
                                                || std::is_same<        typename std::remove_cv<typename std::remove_reference<ARG>::type>::type, ElementArray>::value \
                                                || std::is_convertible< typename std::remove_cv<typename std::remove_reference<ARG>::type>::type, std::string>::value \
                                                || std::is_same<        typename std::decay<ARG>::type, const char*>::value \
                                                || std::is_arithmetic<  typename std::remove_cv<typename std::remove_reference<ARG>::type>::type>::value \
                                                || std::is_same<        typename std::remove_cv<typename std::remove_reference<ARG>::type>::type, bool>::value \
                                                || std::is_same<        typename std::remove_cv<typename std::remove_reference<ARG>::type>::type, Config>::value \
                           >::type* = nullptr>
#define __ONLY_ALLOWED_TYPES_WITHOUT_CONFIG__(ARG) \
template<typename ARG, \
                       typename std::enable_if< \
                                                std::is_same<           typename std::remove_cv<typename std::remove_reference<ARG>::type>::type, ElementJson>::value \
                                                || std::is_same<        typename std::remove_cv<typename std::remove_reference<ARG>::type>::type, ElementArray>::value \
                                                || std::is_convertible< typename std::remove_cv<typename std::remove_reference<ARG>::type>::type, std::string>::value \
                                                || std::is_same<        typename std::decay<ARG>::type, const char*>::value \
                                                || std::is_arithmetic<  typename std::remove_cv<typename std::remove_reference<ARG>::type>::type>::value \
                                                || std::is_same<        typename std::remove_cv<typename std::remove_reference<ARG>::type>::type, bool>::value \
                           >::type* = nullptr>


#define __ONLY_STRING_TYPES__(ARG) \
template<typename ARG, \
                       typename std::enable_if< \
                                                    std::is_convertible<    typename std::decay<ARG>::type, std::string>::value || \
                                                    std::is_same<           typename std::decay<ARG>::type, const char*>::value , \
                                                int \
                           >::type* = nullptr>

#define __ONLY_NUMBER_TYPES__(ARG) \
template<typename ARG, \
                       typename std::enable_if< \
                                                std::is_arithmetic<     typename std::decay<ARG>::type>::value \
                                                && !std::is_same<       typename std::decay<ARG>::type, bool>::value \
                           >::type* = nullptr>

template<bool...> struct bool_pack;
template<bool... Values>
struct all_true : std::is_same<bool_pack<Values..., true>, bool_pack<true, Values...>> {};

} // namespace simpleapi

#endif // TYPE_DEFINES_H
