#ifndef I_ELEMENT_H
#define I_ELEMENT_H

#include "../Comment.h"
#include "../ConfigCommon.h"

#include "IComment.cpp"

#include <map>
#include <vector>


//предобъявление
class Config;
class IElement;

using JPair                 = std::pair<std::string, Config>;
using shared_JPair          = std::pair<std::string, std::shared_ptr<Config>>;

using VPairElement          = std::vector<JPair>;
using shared_VPairElement   = std::vector<shared_JPair>;

using VElement              = std::vector<Config>;
using shared_VElement       = std::vector<std::shared_ptr<Config>>;

//базовый класс, в идеале используется только для ссылки на производный
class IElement : public IComment {
protected:
    ValueType   m_type;
    IElement()                                                  noexcept            { m_type = ValueType::eNull; }
public:
    virtual ~IElement()                                         noexcept            {}

    //FIXME: ЭТО ФУНКЦИОНАЛ КЛАССА Config!!!
//NOTE: API_ - приписка для обозначения интерфейсных функций при использовании через класс Config
//#define API_ALL
//#define API_BOOL
//#define API_NUMBER
//#define API_STRING
//#define API_ARRAY
//#define API_JSON
    // Comment =========================================================================================================
//    void    API_add_comment(const size_t index, const Comment &content)                     API_ARRAY   API_JSON;
//    void    API_add_comment(const size_t index, const std::string &content_before,
//                     const std::string &content_after)                                      API_ARRAY   API_JSON;
//    void    API_add_prefix_comment(const size_t index, const std::string &content)          API_ARRAY   API_JSON;
//    void    API_add_suffix_comment(const size_t index, const std::string &content)          API_ARRAY   API_JSON;
//    Comment&        API_get_comment(const size_t index)                                     API_ARRAY   API_JSON;
//    Comment         API_get_comment(const size_t index)                     const           API_ARRAY   API_JSON;
//    std::string&    API_get_prefix_comment(const size_t index)                              API_ARRAY   API_JSON;
//    std::string     API_get_prefix_comment(const size_t index)              const           API_ARRAY   API_JSON;
//    std::string&    API_get_suffix_comment(const size_t index)                              API_ARRAY   API_JSON;
//    std::string     API_get_suffix_comment(const size_t index)              const           API_ARRAY   API_JSON;
//    void    API_clear_comment(const size_t index)                                           API_ARRAY   API_JSON;
//    void    API_clear_prefix_comment(const size_t index)                                    API_ARRAY   API_JSON;
//    void    API_clear_suffix_comment(const size_t index)                                    API_ARRAY   API_JSON;
//    void    API_delete_comment(const size_t index)                                          API_ARRAY   API_JSON;
//    void    API_delete_prefix_comment(const size_t index)                                   API_ARRAY   API_JSON;
//    void    API_delete_suffix_comment(const size_t index)                                   API_ARRAY   API_JSON;
    // ========================================================================================================= Comment

    // Setters =========================================================================================================
        // заполнение строго через конструкторы или operator=
//    void API_append_if_not_contains(const Config& config)                   noexcept        API_ARRAY;
//    void API_append_if_not_contains(Config&& config)                        noexcept        API_ARRAY;
//    void API_set(const std::string& key, const Config& new_value,
//             bool update = true)                                            noexcept        API_JSON;
//    void API_set(const std::string& key, Config&& new_value,
//             bool update = true)                                            noexcept        API_JSON;
//    void API_set_if_null(const std::string& key, const Config& new_value)   noexcept        API_JSON;
//    void API_set_if_null(const std::string& key, Config&& new_value)        noexcept        API_JSON;
    // ========================================================================================================= Setters

    // Getters =========================================================================================================
        // внутри у каждого потомка должна быть реализация getValue()
//    bool&           API_getBool()                                                           API_BOOL;
//    bool            API_getBool()                                           const           API_BOOL;
//    long double&    API_getNumber()                                                         API_NUMBER;
//    long double     API_getNumber()                                         const           API_NUMBER;
//    std::string&    API_getString()                                                         API_STRING;
//    std::string     API_getString()                                         const           API_STRING;
//    Config& API_get_front()                                                                 API_ARRAY   API_JSON;
//    Config  API_get_front()                                                 const           API_ARRAY   API_JSON;
//    Config& API_get_at(const size_t index)                                                  API_ARRAY   API_JSON;
//    Config  API_get_at(const size_t index)                                  const           API_ARRAY   API_JSON;
//    Config& API_get_back()                                                                  API_ARRAY   API_JSON;
//    Config  API_get_back()                                                  const           API_ARRAY   API_JSON;
//    Config& API_get_at(const std::string& key)                                      noexcept            API_JSON;
//    Config  API_get_at(const std::string& key)                                      const noexcept      API_JSON;
//    Config& API_get_or_default_at(const std::string& key, Config&& default_value)   noexcept            API_JSON;
//    Config  API_get_or_default_at(const std::string& key, Config&& default_value)   const noexcept      API_JSON;
    // ========================================================================================================= Getters

    // Modify ==========================================================================================================
    virtual void    clear()                                     noexcept            = 0;
    // ========================================================================================================== Modify

    // Adding ==========================================================================================================
    //одиночные элементы
//    void API_insert_front(const Config& value)                              noexcept        API_ARRAY;
//    void API_insert_front(Config&& value)                                   noexcept        API_ARRAY;
//    void API_insert_at(const size_t index, const Config& value)             noexcept        API_ARRAY;
//    void API_insert_at(const size_t index, Config&& value)                  noexcept        API_ARRAY;
//    void API_insert_at(shared_VElement::iterator iterator, const Config& value)             API_ARRAY;
//    void API_insert_at(shared_VElement::iterator iterator, Config&& value)                  API_ARRAY;
//    void API_insert_back(const Config& value)                               noexcept        API_ARRAY;
//    void API_insert_back(Config&& value)                                    noexcept        API_ARRAY;
//    void API_insert_front(const std::string& key, const Config& value)      noexcept        API_JSON;
//    void API_insert_front(const std::string& key, Config&& value)           noexcept        API_JSON;
    //@return - был ли изменён индекс вниз при перезаписи ключа
//    bool API_insert_at(const size_t index, const std::string& key,
//                       const Config& value)                                 noexcept        API_JSON;
    //@return - был ли изменён индекс вниз при перезаписи ключа
//    bool API_insert_at(const size_t index, const std::string& key,
//                       Config&& value)                                      noexcept        API_JSON;
//    void API_insert_at(VPairElement::iterator iterator, const std::string& key,
//                       const Config& value)                                                 API_JSON;
//    void API_insert_at(VPairElement::iterator iterator, const std::string& key,
//                       Config&& value)                                                      API_JSON;
//    void API_insert_back(const std::string& key, const Config& value)       noexcept        API_JSON;
//    void API_insert_back(const std::string& key, Config&& value)            noexcept        API_JSON;
    //если нужного элемента нет - положить в конец списка
//    void API_insert_at(const std::string& key, const Config& value)         noexcept        API_JSON;
//    void API_insert_at(const std::string& key, Config&& value)              noexcept        API_JSON;
//    void API_insert_before(const std::string &position_key, const std::string& key,
//                           const Config& value)                                             API_JSON;
//    void API_insert_before(const std::string &position_key, const std::string& key,
//                           Config&& value)                                                  API_JSON;
//    void API_insert_after(const std::string &position_key, const std::string& key,
//                          const Config& value)                                              API_JSON;
//    void API_insert_after(const std::string &position_key, const std::string& key,
//                          Config&& value)                                                   API_JSON;

    //группы элементов
//    void API_insert_front(const VElement& elements)                         noexcept        API_ARRAY;
//    void API_insert_front(VElement&& elements)                              noexcept        API_ARRAY;
//    void API_insert_at(const size_t index, const VElement& elements)        noexcept        API_ARRAY;
//    void API_insert_at(const size_t index, VElement&& elements)             noexcept        API_ARRAY;
//    void API_insert_back(const VElement& elements)                          noexcept        API_ARRAY;
//    void API_insert_back(VElement&& elements)                               noexcept        API_ARRAY;
//    void API_insert_front(const VPairElement& elements)                     noexcept        API_JSON;
//    void API_insert_front(VPairElement&& elements)                          noexcept        API_JSON;
//    void API_insert_at(const size_t index, const VPairElement& elements)    noexcept        API_JSON;
//    void API_insert_at(const size_t index, VPairElement&& elements)         noexcept        API_JSON;
//    void API_insert_back(const VPairElement& elements)                      noexcept        API_JSON;
//    void API_insert_back(VPairElement&& elements)                           noexcept        API_JSON;

    //неизвестное количество элементов
//    template<typename ... Args>
//    void API_insert_front(Args&& ... args)                                  noexcept        API_ARRAY
//    {
//        //TODO: void API_insert_front(Args&& ... args)
//    }
//    template<typename ... Args>
//    void API_insert_at(const size_t index, Args&& ... args)                 noexcept        API_ARRAY
//    {
//        //TODO: void API_insert_at(const size_t index, Args&& ... args)
//    }
//    template<typename ... Args>
//    void API_insert_back(Args&& ... args)                                   noexcept        API_ARRAY
//    {
//        //TODO: void API_insert_back(Args&& ... args)
//    }
//    template<typename ... Value>
//    void    insert_front(std::pair<std::string, Value>&& ... pairs) noexcept {
//        VPairElement vpe;
//        if(vpe.capacity() < sizeof...(pairs))
//            vpe.reserve(sizeof...(pairs));
//        (void)std::initializer_list<int>{(vpe.push_back(std::forward<std::pair<std::string, Value>>(pairs)), 0)...};
//        insert_front(std::move(vpe));
//    }
//    template<typename ... Value>
//    void    insert_at(const size_t index, std::pair<std::string, Value>&& ... pairs) noexcept {
//        VPairElement vpe;
//        if(vpe.capacity() < sizeof...(pairs))
//            vpe.reserve(sizeof...(pairs));
//        (void)std::initializer_list<int>{(vpe.push_back(index, std::forward<std::pair<std::string, Value>>(pairs)), 0)...};
//        insert_at(index, std::move(vpe));
//    }
//    template<typename ... Value>
//    void    insert_back(std::pair<std::string, Value>&& ... pairs) noexcept {
//        (void)std::initializer_list<int>{(insert_back(std::forward<std::pair<std::string, Value>>(pairs)), 0)...};
//    }

    //другое имя для тех же действий
//    void API_push_front(const Config& value)                                noexcept        API_ARRAY;
//    void API_push_front(Config&& value)                                     noexcept        API_ARRAY;
//    void API_push_back(const Config& value)                                 noexcept        API_ARRAY;
//    void API_push_back(Config&& value)                                      noexcept        API_ARRAY;
//    void    push_front(const std::string& key, const Config& value)             noexcept        { insert_front(key, value); }
//    void    push_front(const std::string& key, Config&& value)                  noexcept        { insert_front(key, std::move(value)); }
//    void    push_back(const std::string& key, const Config& value)              noexcept        { insert_back(key, value); }
//    void    push_back(const std::string& key, Config&& value)                   noexcept        { insert_back(key, std::move(value)); }

//    template<typename ... Args>
//    void API_push_front(Args&& ... args)                                    noexcept        API_ARRAY
//    {
//        //TODO: void API_push_front(Args&& ... args)
//    }
//    template<typename ... Args>
//    void API_push_back(Args&& ... args)                                     noexcept        API_ARRAY
//    {
//        //TODO: void API_push_back(Args&& ... args)
//    }
    // ========================================================================================================== Adding

    // Removing ========================================================================================================
    // ======================================================================================================== Removing

    // Info ============================================================================================================
    // ============================================================================================================ Info

    // Operators =======================================================================================================
    // ======================================================================================================= Operators

    // Iterators =======================================================================================================
    // ======================================================================================================= Iterators

    // String ==========================================================================================================
    // ========================================================================================================== String

    // File ============================================================================================================
    // ============================================================================================================ File

    // Parser ==========================================================================================================
    // ========================================================================================================== Parser

    //----------------------------------------------------------

    // Info ============================================================================================================
    ValueType       getType()                                   const noexcept      { return m_type; }
    virtual bool    isContainer()                               const noexcept      { return false; }
    virtual bool    isIndexOnlyContainer()                      const noexcept      { return false; }
    virtual bool    isMapContainer()                            const noexcept      { return false; }

    bool            isEqual(const IElement& other, const bool compare_comments = false) const;

    //числа, контейнеры(размер), строки(длина в видимых символах)
    virtual size_t  size()                                      const noexcept      { return 0; }
    virtual bool    empty()                                     const noexcept      { return true; }
    // ============================================================================================================ Info

    // Operators =======================================================================================================
        // operator=() не реализован для потомков
        // operator==() не реализован для потомков
        // operator!=() не реализован для потомков
        // operator>() не реализован для потомков
        // operator>=() не реализован для потомков
        // operator<() не реализован для потомков
        // operator<=() не реализован для потомков
    // ======================================================================================================= Operators

    // String ==========================================================================================================
    //вывод без комментариев, "tabulation_level == -1" => запись в одну строку
    virtual std::string toString(const ConfigFormat format = ConfigFormat::eONLY_VALUE,
                                 const int8_t tabulation_level = -1, const CommentDesign &design = {})
                                                                const noexcept      = 0;
    virtual std::string toJsonString(const int8_t tabulation_level = -1, const CommentDesign &design = {})
                                                                const noexcept      = 0;
    virtual std::string toIniString(const int8_t tabulation_level = -1, const CommentDesign &design = {})
                                                                const noexcept      = 0;
    virtual std::string toYamlString(const int8_t tabulation_level = -1, const CommentDesign &design = {})
                                                                const noexcept      = 0;
    virtual std::string toXmlString(const int8_t tabulation_level = -1, const CommentDesign &design = {})
                                                                const noexcept      = 0;
    // NOTE: работа с std::cout реализована в Config
    // ========================================================================================================== String

    // File ============================================================================================================
        // работа с файлами и парсингом не предусмотрена для этого класса
    // ============================================================================================================ File

    // Parser ==========================================================================================================
        // работа с файлами и парсингом не предусмотрена для этого класса
    // ========================================================================================================== Parser

    // Iterators =======================================================================================================
        //индивидуально для ElementContainer классов
    // ======================================================================================================= Iterators
};

//----------------------------------------------------------------------------------------------------------------------
//TODO: добавить СЮДА Is<TYPE>(std::string) парсинги?
//----------------------------------------------------------------------------------------------------------------------


#endif // I_ELEMENT_H
