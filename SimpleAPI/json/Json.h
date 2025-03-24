#ifndef JSON_H
#define JSON_H

#include "Comment.h"
#include "../utils/Utils.h"
#include "Json_defines.h"
#include <map>
#include <memory>
#include <vector>


// Format ======================================================================================
enum class ConfigFormat {
    eJSON,
    eYAML,
    eINI,
    eXML
};
// ====================================================================================== Format

enum ValueType {
    eNull,
    eNumber,
    eBool,
    eString,
    eJson,
    eArray
};
static std::string to_string(const ValueType type) noexcept;


//абстрактный класс
class Element {
protected:
    ValueType m_type;
//            m_value;

public:
            Element();
    virtual ~Element() noexcept;

    //PARSING -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //return - получившийся распаршенный корневой элемент, NullElement если не удалось чтение
    Element& readFile(const std::string& file_path, const ConfigFormat format) noexcept {
        switch(format) {
        case ConfigFormat::eJSON:   return readFileJson(file_path);
        case ConfigFormat::eYAML:   return readFileYaml(file_path);
        case ConfigFormat::eINI:    return readFileIni(file_path);
        case ConfigFormat::eXML:    return readFileXml(file_path);
        }
    }
    virtual Element&    readFileJson(const std::string& file_path) noexcept     = 0;
    virtual Element&    readFileYaml(const std::string& file_path) noexcept     = 0;
    virtual Element&    readFileIni(const std::string& file_path) noexcept      = 0;
    virtual Element&    readFileXml(const std::string& file_path) noexcept      = 0;
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= PARSING

    //WRITING -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //return - удалось записать файл или нет
    bool writeFile(const std::string& file_path, const ConfigFormat format) noexcept {
        switch(format) {
        case ConfigFormat::eJSON:   return writeFileJson(file_path);
        case ConfigFormat::eYAML:   return writeFileYaml(file_path);
        case ConfigFormat::eINI:    return writeFileIni(file_path);
        case ConfigFormat::eXML:    return writeFileXml(file_path);
        }
    }
    virtual bool        writeFileJson(const std::string& file_path) noexcept    = 0;
    virtual bool        writeFileYaml(const std::string& file_path) noexcept    = 0;
    virtual bool        writeFileIni(const std::string& file_path) noexcept     = 0;
    virtual bool        writeFileXml(const std::string& file_path) noexcept     = 0;
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= WRITING

    //PRINTING =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //to_one_line - следует ли попытаться вывести всё в одну строку (комментарии будут проигнорированы, \
                    а многострочные значения так же станут занимать несколько строк)
    virtual std::string to_string(const ConfigFormat format = ConfigFormat::eJSON, const bool to_one_line = false) noexcept = 0;
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= PRINTING

    //COMMENTS =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
protected:
    struct CommentPair {
        std::string key_name;
        Comment     comment;
    };
    std::string                 m_preview_comment;
    std::string                 m_suffix_comment;
    std::map<size_t, Comment>   m_map_comments; //TODO: при insert() сместить и комментарии
public:
    void addPreviewComment(const std::string& content) noexcept { m_preview_comment = content; }
    void addSuffixComment(const std::string& content) noexcept  { m_suffix_comment = content; }
    //TODO: addComment(index, content): Json, JArray
    //TODO: addComment(key, content):   Json

    std::string& getPreviewComment() noexcept                   { return m_preview_comment; }
    std::string& getSuffixComment() noexcept                    { return m_suffix_comment; }
    //TODO: getComment(index): Json, JArray
    //TODO: getComment(key):   Json

    void clearPreviewComment() noexcept                         { m_preview_comment.clear(); }
    void clearSuffixComment() noexcept                          { m_suffix_comment.clear(); }
    //TODO: clearComment(index):    Json, JArray
    //TODO: clearComment(key):      Json
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= COMMENTS

    //OPERATORS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //TODO: operators
    //Element&    operator=(const Element& other) noexcept;
    //bool        operator==(const Element& other) const noexcept;
    //bool        operator!=(const Element& other) const noexcept { return !(*this == other); }
    //Element&    operator[](const size_t index):       Json, JArray
    //Element&    operator[](const std::string& key):   Json
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= OPERATORS
};

//TODO: class ElementNull
class ElementNull : Element {
private:
public:
};

//TODO: class ElementNumber
class ElementNumber : Element {
private:
public:
};

//TODO: class ElementBool
class ElementBool : Element {
private:
public:
};

//TODO: class ElementString
class ElementString : Element {
private:
public:
};

//TODO: class ElementJson
class ElementJson : Element {
private:
public:
};

//TODO: class ElementArray
class ElementArray : Element {
private:
public:
};


class Json;
class JArray;
// Element =====================================================================================

class BaseElement {
public:
    virtual ~BaseElement() noexcept {}
    //TODO: исправить вывод для всех типов элементов под разные категории
    virtual std::string to_string(int16_t tabulation_level, const bool enable_comment,
                                  const ConfigFormat config_format = ConfigFormat::eJSON) noexcept = 0;
};

class DoubleElement : BaseElement { //все числовые типы
public:
    double m_value;

    DoubleElement() noexcept                                    {}
    DoubleElement(const double& d) noexcept : m_value(d)        {}
    ~DoubleElement() noexcept                                   {}

    std::string to_string(int16_t tabulation_level = 0, const bool enable_comment = false,
                          const ConfigFormat config_format = ConfigFormat::eJSON) noexcept override
                                                                { return utils::toString(m_value); }
};

class BoolElement : BaseElement {
public:
    bool m_value;

    BoolElement() noexcept                                      {}
    BoolElement(const bool& b) noexcept : m_value(b)            {}
    ~BoolElement() noexcept                                     {}

    std::string to_string(int16_t tabulation_level = 0, const bool enable_comment = false,
                          const ConfigFormat config_format = ConfigFormat::eJSON) noexcept override
                                                                { return m_value ? "true" : "false"; }
};

class NullElement : BaseElement {
public:
    NullElement() noexcept                                      {}
    ~NullElement() noexcept                                     {}

    std::string to_string(int16_t tabulation_level = 0, const bool enable_comment = false,
                          const ConfigFormat config_format = ConfigFormat::eJSON) noexcept override
                                                                { return "null"; }
};

class StringElement : BaseElement {
public:
    std::string m_value;

    StringElement() noexcept                                    {}
    StringElement(const std::string& s) noexcept : m_value(s)   {}
    ~StringElement() noexcept                                   {}

    std::string to_string(int16_t tabulation_level = 0, const bool enable_comment = false,
                          const ConfigFormat config_format = ConfigFormat::eJSON) noexcept override
                                                                { return m_value; }
};

class JsonElement;      //описан в конце документа
class JArrayElement;    //описан в конце документа

struct Element {
    ValueType       first;
    BaseElement*    second;

                Element() noexcept;
                Element(std::nullptr_t) noexcept;
                Element(ValueType type, BaseElement* ptr) noexcept : first(type), second(ptr)
                                                                {}
                __ONLY_NUMBER_TYPES__(T)
                Element(const T& value) noexcept : first(eNumber)
                                                                { second = reinterpret_cast<BaseElement*>(
                                                                        new DoubleElement(static_cast<double>(value))); }
                Element(const bool value) noexcept : first(eBool)
                                                                { second = reinterpret_cast<BaseElement*>(
                                                                        new BoolElement(value)); }
                __ONLY_STRING_TYPES__(T)
                Element(const T& value) noexcept : first(eString)
                                                                { second = reinterpret_cast<BaseElement*>(
                                                                        new StringElement(std::string(value))); }
                Element(const Json& value) noexcept;
                Element(const JArray& value) noexcept;
                Element(const Element& other) noexcept;
                ~Element() noexcept                             { delete second; }

    bool        operator==(const Element& other) const noexcept;
    bool        operator!=(const Element& other) const noexcept { return !(*this == other); }
    Element&    operator=(const Element& other) noexcept;
    Element&    operator[](const std::string& key);
    Element&    operator[](const size_t index);

    double&     getNum() const;
    bool&       getBool() const;
    std::string&getString() const;
    Json&       getJson() const;
    JArray&     getArray() const;

    std::string to_string(const bool enable_comment = false,
                          const ConfigFormat config_format = ConfigFormat::eJSON) const noexcept
                                                                { return second->to_string(0, enable_comment, config_format); }

    bool        readFile(const std::string& path, const bool enable_comment = false,
                    const ConfigFormat config_format = ConfigFormat::eJSON) noexcept;
    bool        readFileJSON(const std::string& path, const bool enable_comment = false) noexcept;
    bool        readFileYAML(const std::string& path, const bool enable_comment = false) noexcept;
    bool        readFileINI(const std::string& path, const bool enable_comment = false) noexcept;

    bool        writeFile(const std::string& path, const bool enable_comment = false,
                    const ConfigFormat config_format = ConfigFormat::eJSON) const noexcept;
    bool        writeFileJSON(const std::string& path, const bool enable_comment = false) const noexcept;
    bool        writeFileYAML(const std::string& path, const bool enable_comment = false) const noexcept;
    bool        writeFileINI(const std::string& path, const bool enable_comment = false) const noexcept;
};
// ===================================================================================== Element


// JArray ======================================================================================
using AVector = std::vector<Element>;
// Упорядоченный список значений
class JArray {
    AVector m_values;

    std::map<size_t, Comment> m_comments;

    char        m_comment_sym; //многострочные комментарии всегда имеют этот символ в начале строки
    uint8_t     m_comment_column_size;

    Comment     m_preview_comment;

public:
                JArray() noexcept                               {}
                JArray(const JArray& array) noexcept;
                template<typename ... Types>
                JArray(const Types... args) noexcept            { for(Element el : {Element(args)...}) push_back(el); }
                ~JArray() noexcept                              {}

    void        parseArray(const std::string& string_of_array, const bool enable_comment = false,
                                const ConfigFormat config_format = ConfigFormat::eJSON);
    void        parseJSON_array(const std::string& string_of_array, const bool enable_comment = false);
    void        parseYAML_array(const std::string& string_of_array, const bool enable_comment = false);
    void        parseINI_array(const std::string& string_of_array, const bool enable_comment = false);

                __ONLY_ALLOWED_TYPES__(T)
    JArray&     push_front(const T& value) noexcept {
                    m_values.insert(m_values.cbegin(), Element(value));
                    return *this;
                }
                template<typename ... Types>
    JArray&     push_front(const Types... args) noexcept {
                    push_front(Element(args)...);
                    return *this;
                }
                __ONLY_ALLOWED_TYPES__(T)
    JArray&     push_back(const T& value) noexcept {
                    push_back(Element(value));
                    return *this;
                }
                template<typename ... Types>
    JArray&     push_back(const Types... args) noexcept {
                    m_values.push_back(Element(args)...);
                    return *this;
                }
    JArray&     append(const JArray& array) noexcept;

    ValueType   getType(const size_t index);
    ValueType   getTypeFront();
    ValueType   getTypeBack();

    Element&    getFront();
    Element&    getAt(const size_t index)                       { return m_values[index]; }
    Element&    getBack();

    JArray&     popFront();
    JArray&     popAt(const size_t index);
    JArray&     popBack();
    JArray&     clear();

    std::string to_string(int16_t tabulation_level = 0, const bool enable_comment = false,
                          const uint8_t column_size = 0, const ConfigFormat config_format = ConfigFormat::eJSON) const noexcept;
    std::string to_JSON_string(int16_t tabulation_level = 0, const bool enable_comment = false,
                               const uint8_t column_size = 0) const noexcept;
    std::string to_YAML_string(int16_t tabulation_level = 0, const bool enable_comment = false,
                               const uint8_t column_size = 0) const noexcept;
    std::string to_INI_string(int16_t tabulation_level = 0, const bool enable_comment = false,
                               const uint8_t column_size = 0, const std::string& preview_title = "") const noexcept;

    size_t      size() const noexcept                           { return m_values.size(); }
    bool        isEmpty() const noexcept                        { return m_values.size() == 0; }

    bool        operator==(const JArray& other) const noexcept;
    bool        operator!=(const JArray& other) const noexcept  { return !(*this == other); }

    Element&    operator[](const size_t index);
    Element&    operator[](const std::vector<std::string>& complex_key);
                template<std::size_t SIZE>
    Element&    operator[](const std::array<std::string, SIZE>& complex_key) {
                    std::vector<std::string> complex_key_vec;
                    complex_key_vec.reserve(SIZE);
                    std::copy(complex_key.begin(), complex_key.end(), complex_key_vec.begin());
                    return (*this)[complex_key_vec];
                }

    Element&    getValue(const size_t index);
    Element&    getValue(std::vector<std::string>& complex_key);

    AVector::iterator       begin() noexcept                    { return m_values.begin(); }
    AVector::iterator       end() noexcept                      { return m_values.end(); }
    AVector::const_iterator cbegin() const noexcept             { return m_values.cbegin(); }
    AVector::const_iterator cend() const noexcept               { return m_values.cend(); }

    //если индекс больше количества вложенных элементов, то добавятся в конец
                __ONLY_ALLOWED_TYPES__(T)
    JArray&     insert(const size_t index, const T& value) noexcept {
                    if(index > m_values.size() - 1) this->push_back(value);
                    else                            m_values.insert(m_values.cbegin() + index, Element(value));
                    return *this;
                }
                __ONLY_ALLOWED_TYPES__(T)
    JArray&     insert(const AVector::iterator& iterator, const T& value) {
                    m_values.insert(iterator, value);
                    return *this;
                }

    JArray&     erase(const size_t index);
    JArray&     erase(const AVector::iterator& iterator) {
                    m_values.erase(iterator);
                    return *this;
                }
    JArray&     erase(const AVector::iterator& begin, const AVector::iterator& end) {
                    m_values.erase(begin, end);
                    return *this;
                }

    //комментирование ------------------------------------------------------------------
    void        setCommentColumnSize(const uint8_t new_comment_column_size) noexcept
                                                                { m_comment_column_size = new_comment_column_size; }
    uint8_t     getCommentColumnSize() noexcept                 { return m_comment_column_size; }
    void        setCommentSymbol(const char new_comment_sym) noexcept
                                                                { m_comment_sym = new_comment_sym; }
    //-----
    void        addPreviewComment(const std::string &comment_before = "", const std::string &comment_after = "") noexcept
                                                                { m_preview_comment = Comment(comment_before, comment_after); }
    void        addPreviewComment_before(const std::string &comment = "") noexcept
                                                                { m_preview_comment.before = comment; }
    void        addPreviewComment_aftrer(const std::string &comment = "") noexcept
                                                                { m_preview_comment.after = comment; }
    void        addPreviewComment(const Comment& comment) noexcept
                                                                { m_preview_comment = comment; }
    //-----
    Comment&    getPreviewComment()                             { return m_preview_comment; }
    //-----
    void        addComment(const size_t index, const std::string &comment_before = "",
                            const std::string &comment_after = "");
    void        addComment(const size_t index, const Comment& comment);
    void        addComment_before(const size_t index, const std::string &comment = "");
    void        addComment_after(const size_t index, const std::string &comment = "");
    //-----
    Comment&    getComment(const size_t index);
    Comment     getComment(const size_t index) const noexcept;
    Comment&    getOrCreateComment(const size_t index);
    //-----
    void        clearPreviewComment() noexcept                  { m_preview_comment = {}; }
    void        clearComment(const size_t index)                { m_comments.erase(m_values[index].first); }
};
// ====================================================================================== JArray


// Json ========================================================================================
using JPair     = std::pair<std::string, Element>;
using JVector   = std::vector<JPair>;
// Неупорядоченный список "ключ-значение" (в данном случае упорядочен)
class Json {
    JVector m_values;

    std::map<std::string, Comment> m_comments;

    char        m_comment_sym; //многострочные комментарии всегда имеют этот символ в начале строки
    uint8_t     m_comment_column_size;

    Comment     m_preview_comment;
public:
                Json() noexcept : m_comment_sym(0)              {}
                Json(const Json& json) noexcept;
                Json(const JPair& pair) noexcept : m_comment_sym(0)
                                                                { put(pair.first, pair.second); }
                //разнесено для решения конфликта, не изменять следующие два конструктора!
                Json(const std::string& input_string)           { parse(input_string); }
                Json(const std::string& input_string, const ConfigFormat config_format,
                     const bool enable_comment = false);
                __ONLY_ALLOWED_TYPES__(T)
                Json(const std::string& key, const T& value) noexcept : m_comment_sym(0)
                                                                { put(key, value); }
                Json(const JVector& vec) noexcept;
                Json(const Element& element) noexcept;;
                ~Json() noexcept                                {}

    Json&       operator=(const Json& other) noexcept;

                __ONLY_ALLOWED_TYPES__(T)
    Json&       put(const std::string& key, const T& value, const bool rewrite = true) noexcept
                                                                { return put(key, Element(value), rewrite); }
    Json&       put(const std::string& key, const Element& element, const bool rewrite = true) noexcept;
    Json&       put(const Json& json, const bool rewrite = true) noexcept;

                __ONLY_ALLOWED_TYPES__(T)
    Json&       add(const std::string& key, const T& value, const bool rewrite = true) noexcept
                                                                { return put(key, value, rewrite); }
    Json&       add(const Json& json, const bool rewrite = true) noexcept
                                                                { return put(json, rewrite); }
    Json&       append(const Json& json, const bool rewrite = true) noexcept
                                                                { return put(json, rewrite); }

    void        parse(const std::string& input_string, const ConfigFormat config_format = ConfigFormat::eJSON,
                    const bool enable_comment = false);
    void        parseJSON(const std::string& string_of_json, const bool enable_comment = false);
    void        parseYAML(const std::string& string_of_yaml, const bool enable_comment = false);
    void        parseINI(const std::string& string_of_ini, const bool enable_comment = false);

    bool        readFile(const std::string& path, const bool enable_comment = false,
                   const ConfigFormat config_format = ConfigFormat::eJSON) noexcept;
    bool        readFileJSON(const std::string& path, const bool enable_comment = false) noexcept
                                                                { return readFile(path, enable_comment); }
    bool        readFileYAML(const std::string& path, const bool enable_comment = false) noexcept
                                                                { return readFile(path, enable_comment, ConfigFormat::eYAML); }
    bool        readFileINI(const std::string& path, const bool enable_comment = false) noexcept
                                                                { return readFile(path, enable_comment, ConfigFormat::eINI); }

    bool        writeFile(const std::string& path, int16_t tabulation_level = 0,
                   const bool enable_comment = false, const ConfigFormat config_format = ConfigFormat::eJSON) noexcept;
    bool        writeJSON(const std::string& path, int16_t tabulation_level = 0,
                   const bool enable_comment = false) noexcept  { return writeFile(path, tabulation_level, enable_comment); }
    bool        writeYAML(const std::string& path, int16_t tabulation_level = 0,
                   const bool enable_comment = false) noexcept  { return writeFile(path, tabulation_level, enable_comment, ConfigFormat::eYAML); }
    bool        writeINI(const std::string& path, int16_t tabulation_level = 0,
                   const bool enable_comment = false) noexcept  { return writeFile(path, tabulation_level, enable_comment, ConfigFormat::eINI); }

    std::string to_string(int16_t tabulation_level = 0, const bool enable_comment = false,
                          const uint8_t column_size = 0, const ConfigFormat config_format = ConfigFormat::eJSON) const noexcept;
    std::string to_JSON_string(int16_t tabulation_level = 0, const bool enable_comment = false,
                               const uint8_t column_size = 0) const noexcept;
    std::string to_YAML_string(int16_t tabulation_level = 0, const bool enable_comment = false,
                               const uint8_t column_size = 0) const noexcept;
    std::string to_INI_string(int16_t tabulation_level = 0, const bool enable_comment = false,
                              const uint8_t column_size = 0, const std::string& preview_title = "") const noexcept;
    size_t      size() const noexcept                           { return m_values.size(); }
    bool        isEmpty() noexcept                              { return m_values.size() == 0; }
    bool        contains(const std::string& key) const noexcept;
                __ONLY_ALLOWED_TYPES__(T)
    Json&       updateValue(const std::string& key, const T& new_value) noexcept {
                    if(contains(key))   (*this)[key] = Element(new_value);
                    else                put(key, new_value);
                    return *this;
                }
    Json&       updateValue(const std::string& key, const Element& new_value) noexcept;
    Json&       clear() noexcept {
                    m_values.clear(); m_comments.clear();
                    return *this;
                }

    JVector::iterator       begin() noexcept                    { return m_values.begin(); }
    JVector::iterator       end() noexcept                      { return m_values.end(); }
    JVector::const_iterator cbegin() const noexcept             { return m_values.begin(); }
    JVector::const_iterator cend() const noexcept               { return m_values.end(); }

    bool        operator==(const Json& other) const noexcept;
    bool        operator!=(const Json& other) const noexcept    { return !(*this == other); }

    Element&    operator[](const size_t index);
    Element&    operator[](const std::string& key);
    Element&    operator[](const std::vector<std::string>& complex_key);
                template<std::size_t SIZE>
    Element&    operator[](const std::array<std::string, SIZE>& complex_key) {
                    std::vector<std::string> complex_key_vec;
                    complex_key_vec.reserve(SIZE);
                    std::copy(complex_key.begin(), complex_key.end(), complex_key_vec.begin());
                    return (*this)[complex_key_vec];
                }
    Element&    getValue(const size_t index)                    { return (*this)[index]; }
    Element&    getValue(const std::string& key)                { return (*this)[key]; }
    Element&    getValue(std::vector<std::string>& complex_key) { return (*this)[complex_key]; }

    //положить значение в указанную позицию
    //если значение существует и флаг поднят - удалить существующее значение
    //если индекс больше количества вложенных элементов, то добавятся в конец
    //если ключ не найден, добавится в конец
                __ONLY_ALLOWED_TYPES__(T)
    Json&       insert(const size_t index, const std::string& key,
                       const T& value, const bool rewrite = true) noexcept
                {
                    if(contains(key) && rewrite)
                        erase(key);
                    else {
                        if(index > m_values.size() - 1)
                            this->put(key, value);
                        else
                            m_values.insert(
                                m_values.cbegin() + index,
                                std::make_pair(key, Element(value)));
                    }
                    return *this;
                }
                __ONLY_ALLOWED_TYPES__(T)
    Json&       insert(const JVector::iterator& iterator, const std::string& key,
                       const T& value, const bool rewrite = true) noexcept
                {
                    if(contains(key) && rewrite) erase(key);
                    else {
                        if(iterator == m_values.end())  this->put(key, value);
                        else                            m_values.insert(iterator, std::make_pair(key, Element(value)));
                    }
                    return *this;
                }
                __ONLY_ALLOWED_TYPES__(T)
    Json&       insertBefore(const std::string& keyIndex, const std::string& key,
                             const T& value, const bool rewrite = true) noexcept
                {
                    bool key_exists = false; //чтобы второй раз не искать
                    //поиск индекса указанного ключа
                    auto key_found_it = m_values.begin();
                    for(; key_found_it != m_values.end(); key_found_it++) {
                        if(key_found_it->first == keyIndex) {
                            key_exists = true;
                            break;
                        }
                    }

                    if(key_exists && (key_found_it->first == key) && rewrite)
                        erase(key);
                    else {
                        if(key_found_it == m_values.end())
                            this->put(key, value);
                        else
                            m_values.insert(key_found_it, std::make_pair(key, Element(value)));
                    }
                    return *this;
                }
                __ONLY_ALLOWED_TYPES__(T)
    Json&       insertAfter(const std::string& keyIndex, const std::string& key,
                            const T& value, const bool rewrite = true) noexcept
                {
                    bool key_exists = false; //чтобы второй раз не искать
                    //поиск индекса указанного ключа
                    auto key_found_it = m_values.begin();
                    for(; key_found_it != m_values.end(); key_found_it++) {
                        if(key_found_it->first == keyIndex) {
                            key_exists = true;
                            key_found_it++; //нужен следующий итератор
                            break;
                        }
                    }

                    if(key_exists && (key_found_it->first == key) && rewrite)
                        erase(key);
                    else {
                        if(key_found_it == m_values.end())
                            this->put(key, value);
                        else
                            m_values.insert(key_found_it, std::make_pair(key, Element(value)));
                    }
                    return *this;
                }

    Json&       erase(const size_t index);
    Json&       erase(const JVector::iterator& iterator) {
                    m_values.erase(iterator);
                    return *this;
                }
    Json&       erase(const JVector::iterator& begin, const JVector::iterator& end) {
                    m_values.erase(begin, end);
                    return *this;
                }
    Json&       erase(const std::string& key) noexcept;
    Json&       erase(const std::vector<std::string>& keys) noexcept;

    //комментирование ------------------------------------------------------------------
    void        setCommentColumnSize(const uint8_t new_comment_column_size) noexcept
                                                                { m_comment_column_size = new_comment_column_size; }
    uint8_t     getCommentColumnSize()  noexcept                { return m_comment_column_size; }
    void        setCommentSymbol(const char new_comment_sym) noexcept
                                                                { m_comment_sym = new_comment_sym; }
    char        getCommentSymbol() noexcept                     { return m_comment_sym; }
    //-----
    void        addPreviewComment(const std::string &comment_before = "", const std::string &comment_after = "") noexcept
                                                                { m_preview_comment = Comment(comment_before, comment_after); }
    void        addPreviewComment_before(const std::string &comment = "") noexcept
                                                                { m_preview_comment.before = comment; }
    void        addPreviewComment_aftrer(const std::string &comment = "") noexcept
                                                                { m_preview_comment.after = comment; }
    void        addPreviewComment(const Comment& comment) noexcept
                                                                { m_preview_comment = comment; }
    //-----
    Comment&    getPreviewComment() noexcept                    { return m_preview_comment; }
    //-----
    void        addComment(const std::string& key, const std::string &comment_before = "",
                    const std::string &comment_after = "")
                {
                    Comment& ct = getOrCreateComment(key);
                    ct = Comment(comment_before, comment_after);
                }
    void        addComment(const std::string& key, const Comment& comment) {
                    Comment& ct = getOrCreateComment(key);
                    ct = comment;
                }
    void        addComment_before(const std::string& key, const std::string &comment = "") {
                    Comment& ct = getOrCreateComment(key);
                    ct.before = comment;
                }
    void        addComment_after(const std::string& key, const std::string &comment = "") {
                    Comment& ct = getOrCreateComment(key);
                    ct.after = comment;
                }
    void        addComment(const size_t index, const std::string &comment_before = "",
                    const std::string &comment_after = "")
                {
                    Comment& ct = getOrCreateComment(index);
                    ct = Comment(comment_before, comment_after);
                }
    void        addComment(const size_t index, const Comment& comment) {
                    Comment& ct = getOrCreateComment(index);
                    ct = comment;
                }
    void        addComment_before(const size_t index, const std::string &comment = "") {
                    Comment& ct = getOrCreateComment(index);
                    ct.before = comment;
                }
    void        addComment_after(const size_t index, const std::string &comment = "") {
                    Comment& ct = getOrCreateComment(index);
                    ct.after = comment;
                }
    //-----
    Comment&    getComment(const std::string& key);
    Comment     getComment(const std::string& key) const noexcept;
    Comment&    getComment(const size_t index);
    Comment     getComment(const size_t index) const noexcept;
    Comment&    getOrCreateComment(const std::string& key);
    Comment&    getOrCreateComment(const size_t index);
    //-----
    void        clearPreviewComment() noexcept                  { m_preview_comment = {}; }
    void        clearComment(const std::string& key) noexcept   { m_comments.erase(key); }
    void        clearComment(const size_t index)                { m_comments.erase(m_values[index].first); }
    //----------------------------------------------------------------------------------
};
// ======================================================================================== Json


// FUNCTIONS ===================================================================================
Element     Parse(const std::string& element_string, const ConfigFormat config_format = ConfigFormat::eJSON,
              const bool enable_comment = false) noexcept;
Element     ParseJSON(const std::string& element_string, const bool enable_comment = false) noexcept;
Element     ParseYAML(const std::string& element_string, const bool enable_comment = false) noexcept;
Element     ParseINI(const std::string& element_string, const bool enable_comment = false) noexcept;
//------------------------------------------------------------
Element     ReadFile(const std::string& path, const bool enable_comment = false,
                 const ConfigFormat config_format = ConfigFormat::eJSON) noexcept;
Element     ReadFileJSON(const std::string& path, const bool enable_comment = false) noexcept;
Element     ReadFileYAML(const std::string& path, const bool enable_comment = false) noexcept;
Element     ReadFileINI(const std::string& path, const bool enable_comment = false) noexcept;
//------------------------------------------------------------
bool        WriteFile(const std::string& path, const Element& element,
               const bool enable_comment = false, const ConfigFormat config_format = ConfigFormat::eJSON) noexcept;
bool        WriteFileJSON(const std::string& path, const Element& element,
                   const bool enable_comment = false) noexcept;
bool        WriteFileYAML(const std::string& path, const Element& element,
                   const bool enable_comment = false) noexcept;
bool        WriteFileINI(const std::string& path, const Element& element,
                  const bool enable_comment = false) noexcept;
//------------------------------------------------------------
bool        GetAllStringsFromFile(const std::string& path, std::string& dest_string) noexcept;
bool        WriteStringToFile(const std::string& path, const std::string& source_string) noexcept;
std::vector<std::string> parseIniKeys(std::string& ini_key_value) noexcept;
std::vector<std::string> parseIniCustomKeys(std::string& preview_key) noexcept;
Json*       GetObjectForIniCustomKey(Json* json, std::vector<std::string>& keys) noexcept;
Element     ParseValueFromString(std::string& value, const bool enable_comments,
                                 const ConfigFormat format);
std::string PrintRecursiveIniElements(const ConfigFormat cfg, const Element& el,
                                      const bool enable_comment, const std::string& preview_key = "") noexcept;
std::string PrintRecursiveIniElements(const ConfigFormat cfg, const JPair& jp,
                                      const bool enable_comment, const std::string& preview_key = "") noexcept;
std::string ToComment(const std::string& comment_string, const uint8_t tabulation_level = 0,
                      const uint8_t column_size = 0, const char border_symbol = 0) noexcept;
std::string FromComment(const std::string& comment_string, uint8_t& column_size,
                        char& border_symbol) noexcept;
// =================================================================================== FUNCTIONS


// STATIC FUNCTIONS ============================================================================
static ValueType    CheckValue(std::string& value, const ConfigFormat& format = ConfigFormat::eJSON) noexcept;
static bool         CheckNumber(const std::string& value) noexcept;
static bool         CheckBool(std::string& value) noexcept;
static bool         CheckNull(std::string& value) noexcept;
static bool         CheckString(std::string& value, const ConfigFormat& format = ConfigFormat::eJSON) noexcept;
static bool         CheckJson(std::string& value) noexcept;
static bool         CheckArray(std::string& value) noexcept;
static void         RemoveIllegalSpaces(std::string& string) noexcept;
// ============================================================================ STATIC FUNCTIONS


// Element (продолжение) =======================================================================
class JsonElement : BaseElement {
public:
    Json m_value;

    JsonElement() noexcept                                      {}
    JsonElement(const Json& j) noexcept : m_value(j)            {}
    ~JsonElement() noexcept                                     {}

    std::string to_string(int16_t tabulation_level = 0, const bool enable_comment = false,
                          const ConfigFormat config_format = ConfigFormat::eJSON) noexcept override
                {
                    return m_value.to_string(tabulation_level, enable_comment,
                                             m_value.getCommentColumnSize(), config_format);
                }
};
class JArrayElement : BaseElement {
public:
    JArray m_value;

    JArrayElement() noexcept                                    {}
    JArrayElement(const JArray& a) noexcept : m_value(a)        {}
    ~JArrayElement() noexcept                                   {}

    std::string to_string(int16_t tabulation_level = 0, const bool enable_comment = false,
                          const ConfigFormat config_format = ConfigFormat::eJSON) noexcept override
                {
                    return m_value.to_string(tabulation_level, enable_comment,
                                             m_value.getCommentColumnSize(), config_format);
                }
};
// ======================================================================= Element (продолжение)


#endif // JSON_H
