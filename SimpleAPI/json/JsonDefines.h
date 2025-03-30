#ifndef JSONDEFINES_H
#define JSONDEFINES_H

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
#define __JSON_EMPTY_EXCEPTION__    throw std::invalid_argument("Json is empty");
#define __ARRAY_EMPTY_EXCEPTION__   throw std::invalid_argument("JArray is empty");
#define __CHECK_INDEX_BOUND_EXCEPTION__(object, index) \
                                    if(index + 1 > object->size()) \
                                        throw std::out_of_range("going beyond the object");
#define __CHECK_INDEX_BOUND_RETURN__(object, ret) \
                                    if(index + 1 > object->size()) \
                                        return ret;
#define __CHECK_INDEX_BOUND2_EXCEPTION__(object, index) \
                                    if(index + 1 > object.size()) \
                                        throw std::out_of_range("going beyond the object");
#define __CHECK_INDEX_BOUND2_RETURN__(object, ret) \
                                    if(index + 1 > object.size()) \
                                        return ret;
#define __KEY_NOT_FOUND_EXCEPTION__(key) \
                                    throw std::invalid_argument("key not found: " + key);
#define __KEY_NOT_FOUND_RETURN__(key, ret) \
                                    return ret;
#define __INCORRECT_TYPE_ELEMENT_FOR_INDEX_EXCEPTION__ \
                                    throw std::invalid_argument( \
                                        "index operator may be used only for Json or JArray elements");
#define __ARRAY_INCORRECT_INDEX_EXCEPTION__ \
                                    throw std::invalid_argument("index for JArray must be a number only");
#define __JSON_KEY_NOT_FOUND_EXCEPTION__ \
                                    throw std::invalid_argument("Json key not found");
#define __NOT_ARRAY_OR_JSON_ELEMENT_EXCEPTION__ \
                                    throw std::invalid_argument("This element cannot contain internal elements");
#define __NOT_JSON_ELEMENT_EXCEPTION__ \
                                    throw std::invalid_argument("This element cannot contain internal named elements");
#define __NO_ELEMENTS_EXCEPTION__   if(size() < 1) throw std::invalid_argument("There are no elements");
//=======================================================================EXEPTIONS



#endif // JSONDEFINES_H
