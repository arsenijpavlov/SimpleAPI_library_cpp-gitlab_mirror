#ifndef CONFIG_H
#define CONFIG_H

#include "Comment.h"
#include "ConfigCommon.h"
#include "ConfigDefines.h"
#include "IElement.h"

class Config {
private:
    ValueType   m_type;
    IElement*   m_value;
    //Комментарии существуют ТОЛЬКО на этом уровне абстракции?
    Comment     m_comment;  //FIXME: сейчас никем не используется!

public:
    Config()                                                noexcept            { init(); }
    Config(const Config& value)                             noexcept            { setValue(value); }
    Config(Config&& value)                                  noexcept            { setValue(std::move(value)); }

    Config(const bool value)                                noexcept            { setValue(value); }
    __ONLY_NUMBER_TYPES__(T)
    Config(T&& value)                                       noexcept            { setValue(static_cast<long double&>(value)); }
    __ONLY_STRING_TYPES__(T)
    Config(T&& value)                                       noexcept            { setValue(std::string(value)); }

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
    bool&           getBool()                                                   { return m_value->getBool(); }
    bool            getBool()                               const               { return m_value->getBool(); }
    long double&    getNumber()                                                 { return m_value->getNumber(); }
    long double     getNumber()                             const               { return m_value->getNumber(); }
    std::string&    getString()                                                 { return m_value->getString(); }
    std::string     getString()                             const               { return m_value->getString(); }

    // вложенные контейнеры
    Config&         get_front()                                                 { return m_value->get_front(); }
    Config          get_front()                             const               { return m_value->get_front(); }
    Config&         get_at(const size_t index)                                  { return m_value->get_at(index); }
    Config          get_at(const size_t index)              const               { return m_value->get_at(index); }
    Config&         get_at(const std::string& key)                              { return m_value->get_at(key); }
    Config          get_at(const std::string& key)          const               { return m_value->get_at(key); }
    Config&         get_back()                                                  { return m_value->get_back(); }
    Config          get_back()                              const               { return m_value->get_back(); }
    // ========================================================================================================= Getters

    // Info ============================================================================================================
    ValueType       getType()                               const noexcept      { return m_type; }
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
    Config&         operator[](const size_t index)          noexcept            { return (*m_value)[index]; }   //(ARRAY, JSON)
    Config          operator[](const size_t index)          const noexcept      { return (*m_value)[index]; }   //(ARRAY, JSON)
    Config&         operator[](const std::string& key)      noexcept            { return (*m_value)[key]; }     //(JSON)
    Config          operator[](const std::string& key)      const noexcept      { return (*m_value)[key]; }     //(JSON)
    // ======================================================================================================= Operators

    //COMMENTS =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    Config&         addComment(const Comment& content)      noexcept            { m_comment = content; return *this; }
    Config&         addComment(const std::string &content_before, const std::string &content_after) noexcept
                                                                                { m_comment = Comment(content_before, content_after); return *this; }
    Config&         addPrefixComment(const std::string& content) noexcept       { m_comment.setPrefix(content); return *this; }
    Config&         addSuffixComment(const std::string& content) noexcept       { m_comment.setSuffix(content); return *this; }

    Comment&        getComment()                            noexcept            { return m_comment; }
    Comment         getComment()                            const noexcept      { return m_comment; }
    std::string&    getPrefixComment()                      noexcept            { return m_comment.prefix(); }
    std::string     getPrefixComment()                      const noexcept      { return m_comment.prefix(); }
    std::string&    getSuffixComment()                      noexcept            { return m_comment.suffix(); }
    std::string     getSuffixComment()                      const noexcept      { return m_comment.suffix(); }

    Config&         clearComment()                          noexcept            { m_comment.clear(); return *this; }
    Config&         clearPrefixComment()                    noexcept            { m_comment.clearPrefix(); return *this; }
    Config&         clearSuffixComment()                    noexcept            { m_comment.clearSuffix(); return *this; }
    Config&         deleteComment()                         noexcept            { m_comment.del(); return *this; }
    Config&         deletePrefixComment()                   noexcept            { m_comment.delPrefix(); return *this; }
    Config&         deleteSuffixComment()                   noexcept            { m_comment.delSuffix(); return *this; }

    CommentDesign&  getCommentDesign()                      noexcept            { return m_comment.commentDesign(); }
    CommentDesign   getCommentDesign()                      const noexcept      { return m_comment.commentDesign(); }
    Config&         setCommentDesign(const CommentDesign& design)
                                                            noexcept            { m_comment.setDesign(design); return *this; }
    Config&         clearCommentDesign()                    noexcept            { m_comment.clearDesign(); return *this; }
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= COMMENTS

    // String ==========================================================================================================
    //вывод без комментариев, "tabulation_level == -1" => запись в одну строку
    std::string     toString(const ConfigFormat format = ConfigFormat::eJSON,
                        const int8_t tabulation_level = 0,
                        const CommentDesign &design = {})   const noexcept      { return m_value->toString(format, tabulation_level, design); }
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
    /* (!) */ void*                 begin()                                     { return m_value->begin(); }
    /* (!) */ void*                 end()                                       { return m_value->end(); }
    /* (!) */ void*                 cbegin()                const               { return m_value->cbegin(); }
    /* (!) */ void*                 cend()                  const               { return m_value->cend(); }
    // (!) Для foreach итераторы необходимо кастить вручную к нужному типу; за основу брать типы ниже
    //Array
    VElement::iterator              array_begin()                               { return m_value->array_begin(); }
    VElement::iterator              array_end()                                 { return m_value->array_end(); }
    VElement::const_iterator        array_cbegin()          const               { return m_value->array_cbegin(); }
    VElement::const_iterator        array_cend()            const               { return m_value->array_cend(); }
    //Json
    VPairElement::iterator          json_begin()                                { return m_value->json_begin(); }
    VPairElement::iterator          json_end()                                  { return m_value->json_end(); }
    VPairElement::const_iterator    json_cbegin()           const               { return m_value->json_cbegin(); }
    VPairElement::const_iterator    json_cend()             const               { return m_value->json_cend(); }
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
