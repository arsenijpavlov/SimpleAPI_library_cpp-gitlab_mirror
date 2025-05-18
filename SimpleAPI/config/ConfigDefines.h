#ifndef JSON_DEFINES_H
#define JSON_DEFINES_H

#include "stdexcept"

//TYPES===========================================================================
#define __ONLY_ALLOWED_TYPES__(ARG) \
    template<typename ARG, \
        typename std::enable_if< \
            std::is_same<ARG, ElementJson>::value \
            || std::is_same<ARG, ElementArray>::value \
            || std::is_convertible<ARG, std::string>::value \
            || std::is_arithmetic<ARG>::value \
            || std::is_same<ARG, bool>::value \
        >::type* = nullptr>
#define __ONLY_STRING_TYPES__(ARG) \
    template<typename ARG, \
        typename std::enable_if<std::is_convertible<ARG, std::string>::value \
        >::type* = nullptr>
//#define __ONLY_STRING_TYPES__(ARG) \
//    template<typename ARG, \
//        typename std::enable_if< \
//            std::is_constructible<std::string, ARG>::value \
//        >::type* = nullptr>
#define __ONLY_NUMBER_TYPES__(ARG) \
    template<typename ARG, \
        typename std::enable_if< \
            std::is_arithmetic<ARG>::value \
            && !std::is_same<ARG, bool>::value \
        >::type* = nullptr>
//===========================================================================TYPES

//VALUES==========================================================================
#define __SPACES__                      " \n\t"
#define __KEY_VALUE_SEPARATOR__         ":="
#define __SEPARATORS__                  ",\n"
#define __SPACES_WITHOUT_SEPARATORS__   " \t"
#define __POSIBLE_COLON__               ":="
#define __BORDER_SYMBOLS__              "@#*-=@"
#define __COMMENT_SEPARATOR_SYMBOLS__   " \t.,;:->+?!/\\*$#@&()[]\n"
#define __POSSIBLE_KEY_SYMBOLS__        "^[a-zA-Z0-9._ #@!*&\"'/\\\\]+$"
//==========================================================================VALUES

//EXEPTIONS=======================================================================
#define __INCORRECT_TYPE_EXCEPTION__(str)   throw std::invalid_argument(std::string("This Element is not a ") + #str + " type");

#define __CHECK_TYPE_IS_NULL__(object)      if(!object.isNull())    __INCORRECT_TYPE_EXCEPTION__("NULL")
#define __CHECK_TYPE_IS_BOOL__(object)      if(!object.isBool())    __INCORRECT_TYPE_EXCEPTION__("BOOL")
#define __CHECK_TYPE_IS_NUMBER__(object)    if(!object.isNumber())  __INCORRECT_TYPE_EXCEPTION__("NUMBER")
#define __CHECK_TYPE_IS_STRING__(object)    if(!object.isString())  __INCORRECT_TYPE_EXCEPTION__("STRING")
#define __CHECK_TYPE_IS_ARRAY__(object)     if(!object.isArray())   __INCORRECT_TYPE_EXCEPTION__("ARRAY")
#define __CHECK_TYPE_IS_JSON__(object)      if(!object.isJson())    __INCORRECT_TYPE_EXCEPTION__("JSON")

#define __CHECK_TYPE_IS_CONTAINER__(object) if(!object.isContainer()) __INCORRECT_TYPE_EXCEPTION__("container")

#define __CHECK_TYPE_IS_NOT_NULL__(object)  if(object.isNull()) \
                                                throw std::invalid_argument("This element is a NULL type");
#define __CHECK_TYPES_IS_EQUAL__(object1, object2) \
                                            if(object1.getType() == object2.getType()) \
                                                throw std::invalid_argument("Types is not equal");

#define __IF_INDEX_NOT_BOUND__(object, index)   if(index + 1 <= object->size())
#define __IF_INDEX_NOT_BOUND2__(object, index)  if(index + 1 <= object.size())

#define __JSON_EMPTY_EXCEPTION__                throw std::invalid_argument("Json is empty");
#define __ARRAY_EMPTY_EXCEPTION__               throw std::invalid_argument("Array is empty");
#define __CHECK_ARRAY_EMPTY_EXCEPTION__(object) if(object.empty()) __ARRAY_EMPTY_EXCEPTION__
#define __CHECK_JSON_EMPTY_EXCEPTION__(object)  if(object.empty()) __JSON_EMPTY_EXCEPTION__

#define __INDEX_BOUND_EXCEPTION__           throw std::out_of_range("going beyond the object");

#define __CHECK_INDEX_BOUND__(object, index) \
                                            if(index + 1 > object.size()) \
                                                __INDEX_BOUND_EXCEPTION__
#define __RETURN_IF_CHECK_INDEX_BOUND__(object, ret) \
                                            if(index + 1 > object->size()) \
                                                return ret;

#define __KEY_NOT_FOUND_EXCEPTION__(key)    throw std::invalid_argument("key not found: " + key);
#define __RETURN_IF_KEY_NOT_FOUND_RETURN__(key, ret) \
                                            return ret;

//deprecated
//#define __INCORRECT_TYPE_ELEMENT_FOR_INDEX_EXCEPTION__ \
                                            throw std::invalid_argument( \
                                                "index operator may be used only for Json or Array Element(s)");
#define __ARRAY_INCORRECT_INDEX_EXCEPTION__ throw std::invalid_argument("index for Array must be a number only");
#define __JSON_KEY_NOT_FOUND_EXCEPTION__    throw std::invalid_argument("Json key not found");

//deprecated
//#define __NOT_ARRAY_OR_JSON_ELEMENT_EXCEPTION__ \
                                            throw std::invalid_argument("This Element cannot contain internal Element(s)");
#define __NOT_JSON_ELEMENT_EXCEPTION__      throw std::invalid_argument("This Element cannot contain internal named Element(s)");
#define __NO_ELEMENTS_EXCEPTION__           if(size() < 1) \
                                                throw std::invalid_argument("There are no Element(s)");
//=======================================================================EXEPTIONS



#endif // JSON_DEFINES_H
