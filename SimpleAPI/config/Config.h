#ifndef CONFIG_H
#define CONFIG_H

#include "ConfigCommon.h"
#include "ConfigDefines.h"
#include "IElement.h"


//класс-обёртка для нормализованного(?) управления всеми элементами системы конфигурации
class Config {
private:
    std::unique_ptr<IElement> m_element;

protected:
    Config()                        noexcept    { init(); }
    Config(const Config& value)     noexcept    { setValue(value); }
    Config(Config&& value)          noexcept    { setValue(value); }
    Config(const IElement& value)   noexcept    { setValue(value); }
    Config(IElement&& value)        noexcept    { setValue(value); }

    Config(const bool value)        noexcept    { setValue(value); }
    __ONLY_NUMBER_TYPES__(T)
    Config(T&& value)               noexcept    { setValue(static_cast<long double&>(value)); }
    __ONLY_STRING_TYPES__(T)
    Config(T&& value)               noexcept    { setValue(std::string(value)); }
    Config(ElementArray&& value)    noexcept    { setValue(value); }
    Config(ElementJson&& value)     noexcept    { setValue(value); }

    ~Config()                       noexcept    {}

private:
    //создание ПУСТОГО(NULL) элемента
    void init()                     noexcept    { setValue(); }

public:
    // Operators =======================================================================================================
    Config& operator=(const Config& other)  noexcept;
    Config& operator=(Config&& other)       noexcept;

    bool operator==(const Config& other)    const;
    bool operator==(const IElement& other)  const noexcept  { return other == *m_element; }
    bool operator!=(const Config& other)    const noexcept  { return *other.m_element != *m_element; }
    bool operator!=(const IElement& other)  const noexcept  { return other != *m_element; }

    //числа, контейнеры(размер)
    bool operator>(const Config& other)     const noexcept;
    bool operator>(const IElement& other)   const noexcept;
    bool operator>=(const Config& other)    const noexcept;
    bool operator>=(const IElement& other)  const noexcept;
    bool operator<(const Config& other)     const noexcept;
    bool operator<(const IElement& other)   const noexcept;
    bool operator<=(const Config& other)    const noexcept;
    bool operator<=(const IElement& other)  const noexcept;

    //контейнеры
    Config& operator<<(const Config& other)     noexcept;       //аналог push_back()
    Config& operator<<(const IElement& other)   noexcept;       //аналог push_back()
    Config  operator>>(const Config& other)     noexcept;       //аналог pop_front()    ??
    Config  operator>>(const IElement& other)   noexcept;       //аналог pop_front()    ??
    Config& operator[](const size_t index)      noexcept;       //(ARRAY, JSON)
    Config  operator[](const size_t index)      const noexcept; //(ARRAY, JSON)
    Config& operator[](const std::string key)   noexcept;       //(JSON)
    Config  operator[](const std::string key)   const noexcept; //(JSON)
    // ======================================================================================================= Operators

    // Info ============================================================================================================
    ValueType       getType()       const noexcept  { return m_element->getType(); }
    bool            isNull()        const noexcept  { return getType() == ValueType::eNull; }
    bool            isBool()        const noexcept  { return getType() == ValueType::eBool; }
    bool            isNumber()      const noexcept  { return getType() == ValueType::eNumber; }
    bool            isString()      const noexcept  { return getType() == ValueType::eString; }
    bool            isArray()       const noexcept  { return getType() == ValueType::eArray; }
    bool            isJson()        const noexcept  { return getType() == ValueType::eJson; }
//TODO: bool isYaml() const noexcept
//TODO: bool isXml() const noexcept
    bool            isContainer()   const noexcept;

    size_t          size()          const noexcept;
    bool            isEqual(const Config& other, const bool compare_comments = false) const noexcept;
    // ============================================================================================================ Info

    // Getters =========================================================================================================
    bool&           getBool();
    bool            getBool()                       const;
    long double&    getNumber();
    long double     getNumber()                     const;
    std::string&    getString();
    std::string     getString()                     const;
    ElementArray&   getArray();
    ElementArray    getArray()                      const;
    ElementJson&    getJson();
    ElementJson     getJson()                       const;

    // вложенные контейнеры
    Config&         get_front(const size_t index);
    Config&         get_front(const size_t index)   const;
    Config&         get_at(const size_t index);
    Config&         get_at(const size_t index)      const;
    Config&         get_back(const size_t index);
    Config&         get_back(const size_t index)    const;
    // ========================================================================================================= Getters

    // Setters =========================================================================================================
    void setValue()                                 noexcept;
    void setValue(const Config& value)              noexcept;
    void setValue(const IElement& value)            noexcept;
    void setValue(const bool value)                 noexcept;
    void setValue(const long double& value)         noexcept;
    void setValue(const std::string& value)         noexcept;
    void setValue(const ElementArray& value)        noexcept;
    void setValue(const ElementJson& value)         noexcept;
    // ========================================================================================================= Setters

    // Comments ========================================================================================================
    void addComment(const Comment& content)             noexcept    { addComment(content); }
    void addComment(const std::string &content_before, const std::string &content_after) noexcept
                                                                    { addComment(content_before, content_after); }
    void addPrefixComment(const std::string& content)   noexcept    { addPrefixComment(content); }
    void addSuffixComment(const std::string& content)   noexcept    { addSuffixComment(content); }

    Comment&        getComment()        noexcept                    { return getComment(); }
    Comment         getComment()        const noexcept              { return getComment(); }
    std::string&    getPrefixComment()  noexcept                    { return getPrefixComment(); }
    std::string     getPrefixComment()  const noexcept              { return getPrefixComment(); }
    std::string&    getSuffixComment()  noexcept                    { return getSuffixComment(); }
    std::string     getSuffixComment()  const noexcept              { return getSuffixComment(); }

    void clearComment()                 noexcept                    { clearComment(); }
    void clearPrefixComment()           noexcept                    { clearPrefixComment(); }
    void clearSuffixComment()           noexcept                    { clearSuffixComment(); }
    void deleteComment()                noexcept                    { deleteComment(); }
    void deletePrefixComment()          noexcept                    { deletePrefixComment(); }
    void deleteSuffixComment()          noexcept                    { deleteSuffixComment(); }

    CommentDesign&  getCommentDesign()  noexcept                    { return getCommentDesign(); }
    CommentDesign   getCommentDesign()  const noexcept              { return getCommentDesign(); }
    void setCommentDesign(const CommentDesign& design)  noexcept    { setCommentDesign(design); }
    void clearCommentDesign()           noexcept                    { clearCommentDesign(); }
    // ======================================================================================================== Comments

    // String ==========================================================================================================
    std::string toString(const ConfigFormat format, const CommentDesign &design,
                         const int8_t tabulation_level = 0) const noexcept;
    //вывод без комментариев, "tabulation_level == -1" => запись в одну строку
    std::string toString(const ConfigFormat format = ConfigFormat::eJSON, const int8_t tabulation_level = 0) const noexcept;
    // ========================================================================================================== String

    // File ============================================================================================================
    //return - получившийся распаршенный корневой элемент, ElementNull если не удалось чтение
    Config& readFile(const std::string& file_path, const ConfigFormat format,
                     const bool with_comments = false, std::string* error_log = nullptr);
    Config& readFileJson(const std::string& file_path, const bool with_comments = 0,
                         std::string* error_log = nullptr);
    Config& readFileIni(const std::string& file_path, const bool with_comments = 0,
                        std::string* error_log = nullptr);

    //return - удалось записать файл или нет
    bool writeFile(const std::string& file_path, const ConfigFormat format,
                   const bool with_comments = 0) noexcept;
    bool writeFileJson(const std::string& file_path, const bool with_comments = 0)  noexcept;
    bool writeFileIni(const std::string& file_path, const bool with_comments = 0)   noexcept;
    // ============================================================================================================ File

    // Parser ==========================================================================================================
    Config& parse(const std::string& content, const ConfigFormat format,
                  const bool with_comments = false, std::string* error_log = nullptr);
    Config& parseJson(const std::string& content, const bool with_comments = 0,
                      std::string* error_log = nullptr);
    Config& parseIni(const std::string& content, const bool with_comments = 0,
                     std::string* error_log = nullptr);
    // ========================================================================================================== Parser

    // Iterators =======================================================================================================
    // (!) Для foreach итераторы необходимо кастить вручную к нужному типу; за основу брать типы ниже
    /* (!) */ void*                 begin()         noexcept;
    /* (!) */ void*                 end()           noexcept;
    /* (!) */ void*                 cbegin()        const noexcept;
    /* (!) */ void*                 cend()          const noexcept;
    // (!) Для foreach итераторы необходимо кастить вручную к нужному типу; за основу брать типы ниже
    //Array
    VElement::iterator              array_begin()   noexcept;
    VElement::iterator              array_end()     noexcept;
    VElement::const_iterator        array_cbegin()  const noexcept;
    VElement::const_iterator        array_cend()    const noexcept;
    //Json
    VPairElement::iterator          json_begin()    noexcept;
    VPairElement::iterator          json_end()      noexcept;
    VPairElement::const_iterator    json_cbegin()   const noexcept;
    VPairElement::const_iterator    json_cend()     const noexcept;
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
                  const ConfigFormat format, const bool with_comments = 0)  noexcept;
bool    WriteFileJson(const Config& config, const std::string& file_path,
                      const bool with_comments = 0)                         noexcept;
bool    WriteFileIni(const Config& config, const std::string& file_path,
                     const bool with_comments = 0)                          noexcept;

Config  Parse(const std::string& content, const ConfigFormat format,
              const bool with_comments = false, std::string* error_log = nullptr);
Config  ParseJson(const std::string& content, const bool with_comments = 0,
                  std::string* error_log = nullptr);
Config  ParseIni(const std::string& content, const bool with_comments = 0,
                 std::string* error_log = nullptr);
//STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC-STATIC

#endif // CONFIG_H
