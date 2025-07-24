#ifndef CONFIG_H
#define CONFIG_H

#include "Comment.h"
#include "ConfigCommon.h"
#include "ConfigDefines.h"
#include "ElementNull.h"
#include "ElementBool.h"
#include "ElementNumber.h"
#include "ElementString.h"
#include "ElementArray.h"
#include "ElementJson.h"

class Config {
private:
    IElement* m_value;

public:
    Config()                                                noexcept            { init(); }
    Config(const Config& other)                             noexcept            { setValue(other); }
    Config(Config&& other)                                  noexcept            { setValue(std::move(other)); }
    explicit Config(const IElement& other)                  noexcept            { setValue(other); }
    explicit Config(IElement&& other)                       noexcept            { setValue(std::move(other)); }

    explicit Config(const bool other)                       noexcept            { setValue(other); }
    __ONLY_NUMBER_TYPES__(T)
    explicit Config(T&& other)                              noexcept            { setValue(static_cast<long double&>(other)); }
    __ONLY_STRING_TYPES__(T)
    explicit Config(T&& other)                              noexcept            { setValue(std::string(other)); }

    ~Config()                                               noexcept            { delete m_value; }

private:
    //создание ПУСТОГО(NULL) элемента
    void init()                                             noexcept            { setValue(); }

public:
    //NOTE: API_ - приписка для обозначения интерфейсных функций при использовании через класс Config
    #define API_ALL
    #define API_BOOL
    #define API_NUMBER
    #define API_STRING
    #define API_CONTAINER

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
    void            add_comment(const size_t index, const Comment &content)                 { ...; }                        API_CONTAINER
    void            add_comment(const size_t index, const std::string &content_before,
                             const std::string &content_after)                              { ...; }                        API_CONTAINER
    void            add_prefix_comment(const size_t index, const std::string &content)      { ...; }                        API_CONTAINER
    void            add_suffix_comment(const size_t index, const std::string &content)      { ...; }                        API_CONTAINER
    Comment&        get_comment(const size_t index)                                         { ...; }                        API_CONTAINER
    Comment         get_comment(const size_t index)                 const                   { ...; }                        API_CONTAINER
    std::string&    get_prefix_comment(const size_t index)                                  { ...; }                        API_CONTAINER
    std::string     get_prefix_comment(const size_t index)          const                   { ...; }                        API_CONTAINER
    std::string&    get_suffix_comment(const size_t index)                                  { ...; }                        API_CONTAINER
    std::string     get_suffix_comment(const size_t index)          const                   { ...; }                        API_CONTAINER
    void            clear_comment(const size_t index)                                       { ...; }                        API_CONTAINER
    void            clear_prefix_comment(const size_t index)                                { ...; }                        API_CONTAINER
    void            clear_suffix_comment(const size_t index)                                { ...; }                        API_CONTAINER
    void            delete_comment(const size_t index)                                      { ...; }                        API_CONTAINER
    void            delete_prefix_comment(const size_t index)                               { ...; }                        API_CONTAINER
    void            delete_suffix_comment(const size_t index)                               { ...; }                        API_CONTAINER
    // ========================================================================================================= Comment

    // Setters =========================================================================================================
    // ========================================================================================================= Setters

    // Getters =========================================================================================================
    // ========================================================================================================= Getters

    // Modify ==========================================================================================================
    // ========================================================================================================== Modify

    // Adding ==========================================================================================================
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


    //COMMENTS =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= COMMENTS

    // Setters =========================================================================================================
    Config&         setValue()                              noexcept;
    Config&         setValue(const Config& other)           noexcept;
    Config&         setValue(Config&& other)                noexcept;
    Config&         setValue(const IElement& other)         noexcept;
    Config&         setValue(IElement&& other)              noexcept;
    Config&         setValue(const bool other)              noexcept;
    Config&         setValue(const long double& other)      noexcept;
    Config&         setValue(long double&& other)           noexcept;
    Config&         setValue(const std::string& other)      noexcept;
    Config&         setValue(std::string&& other)           noexcept;

    Config&         setValue(const ElementArray& other)     noexcept;
    Config&         setValue(ElementArray&& other)          noexcept;
    Config&         setValue(const ElementJson& other)      noexcept;
    Config&         setValue(ElementJson&& other)           noexcept;
//TODO:    Config&         setValue(const ElementYaml& other)      noexcept;
//TODO:    Config&         setValue(ElementYaml&& other)           noexcept;
//TODO:    Config&         setValue(const ElementXml& other)       noexcept;
//TODO:    Config&         setValue(ElementXml&& other)            noexcept;
    // ========================================================================================================= Setters

    // Getters =========================================================================================================
    bool&           getBool();
    bool            getBool()                                                   const;
    long double&    getNumber();
    long double     getNumber()                                                 const;
    std::string&    getString();
    std::string     getString()                                                 const;

    // вложенные контейнеры
    // @complex_key - список индексов(в текстовом виде)/ключей
    Config&         get_front();
    Config          get_front()                                                 const;
    Config&         get_at(const size_t index);
    Config          get_at(const size_t index)                                  const;
    Config&         get_at(const std::vector<size_t>& indexes);
    Config          get_at(const std::vector<size_t>& indexes)                  const;
    //TODO: сделать кастомный класс для хранения строк и чисел для обращения ко вложенным элементам
//TODO:                    template<std::size_t SIZE>
//TODO:    Config&         get_at(const std::array<size_t, SIZE>& indexes);
//TODO:                    template<std::size_t SIZE>
//TODO:    Config          get_at(const std::array<size_t, SIZE>& indexes)             const;
    Config&         get_at(const std::string& key);
    Config          get_at(const std::string& key)                              const;
    Config&         get_at(const VString& complex_key);
    Config          get_at(const VString& complex_key)         const;
    Config&         get_back();
    Config          get_back()                                                  const;

    bool&           get_front_bool();
    bool            get_front_bool()                                            const;
    long double&    get_front_number();
    long double     get_front_number()                                          const;
    std::string&    get_front_string();
    std::string     get_front_string()                                          const;

    bool&           get_bool_at(const size_t index);
    bool            get_bool_at(const size_t index)                             const;
    bool&           get_bool_at(const std::vector<size_t>& indexes);
    bool            get_bool_at(const std::vector<size_t>& indexes)             const;
//TODO:                    template<std::size_t SIZE>
//TODO:    bool&           get_bool_at(const std::array<size_t, SIZE>& indexes);
//TODO:                    template<std::size_t SIZE>
//TODO:    bool            get_bool_at(const std::array<size_t, SIZE>& indexes)             const;
    long double&    get_number_at(const size_t index);
    long double     get_number_at(const size_t index)                           const;
    long double&    get_number_at(const std::vector<size_t>& indexes);
    long double     get_number_at(const std::vector<size_t>& indexes)           const;
//TODO:                    template<std::size_t SIZE>
//TODO:    long double&    get_number_at(const std::array<size_t, SIZE>& indexes);
//TODO:                    template<std::size_t SIZE>
//TODO:    long double     get_number_at(const std::array<size_t, SIZE>& indexes)             const;
    std::string&    get_string_at(const size_t index);
    std::string     get_string_at(const size_t index)                           const;
    std::string&    get_string_at(const std::vector<size_t>& indexes);
    std::string     get_string_at(const std::vector<size_t>& indexes)           const;
//TODO:                    template<std::size_t SIZE>
//TODO:    std::string&    get_string_at(const std::array<size_t, SIZE>& indexes);
//TODO:                    template<std::size_t SIZE>
//TODO:    std::string     get_string_at(const std::array<size_t, SIZE>& indexes)             const;

    bool&           get_bool_at(const std::string& key);
    bool            get_bool_at(const std::string& key)                         const;
    bool&           get_bool_at(const VString& complex_key);
    bool            get_bool_at(const VString& complex_key)    const;
    long double&    get_number_at(const std::string& key);
    long double     get_number_at(const std::string& key)                       const;
    long double&    get_number_at(const VString& complex_key);
    long double     get_number_at(const VString& complex_key)  const;
    std::string&    get_string_at(const std::string& key);
    std::string     get_string_at(const std::string& key)                       const;
    std::string&    get_string_at(const VString& complex_key);
    std::string     get_string_at(const VString& complex_key)  const;

    bool&           get_bool_back();
    bool            get_bool_back()                                             const;
    long double&    get_number_back();
    long double     get_number_back()                                           const;
    std::string&    get_string_back();
    std::string     get_string_back()                                           const;
    // ========================================================================================================= Getters

    // Modify ==========================================================================================================
    //NOTE: не путать с remove(), здесь просто сброс комментариев, сброс значения до:
        // bool    = false
        // number  = 0
        // string  = ""
        // array   = {}
        // json    = {}
        // yaml    = false
        // xml     = false
    Config&         clear()                                 noexcept                { m_value->clear(); return *this; }

    //контейнеры
    //TODO: erase_front
    //TODO: erase_at(iterator)
    //TODO: erase_at(index)
    //TODO: erase_at(key)
    //TODO: erase_backf

    //TODO: insert_front
    //TODO: insert_at(iterator)
    //TODO: insert_at(index)
    //TODO: insert_at(key)
    //TODO: insert_back
    //TODO: insert_after(key)

    //TODO: update
    //TODO: get_and_pop
    //TODO: pop
    //TODO: push
    // ========================================================================================================== Modify

    // Info ============================================================================================================
    ValueType       getType()                               const noexcept          { return m_value->getType(); }
    bool            isNull()                                const noexcept          { return getType() == ValueType::eNull; }
    bool            isBool()                                const noexcept          { return getType() == ValueType::eBool; }
    bool            isNumber()                              const noexcept          { return getType() == ValueType::eNumber; }
    bool            isString()                              const noexcept          { return getType() == ValueType::eString; }
    bool            isArray()                               const noexcept          { return getType() == ValueType::eArray; }
    bool            isJson()                                const noexcept          { return getType() == ValueType::eJson; }
//    bool            isYaml()                                const noexcept          { return getType() == ValueType::eYaml; }
//    bool            isXml()                                 const noexcept          { return getType() == ValueType::eXml; }
    bool            isContainer()                           const noexcept          { return m_value->isContainer(); }
    bool            isMapContainer()                        const noexcept          { return m_value->isMapContainer(); }

    bool            isEqual(const Config& other, const bool compare_comments = false)   const
                                                                                    { return isEqual(*other.m_value, compare_comments); }
    bool            isEqual(const IElement& other, const bool compare_comments = false) const;
    bool            isEqual(const bool other)                                           const;
    bool            isEqual(const long double& other)                                   const;
    bool            isEqual(const std::string& other)                                   const;

    size_t          size()                                  const noexcept          { return m_value->size(); }
    // ============================================================================================================ Info

    // Operators =======================================================================================================
    Config&         operator=(const Config& other)          noexcept                { return setValue(other); }
    Config&         operator=(Config&& other)               noexcept                { return setValue(std::move(other)); }
    Config&         operator=(const IElement& other)        noexcept                { return setValue(other); }
    Config&         operator=(IElement&& other)             noexcept                { return setValue(std::move(other)); }
    Config&         operator=(const bool other)             noexcept                { return setValue(other); }
    Config&         operator=(const long double& other)     noexcept                { return setValue(other); }
    Config&         operator=(long double&& other)          noexcept                { return setValue(std::move(other)); }
    Config&         operator=(const std::string& other)     noexcept                { return setValue(other); }
    Config&         operator=(std::string&& other)          noexcept                { return setValue(std::move(other)); }

    /* WARNING: комментарии при сравнении не учитываются!
     * Учитывание комментариев только при вызове isEqual(<object>, true)
     */
    bool            operator==(const Config& other)         const                   { return isEqual(other); }
    bool            operator==(const IElement& other)       const                   { return isEqual(other); }
    bool            operator==(const bool other)            const                   { return isEqual(other); }
    bool            operator==(const long double& other)    const                   { return isEqual(other); }
    bool            operator==(const std::string& other)    const                   { return isEqual(other); }

    bool            operator!=(const Config& other)         const                   { return !isEqual(other); }
    bool            operator!=(const IElement& other)       const                   { return !isEqual(other); }
    bool            operator!=(const bool other)            const                   { return !isEqual(other); }
    bool            operator!=(const long double& other)    const                   { return !isEqual(other); }
    bool            operator!=(const std::string& other)    const                   { return !isEqual(other); }

    //числа, контейнеры(размер), строки(длина в видимых символах)
    bool            operator>(const Config& other)          const                   { return size() > other.size(); }
    bool            operator>=(const Config& other)         const                   { return size() >= other.size(); }
    bool            operator<(const Config& other)          const                   { return size() < other.size(); }
    bool            operator<=(const Config& other)         const                   { return size() <= other.size(); }

    //контейнеры
    Config&         operator[](const size_t index)                                  { return get_at(index); }       //(ARRAY, JSON)
    Config          operator[](const size_t index)                  const           { return get_at(index); }       //(ARRAY, JSON)
    Config&         operator[](const std::vector<size_t>& indexes)                  { return get_at(indexes); }     //(ARRAY, JSON)
    Config          operator[](const std::vector<size_t>& indexes)  const           { return get_at(indexes); }     //(ARRAY, JSON)
    Config&         operator[](const std::string& key)                              { return get_at(key); }         //(JSON)
    Config          operator[](const std::string& key)              const           { return get_at(key); }         //(JSON)
    Config&         operator[](const VString& complex_key)                          { return get_at(complex_key); } //(ARRAY(при условии, что внутри числа), JSON)
    Config          operator[](const VString& complex_key)          const           { return get_at(complex_key); } //(ARRAY(при условии, что внутри числа), JSON)
    // ======================================================================================================= Operators

    // String ==========================================================================================================
    //вывод без комментариев, "tabulation_level == -1" => запись в одну строку
    std::string     toString(const ConfigFormat format = ConfigFormat::eONLY_VALUE,
                        const int8_t tabulation_level = 0,
                        const CommentDesign &design = {})           const noexcept  { return m_value->toString(format, tabulation_level, design); }
    friend std::ostream& operator<<(std::ostream& os, const Config& config)         noexcept;
    friend std::ostream& operator<<(std::ostream& os, const IElement& config)       noexcept;
    // ========================================================================================================== String

    // File ============================================================================================================
    //return - получившийся распаршенный корневой элемент, ElementNull если не удалось чтение
    Config&         readFile(const std::string& file_path, const ConfigFormat format,
                        const bool with_comments = false, std::string* error_log = nullptr);
    Config&         readFileJson(const std::string& file_path, const bool with_comments = 0,
                        std::string* error_log = nullptr);
    Config&         readFileIni(const std::string& file_path, const bool with_comments = 0,
                        std::string* error_log = nullptr);

    //return - удалось записать файл или нет
    bool            writeFile(const std::string& file_path, const ConfigFormat format,
                        const bool with_comments = 0)                                           noexcept;
    bool            writeFileJson(const std::string& file_path, const bool with_comments = 0)   noexcept;
    bool            writeFileIni(const std::string& file_path, const bool with_comments = 0)    noexcept;
    // ============================================================================================================ File

    // Parser ==========================================================================================================
    Config&         parse(const std::string& content, const ConfigFormat format,
                        const bool with_comments = false, std::string* error_log = nullptr);
    Config&         parseJson(const std::string& content, const bool with_comments = 0,
                        std::string* error_log = nullptr);
    Config&         parseIni(const std::string& content, const bool with_comments = 0,
                        std::string* error_log = nullptr);
    // ========================================================================================================== Parser

    // Iterators =======================================================================================================
    // (!) Для foreach итераторы необходимо кастить вручную к нужному типу; за основу брать типы ниже
    //Array
    shared_VElement::iterator           array_begin();
    shared_VElement::const_iterator     array_cbegin()                                          const;
    shared_VElement::iterator           array_end();
    shared_VElement::const_iterator     array_cend()                                            const;
    //Json
    shared_VPairElement::iterator       json_begin();
    shared_VPairElement::const_iterator json_cbegin()                                           const;
    shared_VPairElement::iterator       json_end();
    shared_VPairElement::const_iterator json_cend()                                             const;
    // ======================================================================================================= Iterators

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
Config  ParseJson(const std::string& content, const bool with_comments = 0,
            std::string* error_log = nullptr);
Config  ParseIni(const std::string& content, const bool with_comments = 0,
            std::string* error_log = nullptr);
//STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC



#endif //CONFIG_H
