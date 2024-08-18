#ifndef JSON_H
#define JSON_H

#include "utils.h"
#include <map>
#include <memory>
#include <vector>

//TODO: add "noexcept"

#define __ONLY_ALLOWED_TYPES__(ARG) \
    template<typename ARG, \
        typename std::enable_if< \
            std::is_same<ARG, Json>::value \
            || std::is_same<ARG, JArray>::value \
            || std::is_convertible<ARG, std::string>::value \
            || std::is_arithmetic<ARG>::value \
            || std::is_same<ARG, bool>::value \
        >::type* = nullptr>
#define __JSON_EMPTY_EXCEPTION__    throw std::invalid_argument("Json is empty");
#define __ARRAY_EMPTY_EXCEPTION__   throw std::invalid_argument("JArray is empty");
#define __CHECK_INDEX_BOUND__(object, index) \
                                    if(index + 1 > object->size()) \
                                        throw std::out_of_range("going beyond the object");
#define __CHECK_INDEX_BOUND2__(object, index) \
                                    if(index + 1 > object.size()) \
                                        throw std::out_of_range("going beyond the object");
#define __KEY_NOT_FOUND_EXCEPTION__(key) \
                                    throw std::invalid_argument("key not found: " + key);
#define __INCORRECT_TYPE_ELEMENT_FOR_INDEX_EXCEPTION__ \
                                    throw std::invalid_argument( \
                                        "index operator may be used only for Json or JArray elements");
#define __ARRAY_INCORRECT_INDEX_EXCEPTION__ \
                                    throw std::invalid_argument("index for JArray must be a number only");
#define __JSON_KEY_NOT_FOUND_EXCEPTION__ \
                                    throw std::invalid_argument("Json key not found");

// Comment =====================================================================================
enum class PrintType {
    eWithoutComment,
    eWithComment
};

enum class CommentType {
    eBeforeValue,           // пользователь сам распределяет переносы строки
    eBeforeValueMultiLine,  // автоматическая расстановка переносов строки
    eAfterValueOneLine      // подсказка на той же строке
};

struct Comment {
    std::string before;
    std::string after;

    Comment(){}
    Comment(const std::string& comment_before, const std::string& comment_after) :
        before(comment_before), after(comment_after) {}
};
// ===================================================================================== Comment
// *
// *
class Json;
class JArray;
// Element =====================================================================================
enum ValueType {
    eNull,

    eNumber,
    eBool,
    eString,
    eJson,
    eArray
};
static std::string to_string(const ValueType type);

class BaseElement {
public:
    virtual ~BaseElement(){}
    virtual std::string to_string(int16_t tabulation_level, const PrintType print_type) = 0;
};

class DoubleElement : BaseElement { //все числовые типы
public:
    double m_value;

    DoubleElement()                                     {}
    DoubleElement(const double& d) : m_value(d)         {}
    ~DoubleElement()                                    {}

    std::string to_string(int16_t tabulation_level = 0,
                          const PrintType print_type = PrintType::eWithoutComment)
                                                        { return utils::toString(m_value); }
};

class BoolElement : BaseElement {
public:
    bool m_value;

    BoolElement()                                       {}
    BoolElement(const bool& b) : m_value(b)             {}
    ~BoolElement()                                      {}

    std::string to_string(int16_t tabulation_level = 0,
                          const PrintType print_type = PrintType::eWithoutComment)
                                                        { return m_value ? "true" : "false"; }
};

class StringElement : BaseElement {
public:
    std::string m_value;

    StringElement()                                     {}
    StringElement(const std::string& s) : m_value(s)    {}
    ~StringElement()                                    {}

    std::string to_string(int16_t tabulation_level = 0,
                          const PrintType print_type = PrintType::eWithoutComment)
                                                        { return "\"" + m_value + "\""; }
};

class JsonElement;      //описан после соответствующего класса
class JArrayElement;    //описан после соответствующего класса

struct Element {
    ValueType       first;
    BaseElement*    second;

                Element() : first(ValueType::eNull), second(nullptr)
                                                                {}
                Element(ValueType type, BaseElement* ptr) : first(type), second(ptr)
                                                                {}
                template<typename T, typename std::enable_if<std::is_arithmetic<T>::value
                                                             && !std::is_same<T, bool>::value>
                         ::type* = nullptr>
                Element(const T& value) : first(eNumber)
                                                                { second = reinterpret_cast<BaseElement*>(
                                                                        new DoubleElement(static_cast<double>(value))); }
                Element(const bool value) : first(eBool)
                                                                { second = reinterpret_cast<BaseElement*>(
                                                                        new BoolElement(value)); }
                template<typename T, typename std::enable_if<std::is_convertible<T, std::string>::value>
                         ::type* = nullptr>
                Element(const T& value) : first(eString)
                                                                { second = reinterpret_cast<BaseElement*>(
                                                                        new StringElement(std::string(value))); }
                Element(const Json& value);
                Element(const JArray& value);
                Element(const Element& other);
                ~Element()                                      { delete second; }

    bool        operator==(const Element& other) const;
    bool        operator!=(const Element& other)          const { return !(*this == other); }
    Element&    operator=(const Element& other);

    double&     getNum() const;
    bool&       getBool() const;
    std::string&getString() const;
    Json&       getJson() const;
    JArray&     getArray() const;

    //NOTE: кандидаты на удаление
    Element&    getInnerValue(const std::string& key) const;
    //FIXME: должно возвращать значение по ссылке
    Element    getInnerValue(const size_t index) const;
};
// ===================================================================================== Element
// *
// *
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
                JArray()                            {}
                JArray(const JArray& array);
                template<typename ... Types>
                JArray(const Types... args)         { for(Element el : {Element(args)...}) push_back(el); }
                ~JArray()                           {}

    bool        parseArray(const std::string& str);
    void        parseArrayWithComment(const std::string& string_of_array, const PrintType print_type = PrintType::eWithoutComment);

                __ONLY_ALLOWED_TYPES__(T)
    JArray&     push_front(const T& value)          { m_values.insert(m_values.cbegin(), Element(value));
                                                        return *this; }
                template<typename ... Types>
    JArray&     push_front(const Types... args)     { push_front(Element(args)...);
                                                        return *this; }
                __ONLY_ALLOWED_TYPES__(T)
    JArray&     push_back(const T& value)           { push_back(Element(value));
                                                        return *this; }
                template<typename ... Types>
    JArray&     push_back(const Types... args)      { m_values.push_back(Element(args)...);
                                                        return *this; }
    JArray&     append(const JArray& array);

    ValueType   getType(const size_t index)         { return m_values[index].first; }
    ValueType   getTypeFront(const size_t index)    { return getType(0); }
    ValueType   getTypeBack(const size_t index)     { return getType(m_values.size() - 1); }

    Element     getFront()                          { return m_values.front(); }
    Element     getBack()                           { return m_values.back(); }

    JArray&     popFornt()                          { m_values.erase(m_values.begin());
                                                        return *this; }
    JArray&     popBack()                           { m_values.pop_back();
                                                        return *this; }
    JArray&     clear()                             { m_values.clear();
                                                        return *this; }

    std::string to_string(int16_t tabulation_level = 0, const PrintType print_type = PrintType::eWithoutComment,
                          const uint8_t column_size = 0) const;

    size_t      size()                        const { return m_values.size(); }
    bool        isEmpty()                           { return m_values.size() == 0; }

    bool        operator==(const JArray& other) const;
    bool        operator!=(const JArray& other)
                                              const { return !(*this == other); }

    Element&    operator[](const size_t index);
    Element&    operator[](const std::vector<std::string>& complex_name);
                template<std::size_t SIZE>
    Element&    operator[](const std::array<std::string, SIZE>& complex_name)
                {
                    if(m_values.empty())
                        __ARRAY_EMPTY_EXCEPTION__

                    Element &el = (*this)[complex_name[0]]; //находим первый элемент списка
                    typename std::array<std::string, SIZE>::const_iterator it = complex_name.begin() + 1; //первый элемент пропускаем
                    for (; el.first != ValueType::eNull && it != complex_name.end(); it++) {
                        bool isNumber = utils::isNumber(*it, false);
                        switch(el.first) {
                        case eJson:
                            el = el.getInnerValue(*it);
                            if(el.first == ValueType::eNull) {
                                if(isNumber)
                                    el = el.getInnerValue(stoi(*it));
                                else
                                    __JSON_KEY_NOT_FOUND_EXCEPTION__
                            }
                            break;
                        case eArray:
                            //для массива возможно обращение только по числовому индексу!
                            if(isNumber)
                                el = el.getInnerValue(stoi(*it));
                            else
                                __ARRAY_INCORRECT_INDEX_EXCEPTION__
                            break;
                        default:
                            //продолжать поиск можно только по двум структурам!
                            __INCORRECT_TYPE_ELEMENT_FOR_INDEX_EXCEPTION__
                        }
                    }

                    return el;
                }

    Element&    getValue(const size_t index)        { return (*this)[index]; }
    Element&    getValue(const std::vector<std::string>& complex_name)
                                                    { return (*this)[complex_name]; }

    AVector::iterator       begin()                 { return m_values.begin(); }
    AVector::iterator       end()                   { return m_values.end(); }
    AVector::const_iterator cbegin()          const { return m_values.cbegin(); }
    AVector::const_iterator cend()            const { return m_values.cend(); }

    //если индекс больше количества вложенных элементов, то добавятся в конец
                __ONLY_ALLOWED_TYPES__(T)
    JArray&     insert(const size_t index, const T& value)
                {
                    if(index > m_values.size() - 1)
                        this->push_back(value);
                    else
                        m_values.insert(m_values.cbegin() + index, Element(value));
                    return *this;
                }

                __ONLY_ALLOWED_TYPES__(T)
    JArray&     insert(const AVector::iterator& iterator, const T& value)
                                                    { m_values.insert(iterator, value);
                                                        return *this; }

    JArray&     erase(const size_t index);
    JArray&     erase(const AVector::iterator& iterator)
                                                    { m_values.erase(m_values.cbegin());
                                                        return *this; }
    JArray&     erase(const AVector::iterator& begin, const AVector::iterator& end)
                                                    { m_values.erase(begin, end);
                                                        return *this; }

    //комментирование ------------------------------------------------------------------
    void        setCommentColumnSize(const uint8_t new_comment_column_size)
                                                                { m_comment_column_size = new_comment_column_size; }
    uint8_t     getCommentColumnSize()                          { return m_comment_column_size; }
    //-----
    void        addPreviewComment(const std::string &comment_before = "", const std::string &comment_after = "")
                                                                { m_preview_comment = Comment(comment_before, comment_after); }
    void        addPreviewComment_before(const std::string &comment = "")
                                                                { m_preview_comment.before = comment; }
    void        addPreviewComment_aftrer(const std::string &comment = "")
                                                                { m_preview_comment.after = comment; }
    void        addPreviewComment(const Comment& comment)       { m_preview_comment = comment; }
    //-----
    Comment&    getPreviewComment()                             { return m_preview_comment; }
    //-----
    void        addComment(const size_t index,
                    const std::string &comment_before = "", const std::string &comment_after = "")
                                                                { Comment ct = getOrCreateComment(index);
                                                                    ct = Comment(comment_before, comment_after); }
    void        addComment(const size_t index, const Comment& comment)
                                                                { Comment ct = getOrCreateComment(index);
                                                                    ct = comment; }
    void        addComment_before(const size_t index, const std::string &comment = "")
                                                                { Comment ct = getOrCreateComment(index);
                                                                    ct.before = comment; }
    void        addComment_after(const size_t index, const std::string &comment = "")
                                                                { Comment ct = getOrCreateComment(index);
                                                                    ct.after = comment; }
    //-----
    Comment&    getComment(const size_t index)
                {
                    __CHECK_INDEX_BOUND2__(m_values, index);
                    auto it = m_comments.find(index);
                    if(it == m_comments.end())
                        throw std::invalid_argument("comment for index '" + std::to_string(index) + "' not found");
                    return it->second;
                }
    Comment&    getOrCreateComment(const size_t index)
                {
                    __CHECK_INDEX_BOUND2__(m_values, index);
                    auto it = m_comments.find(index);
                    if(it == m_comments.end())
                        m_comments.insert(std::make_pair(m_values[index].first, Comment()));
                    return it->second;
                }
    //-----
    void        clearPreviewComment()                           { m_preview_comment = {}; }
    void        clearComment(const size_t index)                { m_comments.erase(m_values[index].first); }
};
// ====================================================================================== JArray
// *
// *
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
                Json()                                          {}
                Json(const Json& json);
                Json(const JPair& pair)                         { put(pair.first, pair.second); }
                Json(const std::string& json_string)            { this->parseJson(json_string); }
                __ONLY_ALLOWED_TYPES__(T)
                Json(const std::string& key, const T& value)    { put(key, value); }
                Json(const JVector& vec);
                ~Json()                                         {}

    Json&       operator=(const Json& other);

                __ONLY_ALLOWED_TYPES__(T)
    Json&       put(const std::string& key, const T& value, const bool rewrite = true)
                                                                { return put(key, Element(value), rewrite); }
    Json&       put(const std::string& key, const Element& element, const bool rewrite = true);
    Json&       put(const Json& json, const bool rewrite = true);

                __ONLY_ALLOWED_TYPES__(T)
    Json&       add(const std::string& key, const T& value, const bool rewrite = true)
                                                                { return this->put(key, value, rewrite); }
    Json&       add(const Json& json, const bool rewrite = true)
                                                                { return this->put(json, rewrite); }
    Json&       append(const Json& json, const bool rewrite = true)
                                                                { return this->put(json, rewrite); }

    bool        parseJson(const std::string& str);
    void        parseJsonWithComment(const std::string& string_of_json, const PrintType print_type = PrintType::eWithoutComment);
    bool        readFile(const std::string& path);
    bool        writeFile(const std::string& path, int16_t tabulation_level = 0);

    std::string to_string(int16_t tabulation_level = 0, const PrintType print_type = PrintType::eWithoutComment,
                          const uint8_t column_size = 0) const;
    size_t      size()                                    const { return m_values.size(); }
    bool        isEmpty()                                       { return m_values.size() == 0; }
    bool        contains(const std::string& key);
                __ONLY_ALLOWED_TYPES__(T)
    Json&       updateValue(const std::string& key, const T& new_value)
                {
                    if(contains(key)) {
                        (*this)[key] = Element(new_value);
                    } else
                        put(key, new_value);
                    return *this;
                }
    Json&       updateValue(const std::string& key, const Element& new_value);
    Json&       clear()                                         { m_values.clear();
                                                                    return *this; }

    JVector::iterator       begin()                             { return m_values.begin(); }
    JVector::iterator       end()                               { return m_values.end(); }
    JVector::const_iterator cbegin()                      const { return m_values.begin(); }
    JVector::const_iterator cend()                        const { return m_values.end(); }

    bool        operator==(const Json& other) const;
    bool        operator!=(const Json& other)             const { return !(*this == other); }

    Element&    operator[](const size_t index);
    Element&    operator[](const std::string& name);
    Element&    operator[](const std::vector<std::string>& complex_name);
                template<std::size_t SIZE>
    Element&    operator[](const std::array<std::string, SIZE>& complex_name)
                {
                    if(m_values.empty())
                        __JSON_EMPTY_EXCEPTION__;

                    Element& el = (*this)[complex_name[0]]; //находим первый элемент списка
                    typename std::array<std::string, SIZE>::const_iterator it = complex_name.begin() + 1; //первый элемент пропускаем
                    for (; el.first != ValueType::eNull && it != complex_name.end(); it++) {
                        bool isNumber = utils::isNumber(*it, false);
                        switch(el.first) {
                        case eJson:
                            el = el.getInnerValue(*it);
                            if(el.first == ValueType::eNull) {
                                if(isNumber)
                                    el = el.getInnerValue(stoi(*it));
                                else
                                    __JSON_KEY_NOT_FOUND_EXCEPTION__
                            }
                            break;
                        case eArray:
                            //для массива возможно обращение только по числовому индексу!
                            if(isNumber)
                                el = el.getInnerValue(stoi(*it));
                            else
                                __ARRAY_INCORRECT_INDEX_EXCEPTION__
                            break;
                        default:
                            //продолжать поиск можно только по двум структурам!
                            __INCORRECT_TYPE_ELEMENT_FOR_INDEX_EXCEPTION__;
                        }
                    }

                    return el;
                }
    Element&    getValue(const size_t index)                    { return (*this)[index]; }
    Element&    getValue(const std::string& name)               { return (*this)[name]; }
    Element&    getValue(const std::vector<std::string>& complex_name)
                                                                { return (*this)[complex_name]; }

    //положить значение в указанную позицию
    //если значение существует и флаг поднят - удалить существующее значение
    //если индекс больше количества вложенных элементов, то добавятся в конец
    //если ключ не найден, добавится в конец
                __ONLY_ALLOWED_TYPES__(T)
    Json&       insert(const size_t index, const std::string& key,
                       const T& value, const bool rewrite = true)
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
                       const T& value, const bool rewrite = true)
                {
                    if(contains(key) && rewrite)
                        erase(key);
                    else {
                        if(iterator == m_values.end())
                            this->put(key, value);
                        else
                            m_values.insert(iterator, std::make_pair(key, Element(value)));
                    }
                    return *this;
                }
                __ONLY_ALLOWED_TYPES__(T)
    Json&       insertBefore(const std::string& keyIndex, const std::string& key,
                             const T& value, const bool rewrite = true)
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
                            const T& value, const bool rewrite = true)
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
    Json&       erase(const JVector::iterator& iterator)
                                                                { m_values.erase(m_values.cbegin());
                                                                    return *this; }
    Json&       erase(const JVector::iterator& begin, const JVector::iterator& end)
                                                                { m_values.erase(begin, end);
                                                                    return *this; }
    Json&       erase(const std::string& key);
    Json&       erase(const std::vector<std::string>& keys);

    //комментирование ------------------------------------------------------------------
    void        setCommentColumnSize(const uint8_t new_comment_column_size)
                                                                { m_comment_column_size = new_comment_column_size; }
    uint8_t     getCommentColumnSize()                          { return m_comment_column_size; }
    //-----
    void        addPreviewComment(const std::string &comment_before = "", const std::string &comment_after = "")
                                                                { m_preview_comment = Comment(comment_before, comment_after); }
    void        addPreviewComment_before(const std::string &comment = "")
                                                                { m_preview_comment.before = comment; }
    void        addPreviewComment_aftrer(const std::string &comment = "")
                                                                { m_preview_comment.after = comment; }
    void        addPreviewComment(const Comment& comment)       { m_preview_comment = comment; }
    //-----
    Comment&    getPreviewComment()                             { return m_preview_comment; }
    //-----
    void        addComment(const std::string& key,
                    const std::string &comment_before = "", const std::string &comment_after = "")
                                                                { Comment ct = getOrCreateComment(key);
                                                                    ct = Comment(comment_before, comment_after); }
    void        addComment(const std::string& key, const Comment& comment)
                                                                { Comment ct = getOrCreateComment(key);
                                                                    ct = comment; }
    void        addComment_before(const std::string& key, const std::string &comment = "")
                                                                { Comment ct = getOrCreateComment(key);
                                                                    ct.before = comment; }
    void        addComment_after(const std::string& key, const std::string &comment = "")
                                                                { Comment ct = getOrCreateComment(key);
                                                                    ct.after = comment; }
    void        addComment(const size_t index,
                    const std::string &comment_before = "", const std::string &comment_after = "")
                                                                { Comment ct = getOrCreateComment(index);
                                                                    ct = Comment(comment_before, comment_after); }
    void        addComment(const size_t index, const Comment& comment)
                                                                { Comment ct = getOrCreateComment(index);
                                                                    ct = comment; }
    void        addComment_before(const size_t index, const std::string &comment = "")
                                                                { Comment ct = getOrCreateComment(index);
                                                                    ct.before = comment; }
    void        addComment_after(const size_t index, const std::string &comment = "")
                                                                { Comment ct = getOrCreateComment(index);
                                                                    ct.after = comment; }
    //-----
    Comment&    getComment(const std::string& key)
                {
                    auto it = m_comments.find(key);
                    if(it == m_comments.end())
                        throw std::invalid_argument("key '" + key + "' not found");
                    return it->second;
                }
    Comment&    getComment(const size_t index)
                {
                    __CHECK_INDEX_BOUND2__(m_values, index);
                    auto it = m_comments.find(m_values[index].first);
                    if(it == m_comments.end())
                        throw std::invalid_argument("key for index '" + std::to_string(index) + "' not found");
                    return it->second;
                }
    Comment&    getOrCreateComment(const std::string& key)
                {
                    auto it = m_comments.find(key);
                    if(it == m_comments.end())
                        m_comments.insert(std::make_pair(key, Comment()));
                    return it->second;
                }
    Comment&    getOrCreateComment(const size_t index)
                {
                    __CHECK_INDEX_BOUND2__(m_values, index);
                    auto it = m_comments.find(m_values[index].first);
                    if(it == m_comments.end())
                        m_comments.insert(std::make_pair(m_values[index].first, Comment()));
                    return it->second;
                }
    //-----
    void        clearPreviewComment()                           { m_preview_comment = {}; }
    void        clearComment(const std::string& key)            { m_comments.erase(key); }
    void        clearComment(const size_t index)                { m_comments.erase(m_values[index].first); }
    //----------------------------------------------------------------------------------
};
// ======================================================================================== Json
// *
// *
// STATIC FUNCTIONS ============================================================================
enum class CommentBool { //TODO: переименовать
    eNotComment,
    eOneLineComment,
    eMultiLineComment
};
static CommentBool  CheckComment(char& first, const char second, size_t& iterator);
static ValueType    CheckValue(std::string& value);
static bool         CheckDouble(std::string& value);
static bool         CheckBool(std::string& value);
static bool         CheckString(std::string& value);
static bool         CheckJson(std::string& value);
static bool         CheckArray(std::string& value);
static void         RemoveIllegalSpaces(std::string& string);
static std::string  ToComment(const std::string& comment_string, const uint8_t tabulation_level = 0, const uint8_t column_size = 0);
/*static*/ std::string  FromComment(const std::string& comment_string, uint8_t& column_size);
// ============================================================================ STATIC FUNCTIONS
// *
// *
// Element (продолжение) =======================================================================
class JsonElement : BaseElement {
public:
    Json m_value;

    JsonElement()                                       {}
    JsonElement(const Json& j) : m_value(j)             {}
    ~JsonElement()                                      {}

    std::string to_string(int16_t tabulation_level = 0,
                          const PrintType print_type = PrintType::eWithoutComment)
                                                        { return m_value.to_string(tabulation_level, print_type); }
};
class JArrayElement : BaseElement {
public:
    JArray m_value;

    JArrayElement()                                     {}
    JArrayElement(const JArray& a) : m_value(a)         {}
    ~JArrayElement()                                    {}

    std::string to_string(int16_t tabulation_level = 0,
                          const PrintType print_type = PrintType::eWithoutComment)
                                                        { return m_value.to_string(tabulation_level, print_type); }
};
// ======================================================================= Element (продолжение)


#endif // JSON_H
