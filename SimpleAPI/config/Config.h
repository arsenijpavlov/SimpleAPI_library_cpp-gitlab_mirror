#pragma once

#include "Comment.h"
#include "ConfigCommon.h"
#include "../utils/TypeDefines.h"
#include "ElementArray.h"
#include "ElementBool.h"
#include "ElementJson.h"
#include "ElementNumber.h"
#include "ElementString.h"
#include "../utils/OnlySizetOrString.h"
#include "../utils/ParserSymbolCounter.h"


namespace simpleapi {

//вынести вовне нельзя, т.к. нет контекста об ElementJson и ElementArray
template<typename T>
struct is_valid_config_type {
    static constexpr bool value =
        std::is_same<       typename std::decay<T>::type, tools::ElementJson>::value ||
        std::is_same<       typename std::decay<T>::type, tools::ElementArray>::value ||
        std::is_convertible<typename std::decay<T>::type, std::string>::value ||
        std::is_arithmetic< typename std::decay<T>::type>::value ||
        std::is_same<       typename std::decay<T>::type, bool>::value ||
        std::is_same<       typename std::decay<T>::type, Config>::value
        ;
};

#define __ONLY_ALLOWED_TYPES_VARIADIC__(ARG) \
    template<typename ... ARG, \
             typename std::enable_if< \
                all_true<is_valid_config_type<ARG>::value...>::value, int \
             >::type* = nullptr>
//самый лёгкий способ достать первый элемент из variadic и не передать его дальше по рекурсии
#define __ONLY_ALLOWED_TYPES_VARIADIC_PAIR__(ARG, ARG2) \
    template<typename ... ARG, \
             typename std::enable_if< \
                all_true<is_valid_config_type<ARG>::value...>::value, int \
             >::type* = nullptr, typename ARG2>


//ПРЕДВАРИТЕЛЬНЫЕ ОБЪЯВЛЕНИЯ
Config CreateElementFromString(std::string &&value_string, const ConfigFormat format,
                               CommentDesign &design, ParserSymbolCounter& start_iterator)              noexcept;

Config ReadFile(const std::string& file_path, const ConfigFormat format,
                const CommentDesign &design = {})                                                       noexcept;
Config ReadFileJson(const std::string& file_path, const CommentDesign &design = {})                     noexcept;
Config ReadFileIni(const std::string& file_path, const CommentDesign &design = {})                      noexcept;

//return - удалось записать файл или нет
bool WriteFile(const Config& config, const std::string& file_path,
               const ConfigFormat format, const CommentDesign &design = {},
               const uint8_t custom_tabulation_level = 0)                                               noexcept;
bool WriteFileJson(const Config& config, const std::string& file_path,
                   const CommentDesign &design = {}, const uint8_t custom_tabulation_level = 0)         noexcept;
bool WriteFileIni(const Config& config, const std::string& file_path,
                  const CommentDesign &design = {}, const uint8_t custom_tabulation_level = 0)          noexcept;

Config Parse(const std::string& content, const ConfigFormat format,
             const CommentDesign &design = {})                                                          noexcept;
Config ParseJson(const std::string& content, const CommentDesign &design = {})                          noexcept;
Config ParseIni(const std::string& content, const CommentDesign &design = {})                           noexcept;

class Config {
private:
    tools::IElement* m_value;

public:
    Config()                                        noexcept : m_value(nullptr)     { init(); }
    Config(const Config& other)                     noexcept : m_value(nullptr)     { setValue(other); }
    Config(Config&& other)                          noexcept : m_value(other.m_value) { other.m_value = nullptr; }
    explicit Config(const tools::IElement& other)   noexcept : m_value(nullptr)     { setValue(other); }
    explicit Config(tools::IElement&& other)        noexcept : m_value(nullptr)     { setValue(std::move(other)); }

    explicit Config(std::nullptr_t)                 noexcept : m_value(nullptr)     { setValue(); }
    explicit Config(const bool other)               noexcept : m_value(nullptr)     { setValue(other); }
    __ONLY_NUMBER_TYPES__(T)
    explicit Config(T&& other)                      noexcept : m_value(nullptr)     { setValue(static_cast<long double>(other)); }

    // NOTE: с explicit Config(const char*) не работает
    __ONLY_STRING_TYPES__(T)
    /*explicit*/ Config(const T& other)             noexcept : m_value(nullptr)     { setValue(std::string(other)); }
    __ONLY_STRING_TYPES__(T)
    /*explicit*/ Config(T&& other)                  noexcept : m_value(nullptr)     { setValue(std::string(std::move(other))); }

    explicit Config(const ValueType config_type) : m_value(nullptr) {
        using namespace tools;

        release();
        switch(config_type) {
        default:
        case ValueType::eNull: {
            init();
            break;
        }
        case ValueType::eBool: {
            m_value = new ElementBool();
            break;
        }
        case ValueType::eNumber: {
            m_value = new ElementNumber();
            break;
        }
        case ValueType::eString: {
            m_value = new ElementString();
            break;
        }
        case ValueType::eArray: {
            m_value = new ElementArray();
            break;
        }
        case ValueType::eJson: {
            m_value = new ElementJson();
            break;
        }
        }
    }

    //контейнеры
    __ONLY_ALLOWED_TYPES_VARIADIC__(T)
    explicit Config(const ValueType config_type, const T& ... values) : m_value(nullptr) {
        using namespace tools;

        release();
        std::vector<Config> vec; //ключи тоже будут преобразованы в Config
        (void)std::initializer_list<int>{(vec.push_back(Config(values)), 0)...};

        switch(config_type) {
        default:
        case ValueType::eNull: {
            if(sizeof...(values) != 0)
                throw std::invalid_argument("zero arguments required (type Null)");

            init();
            break;
        }
        case ValueType::eBool: {
            if(sizeof...(values) > 1)
                throw std::invalid_argument("one argument maximum required (type Bool)");
            if(!vec[0].isBool())
                throw std::invalid_argument("incorrect format (type Bool)");

            if(sizeof...(values) == 1)
                setValue(vec[0]);
            else
                m_value = new ElementBool();
            break;
        }
        case ValueType::eNumber: {
            if(sizeof...(values) > 1)
                throw std::invalid_argument("one argument maximum required (type Number)");
            if(!vec[0].isNumber())
                throw std::invalid_argument("incorrect format (type Number)");

            if(sizeof...(values) == 1)
                setValue(vec[0]);
            else
                m_value = new ElementNumber();
            break;
        }
        case ValueType::eString: {
            if(sizeof...(values) > 1)
                throw std::invalid_argument("one argument maximum required (type String)");
            if(!vec[0].isString())
                throw std::invalid_argument("incorrect format (type String)");

            if(sizeof...(values) == 1)
                setValue(vec[0]);
            else
                m_value = new ElementString();
            break;
        }
        case ValueType::eArray: {
            m_value = new ElementArray();
            (void)std::initializer_list<int>{(push_back(values), 0)...};
            break;
        }
        case ValueType::eJson: {
            if(sizeof...(values) % 2 != 0)
                throw std::invalid_argument("Even number of arguments required (type Json)");

            m_value = new ElementJson();

            for(size_t i = 0; i < vec.size(); i+=2) {
                push_back(vec[i].getString(), vec[i+1]);
            }
            break;
        }
        }
    }

    __ONLY_ALLOWED_TYPES_VARIADIC__(T)
    explicit Config(const ValueType config_type, T&& ... values) : m_value(nullptr) {
        using namespace tools;

        release();
        std::vector<Config> vec; //ключи тоже будут преобразованы в Config
        (void)std::initializer_list<int>{(vec.push_back(Config(std::move(values))), 0)...};

        switch(config_type) {
        default:
        case ValueType::eNull: {
            if(sizeof...(values) != 0)
                throw std::invalid_argument("zero arguments required (type Null)");

            init();
            break;
        }
        case ValueType::eBool: {
            if(sizeof...(values) > 1)
                throw std::invalid_argument("one argument maximum required (type Bool)");
            if(!vec[0].isBool())
                throw std::invalid_argument("incorrect format (type Bool)");

            if(sizeof...(values) == 1)
                setValue(std::move(vec[0]));
            else
                m_value = new ElementBool();
            break;
        }
        case ValueType::eNumber: {
            if(sizeof...(values) > 1)
                throw std::invalid_argument("one argument maximum required (type Number)");
            if(!vec[0].isNumber())
                throw std::invalid_argument("incorrect format (type Number)");

            if(sizeof...(values) == 1)
                setValue(std::move(vec[0]));
            else
                m_value = new ElementNumber();
            break;
        }
        case ValueType::eString: {
            if(sizeof...(values) > 1)
                throw std::invalid_argument("one argument maximum required (type String)");
            if(!vec[0].isString())
                throw std::invalid_argument("incorrect format (type String)");

            if(sizeof...(values) == 1)
                setValue(std::move(vec[0]));
            else
                m_value = new ElementString();
            break;
        }
        case ValueType::eArray: {
            m_value = new ElementArray();
            for(size_t i = 0; i < vec.size(); i++) {
                push_back(std::move(vec[i]));
            }
            break;
        }
        case ValueType::eJson: {
            if(sizeof...(values) % 2 != 0)
                throw std::invalid_argument("Even number of arguments required (type Json)");

            m_value = new ElementJson();

            for(size_t i = 0; i < vec.size(); i+=2) {
                push_back(vec[i].getString(), std::move(vec[i+1]));
            }
            break;
        }
        }
    }

    __ONLY_ALLOWED_TYPES__(T)
    explicit Config(const std::vector<std::pair<std::string, T>>& pairs_key_config) : m_value(nullptr) {
        m_value = new tools::ElementJson();
        for(const auto& pair : pairs_key_config)
            push_at(pair.first, pair.second);
    }
    __ONLY_ALLOWED_TYPES__(T)
    explicit Config(std::vector<std::pair<std::string, T>>&& pairs_key_config) : m_value(nullptr) {
        m_value = new tools::ElementJson();
        for(auto& pair : pairs_key_config)
            push_at(std::move(pair.first), std::move(pair.second));
    }

    ~Config()                                                       noexcept    { release(); }

private:
    //создание ПУСТОГО(NULL) элемента
    void init()                                                     noexcept    { setValue(); }
    void release()                                                  noexcept;

public:
    //NOTE: API_ - приписка для обозначения интерфейсных функций при использовании через класс Config
    #define API_ALL
    #define API_BOOL
    #define API_NUMBER
    #define API_STRING
    #define API_CONTAINER
    #define API_INDEX_ARRAY
    #define API_MAP_CONTAINER

    // Comment =========================================================================================================
    Config&         setComment(const Comment& content)              noexcept;                                               API_ALL
    Config&         setComment(const std::string &content_before, const std::string &content_after) noexcept;               API_ALL
    Config&         setPrefixComment(const std::string& content)    noexcept;                                               API_ALL
    Config&         setSuffixComment(const std::string& content)    noexcept;                                               API_ALL

    Comment&        getComment()                                    noexcept        { return m_value->getComment(); }       API_ALL
    Comment         getComment()                                    const noexcept  { return m_value->getComment(); }       API_ALL
    std::string     getPrefixComment()                              const noexcept  { return m_value->getPrefixComment(); } API_ALL
    std::string     getSuffixComment()                              const noexcept  { return m_value->getSuffixComment(); } API_ALL

    //NOTE: доступы к внутренним значениям строго по set() и get() запросам
    //std::string&    getPrefixComment()                              noexcept        { return m_value->getPrefixComment(); } API_ALL
    //std::string&    getSuffixComment()                              noexcept        { return m_value->getSuffixComment(); } API_ALL

    Config&         clearComment()                                  noexcept;                                               API_ALL
    Config&         clearPrefixComment()                            noexcept;                                               API_ALL
    Config&         clearSuffixComment()                            noexcept;                                               API_ALL
    Config&         deleteComment()                                 noexcept;                                               API_ALL
    Config&         deletePrefixComment()                           noexcept;                                               API_ALL
    Config&         deleteSuffixComment()                           noexcept;                                               API_ALL

    CommentDesign&  getCommentDesign()                              noexcept        { return m_value->getCommentDesign(); } API_ALL
    CommentDesign   getCommentDesign()                              const noexcept  { return m_value->getCommentDesign(); } API_ALL
    Config&         setCommentDesign(const CommentDesign &design)   noexcept;                                               API_ALL
    Config&         clearCommentDesign()                            noexcept;                                               API_ALL

    // вложенные контейнеры
    Config&         set_comment(const size_t& index, const Comment &content);                                               API_CONTAINER
    Config&         set_comment(const std::string& key, const Comment &content);                                            API_MAP_CONTAINER

    Config&         set_comment(const size_t& index, const std::string &content_before,
                     const std::string &content_after);                                                                     API_CONTAINER
    Config&         set_comment(const std::string& key, const std::string &content_before,
                     const std::string &content_after);                                                                     API_MAP_CONTAINER

    Config&         set_prefix_comment(const size_t& index, const std::string &content);                                    API_CONTAINER
    Config&         set_prefix_comment(const std::string& key, const std::string &content);                                 API_MAP_CONTAINER

    Config&         set_suffix_comment(const size_t& index, const std::string &content);                                    API_CONTAINER
    Config&         set_suffix_comment(const std::string& key, const std::string &content);                                 API_MAP_CONTAINER

    Comment&        get_comment(const size_t& index);                                                                       API_CONTAINER
    Comment&        get_comment(const std::string& key);                                                                    API_MAP_CONTAINER

    Comment         get_comment(const size_t& index)                 const;                                                 API_CONTAINER
    Comment         get_comment(const std::string& key)             const;                                                  API_MAP_CONTAINER

    std::string     get_prefix_comment(const size_t& index)          const;                                                 API_CONTAINER
    std::string     get_prefix_comment(const std::string& key)      const;                                                  API_MAP_CONTAINER

    //NOTE: доступы к внутренним значениям строго по set() и get() запросам
//    std::string&    get_prefix_comment(const size_t& index);                                                                API_CONTAINER
//    std::string&    get_prefix_comment(const std::string& key);                                                             API_MAP_CONTAINER

    std::string     get_suffix_comment(const size_t& index)          const;                                                 API_CONTAINER
    std::string     get_suffix_comment(const std::string& key)      const;                                                  API_MAP_CONTAINER

    //NOTE: доступы к внутренним значениям строго по set() и get() запросам
//    std::string&    get_suffix_comment(const size_t& index);                                                                 API_CONTAINER
//    std::string&    get_suffix_comment(const std::string& key);                                                             API_MAP_CONTAINER

    Config&         clear_comment(const size_t& index);                                                                     API_CONTAINER
    Config&         clear_comment(const std::string& key);                                                                  API_MAP_CONTAINER

    Config&         clear_prefix_comment(const size_t& index);                                                              API_CONTAINER
    Config&         clear_prefix_comment(const std::string& key);                                                           API_MAP_CONTAINER

    Config&         clear_suffix_comment(const size_t& index);                                                              API_CONTAINER
    Config&         clear_suffix_comment(const std::string& key);                                                           API_MAP_CONTAINER

    Config&         delete_comment(const size_t& index);                                                                    API_CONTAINER
    Config&         delete_comment(const std::string& key);                                                                 API_MAP_CONTAINER

    Config&         delete_prefix_comment(const size_t& index);                                                             API_CONTAINER
    Config&         delete_prefix_comment(const std::string& key);                                                          API_MAP_CONTAINER

    Config&         delete_suffix_comment(const size_t& index);                                                             API_CONTAINER
    Config&         delete_suffix_comment(const std::string& key);                                                          API_MAP_CONTAINER
    // ========================================================================================================= Comment

    // Setters =========================================================================================================
    Config&         setValue()                                      noexcept;                                               API_ALL
    Config&         setValue(std::nullptr_t)                        noexcept;                                               API_ALL
    Config&         setValue(const Config& other)                   noexcept;                                               API_ALL
    Config&         setValue(Config&& other)                        noexcept;                                               API_ALL
    Config&         setValue(const tools::IElement& other)          noexcept;                                               API_ALL
    Config&         setValue(tools::IElement&& other)               noexcept;                                               API_ALL
    Config&         setValue(const bool other)                      noexcept;                                               API_ALL

    Config&         setValue(const long double& other)              noexcept;                                               API_ALL
    Config&         setValue(long double&& other)                   noexcept;                                               API_ALL
                    __ONLY_NUMBER_TYPES__(T)
    Config&         setValue(const T& other)    noexcept    { return setValue(static_cast<long double>(other)); }           API_ALL
                    __ONLY_NUMBER_TYPES__(T)
    Config&         setValue(T&& other)         noexcept    { return setValue(static_cast<long double>(std::move(other))); }API_ALL

    Config&         setValue(const std::string& other)              noexcept;                                               API_ALL
    Config&         setValue(std::string&& other)                   noexcept;                                               API_ALL
                    __ONLY_STRING_TYPES__(T)
    Config&         setValue(const T& other)    noexcept    { return setValue(std::string(other));}                         API_ALL
                    __ONLY_STRING_TYPES__(T)
    Config&         setValue(T&& other)         noexcept    { return setValue(std::move(std::string(std::move(other))));}   API_ALL

    Config&         setValue(const tools::ElementArray& other)      noexcept;                                               API_ALL
    Config&         setValue(tools::ElementArray&& other)           noexcept;                                               API_ALL
    Config&         setValue(const tools::ElementJson& other)       noexcept;                                               API_ALL
    Config&         setValue(tools::ElementJson&& other)            noexcept;                                               API_ALL
    //TODO (потом):    Config&         setValue(const tools::ElementYaml& other)      noexcept;
    //TODO (потом):    Config&         setValue(tools::ElementYaml&& other)           noexcept;
    //TODO (потом):    Config&         setValue(const tools::ElementXml& other)       noexcept;
    //TODO (потом):    Config&         setValue(tools::ElementXml&& other)            noexcept;

    // вложенные контейнеры (используют insert_*() ниже, но возвращают этот же базовый элемент)
    Config&         set(const std::string& key, const Config& value);                                                       API_MAP_CONTAINER
    Config&         set(const std::string& key, Config&& value);                                                            API_MAP_CONTAINER
    Config&         set(const size_t& index, const Config& value);                                                          API_CONTAINER
    Config&         set(const size_t& index, Config&& value);                                                               API_CONTAINER
    Config&         set(const size_t& index, const std::string& key, const Config& value);                                  API_MAP_CONTAINER
    Config&         set(const size_t& index, const std::string& key, Config&& value);                                       API_MAP_CONTAINER
    // ========================================================================================================= Setters

    // Getters =========================================================================================================
    bool&           getBool();                                                                                              API_BOOL
    bool            getBool()                                                                   const;                      API_BOOL
    long double&    getNumber();                                                                                            API_NUMBER
    long double     getNumber()                                                                 const;                      API_NUMBER
    uint8_t         getUInt8()                                                                  const;                      API_NUMBER
    uint16_t        getUInt16()                                                                 const;                      API_NUMBER
    uint32_t        getUInt32()                                                                 const;                      API_NUMBER
    uint64_t        getUInt64()                                                                 const;                      API_NUMBER
    int8_t          getInt8()                                                                   const;                      API_NUMBER
    int16_t         getInt16()                                                                  const;                      API_NUMBER
    int32_t         getInt32()                                                                  const;                      API_NUMBER
    int64_t         getInt64()                                                                  const;                      API_NUMBER
    float           getFloat()                                                                  const;                      API_NUMBER
    double          getDouble()                                                                 const;                      API_NUMBER
    long double     getLDouble()                                                                const;                      API_NUMBER
    std::string&    getString();                                                                                            API_STRING
    std::string     getString()                                                                 const;                      API_STRING

    bool            error()                                                                     const noexcept;             API_CONTAINER
    std::string     getError()                                                                  const noexcept;             API_CONTAINER
private:
    void            setError();                                                                                             API_CONTAINER
    void            setError(const std::string& error_string);                                                              API_CONTAINER
    void            setError(std::string&& error_string);                                                                   API_CONTAINER
public:

    // вложенные контейнеры
    //TODO: преобразование eNull в eJson и eArray при необходимости
    // @complex_key - список индексов/ключей: {1, "k1", 2} -> el[1]["k1"][2]
    Config&         get_front();                                                                                            API_CONTAINER
    Config          get_front()                                                                 const;                      API_CONTAINER

    Config&         get_at(const size_t& index);                                                                            API_CONTAINER
    Config          get_at(const size_t& index)                                                  const;                     API_CONTAINER
    Config&         get_at(const std::string& key);                                                                         API_MAP_CONTAINER
    Config          get_at(const std::string& key)                                              const;                      API_MAP_CONTAINER
    Config&         get_at(const std::vector<OnlySizetOrString>& complex_key);                                              API_CONTAINER
    Config          get_at(const std::vector<OnlySizetOrString>& complex_key)                   const;                      API_CONTAINER
    //фикс для вызова через {}
    Config&         get_at(const std::initializer_list<OnlySizetOrString>& complex_key)
                    { return get_at(std::vector<OnlySizetOrString>(complex_key)); }                                         API_CONTAINER
    Config          get_at(const std::initializer_list<OnlySizetOrString>& complex_key)         const
                    { return get_at(std::vector<OnlySizetOrString>(complex_key)); }                                         API_CONTAINER

    Config&         get_back();                                                                                             API_MAP_CONTAINER
    Config          get_back()                                                                  const;                      API_MAP_CONTAINER

    bool&           get_front_bool();                                                                                       API_CONTAINER
    bool            get_front_bool()                                                            const;                      API_CONTAINER
    long double&    get_front_number();                                                                                     API_CONTAINER
    long double     get_front_number()                                                          const;                      API_CONTAINER
    uint8_t         get_front_UInt8()                                                           const;                      API_CONTAINER
    uint16_t        get_front_UInt16()                                                          const;                      API_CONTAINER
    uint32_t        get_front_UInt32()                                                          const;                      API_CONTAINER
    uint64_t        get_front_UInt64()                                                          const;                      API_CONTAINER
    int8_t          get_front_Int8()                                                            const;                      API_CONTAINER
    int16_t         get_front_Int16()                                                           const;                      API_CONTAINER
    int32_t         get_front_Int32()                                                           const;                      API_CONTAINER
    int64_t         get_front_Int64()                                                           const;                      API_CONTAINER
    float           get_front_Float()                                                           const;                      API_CONTAINER
    double          get_front_Double()                                                          const;                      API_CONTAINER
    long double     get_front_LDouble()                                                         const;                      API_CONTAINER
    std::string&    get_front_string();                                                                                     API_CONTAINER
    std::string     get_front_string()                                                          const;                      API_CONTAINER

    bool&           get_bool_at(const size_t& index);                                                                       API_CONTAINER
    bool            get_bool_at(const size_t& index)                                             const;                     API_CONTAINER
    bool&           get_bool_at(const std::string& key);                                                                    API_MAP_CONTAINER
    bool            get_bool_at(const std::string& key)                                         const;                      API_MAP_CONTAINER
    bool&           get_bool_at(const std::vector<OnlySizetOrString>& complex_key);                                         API_CONTAINER
    bool            get_bool_at(const std::vector<OnlySizetOrString>& complex_key)              const;                      API_CONTAINER

    //фикс для вызова через {}
    bool&           get_bool_at(const std::initializer_list<OnlySizetOrString>& complex_key)
                    { return get_bool_at(std::vector<OnlySizetOrString>(complex_key)); }                                    API_CONTAINER
    bool            get_bool_at(const std::initializer_list<OnlySizetOrString>& complex_key)    const
                    { return get_bool_at(std::vector<OnlySizetOrString>(complex_key)); }                                    API_CONTAINER

    long double&    get_number_at(const size_t& index);                                                                     API_CONTAINER
    long double     get_number_at(const size_t& index)                                          const;                      API_CONTAINER
    uint8_t         get_UInt8_at(const size_t& index)                                           const;                      API_CONTAINER
    uint16_t        get_UInt16_at(const size_t& index)                                          const;                      API_CONTAINER
    uint32_t        get_UInt32_at(const size_t& index)                                          const;                      API_CONTAINER
    uint64_t        get_UInt64_at(const size_t& index)                                          const;                      API_CONTAINER
    int8_t          get_Int8_at(const size_t& index)                                            const;                      API_CONTAINER
    int16_t         get_Int16_at(const size_t& index)                                           const;                      API_CONTAINER
    int32_t         get_Int32_at(const size_t& index)                                           const;                      API_CONTAINER
    int64_t         get_Int64_at(const size_t& index)                                           const;                      API_CONTAINER
    float           get_Float_at(const size_t& index)                                           const;                      API_CONTAINER
    double          get_Double_at(const size_t& index)                                          const;                      API_CONTAINER
    long double     get_LDouble_at(const size_t& index)                                         const;                      API_CONTAINER

    long double&    get_number_at(const std::string& key);                                                                  API_MAP_CONTAINER
    long double     get_number_at(const std::string& key)                                       const;                      API_MAP_CONTAINER
    uint8_t         get_UInt8_at(const std::string& key)                                        const;                      API_MAP_CONTAINER
    uint16_t        get_UInt16_at(const std::string& key)                                       const;                      API_MAP_CONTAINER
    uint32_t        get_UInt32_at(const std::string& key)                                       const;                      API_MAP_CONTAINER
    uint64_t        get_UInt64_at(const std::string& key)                                       const;                      API_MAP_CONTAINER
    int8_t          get_Int8_at(const std::string& key)                                         const;                      API_MAP_CONTAINER
    int16_t         get_Int16_at(const std::string& key)                                        const;                      API_MAP_CONTAINER
    int32_t         get_Int32_at(const std::string& key)                                        const;                      API_MAP_CONTAINER
    int64_t         get_Int64_at(const std::string& key)                                        const;                      API_MAP_CONTAINER
    float           get_Float_at(const std::string& key)                                        const;                      API_MAP_CONTAINER
    double          get_Double_at(const std::string& key)                                       const;                      API_MAP_CONTAINER
    long double     get_LDouble_at(const std::string& key)                                      const;                      API_MAP_CONTAINER

    long double&    get_number_at(const std::vector<OnlySizetOrString>& complex_key);                                       API_CONTAINER
    long double     get_number_at(const std::vector<OnlySizetOrString>& complex_key)            const;                      API_CONTAINER
    uint8_t         get_UInt8_at(const std::vector<OnlySizetOrString>& complex_key)             const;                      API_CONTAINER
    uint16_t        get_UInt16_at(const std::vector<OnlySizetOrString>& complex_key)            const;                      API_CONTAINER
    uint32_t        get_UInt32_at(const std::vector<OnlySizetOrString>& complex_key)            const;                      API_CONTAINER
    uint64_t        get_UInt64_at(const std::vector<OnlySizetOrString>& complex_key)            const;                      API_CONTAINER
    int8_t          get_Int8_at(const std::vector<OnlySizetOrString>& complex_key)              const;                      API_CONTAINER
    int16_t         get_Int16_at(const std::vector<OnlySizetOrString>& complex_key)             const;                      API_CONTAINER
    int32_t         get_Int32_at(const std::vector<OnlySizetOrString>& complex_key)             const;                      API_CONTAINER
    int64_t         get_Int64_at(const std::vector<OnlySizetOrString>& complex_key)             const;                      API_CONTAINER
    float           get_Float_at(const std::vector<OnlySizetOrString>& complex_key)             const;                      API_CONTAINER
    double          get_Double_at(const std::vector<OnlySizetOrString>& complex_key)            const;                      API_CONTAINER
    long double     get_LDouble_at(const std::vector<OnlySizetOrString>& complex_key)           const;                      API_CONTAINER

    //фикс для вызова через {}
    long double&    get_number_at(const std::initializer_list<OnlySizetOrString>& complex_key);                             API_CONTAINER
    long double     get_number_at(const std::initializer_list<OnlySizetOrString>& complex_key)  const;                      API_CONTAINER
    uint8_t         get_UInt8_at(const std::initializer_list<OnlySizetOrString>& complex_key)   const;                      API_CONTAINER
    uint16_t        get_UInt16_at(const std::initializer_list<OnlySizetOrString>& complex_key)  const;                      API_CONTAINER
    uint32_t        get_UInt32_at(const std::initializer_list<OnlySizetOrString>& complex_key)  const;                      API_CONTAINER
    uint64_t        get_UInt64_at(const std::initializer_list<OnlySizetOrString>& complex_key)  const;                      API_CONTAINER
    int8_t          get_Int8_at(const std::initializer_list<OnlySizetOrString>& complex_key)    const;                      API_CONTAINER
    int16_t         get_Int16_at(const std::initializer_list<OnlySizetOrString>& complex_key)   const;                      API_CONTAINER
    int32_t         get_Int32_at(const std::initializer_list<OnlySizetOrString>& complex_key)   const;                      API_CONTAINER
    int64_t         get_Int64_at(const std::initializer_list<OnlySizetOrString>& complex_key)   const;                      API_CONTAINER
    float           get_Float_at(const std::initializer_list<OnlySizetOrString>& complex_key)   const;                      API_CONTAINER
    double          get_Double_at(const std::initializer_list<OnlySizetOrString>& complex_key)  const;                      API_CONTAINER
    long double     get_LDouble_at(const std::initializer_list<OnlySizetOrString>& complex_key) const;                      API_CONTAINER

    std::string&    get_string_at(const size_t& index);                                                                     API_CONTAINER
    std::string     get_string_at(const size_t& index)                                          const;                      API_CONTAINER
    std::string&    get_string_at(const std::string& key);                                                                  API_MAP_CONTAINER
    std::string     get_string_at(const std::string& key)                                       const;                      API_MAP_CONTAINER
    std::string&    get_string_at(const std::vector<OnlySizetOrString>& complex_key);                                       API_CONTAINER
    std::string     get_string_at(const std::vector<OnlySizetOrString>& complex_key)            const;                      API_CONTAINER

    //фикс для вызова через {}
    std::string&    get_string_at(const std::initializer_list<OnlySizetOrString>& complex_key);                             API_CONTAINER
    std::string     get_string_at(const std::initializer_list<OnlySizetOrString>& complex_key)  const;                      API_CONTAINER

    bool&           get_bool_back();                                                                                        API_CONTAINER
    bool            get_bool_back()                                                             const;                      API_CONTAINER
    long double&    get_number_back();                                                                                      API_CONTAINER
    long double     get_number_back()                                                           const;                      API_CONTAINER
    uint8_t         get_UInt8_back()                                                            const;                      API_CONTAINER
    uint16_t        get_UInt16_back()                                                           const;                      API_CONTAINER
    uint32_t        get_UInt32_back()                                                           const;                      API_CONTAINER
    uint64_t        get_UInt64_back()                                                           const;                      API_CONTAINER
    int8_t          get_Int8_back()                                                             const;                      API_CONTAINER
    int16_t         get_Int16_back()                                                            const;                      API_CONTAINER
    int32_t         get_Int32_back()                                                            const;                      API_CONTAINER
    int64_t         get_Int64_back()                                                            const;                      API_CONTAINER
    float           get_Float_back()                                                            const;                      API_CONTAINER
    double          get_Double_back()                                                           const;                      API_CONTAINER
    long double     get_LDouble_back()                                                          const;                      API_CONTAINER
    std::string&    get_string_back();                                                                                      API_CONTAINER
    std::string     get_string_back()                                                           const;                      API_CONTAINER
    // ========================================================================================================= Getters

    // Modify ==========================================================================================================
    //NOTE: здесь просто сброс комментариев, сброс значения до:
        // bool    = false
        // number  = 0
        // string  = ""
        // array   = []
        // json    = {}
        // yaml    =
        // xml     =
    Config&         clear()                                     noexcept    { m_value->clear(); return *this; }             API_ALL
    //очистка контейнера, главные комментарии и CommentDesign не сбрасывается
    Config&         clearContainer();                                                                                       API_ALL
    // ========================================================================================================== Modify

    // Adding ==========================================================================================================
private:
    // если элемент был не null, шаг будет пропущен
    inline void try_convert_null_to_json()                      noexcept;
    inline void try_convert_null_to_json_array()                noexcept;
public:
    Config& insert_front(const Config& other);                                                                              API_CONTAINER
    Config& insert_front(Config&& other);                                                                                   API_CONTAINER
    Config& insert_front(const std::string& key, const Config& other);                                                      API_MAP_CONTAINER
    Config& insert_front(const std::string& key, Config&& other);                                                           API_MAP_CONTAINER

    Config& insert_at(const size_t& index, const Config& other);                                                            API_CONTAINER
    Config& insert_at(const size_t& index, Config&& other);                                                                 API_CONTAINER
    Config& insert_at(const std::string& key, const Config& other);                                                         API_MAP_CONTAINER
    Config& insert_at(const std::string& key, Config&& other);                                                              API_MAP_CONTAINER
    Config& insert_at(const size_t& index, const std::string& key, const Config& other);                                    API_MAP_CONTAINER
    Config& insert_at(const size_t& index, const std::string& key, Config&& other);                                         API_MAP_CONTAINER

    Config& insert_at(const shared_VElement::iterator iterator, const Config& other);                                       API_INDEX_ARRAY
    Config& insert_at(const shared_VElement::iterator iterator, Config&& other);                                            API_INDEX_ARRAY

    Config& insert_at(const shared_VPairElement::iterator iterator, const std::string& key, const Config& other);           API_MAP_CONTAINER
    Config& insert_at(const shared_VPairElement::iterator iterator, const std::string& key, Config&& other);                API_MAP_CONTAINER

    Config& insert_back(const Config& other);                                                                               API_CONTAINER
    Config& insert_back(Config&& other);                                                                                    API_CONTAINER
    Config& insert_back(const std::string& key, const Config& other);                                                       API_MAP_CONTAINER
    Config& insert_back(const std::string& key, Config&& other);                                                            API_MAP_CONTAINER

    //если путь не существовал - будут созданы все необходимые array и json, чтобы путь был достижим
    //если указанный путь уже занят, то элемент по пути должен быть преобразован в array
    Config& insert_back_force(const VString& keys, const Config& other) noexcept;                                           API_ALL
    Config& insert_back_force(const VString& keys, Config&& other)      noexcept;                                           API_ALL

    Config& insert_before(const std::string& before_key, const std::string& key, const Config& other);                      API_MAP_CONTAINER
    Config& insert_before(const std::string& before_key, const std::string& key, Config&& other);                           API_MAP_CONTAINER
    Config& insert_after(const std::string& after_key, const std::string& key, const Config& other);                        API_MAP_CONTAINER
    Config& insert_after(const std::string& after_key, const std::string& key, Config&& other);                             API_MAP_CONTAINER

    Config& push_front(const Config& other)                             { return insert_front(other); }                     API_CONTAINER
    Config& push_front(Config&& other)                                  { return insert_front(std::move(other)); }          API_CONTAINER
    Config& push_front(const std::string& key, const Config& other)     { return insert_front(key, other); }                API_MAP_CONTAINER
    Config& push_front(const std::string& key, Config&& other)          { return insert_front(key, std::move(other)); }     API_MAP_CONTAINER

    Config& push_at(const size_t& index, const Config& other)           { return insert_at(index, other); }                 API_CONTAINER
    Config& push_at(const size_t& index, Config&& other)                { return insert_at(index, std::move(other)); }      API_CONTAINER
    Config& push_at(const std::string& key, const Config& other)        { return insert_at(key, other); }                   API_MAP_CONTAINER
    Config& push_at(const std::string& key, Config&& other)             { return insert_at(key, std::move(other)); }        API_MAP_CONTAINER

    Config& push_back(const Config& other)                              { return insert_back(other); }                      API_CONTAINER
    Config& push_back(Config&& other)                                   { return insert_back(std::move(other)); }           API_CONTAINER
    Config& push_back(const std::string& key, const Config& other)      { return insert_back(key, other); }                 API_MAP_CONTAINER
    Config& push_back(const std::string& key, Config&& other)           { return insert_back(key, std::move(other)); }      API_MAP_CONTAINER

    Config& push_back_force(const VString& keys, const Config& other)   noexcept { return insert_back_force(keys, other); }             API_ALL
    Config& push_back_force(const VString& keys, Config&& other)        noexcept { return insert_back_force(keys, std::move(other)); }  API_ALL

    Config& push_before(const std::string& before_key, const std::string& key,
                        const Config& other)                            { return insert_before(before_key, key, other); }               API_MAP_CONTAINER
    Config& push_before(const std::string& before_key, const std::string& key,
                        Config&& other)                                 { return insert_before(before_key, key, std::move(other)); }    API_MAP_CONTAINER
    Config& push_after(const std::string& after_key, const std::string& key,
                       const Config& other)                             { return insert_after(after_key, key, other); }                 API_MAP_CONTAINER
    Config& push_after(const std::string& after_key, const std::string& key,
                       Config&& other)                                  { return insert_after(after_key, key, std::move(other)); }      API_MAP_CONTAINER

    //обход explicit
            __ONLY_ALLOWED_TYPES__(T)
    Config& push_front(const T& other)                                  { return insert_front(Config(other)); }                 API_CONTAINER
            __ONLY_ALLOWED_TYPES__(T)
    Config& push_front(T&& other)                                       { return insert_front(std::move(Config(other))); }      API_CONTAINER
            __ONLY_ALLOWED_TYPES__(T)
    Config& push_front(const std::string& key, const T& other)          { return insert_front(key, Config(other)); }            API_MAP_CONTAINER
            __ONLY_ALLOWED_TYPES__(T)
    Config& push_front(const std::string& key, T&& other)               { return insert_front(key, std::move(Config(other))); } API_MAP_CONTAINER
            __ONLY_ALLOWED_TYPES__(T)
    Config& push_at(const size_t& index, const T& other)                 { return insert_at(index, Config(other)); }            API_CONTAINER
            __ONLY_ALLOWED_TYPES__(T)
    Config& push_at(const size_t& index, T&& other)                      { return insert_at(index, std::move(Config(other))); } API_CONTAINER
            __ONLY_ALLOWED_TYPES__(T)
    Config& push_at(const std::string& key, const T& other)             { return insert_at(key, Config(other)); }               API_MAP_CONTAINER
            __ONLY_ALLOWED_TYPES__(T)
    Config& push_at(const std::string& key, T&& other)                  { return insert_at(key, std::move(Config(other))); }    API_MAP_CONTAINER
            __ONLY_ALLOWED_TYPES__(T)
    Config& push_back(const T& other)                                   { return insert_back(Config(other)); }                  API_CONTAINER
            __ONLY_ALLOWED_TYPES__(T)
    Config& push_back(T&& other)                                        { return insert_back(std::move(Config(other))); }       API_CONTAINER
            __ONLY_ALLOWED_TYPES__(T)
    Config& push_back(const std::string& key, const T& other)           { return insert_back(key, Config(other)); }             API_MAP_CONTAINER
            __ONLY_ALLOWED_TYPES__(T)
    Config& push_back(const std::string& key, T&& other)                { return insert_back(key, std::move(Config(other))); }  API_MAP_CONTAINER

            __ONLY_ALLOWED_TYPES__(T)
    Config& push_after(const std::string& after_key, const std::string& key,
                       const T& other)                                  { return insert_after(after_key, key, Config(other)); }             API_MAP_CONTAINER
            __ONLY_ALLOWED_TYPES__(T)
    Config& push_after(const std::string& after_key, const std::string& key,
                       T&& other)                                       { return insert_after(after_key, key, std::move(Config(other))); }  API_MAP_CONTAINER

    //добавить существующий список к другому списку (только при совместимости списков)
    Config& append(const Config& config);                                                                                       API_CONTAINER
    Config& append(Config&& config);                                                                                            API_CONTAINER
    // ========================================================================================================== Adding

    // Removing ========================================================================================================
    Config& erase_front();                                                                                                      API_CONTAINER
    Config& erase_at(const size_t& index);                                                                                      API_CONTAINER
    Config& erase_at(const std::string& key);                                                                                   API_MAP_CONTAINER
    Config& erase_back();                                                                                                       API_CONTAINER

    Config& erase_at(const shared_VElement::iterator iterator);                                                                 API_INDEX_ARRAY
    Config& erase_at(const shared_VPairElement::iterator iterator);                                                             API_MAP_CONTAINER

    Config& pop_front()                                                 { return erase_front(); }                               API_CONTAINER
    Config& pop_at(const std::string& key)                              { return erase_at(key); }                               API_MAP_CONTAINER
    Config& pop_at(const size_t& index)                                 { return erase_at(index); }                             API_CONTAINER
    Config& pop_back()                                                  { return erase_back(); }                                API_CONTAINER

    Config& pop_at(const shared_VElement::iterator iterator)            { return erase_at(iterator); }                          API_INDEX_ARRAY
    Config& pop_at(const shared_VPairElement::iterator iterator)        { return erase_at(iterator); }                          API_MAP_CONTAINER

    Config  get_and_pop_front();                                                                                                API_CONTAINER
    Config  get_and_pop_at(const size_t& index);                                                                                API_CONTAINER
    Config  get_and_pop_at(const std::string& key);                                                                             API_MAP_CONTAINER
    Config  get_and_pop_back();                                                                                                 API_CONTAINER
    // ======================================================================================================== Removing

    // Info ============================================================================================================
    ValueType       getType()                               const noexcept
                    { return m_value != nullptr ? m_value->getType() : ValueType::eNull; }                                      API_ALL
                    // @TEST(ELEMENT, create_empty)
    bool            isNull()                                const noexcept          { return getType() == ValueType::eNull; }   API_ALL
                    // @TEST(ELEMENT, create_bool)
    bool            isBool()                                const noexcept          { return getType() == ValueType::eBool; }   API_ALL
                    // @TEST(ELEMENT, create_number)
    bool            isNumber()                              const noexcept          { return getType() == ValueType::eNumber; } API_ALL
                    // @TEST(ELEMENT, create_string)
    bool            isString()                              const noexcept          { return getType() == ValueType::eString; } API_ALL
                    // @TEST(ELEMENT, create_array)
    bool            isArray()                               const noexcept          { return getType() == ValueType::eArray; }  API_ALL
                    // @TEST(ELEMENT, create_json)
    bool            isJson()                                const noexcept          { return getType() == ValueType::eJson; }   API_ALL
//    bool            isYaml()                                const noexcept          { return getType() == ValueType::eYaml; }
//    bool            isXml()                                 const noexcept          { return getType() == ValueType::eXml; }
    bool            isContainer()                           const noexcept          { return m_value->isContainer(); }          API_ALL
    bool            isIndexContainer()                      const noexcept          { return m_value->isIndexContainer(); }     API_ALL
    bool            isMapContainer()                        const noexcept          { return m_value->isMapContainer(); }       API_ALL

    bool            isEqual(const Config& other, const bool compare_comments = false,
                            const bool map_sort_important = false) const noexcept   { return isEqual(*other.m_value, compare_comments, map_sort_important); }
    bool            isEqual(const tools::IElement& other, const bool compare_comments = false,
                            const bool map_sort_important = false) const noexcept;                                              API_ALL
    bool            isEqual(const bool other)               const noexcept;                                                     API_ALL
    bool            isEqual(std::nullptr_t)                 const noexcept          { return isNull(); }                        API_ALL
    bool            isEqual(const long double& other)       const noexcept;                                                     API_ALL
    bool            isEqual(const std::string& other)       const noexcept;                                                     API_ALL

    //WIKI: для контейнеров вернёт количество элементов
    //WIKI: для строк вернёт количество символов
    //WIKI: для null вернёт 0
    //WIKI: для всех остальных значений вернёт 1
    size_t          size()                                  const noexcept          { return m_value->size(); }                 API_ALL
    bool            isEmpty()                               const noexcept          { return size() == 0; }                     API_CONTAINER
    bool            containsValue(const Config& config)     const noexcept;                                                     API_CONTAINER
                    __ONLY_ALLOWED_TYPES_WITHOUT_CONFIG__(T)
    bool            containsValue(const T& other)
                    { return containsValue(Config(other)); }                                                                    API_CONTAINER
    bool            containsKey(const std::string& key)     const noexcept;                                                     API_MAP_CONTAINER
    // ============================================================================================================ Info

    // Operators =======================================================================================================
    Config&         operator=(const Config& other)          noexcept                { return setValue(other); }                 API_ALL
    Config&         operator=(Config&& other)               noexcept                { return setValue(std::move(other)); }      API_ALL
    Config&         operator=(const tools::IElement& other) noexcept                { return setValue(other); }                 API_ALL
    Config&         operator=(tools::IElement&& other)      noexcept                { return setValue(std::move(other)); }      API_ALL
    Config&         operator=(std::nullptr_t)               noexcept                { return setValue(); }                      API_ALL
    Config&         operator=(const bool other)             noexcept                { return setValue(other); }                 API_ALL
                    __ONLY_NUMBER_TYPES__(T)
    Config&         operator=(const T& other)               noexcept                { return setValue(static_cast<const long double&>(other)); }    API_ALL
    Config&         operator=(long double&& other)          noexcept                { return setValue(std::move(other)); }      API_ALL
                    __ONLY_STRING_TYPES__(T)
    Config&         operator=(const T& other)               noexcept                { return setValue(other); }                 API_ALL
                    __ONLY_STRING_TYPES__(T)
    Config&         operator=(T&& other)                    noexcept                { return setValue(std::move(other)); }      API_ALL

    /* WARNING: комментарии при сравнении не учитываются!
     * Учитывание комментариев только при вызове isEqual(<object>, true)
     */
    bool            operator==(const Config& other)         const                   { return isEqual(other); }                  API_ALL
    bool            operator==(const tools::IElement& other)const                   { return isEqual(other); }                  API_ALL
    bool            operator==(const bool other)            const                   { return isEqual(other); }                  API_ALL
    bool            operator==(std::nullptr_t)            const                     { return isNull(); }                        API_ALL
                    __ONLY_NUMBER_TYPES__(T)
    bool            operator==(const T& other)              const                   { return isEqual(static_cast<const long double&>(other)); }     API_ALL
                    __ONLY_STRING_TYPES__(T)
    bool            operator==(const T& other)              const                   { return isEqual(std::string(other)); }     API_ALL

    bool            operator!=(const Config& other)         const                   { return !isEqual(other); }                 API_ALL
    bool            operator!=(const tools::IElement& other)const                   { return !isEqual(other); }                 API_ALL
    bool            operator!=(const bool other)            const                   { return !isEqual(other); }                 API_ALL
    bool            operator!=(std::nullptr_t)            const                     { return !isNull(); }                       API_ALL
                    __ONLY_NUMBER_TYPES__(T)
    bool            operator!=(const T& other)              const                   { return !isEqual(static_cast<const long double&>(other)); }    API_ALL
    bool            operator!=(const std::string& other)    const                   { return !isEqual(other); }                 API_ALL

    //числа, контейнеры(размер), строки(длина в видимых символах)
    bool            operator>(const Config& other)          const                   { return size() > other.size(); }           API_ALL
    bool            operator>=(const Config& other)         const                   { return size() >= other.size(); }          API_ALL
    bool            operator<(const Config& other)          const                   { return size() < other.size(); }           API_ALL
    bool            operator<=(const Config& other)         const                   { return size() <= other.size(); }          API_ALL

    //контейнеры
    //TODO: преобразование eNull в eJson и eArray при необходимости
    Config&         operator[](const size_t& index)                                 { return get_at(index); }                   API_CONTAINER
    Config          operator[](const size_t& index)          const                  { return get_at(index); }                   API_CONTAINER
    Config&         operator[](const std::string& key)                              { return get_at(key); }                     API_CONTAINER
    Config          operator[](const std::string& key)      const                   { return get_at(key); }                     API_CONTAINER

    Config&         operator[](const std::vector<OnlySizetOrString>& complex_key)           { return get_at(complex_key); }     API_CONTAINER
    Config          operator[](const std::vector<OnlySizetOrString>& complex_key)   const   { return get_at(complex_key); }     API_CONTAINER
    //фикс для вызова через {}
    Config&         operator[](const std::initializer_list<OnlySizetOrString>& complex_key)
                    { return get_at(std::vector<OnlySizetOrString>(complex_key)); }                                             API_CONTAINER
    Config          operator[](const std::initializer_list<OnlySizetOrString>& complex_key)     const
                    { return get_at(std::vector<OnlySizetOrString>(complex_key)); }                                             API_CONTAINER
    // ======================================================================================================= Operators

    // Iterators =======================================================================================================
    // WARNING: Для foreach необходимо использовать обёртки getRange() и getNamedRange()
    // Array
    class Range {
    private:
        Config*         m_config;
        const Config*   m_const_config;
    public:
        explicit Range(Config& config) : m_config(&config), m_const_config(nullptr)             {}
        explicit Range(const Config& config) : m_config(nullptr), m_const_config(&config)       {}

        shared_VElement::iterator       begin()                 { return m_config->array_begin(); }
        shared_VElement::iterator       end()                   { return m_config->array_end(); }
        //в силу приколов C++ константные begin() и end() решают проблему
        shared_VElement::const_iterator begin()         const   { return m_const_config->array_cbegin(); }
        shared_VElement::const_iterator end()           const   { return m_const_config->array_cend(); }
        shared_VElement::const_iterator cbegin()        const   { return m_const_config->array_cbegin(); }
        shared_VElement::const_iterator cend()          const   { return m_const_config->array_cend(); }

        //для корректной работы через STL-методы
        friend shared_VElement::iterator                begin(Range& r)                         { return r.begin(); }
        friend shared_VElement::iterator                end(Range& r)                           { return r.end(); }
        friend shared_VElement::const_iterator          begin(const Range& r)                   { return r.cbegin(); }
        friend shared_VElement::const_iterator          end(const Range& r)                     { return r.cend(); }
        friend shared_VElement::const_iterator          cbegin(const Range& r)                  { return r.cbegin(); }
        friend shared_VElement::const_iterator          cend(const Range& r)                    { return r.cend(); }
    };
    Range getRange()                                            { return Range(*this); }
    const Range getRange()                              const   { return Range(*this); }
    shared_VElement::iterator           array_begin();
    shared_VElement::const_iterator     array_cbegin()                                          const;
    shared_VElement::iterator           array_end();
    shared_VElement::const_iterator     array_cend()                                            const;

    // Json
    class MapRange {
    private:
        Config*         m_config;
        const Config*   m_const_config;
    public:
        explicit MapRange(Config& config) : m_config(&config), m_const_config(nullptr)        {}
        explicit MapRange(const Config& config) : m_config(nullptr), m_const_config(&config)  {}

        shared_VPairElement::iterator       begin()             { return m_config->map_begin(); }
        shared_VPairElement::iterator       end()               { return m_config->map_end(); }
        //в силу приколов C++ константные begin() и end() решают проблему
        shared_VPairElement::const_iterator begin()     const   { return m_const_config->map_cbegin(); }
        shared_VPairElement::const_iterator end()       const   { return m_const_config->map_cend(); }
        shared_VPairElement::const_iterator cbegin()    const   { return m_const_config->map_cbegin(); }
        shared_VPairElement::const_iterator cend()      const   { return m_const_config->map_cend(); }

        //для корректной работы через STL-методы
        friend shared_VPairElement::iterator            begin(MapRange& mr)                     { return mr.begin(); }
        friend shared_VPairElement::iterator            end(MapRange& mr)                       { return mr.end(); }
        friend shared_VPairElement::const_iterator      begin(const MapRange& mr)               { return mr.cbegin(); }
        friend shared_VPairElement::const_iterator      end(const MapRange& mr)                 { return mr.cend(); }
        friend shared_VPairElement::const_iterator      cbegin(const MapRange& mr)              { return mr.cbegin(); }
        friend shared_VPairElement::const_iterator      cend(const MapRange& mr)                { return mr.cend(); }
    };
    MapRange        getNamedRange()                             { return MapRange(*this); }
    const MapRange  getNamedRange()                     const   { return MapRange(*this); }
    shared_VPairElement::iterator       map_begin();
    shared_VPairElement::const_iterator map_cbegin()                                            const;
    shared_VPairElement::iterator       map_end();
    shared_VPairElement::const_iterator map_cend()                                              const;
    // ======================================================================================================= Iterators

    // String ==========================================================================================================
    //вывод без комментариев, "tabulation_level == -1" => запись в одну строку
    std::string     toString(const ConfigFormat format = ConfigFormat::eONLY_VALUE,
                         const CommentDesign &design = {},
                         const int8_t tabulation_level = 0)                                         const noexcept;             API_ALL
    //для совместимости с STL
    friend std::ostream& operator<<(std::ostream& os, const Config& config)                         noexcept;                   API_ALL
    friend std::ostream& operator<<(std::ostream& os, const tools::IElement& config)                noexcept;                   API_ALL
    // ========================================================================================================== String

    // File ============================================================================================================
    //return - получившийся распаршенный корневой элемент, ElementNull если не удалось чтение
    bool            readFile(const std::string& file_path, const ConfigFormat format,
                             const CommentDesign &design = {})                                      noexcept;                   API_ALL
    bool            readFileJson(const std::string& file_path, const CommentDesign &design = {})    noexcept;                   API_ALL
    bool            readFileIni(const std::string& file_path, const CommentDesign &design = {})     noexcept;                   API_ALL

    //return - удалось записать файл или нет
    bool            writeFile(const std::string& file_path, const ConfigFormat format,
                     const CommentDesign &design = {},
                     const int8_t custom_tabulation_level = 0)                                      noexcept;                   API_ALL
    bool            writeFileJson(const std::string& file_path, const CommentDesign &design = {},
                     const int8_t custom_tabulation_level = 0)                                      noexcept;                   API_ALL
    bool            writeFileIni(const std::string& file_path, const CommentDesign &design = {},
                     const int8_t custom_tabulation_level = 0)                                      noexcept;                   API_ALL
    // ============================================================================================================ File

    // Parser ==========================================================================================================
    bool            parse(const std::string& content, const ConfigFormat format,
                          const CommentDesign &design = {})                                         noexcept;                   API_ALL
    bool            parseJson(const std::string& content, const CommentDesign &design = {})         noexcept;                   API_ALL
    bool            parseIni(const std::string& content, const CommentDesign &design = {})          noexcept;                   API_ALL
    bool            parseYaml(const std::string& content, const CommentDesign &design = {})         noexcept;                   API_ALL
    bool            parseXml(const std::string& content, const CommentDesign &design = {})          noexcept;                   API_ALL
    // ========================================================================================================== Parser

private:
    enum class ParseStateJson {
        eJSON_START,
        eJSON_KEY,
        eJSON_KEY_VALUE_SEPARATOR,
        eJSON_VALUE,
        eJSON_SEPARATOR,
        eJSON_FINISH,
        eJSON_ERROR_STATE
    };
    static std::string to_string(const ParseStateJson state)                                        noexcept;
    static void     UpdateState(ParseStateJson& state, const ParseStateJson new_state)              noexcept;
    void            parseFullJsonDoc(std::string&& content)                                         noexcept;

    enum class ParseStateJsonArray {
        eARRAY_START,
        eARRAY_VALUE,
        eARRAY_SEPARATOR,
        eARRAY_FINISH,
        eARRAY_ERROR_STATE
    };
    static std::string to_string(const ParseStateJsonArray state)                                   noexcept;
    static void     UpdateState(ParseStateJsonArray& state, const ParseStateJsonArray new_state)    noexcept;
    void            parseFullJsonArrayDoc(std::string&& content)                                    noexcept;

    static Config&  GetFirstArrayFromThis(Config& config)                                           noexcept;
    static Config&  GetFirstJsonFromThis(Config& config)                                            noexcept;

public:
    // вернёт текст ошибки, указывающий на тип некорректно прочитанного значения
    friend Config CreateElementFromString(std::string &&value_string, const ConfigFormat format,
                                          CommentDesign &design, ParserSymbolCounter& start_iterator)       noexcept;

    friend Config ReadFile(const std::string& file_path, const ConfigFormat format,
                           const CommentDesign &design)                                                     noexcept;
    friend Config ReadFileJson(const std::string& file_path, const CommentDesign &design)                   noexcept;
    friend Config ReadFileIni(const std::string& file_path, const CommentDesign &design)                    noexcept;

    //return - удалось записать файл или нет
    friend bool WriteFile(const Config& config, const std::string& file_path,
                          const ConfigFormat format, const CommentDesign &design,
                          const uint8_t custom_tabulation_level)                                            noexcept;
    friend bool WriteFileJson(const Config& config, const std::string& file_path,
                              const CommentDesign &design, const uint8_t custom_tabulation_level)           noexcept;
    friend bool WriteFileIni(const Config& config, const std::string& file_path,
                             const CommentDesign &design, const uint8_t custom_tabulation_level)            noexcept;

    friend Config Parse(const std::string& content, const ConfigFormat format,
                        const CommentDesign &design)                                                        noexcept;
    friend Config ParseJson(const std::string& content, const CommentDesign &design)                        noexcept;
    friend Config ParseIni(const std::string& content, const CommentDesign &design)                         noexcept;
};

} // namespace simpleapi

//TODO: перейти на pragma once
//TODO: добавить методы для сортировки контейнеров Config
//TODO: ручное включение автоматического кэширования для больших списков ключ-значение
//TODO: алогоритм Дефи-Хэлмана для шифрования сокетных каналов
//TODO: сервер типа "раздатчик новостей", который не проверяет существование удалённого хоста и игнорирует отчёты о доставке
//TODO: работа с JWT (Json Web Token)
