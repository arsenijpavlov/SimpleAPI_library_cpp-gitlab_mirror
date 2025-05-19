#ifndef CONFIG_H
#define CONFIG_H

#include "Comment.h"
#include "ConfigCommon.h"
#include "ConfigDefines.h"
#include "IElement.h"
#include "ElementArray.h"
#include "ElementJson.h"

class Config {
private:
    IElement* m_value;

public:
    Config()                                                noexcept            { init(); }
    Config(const Config& value)                             noexcept            { setValue(value); }
    Config(Config&& value)                                  noexcept            { setValue(std::move(value)); }

    Config(const bool value)                                noexcept            { setValue(value); }
    __ONLY_NUMBER_TYPES__(T)
    Config(T&& value)                                       noexcept            { setValue(static_cast<long double&>(value)); }
    __ONLY_STRING_TYPES__(T)
    Config(T&& value)                                       noexcept            { setValue(std::string(value)); }
    //TODO: подумать, как исключить прямое указание типа при вызове конструктора
    Config(const ElementArray& value)                       noexcept            { setValue(value); }
    Config(ElementArray&& value)                            noexcept            { setValue(value); }
    Config(const ElementJson& value)                        noexcept            { setValue(value); }
    Config(ElementJson&& value)                             noexcept            { setValue(value); }

    ~Config()                                               noexcept            { delete m_value; }

private:
    //создание ПУСТОГО(NULL) элемента
    void init()                                             noexcept            { setValue(); }

public:
    // Setters =========================================================================================================
    Config&         setValue()                              noexcept;
    Config&         setValue(const Config& value)           noexcept;
    Config&         setValue(Config&& value)                noexcept;
    Config&         setValue(const bool value)              noexcept;
    Config&         setValue(const long double& value)      noexcept;
    Config&         setValue(long double&& value)           noexcept;
    Config&         setValue(const std::string& value)      noexcept;
    Config&         setValue(std::string&& value)           noexcept;
    Config&         setValue(const ElementArray& value)     noexcept;
    Config&         setValue(ElementArray&& value)          noexcept;
    Config&         setValue(const ElementJson& value)      noexcept;
    Config&         setValue(ElementJson&& value)           noexcept;
    // ========================================================================================================= Setters

    // Getters =========================================================================================================
    bool&           getBool();
    bool            getBool()                               const;
    long double&    getNumber();
    long double     getNumber()                             const;
    std::string&    getString();
    std::string     getString()                             const;

    // вложенные контейнеры
    Config&         get_front();
    Config          get_front()                             const;
    Config&         get_at(const size_t index);
    Config          get_at(const size_t index)              const;
    //TODO: получение значения по списку индексов/ключей
//    Config&         get_at(const std::vector<size_t>& indexes)                  {}
//    Config          get_at(const std::vector<size_t>& indexes)  const           {}
    Config&         get_at(const std::string& key);
    Config          get_at(const std::string& key)          const;
    //TODO: получение значения по списку индексов/ключей
//    Config&         get_at(const std::vector<std::string>& indexes)             {}
//    Config          get_at(const std::vector<std::string>& indexes)  const      {}
    Config&         get_back();
    Config          get_back()                              const;

    bool&           get_front_bool();
    bool            get_front_bool()                        const;
    long double&    get_front_number();
    long double     get_front_number()                      const;
    std::string&    get_front_string();
    std::string     get_front_string()                      const;

    //TODO: получение значения по списку индексов/ключей
    bool&           get_bool_at(const size_t index);
    bool            get_bool_at(const size_t index)         const;
    long double&    get_number_at(const size_t index);
    long double     get_number_at(const size_t index)       const;
    std::string&    get_string_at(const size_t index);
    std::string     get_string_at(const size_t index)       const;

    //TODO: получение значения по списку индексов/ключей
    bool&           get_bool_at(const std::string& key);
    bool            get_bool_at(const std::string& key)     const;
    long double&    get_number_at(const std::string& key);
    long double     get_number_at(const std::string& key)   const;
    std::string&    get_string_at(const std::string& key);
    std::string     get_string_at(const std::string& key)   const;

    bool&           get_back_bool();
    bool            get_back_bool()                         const;
    long double&    get_back_number();
    long double     get_back_number()                       const;
    std::string&    get_back_string();
    std::string     get_back_string()                       const;
    // ========================================================================================================= Getters

    // Info ============================================================================================================
    ValueType       getType()                               const noexcept      { return m_value->getType(); }
    bool            isNull()                                const noexcept      { return getType() == ValueType::eNull; }
    bool            isBool()                                const noexcept      { return getType() == ValueType::eBool; }
    bool            isNumber()                              const noexcept      { return getType() == ValueType::eNumber; }
    bool            isString()                              const noexcept      { return getType() == ValueType::eString; }
    bool            isArray()                               const noexcept      { return getType() == ValueType::eArray; }
    bool            isJson()                                const noexcept      { return getType() == ValueType::eJson; }
//    bool            isYaml()                                const noexcept      { return getType() == ValueType::eYaml; }
//    bool            isXml()                                 const noexcept      { return getType() == ValueType::eXml; }
    bool            isContainer()                           const noexcept      { return m_value->isContainer(); }

    bool            isEqual(const Config& other, const bool compare_comments = false)
                                                            const noexcept;
//    bool            isEqual(const bool other)               const noexcept;
//    bool            isEqual(const long double& other)       const noexcept;
//    bool            isEqual(const std::string& other)       const noexcept;

    size_t          size()                                  const noexcept      { return m_value->size(); }
    // ============================================================================================================ Info

    // Operators =======================================================================================================
    Config&         operator=(const Config& other)          noexcept;
    Config&         operator=(Config&& other)               noexcept;
//    Config&         operator=(const bool other)             noexcept;
//    Config&         operator=(const long double& other)     noexcept;
//    Config&         operator=(long double&& other)          noexcept;
//    Config&         operator=(const std::string& other)     noexcept;
//    Config&         operator=(std::string&& other)          noexcept;

    //WARNING: комментарии не учитываются! Учитывание комментариев только при вызове isEqual()
    bool            operator==(const Config& other)         const noexcept      { return isEqual(other); }
//    bool            operator==(const bool other)            const noexcept      { return isEqual(other); }
//    bool            operator==(const long double& other)    const noexcept      { return isEqual(other); }
//    bool            operator==(const std::string& other)    const noexcept      { return isEqual(other); }

    bool            operator!=(const Config& other)         const noexcept      { return !isEqual(other); }
//    bool            operator!=(const bool other)            const noexcept      { return !isEqual(other); }
//    bool            operator!=(const long double& other)    const noexcept      { return !isEqual(other); }
//    bool            operator!=(const std::string& other)    const noexcept      { return !isEqual(other); }

    //числа, контейнеры(размер), строки(длина в видимых символах)
    bool            operator>(const Config& other)          const noexcept      { return m_value > other.m_value; }
    bool            operator>=(const Config& other)         const noexcept      { return m_value >= other.m_value; }
    bool            operator<(const Config& other)          const noexcept      { return m_value < other.m_value; }
    bool            operator<=(const Config& other)         const noexcept      { return m_value <= other.m_value; }

    //контейнеры
    //FIXME: на подумать: https://en.cppreference.com/w/cpp/language/operators
    //Config&         operator<<(const IElement& other)       noexcept;       //аналог push_back()
    //Config&         operator<<(const IElement& other)       noexcept;       //аналог push_back()
    //Config          operator>>()                            noexcept;       //аналог pop_front()
    //TODO: получение значения по списку индексов/ключей
    Config&         operator[](const size_t index)          noexcept            { return get_at(index); }   //(ARRAY, JSON)
    Config          operator[](const size_t index)          const noexcept      { return get_at(index); }   //(ARRAY, JSON)
    Config&         operator[](const std::string& key)      noexcept            { return get_at(key); }     //(JSON)
    Config          operator[](const std::string& key)      const noexcept      { return get_at(key); }     //(JSON)
    // ======================================================================================================= Operators

    //COMMENTS =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    Config&         addComment(const Comment& content)              noexcept;
    Config&         addComment(const std::string &content_before, const std::string &content_after) noexcept;
    Config&         addPrefixComment(const std::string& content)    noexcept;
    Config&         addSuffixComment(const std::string& content)    noexcept;

    Comment&        getComment()                                    noexcept        { return m_value->getComment(); }
    Comment         getComment()                                    const noexcept  { return m_value->getComment(); }
    std::string&    getPrefixComment()                              noexcept        { return m_value->getPrefixComment(); }
    std::string     getPrefixComment()                              const noexcept  { return m_value->getPrefixComment(); }
    std::string&    getSuffixComment()                              noexcept        { return m_value->getSuffixComment(); }
    std::string     getSuffixComment()                              const noexcept  { return m_value->getSuffixComment(); }

    Config&         clearComment()                                  noexcept;
    Config&         clearPrefixComment()                            noexcept;
    Config&         clearSuffixComment()                            noexcept;
    Config&         deleteComment()                                 noexcept;
    Config&         deletePrefixComment()                           noexcept;
    Config&         deleteSuffixComment()                           noexcept;

    CommentDesign&  getCommentDesign()                              noexcept        { return m_value->getCommentDesign(); }
    CommentDesign   getCommentDesign()                              const noexcept  { return m_value->getCommentDesign(); }
    Config&         setCommentDesign(const CommentDesign& design)   noexcept;
    Config&         clearCommentDesign()                            noexcept;
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= COMMENTS

    // String ==========================================================================================================
    //вывод без комментариев, "tabulation_level == -1" => запись в одну строку
    std::string     toString(const ConfigFormat format = ConfigFormat::eONLY_VALUE,
                        const int8_t tabulation_level = 0,
                        const CommentDesign &design = {})           const noexcept  { return m_value->toString(format, tabulation_level, design); }
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
    /* (!) */ void*                 begin();
    /* (!) */ void*                 cbegin()                const;
    /* (!) */ void*                 end();
    /* (!) */ void*                 cend()                  const;
    // (!) Для foreach итераторы необходимо кастить вручную к нужному типу; за основу брать типы ниже
    //Array
    VElement::iterator              array_begin();
    VElement::const_iterator        array_cbegin()          const;
    VElement::iterator              array_end();
    VElement::const_iterator        array_cend()            const;
    //Json
    VPairElement::iterator          json_begin();
    VPairElement::const_iterator    json_cbegin()           const;
    VPairElement::iterator          json_end();
    VPairElement::const_iterator    json_cend()             const;
    // ======================================================================================================= Iterators
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
