#ifndef CONFIG_H
#define CONFIG_H

#include "Comment.h"
#include "ConfigCommon.h"
#include "ConfigDefines.h"
#include "ElementArray.h"
#include "ElementBool.h"
#include "ElementJson.h"
#include "ElementNumber.h"
#include "ElementString.h"

template<typename T>
struct is_valid_config_type {
    static constexpr bool value =
        std::is_same<       typename std::decay<T>::type, ElementJson>::value ||
        std::is_same<       typename std::decay<T>::type, ElementArray>::value ||
        std::is_convertible<typename std::decay<T>::type, std::string>::value ||
        std::is_arithmetic< typename std::decay<T>::type>::value ||
        std::is_same<       typename std::decay<T>::type, bool>::value ||
        std::is_same<       typename std::decay<T>::type, Config>::value
//                                  || std::is_same<       typename std::decay<T>::type, const char*>::value
        ;
};
template<bool...> struct bool_pack;
template<bool... bs>
struct all_true : std::is_same<bool_pack<bs..., true>, bool_pack<true, bs...>> {};

#define __ONLY_ALLOWED_TYPES_VARIADIC__(ARG) \
    template<typename ... ARG, \
             typename std::enable_if< \
                all_true<is_valid_config_type<ARG>::value...>::value, int \
             >::type* = nullptr>


class Config {
private:
    IElement* m_value;

public:
    Config()                                noexcept : m_value(nullptr)         { init(); }
    Config(const Config& other)             noexcept : m_value(nullptr)         { setValue(other); }
    Config(Config&& other)                  noexcept : m_value(nullptr)         { setValue(std::move(other)); }
    explicit Config(const IElement& other)  noexcept : m_value(nullptr)         { setValue(other); }
    explicit Config(IElement&& other)       noexcept : m_value(nullptr)         { setValue(std::move(other)); }

    explicit Config(const bool other)       noexcept : m_value(nullptr)         { setValue(other); }
    __ONLY_NUMBER_TYPES__(T)
    explicit Config(T&& other)              noexcept : m_value(nullptr)         { setValue(static_cast<long double>(other)); }

    // NOTE: с explicit Config(const char*) не работает
    __ONLY_STRING_TYPES__(T)
    /*explicit*/ Config(const T& other)         noexcept : m_value(nullptr)         { setValue(std::string(other)); }
    __ONLY_STRING_TYPES__(T)
    /*explicit*/ Config(T&& other)              noexcept : m_value(nullptr)         { setValue(std::string(std::move(other))); }

    explicit Config(const ValueType config_type) : m_value(nullptr) {
        release();
        switch(config_type) {
        default:
        case ValueType::eNull: {
            init();
            break;
        }
        case ValueType::eBool: {
            m_value = dynamic_cast<IElement*>(new ElementBool());
            break;
        }
        case ValueType::eNumber: {
            m_value = dynamic_cast<IElement*>(new ElementNumber());
            break;
        }
        case ValueType::eString: {
            m_value = dynamic_cast<IElement*>(new ElementString());
            break;
        }
        case ValueType::eArray: {
            m_value = dynamic_cast<IElement*>(new ElementArray());
            break;
        }
        case ValueType::eJson: {
            m_value = dynamic_cast<IElement*>(new ElementJson());
            break;
        }
        }
    }

    //контейнеры
    __ONLY_ALLOWED_TYPES_VARIADIC__(T)
    explicit Config(const ValueType config_type, const T& ... values) : m_value(nullptr) {
        release();
        switch(config_type) {
        default:
        case ValueType::eNull: {
            init();
            break;
        }
        case ValueType::eBool: {
            m_value = dynamic_cast<IElement*>(new ElementBool());
            break;
        }
        case ValueType::eNumber: {
            m_value = dynamic_cast<IElement*>(new ElementNumber());
            break;
        }
        case ValueType::eString: {
            m_value = dynamic_cast<IElement*>(new ElementString());
            break;
        }
        case ValueType::eArray: {
            m_value = dynamic_cast<IElement*>(new ElementArray());
            (void)std::initializer_list<int>{(push_back(values), 0)...};
            break;
        }
        case ValueType::eJson: {
            m_value = dynamic_cast<IElement*>(new ElementJson());
            if(sizeof...(values) != 0)
                variadicKVInputter(values...);
            break;
        }
        }
    }

    __ONLY_ALLOWED_TYPES_VARIADIC__(T)
    explicit Config(const ValueType config_type, T&& ... values) : m_value(nullptr) {
        release();
        switch(config_type) {
        default:
        case ValueType::eNull: {
            init();
            break;
        }
        case ValueType::eBool: {
            m_value = dynamic_cast<IElement*>(new ElementBool());
            break;
        }
        case ValueType::eNumber: {
            m_value = dynamic_cast<IElement*>(new ElementNumber());
            break;
        }
        case ValueType::eString: {
            m_value = dynamic_cast<IElement*>(new ElementString());
            break;
        }
        case ValueType::eArray: {
            m_value = dynamic_cast<IElement*>(new ElementArray());
            (void)std::initializer_list<int>{(push_back(std::move(values)), 0)...};
            break;
        }
        case ValueType::eJson: {
            m_value = dynamic_cast<IElement*>(new ElementJson());
            if(sizeof...(values) != 0)
                variadicKVInputter(std::move(values)...);
            break;
        }
        }
    }

    __ONLY_ALLOWED_TYPES__(T)
    explicit Config(const std::vector<std::pair<std::string, T>>& pairs_key_config) : m_value(nullptr) {
        m_value = dynamic_cast<IElement*>(new ElementJson());
        for(const auto& pair : pairs_key_config)
            push_at(pair.first, pair.second);
    }
    __ONLY_ALLOWED_TYPES__(T)
    explicit Config(std::vector<std::pair<std::string, T>>&& pairs_key_config) : m_value(nullptr) {
        m_value = dynamic_cast<IElement*>(new ElementJson());
        for(auto& pair : pairs_key_config)
            push_at(std::move(pair.first), std::move(pair.second));
    }


    ~Config()                                                       noexcept    { release(); }

private:
    //создание ПУСТОГО(NULL) элемента
    void init()                                                     noexcept    { setValue(); }
    void release()                                                  noexcept;

    __ONLY_ALLOWED_TYPES_VARIADIC__(T)
    void variadicKVInputter(const std::string& key, const Config& config, const T& ... others) {
        static_assert(sizeof...(others) % 2 == 0, "Even number of arguments required");
        push_at(key, config);
        if(sizeof...(others) == 0) return;
        variadicKVInputter(others...);
    }
    __ONLY_ALLOWED_TYPES_VARIADIC__(T)
    void variadicKVInputter(const std::string& key, Config&& config, T&& ... others) {
        static_assert(sizeof...(others) % 2 == 0, "Even number of arguments required");
        push_at(key, std::move(config));
        if(sizeof...(others) == 0) return;
        variadicKVInputter(std::move(others)...);
    }
    static void variadicKVInputter() {} //NOTE: нужна только для "пустых" вызовов variadic-функции

    // WARNING: в идеале, методы ниже никогда не должны быть использованы
    __ONLY_ALLOWED_TYPES__(T)
    static void variadicKVInputter(const T& others)    {} //NOTE: нужна только для компиляции работы с массивами
    __ONLY_ALLOWED_TYPES__(T)
    static void variadicKVInputter(T&& others)         {} //NOTE: нужна только для компиляции работы с массивами

public:
    //NOTE: API_ - приписка для обозначения интерфейсных функций при использовании через класс Config
    #define API_ALL
    #define API_BOOL
    #define API_NUMBER
    #define API_STRING
    #define API_CONTAINER
    #define API_MAP_CONTAINER

    // Comment =========================================================================================================
    Config&         addComment(const Comment& content)              noexcept;                                               API_ALL
    Config&         addComment(const std::string &content_before, const std::string &content_after) noexcept;               API_ALL
    Config&         addPrefixComment(const std::string& content)    noexcept;                                               API_ALL
    Config&         addSuffixComment(const std::string& content)    noexcept;                                               API_ALL

    Comment&        getComment()                                    noexcept        { return m_value->getComment(); }       API_ALL
    Comment         getComment()                                    const noexcept  { return m_value->getComment(); }       API_ALL
    std::string&    getPrefixComment()                              noexcept        { return m_value->getPrefixComment(); } API_ALL
    std::string     getPrefixComment()                              const noexcept  { return m_value->getPrefixComment(); } API_ALL
    std::string&    getSuffixComment()                              noexcept        { return m_value->getSuffixComment(); } API_ALL
    std::string     getSuffixComment()                              const noexcept  { return m_value->getSuffixComment(); } API_ALL

    Config&         clearComment()                                  noexcept;                                               API_ALL
    Config&         clearPrefixComment()                            noexcept;                                               API_ALL
    Config&         clearSuffixComment()                            noexcept;                                               API_ALL
    Config&         deleteComment()                                 noexcept;                                               API_ALL
    Config&         deletePrefixComment()                           noexcept;                                               API_ALL
    Config&         deleteSuffixComment()                           noexcept;                                               API_ALL

    CommentDesign&  getCommentDesign()                              noexcept        { return m_value->getCommentDesign(); } API_ALL
    CommentDesign   getCommentDesign()                              const noexcept  { return m_value->getCommentDesign(); } API_ALL
    Config&         setCommentDesign(const CommentDesign& design)   noexcept;                                               API_ALL
    Config&         clearCommentDesign()                            noexcept;                                               API_ALL

    // вложенные контейнеры
    Config&         add_comment(const size_t index, const Comment &content);                                                API_CONTAINER
    Config&         add_comment(const size_t index, const std::string &content_before,
                     const std::string &content_after);                                                                     API_CONTAINER
    Config&         add_prefix_comment(const size_t index, const std::string &content);                                     API_CONTAINER
    Config&         add_suffix_comment(const size_t index, const std::string &content);                                     API_CONTAINER

    Comment&        get_comment(const size_t index);                                                                        API_CONTAINER
    Comment         get_comment(const size_t index)                 const;                                                  API_CONTAINER
    std::string&    get_prefix_comment(const size_t index);                                                                 API_CONTAINER
    std::string     get_prefix_comment(const size_t index)          const;                                                  API_CONTAINER
    std::string&    get_suffix_comment(const size_t index);                                                                 API_CONTAINER
    std::string     get_suffix_comment(const size_t index)          const;                                                  API_CONTAINER

    Config&         clear_comment(const size_t index);                                                                      API_CONTAINER
    Config&         clear_prefix_comment(const size_t index);                                                               API_CONTAINER
    Config&         clear_suffix_comment(const size_t index);                                                               API_CONTAINER
    Config&         delete_comment(const size_t index);                                                                     API_CONTAINER
    Config&         delete_prefix_comment(const size_t index);                                                              API_CONTAINER
    Config&         delete_suffix_comment(const size_t index);                                                              API_CONTAINER
    // ========================================================================================================= Comment

    // Setters =========================================================================================================
    Config&         setValue()                                      noexcept;                                               API_ALL
    Config&         setValue(const Config& other)                   noexcept;                                               API_ALL
    Config&         setValue(Config&& other)                        noexcept;                                               API_ALL
    Config&         setValue(const IElement& other)                 noexcept;                                               API_ALL
    Config&         setValue(IElement&& other)                      noexcept;                                               API_ALL
    Config&         setValue(const bool other)                      noexcept;                                               API_ALL
    Config&         setValue(const long double& other)              noexcept;                                               API_ALL
    Config&         setValue(long double&& other)                   noexcept;                                               API_ALL
    Config&         setValue(const std::string& other)              noexcept;                                               API_ALL
    Config&         setValue(std::string&& other)                   noexcept;                                               API_ALL

    Config&         setValue(const ElementArray& other)             noexcept;                                               API_ALL
    Config&         setValue(ElementArray&& other)                  noexcept;                                               API_ALL
    Config&         setValue(const ElementJson& other)              noexcept;                                               API_ALL
    Config&         setValue(ElementJson&& other)                   noexcept;                                               API_ALL
    //TODO:    Config&         setValue(const ElementYaml& other)      noexcept;
    //TODO:    Config&         setValue(ElementYaml&& other)           noexcept;
    //TODO:    Config&         setValue(const ElementXml& other)       noexcept;
    //TODO:    Config&         setValue(ElementXml&& other)            noexcept;
    // ========================================================================================================= Setters

    // Getters =========================================================================================================
    bool&           getBool();                                                                                              API_BOOL
    bool            getBool()                                                   const;                                      API_BOOL
    long double&    getNumber();                                                                                            API_NUMBER
    long double     getNumber()                                                 const;                                      API_NUMBER
    std::string&    getString();                                                                                            API_STRING
    std::string     getString()                                                 const;                                      API_STRING

    // вложенные контейнеры
    // @complex_key - список индексов(в текстовом виде)/ключей
    Config&         get_front();                                                                                            API_CONTAINER
    Config          get_front()                                                 const;                                      API_CONTAINER
    Config&         get_at(const size_t index);                                                                             API_CONTAINER
    Config          get_at(const size_t index)                                  const;                                      API_CONTAINER
    Config&         get_at(const std::vector<size_t>& indexes);                                                             API_CONTAINER
    Config          get_at(const std::vector<size_t>& indexes)                  const;                                      API_CONTAINER
    //TODO: сделать кастомный класс для хранения строк и чисел для обращения ко вложенным элементам
    //TODO:                    template<std::size_t SIZE>
    //TODO:    Config&         get_at(const std::array<size_t, SIZE>& indexes);
    //TODO:                    template<std::size_t SIZE>
    //TODO:    Config          get_at(const std::array<size_t, SIZE>& indexes)             const;
    Config&         get_at(const std::string& key);                                                                         API_MAP_CONTAINER
    Config          get_at(const std::string& key)                              const;                                      API_MAP_CONTAINER
    Config&         get_at(const VString& complex_key);                                                                     API_CONTAINER
    Config          get_at(const VString& complex_key)                          const;                                      API_CONTAINER
    Config&         get_back();                                                                                             API_MAP_CONTAINER
    Config          get_back()                                                  const;                                      API_MAP_CONTAINER

    bool&           get_front_bool();                                                                                       API_CONTAINER
    bool            get_front_bool()                                            const;                                      API_CONTAINER
    long double&    get_front_number();                                                                                     API_CONTAINER
    long double     get_front_number()                                          const;                                      API_CONTAINER
    std::string&    get_front_string();                                                                                     API_CONTAINER
    std::string     get_front_string()                                          const;                                      API_CONTAINER

    bool&           get_bool_at(const size_t index);                                                                        API_CONTAINER
    bool            get_bool_at(const size_t index)                             const;                                      API_CONTAINER
    bool&           get_bool_at(const std::vector<size_t>& indexes);                                                        API_CONTAINER
    bool            get_bool_at(const std::vector<size_t>& indexes)             const;                                      API_CONTAINER
    //TODO:                    template<std::size_t SIZE>
    //TODO:    bool&           get_bool_at(const std::array<size_t, SIZE>& indexes);
    //TODO:                    template<std::size_t SIZE>
    //TODO:    bool            get_bool_at(const std::array<size_t, SIZE>& indexes)             const;
    long double&    get_number_at(const size_t index);                                                                      API_CONTAINER
    long double     get_number_at(const size_t index)                           const;                                      API_CONTAINER
    long double&    get_number_at(const std::vector<size_t>& indexes);                                                      API_CONTAINER
    long double     get_number_at(const std::vector<size_t>& indexes)           const;                                      API_CONTAINER
    //TODO:                    template<std::size_t SIZE>
    //TODO:    long double&    get_number_at(const std::array<size_t, SIZE>& indexes);
    //TODO:                    template<std::size_t SIZE>
    //TODO:    long double     get_number_at(const std::array<size_t, SIZE>& indexes)             const;
    std::string&    get_string_at(const size_t index);                                                                      API_CONTAINER
    std::string     get_string_at(const size_t index)                           const;                                      API_CONTAINER
    std::string&    get_string_at(const std::vector<size_t>& indexes);                                                      API_CONTAINER
    std::string     get_string_at(const std::vector<size_t>& indexes)           const;                                      API_CONTAINER
    //TODO:                    template<std::size_t SIZE>
    //TODO:    std::string&    get_string_at(const std::array<size_t, SIZE>& indexes);
    //TODO:                    template<std::size_t SIZE>
    //TODO:    std::string     get_string_at(const std::array<size_t, SIZE>& indexes)             const;

    bool&           get_bool_at(const std::string& key);                                                                    API_MAP_CONTAINER
    bool            get_bool_at(const std::string& key)                         const;                                      API_MAP_CONTAINER
    bool&           get_bool_at(const VString& complex_key);                                                                API_CONTAINER
    bool            get_bool_at(const VString& complex_key)                     const;                                      API_CONTAINER
    long double&    get_number_at(const std::string& key);                                                                  API_MAP_CONTAINER
    long double     get_number_at(const std::string& key)                       const;                                      API_MAP_CONTAINER
    long double&    get_number_at(const VString& complex_key);                                                              API_CONTAINER
    long double     get_number_at(const VString& complex_key)                   const;                                      API_CONTAINER
    std::string&    get_string_at(const std::string& key);                                                                  API_MAP_CONTAINER
    std::string     get_string_at(const std::string& key)                       const;                                      API_MAP_CONTAINER
    std::string&    get_string_at(const VString& complex_key);                                                              API_CONTAINER
    std::string     get_string_at(const VString& complex_key)                   const;                                      API_CONTAINER

    bool&           get_bool_back();                                                                                        API_CONTAINER
    bool            get_bool_back()                                             const;                                      API_CONTAINER
    long double&    get_number_back();                                                                                      API_CONTAINER
    long double     get_number_back()                                           const;                                      API_CONTAINER
    std::string&    get_string_back();                                                                                      API_CONTAINER
    std::string     get_string_back()                                           const;                                      API_CONTAINER
    // ========================================================================================================= Getters

    // Modify ==========================================================================================================
    //NOTE: не путать с remove(), здесь просто сброс комментариев, сброс значения до:
        // bool    = false
        // number  = 0
        // string  = ""
        // array   = []
        // json    = {}
        // yaml    =
        // xml     =
    Config&         clear()                                     noexcept    { m_value->clear(); return *this; }             API_ALL
    // ========================================================================================================== Modify

    // Adding ==========================================================================================================
    //TODO: insert_at(index, key, other)
    Config& insert_front(const Config& other);                                                                              API_CONTAINER
    Config& insert_front(Config&& other);                                                                                   API_CONTAINER
    Config& insert_front(const std::string& key, const Config& other);                                                      API_MAP_CONTAINER
    Config& insert_front(const std::string& key, Config&& other);                                                           API_MAP_CONTAINER
    Config& insert_at(const size_t index, const Config& other);                                                             API_CONTAINER
    Config& insert_at(const size_t index, Config&& other);                                                                  API_CONTAINER
    Config& insert_at(const std::string& key, const Config& other);                                                         API_MAP_CONTAINER
    Config& insert_at(const std::string& key, Config&& other);                                                              API_MAP_CONTAINER
    Config& insert_back(const Config& other);                                                                               API_CONTAINER
    Config& insert_back(Config&& other);                                                                                    API_CONTAINER
    Config& insert_back(const std::string& key, const Config& other);                                                       API_MAP_CONTAINER
    Config& insert_back(const std::string& key, Config&& other);                                                            API_MAP_CONTAINER
    Config& insert_after(const std::string& after_key, const std::string& key, const Config& other);                        API_MAP_CONTAINER
    Config& insert_after(const std::string& after_key, const std::string& key, Config&& other);                             API_MAP_CONTAINER

    //NOTE: в использовании итераторов здесь мало смысла видится
    //NOTE: функции с произвольным количеством аргументов пока не кажутся необходимыми пользователю API

    //TODO: update

    Config& push_front(const Config& other)                             { return insert_front(other); }                     API_CONTAINER
    Config& push_front(Config&& other)                                  { return insert_front(std::move(other)); }          API_CONTAINER
    Config& push_front(const std::string& key, const Config& other)     { return insert_front(key, other); }                API_MAP_CONTAINER
    Config& push_front(const std::string& key, Config&& other)          { return insert_front(key, std::move(other)); }     API_MAP_CONTAINER
    Config& push_at(const size_t index, const Config& other)            { return insert_at(index, other); }                 API_CONTAINER
    Config& push_at(const size_t index, Config&& other)                 { return insert_at(index, std::move(other)); }      API_CONTAINER
    Config& push_at(const std::string& key, const Config& other)        { return insert_at(key, other); }                   API_MAP_CONTAINER
    Config& push_at(const std::string& key, Config&& other)             { return insert_at(key, std::move(other)); }        API_MAP_CONTAINER
    Config& push_back(const Config& other)                              { return insert_back(other); }                      API_CONTAINER
    Config& push_back(Config&& other)                                   { return insert_back(std::move(other)); }           API_CONTAINER
    Config& push_back(const std::string& key, const Config& other)      { return insert_back(key, other); }                 API_MAP_CONTAINER
    Config& push_back(const std::string& key, Config&& other)           { return insert_back(key, std::move(other)); }      API_MAP_CONTAINER

    Config& push_after(const std::string& after_key, const std::string& key,
                       const Config& other)                             { return insert_after(after_key, key, other); }             API_MAP_CONTAINER
    Config& push_after(const std::string& after_key, const std::string& key,
                       Config&& other)                                  { return insert_after(after_key, key, std::move(other)); }  API_MAP_CONTAINER

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
    Config& push_at(const size_t index, const T& other)                 { return insert_at(index, Config(other)); }             API_CONTAINER
    __ONLY_ALLOWED_TYPES__(T)
    Config& push_at(const size_t index, T&& other)                      { return insert_at(index, std::move(Config(other))); }  API_CONTAINER
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
    Config& append(const Config& config);                   API_CONTAINER
    Config& append(Config&& config);                                                                                            API_CONTAINER
    // ========================================================================================================== Adding

    // Removing ========================================================================================================
    Config& erase_front();                                                                                                      API_CONTAINER
    Config& erase_at(const size_t index);                                                                                       API_CONTAINER
    Config& erase_at(const std::string& key);                                                                                   API_MAP_CONTAINER
    Config& erase_back();                                                                                                       API_CONTAINER

    Config& pop_front()                                                 { return erase_front(); }                               API_CONTAINER
    Config& pop_at(const std::string& key)                              { return erase_at(key); }                               API_MAP_CONTAINER
    Config& pop_at(const size_t index)                                  { return erase_at(index); }                             API_CONTAINER
    Config& pop_back()                                                  { return erase_back(); }                                API_CONTAINER

    Config  get_and_pop_front();                                                                                                API_CONTAINER
    Config  get_and_pop_at(const size_t index);                                                                                 API_CONTAINER
    Config  get_and_pop_at(const std::string& key);                                                                             API_MAP_CONTAINER
    Config  get_and_pop_back();                                                                                                 API_CONTAINER
    // ======================================================================================================== Removing

    // Info ============================================================================================================
    ValueType       getType()                               const noexcept          { return m_value->getType(); }              API_ALL
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
    bool            isIndexContainer()                      const noexcept          { return m_value->isIndexContainer(); }          API_ALL
    bool            isMapContainer()                        const noexcept          { return m_value->isMapContainer(); }       API_ALL

    bool            isEqual(const Config& other, const bool compare_comments = false,
                            const bool map_sort_important = false) const noexcept   { return isEqual(*other.m_value, compare_comments, map_sort_important); }
    bool            isEqual(const IElement& other, const bool compare_comments = false,
                            const bool map_sort_important = false) const noexcept;                                              API_ALL
    bool            isEqual(const bool other)                                           const noexcept;                         API_ALL
    bool            isEqual(const long double& other)                                   const noexcept;                         API_ALL
    bool            isEqual(const std::string& other)                                   const noexcept;                         API_ALL

    size_t          size()                                  const noexcept          { return m_value->size(); }                 API_ALL
    bool            isEmpty()                               const noexcept          { return size() == 0; }                     API_CONTAINER
    bool            containsValue(const Config& config)     const noexcept;                                                     API_CONTAINER
    bool            containsKey(const std::string& key)     const noexcept;                                                     API_MAP_CONTAINER

    __ONLY_ALLOWED_TYPES__(T)
    bool            valueContains(const T& other)                                   { return valueContains((Config(other))); }  API_CONTAINER
    // ============================================================================================================ Info

    // Operators =======================================================================================================
    Config&         operator=(const Config& other)          noexcept                { return setValue(other); }                 API_ALL
    Config&         operator=(Config&& other)               noexcept                { return setValue(std::move(other)); }      API_ALL
    Config&         operator=(const IElement& other)        noexcept                { return setValue(other); }                 API_ALL
    Config&         operator=(IElement&& other)             noexcept                { return setValue(std::move(other)); }      API_ALL
    Config&         operator=(const bool other)             noexcept                { return setValue(other); }                 API_ALL
    __ONLY_NUMBER_TYPES__(T)
    Config&         operator=(const T& other)               noexcept                { return setValue(static_cast<const long double&>(other)); }                 API_ALL
    Config&         operator=(long double&& other)          noexcept                { return setValue(std::move(other)); }      API_ALL
    Config&         operator=(const std::string& other)     noexcept                { return setValue(other); }                 API_ALL
    Config&         operator=(std::string&& other)          noexcept                { return setValue(std::move(other)); }      API_ALL

    /* WARNING: комментарии при сравнении не учитываются!
     * Учитывание комментариев только при вызове isEqual(<object>, true)
     */
    bool            operator==(const Config& other)         const                   { return isEqual(other); }                  API_ALL
    bool            operator==(const IElement& other)       const                   { return isEqual(other); }                  API_ALL
    bool            operator==(const bool other)            const                   { return isEqual(other); }                  API_ALL
    __ONLY_NUMBER_TYPES__(T)
    bool            operator==(const T& other)              const                   { return isEqual(static_cast<const long double&>(other)); }                  API_ALL
    __ONLY_STRING_TYPES__(T)
    bool            operator==(const T& other)              const                   { return isEqual(std::string(other)); }                  API_ALL

    bool            operator!=(const Config& other)         const                   { return !isEqual(other); }                 API_ALL
    bool            operator!=(const IElement& other)       const                   { return !isEqual(other); }                 API_ALL
    bool            operator!=(const bool other)            const                   { return !isEqual(other); }                 API_ALL
    __ONLY_NUMBER_TYPES__(T)
    bool            operator!=(const T& other)              const                   { return !isEqual(static_cast<const long double&>(other)); }                 API_ALL
    bool            operator!=(const std::string& other)    const                   { return !isEqual(other); }                 API_ALL

    //числа, контейнеры(размер), строки(длина в видимых символах)
    bool            operator>(const Config& other)          const                   { return size() > other.size(); }           API_ALL
    bool            operator>=(const Config& other)         const                   { return size() >= other.size(); }          API_ALL
    bool            operator<(const Config& other)          const                   { return size() < other.size(); }           API_ALL
    bool            operator<=(const Config& other)         const                   { return size() <= other.size(); }          API_ALL

    //контейнеры
    Config&         operator[](const size_t index)                                  { return get_at(index); }                   API_CONTAINER
    Config          operator[](const size_t index)                  const           { return get_at(index); }                   API_CONTAINER
    Config&         operator[](const std::vector<size_t>& indexes)                  { return get_at(indexes); }                 API_CONTAINER
    Config          operator[](const std::vector<size_t>& indexes)  const           { return get_at(indexes); }                 API_CONTAINER
    Config&         operator[](const std::string& key)                              { return get_at(key); }                     API_CONTAINER
    Config          operator[](const std::string& key)              const           { return get_at(key); }                     API_CONTAINER
    Config&         operator[](const VString& complex_key)                          { return get_at(complex_key); }             API_CONTAINER
    Config          operator[](const VString& complex_key)          const           { return get_at(complex_key); }             API_CONTAINER
    // ======================================================================================================= Operators

    // Iterators =======================================================================================================
    // (!) Для foreach необходимо использовать обёртки getRange() и getNamedRange()
    //Array
    class Range {
    private:
        Config*         m_config;
        const Config*   m_const_config;
    public:
        explicit Range(Config& config) : m_config(&config), m_const_config(nullptr)             {}
        explicit Range(const Config& config) : m_config(nullptr), m_const_config(&config)       {}

        shared_VElement::iterator       begin()                 { return m_config->array_begin(); }
        shared_VElement::iterator       end()                   { return m_config->array_end(); }
        shared_VElement::const_iterator cbegin()        const   { return m_const_config->array_cbegin(); }
        shared_VElement::const_iterator cend()          const   { return m_const_config->array_cend(); }
    };
    Range getRange()                                            { return Range(*this); }
    Range getRange()                                    const   { return Range(*this); }
    shared_VElement::iterator           array_begin();
    shared_VElement::const_iterator     array_cbegin()                                          const;
    shared_VElement::iterator           array_end();
    shared_VElement::const_iterator     array_cend()                                            const;

    //Json
    class NamedRange {
    private:
        Config*         m_config;
        const Config*   m_const_config;
    public:
        explicit NamedRange(Config& config) : m_config(&config), m_const_config(nullptr)        {}
        explicit NamedRange(const Config& config) : m_config(nullptr), m_const_config(&config)  {}

        shared_VPairElement::iterator       begin()             { return m_config->named_begin(); }
        shared_VPairElement::iterator       end()               { return m_config->named_end(); }
        shared_VPairElement::const_iterator cbegin()    const   { return m_const_config->named_cbegin(); }
        shared_VPairElement::const_iterator cend()      const   { return m_const_config->named_cend(); }
    };
    NamedRange getNamedRange()                                  { return NamedRange(*this); }
    NamedRange getNamedRange()                          const   { return NamedRange(*this); }
    shared_VPairElement::iterator       named_begin();
    shared_VPairElement::const_iterator named_cbegin()                                           const;
    shared_VPairElement::iterator       named_end();
    shared_VPairElement::const_iterator named_cend()                                             const;
    // ======================================================================================================= Iterators

    // String ==========================================================================================================
    //вывод без комментариев, "tabulation_level == -1" => запись в одну строку
    std::string     toString(const ConfigFormat format = ConfigFormat::eONLY_VALUE,
                         const int8_t tabulation_level = 0, const CommentDesign &design = {})   const noexcept;                 API_ALL
    friend std::ostream& operator<<(std::ostream& os, const Config& config)                     noexcept;                       API_ALL
    friend std::ostream& operator<<(std::ostream& os, const IElement& config)                   noexcept;                       API_ALL
    // ========================================================================================================== String

    // File ============================================================================================================
    //return - получившийся распаршенный корневой элемент, ElementNull если не удалось чтение
    Config&         readFile(const std::string& file_path, const ConfigFormat format,
                     const bool with_comments = false, std::string* error_log = nullptr);                                       API_ALL
    Config&         readFileJson(const std::string& file_path, const bool with_comments = 0,
                     std::string* error_log = nullptr);                                                                         API_ALL
    Config&         readFileIni(const std::string& file_path, const bool with_comments = 0,
                     std::string* error_log = nullptr);                                                                         API_ALL

    //return - удалось записать файл или нет
    bool            writeFile(const std::string& file_path, const ConfigFormat format,
                     const bool with_comments = 0)                                              noexcept;                       API_ALL
    bool            writeFileJson(const std::string& file_path, const bool with_comments = 0)   noexcept;                       API_ALL
    bool            writeFileIni(const std::string& file_path, const bool with_comments = 0)    noexcept;                       API_ALL
    // ============================================================================================================ File

    // Parser ==========================================================================================================
    Config&         parse(const std::string& content, const ConfigFormat format,
                      const bool with_comments = false, std::string* error_log = nullptr);                                      API_ALL
    Config&         parseArray(const std::string& content, const bool with_comments = 0,
                      const int8_t tabulation_level = 0, std::string* error_log = nullptr);                                     API_ALL
    Config&         parseJson(const std::string& content, const bool with_comments = 0,
                      const int8_t tabulation_level = 0, std::string* error_log = nullptr);                                     API_ALL
    Config&         parseIni(const std::string& content, const bool with_comments = 0,
                      const int8_t tabulation_level = 0, std::string* error_log = nullptr);                                     API_ALL
    // ========================================================================================================== Parser

    //STATIC
    static Config CreateElementFromString(std::string &&value_string, const ConfigFormat format,
                                          const bool enable_comments = true, const CommentDesign& design = {},
                                          const int8_t tabulation_level = 0);
};


//STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC
//return - получившийся распаршенный корневой элемент, ElementNull если не удалось чтение
Config  ReadFile(const std::string& file_path, const ConfigFormat format,
            const bool with_comments = false, std::string* error_log = nullptr);
Config  ReadFileJson(const std::string& file_path, const bool with_comments = 0,
            std::string* error_log = nullptr);
Config  ReadFileIni(const std::string& file_path, const bool with_comments = 0,
            std::string* error_log = nullptr);

//return - удалось записать файл или нет
bool    WriteFile(const Config& config, const std::string& file_path,
            const ConfigFormat format, const bool with_comments = 0)            noexcept;
bool    WriteFileJson(const Config& config, const std::string& file_path,
            const bool with_comments = 0)                                       noexcept;
bool    WriteFileIni(const Config& config, const std::string& file_path,
            const bool with_comments = 0)                                       noexcept;

Config  Parse(const std::string& content, const ConfigFormat format,
            const bool with_comments = false, std::string* error_log = nullptr);
Config  ParseArray(const std::string& content, const bool with_comments = 0,
            const int8_t tabulation_level = 0, std::string* error_log = nullptr);
Config  ParseJson(const std::string& content, const bool with_comments = 0,
            const int8_t tabulation_level = 0, std::string* error_log = nullptr);
Config  ParseIni(const std::string& content, const bool with_comments = 0,
            const int8_t tabulation_level = 0, std::string* error_log = nullptr);
//STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC



#endif //CONFIG_H
