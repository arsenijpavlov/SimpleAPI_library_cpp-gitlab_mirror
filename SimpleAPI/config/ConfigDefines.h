#pragma once

#include <stdexcept>


//VALUES==========================================================================
#define __SPACES__                                  " \n\t"
#define __KEY_VALUE_SEPARATOR__                     ":="
#define __SEPARATORS__                              ",\n"
#define __SPACES_WITHOUT_SEPARATORS__               " \t"
#define __POSIBLE_COLON__                           ":="
#define __BORDER_SYMBOLS__                          "@#*-=@"
                                                    // перенос есть, пробел после знака
#define __COMMENT_SEPARATOR_SYMBOLS__               ".,;:?!)]"
                                                    // перенос есть, пробел не нужен
#define __COMMENT_OTHERS_SPEC_SYMBOLS__             "-<>+/\\*$#@&([~"
#define __COMMENT_SEPARATOR_SYMBOLS_FOR_NUMBER__    ",;?!)]<>~+/\\*$#@&([" //без .:-
#define __POSSIBLE_KEY_SYMBOLS__                    "^[a-zA-Z0-9._ #@!*&\"'/\\\\]+$"
//==========================================================================VALUES

//EXEPTIONS=======================================================================
#define __INCORRECT_TYPE_EXCEPTION__(str)       throw std::invalid_argument(std::string("This Element is not a ") + #str + " type");
#define __INCORRECT_NOT_TYPE_EXCEPTION__(str)   throw std::invalid_argument(std::string("This Element must be a ") + #str + " type");

#define __CHECK_TYPE_NOT_NULL_ACTION__(object)      if(!object.isNull())
#define __CHECK_TYPE_NOT_BOOL_ACTION__(object)      if(!object.isBool())
#define __CHECK_TYPE_NOT_CONVERTIBLE_TO_BOOL_ACTION__(object) \
                                                    if(!(object.isBool() || object.isNumber() || object.isChar() || object.isString()))
#define __CHECK_TYPE_NOT_NUMBER_ACTION__(object)    if(!object.isNumber())
#define __CHECK_TYPE_NOT_CHAR_ACTION__(object)      if(!object.isChar())
#define __CHECK_TYPE_NOT_STRING_ACTION__(object)    if(!object.isString())
#define __CHECK_TYPE_NOT_CHAR_OR_STRING_ACTION__(object) \
                                                    if(!object.isString() && !object.isChar())
#define __CHECK_TYPE_NOT_ARRAY_ACTION__(object)     if(!object.isArray())
#define __CHECK_TYPE_NOT_JSON_ACTION__(object)      if(!object.isJson())

#define __CHECK_TYPE_IS_NULL__(object)      __CHECK_TYPE_NOT_NULL_ACTION__(object)      __INCORRECT_TYPE_EXCEPTION__("NULL")
#define __CHECK_TYPE_IS_BOOL__(object)      __CHECK_TYPE_NOT_BOOL_ACTION__(object)      __INCORRECT_TYPE_EXCEPTION__("BOOL")
#define __CHECK_TYPE_IS_CONVERIBLE_TO_BOOL__(object) \
                                            __CHECK_TYPE_NOT_CONVERTIBLE_TO_BOOL_ACTION__(object) \
                                                __INCORRECT_TYPE_EXCEPTION__("BOOL/NUMBER/CHAR/STRING")
#define __CHECK_TYPE_IS_NUMBER__(object)    __CHECK_TYPE_NOT_NUMBER_ACTION__(object)    __INCORRECT_TYPE_EXCEPTION__("NUMBER")
#define __CHECK_TYPE_IS_CHAR__(object)      __CHECK_TYPE_NOT_CHAR_ACTION__(object)      __INCORRECT_TYPE_EXCEPTION__("CHAR")
#define __CHECK_TYPE_IS_STRING__(object)    __CHECK_TYPE_NOT_STRING_ACTION__(object)    __INCORRECT_TYPE_EXCEPTION__("STRING")
#define __CHECK_TYPE_IS_CHAR_OR_STRING__(object) \
                                            __CHECK_TYPE_NOT_CHAR_OR_STRING_ACTION__(object) __INCORRECT_TYPE_EXCEPTION__("CHAR or STRING")
#define __CHECK_TYPE_IS_ARRAY__(object)     __CHECK_TYPE_NOT_ARRAY_ACTION__(object)     __INCORRECT_TYPE_EXCEPTION__("ARRAY")
#define __CHECK_TYPE_IS_JSON__(object)      __CHECK_TYPE_NOT_JSON_ACTION__(object)      __INCORRECT_TYPE_EXCEPTION__("JSON")

#define __CHECK_TYPE_IS_CONTAINER__(object)     if(!object.isContainer())       __INCORRECT_TYPE_EXCEPTION__("container")
#define __CHECK_TYPE_IS_NOT_CONTAINER__(object) if(object.isContainer())        __INCORRECT_NOT_TYPE_EXCEPTION__("not container")
#define __CHECK_TYPE_IS_INDEX_CONTAINER__(object) \
                                                if(!object.isIndexContainer())  __INCORRECT_TYPE_EXCEPTION__("index container")
#define __CHECK_TYPE_IS_MAP_CONTAINER__(object) \
                                                if(!object.isMapContainer())    __INCORRECT_TYPE_EXCEPTION__("map<key,value>")
#define __CHECK_TYPE_IS_NOT_MAP_CONTAINER__(object) \
                                                if(object.isMapContainer())     __INCORRECT_NOT_TYPE_EXCEPTION__("not map<key,value>")

#define __CHECK_TYPE_IS_NOT_NULL__(object)      if(object.isNull()) \
                                                    throw std::invalid_argument("This element is a NULL type");

// типы eChar и eString допустимы для сравнения
#define __CHECK_TYPES_NOT_EQUAL_ACTION__(object1, object2) \
                                                if((object1.getType() != object2.getType()) \
                                                    && !((object1.getType() == ValueType::eChar || object1.getType() == ValueType::eString) \
                                                         && (object2.getType() == ValueType::eChar || object2.getType() == ValueType::eString)))
#define __CHECK_TYPES_IS_EQUAL__(object1, object2) \
                                                __CHECK_TYPES_NOT_EQUAL_ACTION__(object1, object2) \
                                                    throw std::invalid_argument("Types is not equal");

#define __INCORRECT_INDEX_EXCEPTION__(index)    throw std::out_of_range(std::string("Index ") \
                                                    + std::to_string(index) + " not contain an object");

#define __CHECK_CONTAINER_INDEX_CORRECT__(object, index) \
                                                if(index > object.size() - 1)   __INCORRECT_INDEX_EXCEPTION__(index)

//--------------------

#define __IF_INDEX_NOT_BOUND__(object, index)   if(index + 1 <= object->size())
#define __IF_INDEX_NOT_BOUND2__(object, index)  if(index + 1 <= object.size())

#define __JSON_EMPTY_EXCEPTION__                throw std::invalid_argument("Json is empty");
#define __ARRAY_EMPTY_EXCEPTION__               throw std::invalid_argument("Array is empty");
#define __CHECK_ARRAY_EMPTY_EXCEPTION__(object) if(object.empty()) __ARRAY_EMPTY_EXCEPTION__
#define __CHECK_JSON_EMPTY_EXCEPTION__(object)  if(object.empty()) __JSON_EMPTY_EXCEPTION__

#define __INDEX_BOUND_EXCEPTION__               throw std::out_of_range("going beyond the object");

#define __CHECK_INDEX_BOUND__(object, index) \
                                                if(index + 1 > object.size()) \
                                                    __INDEX_BOUND_EXCEPTION__
#define __RETURN_IF_CHECK_INDEX_BOUND__(object, ret) \
                                                if(index + 1 > object->size()) \
                                                    return ret;

#define __KEY_NOT_FOUND_EXCEPTION__(key)        throw std::out_of_range("key not found: " + key);
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
