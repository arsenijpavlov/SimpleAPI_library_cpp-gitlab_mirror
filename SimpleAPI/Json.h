#ifndef JSON_H
#define JSON_H

#include "utils.h"
#include <map>
#include <memory>
#include <vector>

//TODO: add "noexcept"
//namespace json {

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
static std::string ToString(const ValueType type);

class Json;
class BaseElement {
public:
    virtual ~BaseElement(){};
    virtual std::string to_string(int16_t tabultation_level) = 0;
};

class Array;
class DoubleElement;
class BoolElement;
class StringElement;
class JsonElement;
class ArrayElement;
struct Element {
    ValueType first;
    BaseElement* second;

    Element() : first(ValueType::eNull), second(nullptr) {}
    Element(ValueType type, BaseElement* ptr) : first(type), second(ptr) {}
    Element(const double value);
    Element(const bool value);
    Element(const std::string value);
    Element(const char* value);
    Element(const Json& value);
    Element(const Array& value);

    double      getNum();
    bool        getBool();
    std::string getString();
    Json        getJson();
    Array       getArray();

    Element getInnerValue(std::string name);
    Element getInnerValue(size_t index);
};

using AVector = std::vector<Element>;
// Упорядоченный список значений
class Array {
    AVector values;

    bool checkIndexes(const size_t index);
public:
    Array(){};
    Array(const Array& array);
    ~Array();

    bool parseArray(const std::string& str);

    __ONLY_ALLOWED_TYPES__(T)
    void push_back(const T value)
    { this->values.push_back(Element(value)); }

    __ONLY_ALLOWED_TYPES__(T)
    void push_front(const T value)
    { this->values.insert(this->values.cbegin(), Element(value)); }

    ValueType getType(const size_t index);
    ValueType getTypeFront(const size_t index);
    ValueType getTypeBack(const size_t index);

    Element getAt(const size_t index);
    Element getFront();
    Element getBack();

    void popBack();
    void clear();

    std::string to_string(int16_t tabulation_level = 0);
    size_t size();
    bool isEmpty();

    Element operator[](const size_t index);
    Element operator[](const std::vector<std::string>& complex_name);
    template<std::size_t SIZE>
    Element operator[](const std::array<std::string, SIZE>& complex_name)
    {
        if(this->values.empty()) return {};

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

    Element value(const size_t index);
    Element value(const std::vector<std::string>& complex_name);

    AVector::iterator begin()                   { return values.begin(); }
    AVector::iterator end()                     { return values.end(); }
    AVector::const_iterator cbegin()      const { return values.begin(); }
    AVector::const_iterator cend()        const { return values.end(); }

    //если индекс больше количества вложенных элементов, то добавятся в конец
    __ONLY_ALLOWED_TYPES__(T)
    void insert(const size_t index, const T value)
    {
        if(index > this->values.size() - 1)
            this->push_back(value);
        else
            this->values.insert(this->values.cbegin() + index, Element(value));
    }

    __ONLY_ALLOWED_TYPES__(T)
    void insert(AVector::iterator iterator, const T value)

    { this->values.insert(iterator, value); }

    void erase(const size_t index);
    void erase(const AVector::iterator iterator);
    void erase(const AVector::iterator begin, const AVector::iterator end);
}; /// class Array

using JVector = std::vector<std::pair<std::string, Element>>;
using JMap = std::map<std::string, Element>; //TODO: переделать всё на этот вариант
// Неупорядоченный список "ключ-значение" (в данном случае упорядочен)
class Json
{
    JVector values;

    bool checkIndexes(const size_t index);
public:
    Json();
    Json(const Json& json);
//    TODO: Json(const std::string& json_string);
    ~Json();

    __ONLY_ALLOWED_TYPES__(T)
    bool put(const std::string& key, const T value)
    {
        if(!isValueExists(key)) { //без дубликатов
            this->values.push_back(std::pair<std::string, Element>(key, Element(value)));
            return true;
        } else
            return false;
    }

    __ONLY_ALLOWED_TYPES__(T)
    bool add(const std::string& key, const T value) { return this->put(key, value); }

    bool isValueExists(const std::string& name);

    bool parseJson(const std::string& str);
    bool readFile(const std::string& path);
    bool writeFile(const std::string& path, int16_t tabulation_level = 0);

    std::string to_string(int16_t tabulation_level = 0) const;
    size_t size()                                               { return values.size(); }
    bool isEmpty()                                              { return values.size() == 0; }
    bool contains(const std::string& key);


    JVector::iterator begin()                                   { return values.begin(); }
    JVector::iterator end()                                     { return values.end(); }
    JVector::const_iterator cbegin()                      const { return values.begin(); }
    JVector::const_iterator cend()                        const { return values.end(); }

    Element operator[](const size_t index);
    Element operator[](const std::string& name);
    Element operator[](const std::vector<std::string>& complex_name);
    template<std::size_t SIZE>
    Element operator[](const std::array<std::string, SIZE>& complex_name)
    {
        if(this->values.empty()) return {};

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
    Element value(const size_t index)                           { return (*this)[index]; }
    Element value(const std::string& name)                      { return (*this)[name]; }
    Element value(const std::vector<std::string>& complex_name) { return (*this)[complex_name]; }

    //если индекс больше количества вложенных элементов, то добавятся в конец
    __ONLY_ALLOWED_TYPES__(T)
    bool insert(const size_t index, const std::string& key, const T value)
    {
        if(this->isValueExists(key)) return false; //без дубликатов
        if(index > this->values.size() - 1)
            this->put(key, value);
        else {
            this->values.insert(
                this->values.cbegin() + index,
                std::make_pair(key, Element(value)));
        }
        return true;
    }

    //если индекс больше количества вложенных элементов, то добавятся в конец
    __ONLY_ALLOWED_TYPES__(T)
    bool insert(JVector::iterator iterator, const std::string& key, const T value)
    {
        if(this->isValueExists(key)) return false; //без дубликатов
        this->values.insert(iterator, std::make_pair(key, Element(value)));
        return true;
    }

    //если индекс больше количества вложенных элементов, то добавятся в конец
    __ONLY_ALLOWED_TYPES__(T)
    bool insertBefore(const std::string& keyIndex, const std::string& key, const T value)
    {
        if(this->isValueExists(key)) return false; //без дубликатов

        //поиск индекса указанного ключа
        for(std::pair<std::string, Element>& it : this->values) {
            if(it.first == keyIndex) {
//FIXME:                this->insert(it, std::make_pair(key, value));
                return true;
            }
        }
        return false;
    }

    //если индекс больше количества вложенных элементов, то добавятся в конец
    __ONLY_ALLOWED_TYPES__(T)
    bool insertAfter(const std::string& keyIndex, const std::string& key, const T value)
    {
        if(this->isValueExists(key)) return false; //без дубликатов

        //поиск индекса указанного ключа
        for(std::pair<std::string, Element>& it : this->values) {
            if(it.first == keyIndex) {
//FIXME:                this->insert(it + 1, std::make_pair(key, value));
                return true;
            }
        }
        return false;

    }

    void erase(const size_t index);
    void erase(const JVector::iterator iterator);
    void erase(const JVector::iterator begin, const JVector::iterator end);
    void erase(const std::string& key);
    void erase(const std::vector<std::string>& keys);
}; ///class Json

static ValueType CheckValue(std::string& value);
static bool CheckDouble(std::string& value);
static bool CheckBool(std::string& value);
static bool CheckString(std::string& value);
static bool CheckJson(std::string& value);
static bool CheckArray(std::string& value);

class DoubleElement : BaseElement {
public:
    double value;

    DoubleElement(){}
    DoubleElement(const double& d) : value(d){};
    ~DoubleElement(){}

    std::string to_string(int16_t tabulation_level = 0);
};
class BoolElement : BaseElement {
public:
    bool value;

    BoolElement(){}
    BoolElement(const bool& b) : value(b){};
    ~BoolElement(){}

    std::string to_string(int16_t tabulation_level = 0);
};
class StringElement : BaseElement {
public:
    std::string value;

    StringElement(){}
    StringElement(const std::string& s) : value(s){};
    ~StringElement(){}

    std::string to_string(int16_t tabulation_level = 0);
};
class JsonElement : BaseElement {
public:
    Json value;

    JsonElement(){}
    JsonElement(const Json& j) : value(j){};
    ~JsonElement(){}

    std::string to_string(int16_t tabulation_level = 0);
};
class ArrayElement : BaseElement {
public:
    Array value;

    ArrayElement(){}
    ArrayElement(const Array& a) : value(a){};
    ~ArrayElement(){}

    std::string to_string(int16_t tabulation_level = 0);
};

//} /// namespace json

#endif // JSON_H
