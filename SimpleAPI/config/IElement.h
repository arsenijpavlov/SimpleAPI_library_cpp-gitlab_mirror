#ifndef I_ELEMENT_H
#define I_ELEMENT_H

#include "Comment.h"
#include "ConfigCommon.h"

#include <map>
#include <vector>


//предобъявление
class Config;
class IElement;

using JPair         = std::pair<std::string, std::shared_ptr<IElement>>;
using VPairElement  = std::vector<JPair>;
using VElement      = std::vector<std::shared_ptr<IElement>>;

//базовый класс, в идеале используется только для ссылки на производный
class IElement {
public:
    IElement()                                                  noexcept            {}
    virtual ~IElement()                                         noexcept            {}

    // Getters =========================================================================================================
    virtual bool&           getBool();
    virtual bool            getBool()                           const;
    virtual long double&    getNumber();
    virtual long double     getNumber()                         const;
    virtual std::string&    getString();
    virtual std::string     getString()                         const;

    // вложенные контейнеры
    virtual Config&         get_front();
    virtual Config          get_front()                         const;
    virtual Config&         get_at(const size_t index);
    virtual Config          get_at(const size_t index)          const;
    virtual Config&         get_at(const std::string& key);
    virtual Config          get_at(const std::string& key)      const;
    virtual Config&         get_back();
    virtual Config          get_back()                          const;
    // ========================================================================================================= Getters

    // Info ============================================================================================================
    virtual bool            isContainer()                       const noexcept      { return false; }
    virtual bool            isEqual(const Config& other)        const noexcept      { return false; }
    virtual bool            isEqual(const bool other)           const noexcept      { return false; }
    virtual bool            isEqual(const long double& other)   const noexcept      { return false; }
    virtual bool            isEqual(const std::string& other)   const noexcept      { return false; }
    virtual size_t          size()                              const noexcept      { return 0; }
    // ============================================================================================================ Info

    // Operators =======================================================================================================
    virtual Config&         operator[](const size_t index)      noexcept;
    virtual Config          operator[](const size_t index)      const noexcept;
    virtual Config&         operator[](const std::string& key)  noexcept;
    virtual Config          operator[](const std::string& key)  const noexcept;
    // ======================================================================================================= Operators

    // String ==========================================================================================================
    //вывод без комментариев, "tabulation_level == -1" => запись в одну строку
    virtual std::string     toString(const ConfigFormat format = ConfigFormat::eJSON,
                         const int8_t tabulation_level = 0,
                         const CommentDesign &design = {})      const noexcept      { return ""; }
    // ========================================================================================================== String
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

#endif // I_ELEMENT_H
