#ifndef JSON_H
#define JSON_H

#include "utils.h"
#include <map>
#include <memory>
#include <vector>

//TODO: add "noexcept"
//namespace json {

//std::is_integral<ARG>::value
#define __ONLY_ALLOWED_TYPES__(ARG) \
    template<typename ARG, \
        typename std::enable_if< \
            std::is_same<ARG, double>::value \
            || std::is_same<ARG, bool>::value \
            || std::is_same<ARG, std::string>::value \
            || std::is_same<ARG, const char*>::value \
            || std::is_same<ARG, Json>::value \
            || std::is_same<ARG, Array>::value \
        >::type* = nullptr>

enum ValueType {
    eNull,

    eNumber,
    eBool,
    eString,
    eJson,
    eArray
};
static std::string to_string(const ValueType type);

class Json;
class BaseElement {
public:
    virtual ~BaseElement(){}
    virtual std::string to_string(int16_t tabultation_level) = 0;
};

class Array;
class DoubleElement;
class BoolElement;
class StringElement;
class JsonElement;
class ArrayElement;
struct Element {
    ValueType       first;
    BaseElement*    second;

                Element() : first(ValueType::eNull), second(nullptr) {}
                Element(ValueType type, BaseElement* ptr) : first(type), second(ptr) {}
                Element(const double value);
                Element(const bool value);
                Element(const std::string value);
                Element(const char* value);
                Element(const Json& value);
                Element(const Array& value);

    bool        operator==(const Element& other) const;
    bool        operator!=(const Element& other) const;

    double      getNum();
    bool        getBool();
    std::string getString();
    Json        getJson();
    Array       getArray();

    Element     getInnerValue(std::string name);
    Element     getInnerValue(size_t index);
};

//TODO: общий тип для всех числовых значений JSON

using AVector = std::vector<Element>;
// Упорядоченный список значений
class Array {
    AVector m_values;

    bool        checkIndexes(const size_t index);
public:
                Array()                             {}
                Array(const Array& array);
                template<typename ... Types>
                Array(Types... args)                { push_back(args...); }
                ~Array();

    bool        parseArray(const std::string& str);

                __ONLY_ALLOWED_TYPES__(T)
    Array&      push_front(const T value)           { m_values.insert(m_values.cbegin(), Element(value));
                                                        return *this; }
                __ONLY_ALLOWED_TYPES__(T)
    Array&      push_back(const T value)            { m_values.push_back(Element(value));
                                                        return *this; }

    ValueType   getType(const size_t index)         { return m_values[index].first; }
    ValueType   getTypeFront(const size_t index)    { return getType(0); }
    ValueType   getTypeBack(const size_t index)     { return getType(m_values.size() - 1); }

    Element     getAt(const size_t index)           { return m_values[index]; }
    Element     getFront()                          { return m_values.front(); }
    Element     getBack()                           { return m_values.back(); }

    void        popBack()                           { m_values.pop_back(); }
    void        clear()                             { m_values.clear(); }

    std::string to_string(int16_t tabulation_level = 0);

    size_t      size()                        const { return m_values.size(); }
    bool        isEmpty()                           { return m_values.size() == 0; }

    bool        operator==(const Array& other) const;

    Element     operator[](const size_t index);
    Element     operator[](const std::vector<std::string>& complex_name);
                template<std::size_t SIZE>
    Element     operator[](const std::array<std::string, SIZE>& complex_name)
    {
        if(m_values.empty()) return {};

        Element el = (*this)[complex_name[0]]; //находим первый элемент списка
        typename std::array<std::string, SIZE>::const_iterator it = complex_name.begin() + 1; //первый элемент пропускаем
        for (; el.first != ValueType::eNull && it != complex_name.end(); it++) {
            bool isNumber = utils::isNumber(*it, false);
            switch(el.first) {
            case eJson:
                el = el.getInnerValue(*it);
                if(el.first == ValueType::eNull) {
                    if(isNumber)    el = el.getInnerValue(stoi(*it));
                    else            el = {};
                }
                break;
            case eArray:
                //для массива возможно обращение только по числовому индексу!
                if(isNumber)    el = el.getInnerValue(stoi(*it));
                else            el = {};
                break;
            default: return {}; //продолжать поиск можно только по двум структурам!
            }
        }

        return el;
    }

    Element     value(const size_t index)           { return (*this)[index]; }
    Element     value(const std::vector<std::string>& complex_name)
                                                    { return (*this)[complex_name]; }

    AVector::iterator begin()                       { return m_values.begin(); }
    AVector::iterator end()                         { return m_values.end(); }
    AVector::const_iterator cbegin()          const { return m_values.cbegin(); }
    AVector::const_iterator cend()            const { return m_values.cend(); }

    //если индекс больше количества вложенных элементов, то добавятся в конец
                __ONLY_ALLOWED_TYPES__(T)
    Array&      insert(const size_t index, const T value)
    {
        if(index > m_values.size() - 1)
            this->push_back(value);
        else
            m_values.insert(m_values.cbegin() + index, Element(value));
        return *this;
    }

                __ONLY_ALLOWED_TYPES__(T)
    Array&      insert(AVector::iterator iterator, const T value)
                                                    { m_values.insert(iterator, value);
                                                        return *this; }

    Array&      erase(const size_t index);
    Array&      erase(const AVector::iterator iterator)
                                                    { m_values.erase(m_values.cbegin());
                                                        return *this; }
    Array&      erase(const AVector::iterator begin, const AVector::iterator end)
                                                    { m_values.erase(begin, end);
                                                        return *this; }
}; /// class Array

using JPair     = std::pair<std::string, Element>;
using JVector   = std::vector<JPair>;
// Неупорядоченный список "ключ-значение" (в данном случае упорядочен)
class Json
{
    JVector m_values;

    bool        checkIndexes(const size_t index);
public:
                Json()                                          {}
                Json(const Json& json);
                Json(const std::string& json_string)            { this->parseJson(json_string); }
                __ONLY_ALLOWED_TYPES__(T)
                Json(const std::string& key, const T& value)    { put(key, value); }
                ~Json();

    Json&       operator=(const Json& other);

                __ONLY_ALLOWED_TYPES__(T)
    Json&        put(const std::string& key, const T value, const bool rewrite = true)
    {
        if(!contains(key))
            m_values.push_back(JPair(key, Element(value)));
        else if(rewrite)
            updateValue(key, value);
        return *this;
    }

    Json&       put(const Json& json, const bool rewrite = true);

                __ONLY_ALLOWED_TYPES__(T)
    Json&       add(const std::string& key, const T& value, const bool rewrite = true)
                                                                { return this->put(key, value, rewrite); }
    Json&       add(const Json& json, const bool rewrite = true)
                                                                { return this->put(json, rewrite); }
    Json&       append(const Json& json, const bool rewrite = true)
                                                                { return this->put(json, rewrite); }

    bool        parseJson(const std::string& str);
    bool        readFile(const std::string& path);
    bool        writeFile(const std::string& path, int16_t tabulation_level = 0);

    std::string to_string(int16_t tabulation_level = 0) const;
    size_t      size()                                    const { return m_values.size(); }
    bool        isEmpty()                                       { return m_values.size() == 0; }
    bool        contains(const std::string& key);
                __ONLY_ALLOWED_TYPES__(T)//TODO: тест на эту функцию
    Json&       updateValue(const std::string& key, const T& new_value) {
        delete (*this)[key].second;
        (*this)[key] = Element(new_value);
        return *this;
    }
    Json&        clear()                                        { m_values.clear();
                                                                    return *this; }

    JVector::iterator begin()                                   { return m_values.begin(); }
    JVector::iterator end()                                     { return m_values.end(); }
    JVector::const_iterator cbegin()                      const { return m_values.begin(); }
    JVector::const_iterator cend()                        const { return m_values.end(); }

    bool        operator==(const Json& other) const;

    Element     operator[](const size_t index);
    Element     operator[](const std::string& name);
    Element     operator[](const std::vector<std::string>& complex_name);
    template<std::size_t SIZE>
    Element     operator[](const std::array<std::string, SIZE>& complex_name)
    {
        if(m_values.empty()) return {};

        Element el = (*this)[complex_name[0]]; //находим первый элемент списка
        typename std::array<std::string, SIZE>::const_iterator it = complex_name.begin() + 1; //первый элемент пропускаем
        for (; el.first != ValueType::eNull && it != complex_name.end(); it++) {
            bool isNumber = utils::isNumber(*it, false);
            switch(el.first) {
            case eJson:
                el = el.getInnerValue(*it);
                if(el.first == ValueType::eNull) {
                    if(isNumber)    el = el.getInnerValue(stoi(*it));
                    else            el = {};
                }
                break;
            case eArray:
                //для массива возможно обращение только по числовому индексу!
                if(isNumber)    el = el.getInnerValue(stoi(*it));
                else            el = {};
                break;
            default: return {}; //продолжать поиск можно только по двум структурам!
            }
        }

        return el;
    }
    Element     value(const size_t index)                       { return (*this)[index]; }
    Element     value(const std::string& name)                  { return (*this)[name]; }
    Element     value(const std::vector<std::string>& complex_name)
                                                                { return (*this)[complex_name]; }

    //положить значение в указанную позицию
    //если значение существует и флаг поднят - удалить существующее значение
    //если индекс больше количества вложенных элементов, то добавятся в конец
                __ONLY_ALLOWED_TYPES__(T)
    Json&       insert(const size_t index, const std::string& key,
                       const T value, const bool rewrite = true)
                {
                    if(contains(key) && rewrite) {
                        erase(key);
                    } else {
                        if(index > m_values.size() - 1)
                            this->put(key, value);
                        else
                            m_values.insert(
                                m_values.cbegin() + index,
                                std::make_pair(key, Element(value)));
                    }
                    return *this;
                }

    //если индекс больше количества вложенных элементов, то добавятся в конец
                __ONLY_ALLOWED_TYPES__(T)
    Json&       insert(JVector::iterator iterator, const std::string& key,
                       const T value, const bool rewrite = true)
                {
                    if(!this->contains(key))
                        m_values.insert(iterator, std::make_pair(key, Element(value)), rewrite);

                    return *this;
                }

    //если индекс больше количества вложенных элементов, то добавятся в конец
                __ONLY_ALLOWED_TYPES__(T)
    Json&       insertBefore(const std::string& keyIndex, const std::string& key, const T value)
    {
        if(this->contains(key)) return false; //без дубликатов

        //поиск индекса указанного ключа
        for(JPair& it : m_values) {
            if(it.first == keyIndex) {
//FIXME:                this->insert(it, std::make_pair(key, value));
                return true;
            }
        }
        return false;
    }

    //если индекс больше количества вложенных элементов, то добавятся в конец
                __ONLY_ALLOWED_TYPES__(T)
    Json&       insertAfter(const std::string& keyIndex, const std::string& key, const T value)
    {
        if(this->contains(key)) return false; //без дубликатов

        //поиск индекса указанного ключа
        for(JPair& it : m_values) {
            if(it.first == keyIndex) {
//FIXME:                this->insert(it + 1, std::make_pair(key, value));
                return true;
            }
        }
        return false;

    }

    Json&       erase(const size_t index);
    Json&       erase(const JVector::iterator iterator)
                                                                { m_values.erase(m_values.cbegin());
                                                                    return *this; }
    Json&       erase(const JVector::iterator begin, const JVector::iterator end)
                                                                { m_values.erase(begin, end);
                                                                    return *this; }
    Json&       erase(const std::string& key);
    Json&       erase(const std::vector<std::string>& keys);
}; ///class Json

static ValueType    CheckValue(std::string& value);
static bool         CheckDouble(std::string& value);
static bool         CheckBool(std::string& value);
static bool         CheckString(std::string& value);
static bool         CheckJson(std::string& value);
static bool         CheckArray(std::string& value);

class DoubleElement : BaseElement {
public:
    double m_value;

    DoubleElement()                                     {}
    DoubleElement(const double& d) : m_value(d)         {}
    ~DoubleElement()                                    {}

    std::string to_string(int16_t tabulation_level = 0) { return utils::toString(m_value); }
};
class BoolElement : BaseElement {
public:
    bool m_value;

    BoolElement()                                       {}
    BoolElement(const bool& b) : m_value(b)             {}
    ~BoolElement()                                      {}

    std::string to_string(int16_t tabulation_level = 0) { return m_value ? "true" : "false"; }
};
class StringElement : BaseElement {
public:
    std::string m_value;

    StringElement()                                     {}
    StringElement(const std::string& s) : m_value(s)    {}
    ~StringElement()                                    {}

    std::string to_string(int16_t tabulation_level = 0) { return "\"" + m_value + "\""; }
};
class JsonElement : BaseElement {
public:
    Json m_value;

    JsonElement()                                       {}
    JsonElement(const Json& j) : m_value(j)             {}
    ~JsonElement()                                      {}

    std::string to_string(int16_t tabulation_level = 0) { return m_value.to_string(tabulation_level); }
};
class ArrayElement : BaseElement {
public:
    Array m_value;

    ArrayElement()                                      {}
    ArrayElement(const Array& a) : m_value(a)           {}
    ~ArrayElement()                                     {}

    std::string to_string(int16_t tabulation_level = 0) { return m_value.to_string(tabulation_level); }
};

//} /// namespace json

#endif // JSON_H
