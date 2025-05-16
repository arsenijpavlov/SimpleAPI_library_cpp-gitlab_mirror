#ifndef I_ELEMENT_H
#define I_ELEMENT_H

#include "Comment.h"
#include "ConfigCommon.h"
#include "ConfigDefines.h"

#include <map>
#include <vector>


//предобъявление
class ElementArray;
class ElementJson;
class IElement;

using JPair         = std::pair<std::string, std::shared_ptr<IElement>>;
using VPairElement  = std::vector<JPair>;
using VElement      = std::vector<std::shared_ptr<IElement>>;

//базовый класс
//NOTE: равносилен ELEMENT_NULL
class IElement {

public:
    IElement()                      noexcept    {}
    virtual ~IElement()             noexcept    {}

public:
    // String ==========================================================================================================


//TODO:    std::string toStringWithComments();
    virtual std::string toString() const noexcept;

    virtual std::string toString(const ConfigFormat format = ConfigFormat::eJSON, const int8_t tabulation_level = 0) const noexcept;
    // ========================================================================================================== String


};


//    //PRINTING =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//    //для рекурсивного вызова, без комментариев, в одну строку
//    virtual std::string toString(const ConfigFormat format = ConfigFormat::eJSON,
//                                  const int8_t tabulation_level = 0) const noexcept
//                                                                { return ""; }
//    //для рекурсивного вызова, с использованием комментариев
//    virtual std::string toString(const ConfigFormat format, const CommentDesign &design,
//                                  const int8_t tabulation_level = 0) const noexcept
//                                                                { return ""; }
//    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= PRINTING



//    virtual bool isEqual(const IElement& other)                 const noexcept { return false; }
//    virtual bool isEqualWithComments(const IElement& other)     const noexcept { return false; }

//    //OPERATORS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//    //NOTE: комментарии при присваивании копируются
////    IElement& operator=(const IElement& other) noexcept;
//    //NOTE: комментарии при сравнении не учитываются
//    bool operator==(const IElement& other) const noexcept       { return (m_type == other.m_type ? isEqual(other) : false); }
//    bool operator!=(const IElement& other) const noexcept       { return !(*this == other); }
//    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= OPERATORS
//};


//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

#endif // I_ELEMENT_H
