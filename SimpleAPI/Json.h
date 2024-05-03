#ifndef JSON_H
#define JSON_H

#include "utils.h"
#include <map>
#include <memory>
#include <vector>

//TODO: add "noexcept"
namespace json {

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
    Element(const Json& value);
    Element(const Array& value);

    double*         getNum();
    bool*           getBool();
    std::string*    getString();
    Json*           getJson();
    Array*          getArray();

    Element getInnerValue(std::string name);
    Element getInnerValue(size_t index);
};

using AVector = std::vector<Element>;
// Упорядоченный список значений
class Array {
    AVector values;

    bool checkIndexes(const size_t index) {
        if(index + 1 > values.size()) {
            //TODO: std::outofrange
            throw "Going beyond Array boundaries";
            return false;
        }
        return true;
    }
public:
    Array(){};
    Array(const Array& array);
    ~Array() {
        for(Element& el : this->values)
            delete el.second;
    }

    void push_back(const double value)
    { this->values.push_back(Element(value)); }
    void push_back(const bool value)
    { this->values.push_back(Element(value)); }
    void push_back(const std::string value)
    { this->values.push_back(Element(value)); }
    void push_back(const char* value)
    { push_back(std::string(value)); }
    void push_back(const Json& value)
    { this->values.push_back(Element(value)); }
    void push_back(const Array& value)
    { this->values.push_back(Element(value)); }

    void push_front(const double value)
    { this->values.insert(this->values.cbegin(), Element(value)); }
    void push_front(const bool value)
    { this->values.insert(this->values.cbegin(), Element(value)); }
    void push_front(const std::string value)
    { this->values.insert(this->values.cbegin(), Element(value)); }
    void push_front(const char* value)
    { push_front(std::string(value)); }
    void push_front(const Json& value)
    { this->values.insert(this->values.cbegin(), Element(value)); }
    void push_front(const Array& value)
    { this->values.insert(this->values.cbegin(), Element(value)); }

    ValueType getType(const size_t index)       { return this->values[index].first; }
    ValueType getTypeFront(const size_t index)  { return getType(0); }
    ValueType getTypeBack(const size_t index)   { return getType(this->values.size() - 1); }

    Element getAt(const size_t index)           { return this->values[index]; }
    Element getFront()                          { return this->values.front(); }
    Element getBack()                           { return this->values.back(); }

    void popBack()                              { this->values.pop_back(); }
    void clear()                                { this->values.clear(); }

    std::string to_string(int16_t tabulation_level = 0);
    size_t size()                               { return values.size(); }

    Element operator[](const size_t index)
    {
        if(this->values.empty()) return {};
        if(!checkIndexes(index)) return {};

        return Element(this->values[index].first, this->values[index].second);
    }
    Element operator[](const std::vector<std::string>& complex_name)
    {
        if(this->values.empty()) return {};

        std::vector<std::string>::const_iterator it = complex_name.begin();
        if(!utils::IsNumber(*it++, false))
            return {};
        Element el = (*this)[stoi(*it)]; //находим первый элемент списка
        for (; el.first != ValueType::eNull && it != complex_name.end(); it++) {
            bool isNumber = utils::IsNumber(*it, false);
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
    template<std::size_t SIZE>
    Element operator[](const std::array<std::string, SIZE>& complex_name)
    {
        if(this->values.empty()) return {};

        Element el = (*this)[complex_name[0]]; //находим первый элемент списка
        typename std::array<std::string, SIZE>::const_iterator it = complex_name.begin() + 1; //первый элемент пропускаем
        for (; el.first != ValueType::eNull && it != complex_name.end(); it++) {
            bool isNumber = utils::IsNumber(*it, false);
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

    Element value(const size_t index)           { return (*this)[index]; }
    Element value(const std::vector<std::string>& complex_name)
                                                { return (*this)[complex_name]; }

    AVector::iterator begin()                   { return values.begin(); }
    AVector::iterator end()                     { return values.end(); }
    AVector::const_iterator cbegin()      const { return values.begin(); }
    AVector::const_iterator cend()        const { return values.end(); }

    //если индекс больше количества вложенных элементов, то добавятся в конец
    void insert(const size_t index, const double value)
    {
        if(index > this->values.size() - 1)
            this->push_back(value);
        else
            this->values.insert(this->values.cbegin() + index, Element(value));
    }
    void insert(const size_t index, const bool value)
    {
        if(index > this->values.size() - 1)
            this->push_back(value);
        else
            this->values.insert(this->values.cbegin() + index, Element(value));
    }
    void insert(const size_t index, const std::string value)
    {
        if(index > this->values.size() - 1)
            this->push_back(value);
        else
            this->values.insert(this->values.cbegin() + index, Element(value));
    }
    void insert(const size_t index, const char* value)
    { this->insert(index, std::string(value)); }
    void insert(const size_t index, const Json* value)
    {
        if(index > this->values.size() - 1)
            this->push_back(value);
        else
            this->values.insert(this->values.cbegin() + index, Element(value));
    }
    void insert(const size_t index, const Array* value)
    {
        if(index > this->values.size() - 1)
            this->push_back(value);
        else
            this->values.insert(this->values.cbegin() + index, Element(value));
    }

    void insert(AVector::iterator iterator, const double value)
    { this->values.insert(iterator, value); }
    void insert(AVector::iterator iterator, const bool value)
    { this->values.insert(iterator, value); }
    void insert(AVector::iterator iterator, const std::string value)
    { this->values.insert(iterator, value); }
    void insert(AVector::iterator iterator, const char* value)
    { this->values.insert(iterator, std::string(value)); }
    void insert(AVector::iterator iterator, const Json* value)
    { this->values.insert(iterator, value); }
    void insert(AVector::iterator iterator, const Array* value)
    { this->values.insert(iterator, value); }

    void erase(const size_t index)
    {
        if(index > this->values.size() - 1) return;

        this->values.erase(this->values.cbegin() + index);
    }
    void erase(const AVector::iterator iterator)
    { this->values.erase(this->values.cbegin()); }
    void erase(const AVector::iterator begin, const AVector::iterator end)
    { this->values.erase(begin, end); }
}; /// class Array

using JVector = std::vector<std::pair<std::string, Element>>;
// Неупорядоченный список "ключ-значение"
class Json
{
    JVector values;

    bool checkIndexes(const size_t index) {
        if(index + 1 > values.size()) {
            throw "Going beyond Json boundaries";
            return false;
        }
        return true;
    }
public:
    Json(){};
    Json(const Json& json);
    ~Json() {
        for(std::pair<std::string, Element>& el : this->values)
            delete el.second.second;
    }

    bool put(const std::string& key, const double value)
    {
        if(!isValueExists(key)) {
            this->values.push_back(std::pair<std::string, Element>(key, Element(value)));
            return true;
        } else
            return false;
    }
    bool put(const std::string& key, const bool value)
    {
        if(!isValueExists(key)) {
            this->values.push_back(std::pair<std::string, Element>(key, Element(value)));
            return true;
        } else
            return false;
    }
    bool put(const std::string& key, const std::string value)
    {
        //NOTE: (возможно) экранирование спецсимволов, как минимум кавычек
        if(!isValueExists(key)) {
            this->values.push_back(std::pair<std::string, Element>(key, Element(value)));
            return true;
        } else
            return false;
    }
    bool put(const std::string& key, const char* value)
    { return this->put(key, std::string(value)); }
    bool put(const std::string& key, const Json& value)
    {
        if(!isValueExists(key)) {
            this->values.push_back(std::pair<std::string, Element>(key, Element(value)));
            return true;
        } else
            return false;
    }
    bool put(const std::string& key, const Array& value)
    {
        if(!isValueExists(key)) {
            this->values.push_back(std::pair<std::string, Element>(key, Element(value)));
            return true;
        } else
            return false;
    }

    bool add(const std::string& key, const double value)     { return this->put(key, value); }
    bool add(const std::string& key, const bool value)       { return this->put(key, value); }
    bool add(const std::string& key, const std::string value){ return this->put(key, value); }
    bool add(const std::string& key, const char* value)      { return this->put(key, value); }
    bool add(const std::string& key, const Json& value)      { return this->put(key, value); }
    bool add(const std::string& key, const Array& value)     { return this->put(key, value); }

    bool isValueExists(const std::string& name);

    bool readFile(const std::string& path);
    bool writeFile(const std::string& path, int16_t tabulation_level = 0);

    std::string to_string(int16_t tabulation_level = 0);
    size_t size()                       { return values.size(); }
    JVector::iterator begin()           { return values.begin(); };
    JVector::iterator end()             { return values.end(); };
    JVector::const_iterator cbegin()    const { return values.begin(); };
    JVector::const_iterator cend()      const { return values.end(); };

    Element operator[](const size_t index)
    {
        if(this->values.empty()) return {};
        if(!checkIndexes(index)) return {};

        return Element(this->values[index].second.first, this->values[index].second.second);
    }
    Element operator[](const std::string& name)
    {
        if(this->values.empty()) return {};

        for(size_t i = 0; i < this->values.size(); i++)
            if(this->values[i].first == name)
                return Element(
                    this->values[i].second.first,
                    this->values[i].second.second);
        return {};
    }
    Element operator[](const std::vector<std::string>& complex_name)
    {
        if(this->values.empty()) return {};

        Element el = (*this)[complex_name[0]]; //находим первый элемент списка
        std::vector<std::string>::const_iterator it = complex_name.begin() + 1; //первый элемент пропускаем
        for (; el.first != ValueType::eNull && it != complex_name.end(); it++) {
            bool isNumber = utils::IsNumber(*it, false);
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
    template<std::size_t SIZE>
    Element operator[](const std::array<std::string, SIZE>& complex_name)
    {
        if(this->values.empty()) return {};

        Element el = (*this)[complex_name[0]]; //находим первый элемент списка
        typename std::array<std::string, SIZE>::const_iterator it = complex_name.begin() + 1; //первый элемент пропускаем
        for (; el.first != ValueType::eNull && it != complex_name.end(); it++) {
            bool isNumber = utils::IsNumber(*it, false);
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

    void insert(const size_t index, const std::string& key, const double value)
    {
        //без дубликатов
        if(this->isValueExists(key)) return;

        if(index > this->values.size() - 1) {
            this->put(key, value);
        } else {
            this->values.insert(
                this->values.cbegin() + index,
                std::make_pair(key, Element(value)));
        }
    }
    void insert(const size_t index, const std::string& key, const bool value)
    {
        //без дубликатов
        if(this->isValueExists(key)) return;

        if(index > this->values.size() - 1) {
            this->put(key, value);
        } else {
            this->values.insert(
                this->values.cbegin() + index,
                std::make_pair(key, Element(value)));
        }
    }
    void insert(const size_t index, const std::string& key, const std::string value)
    {
        //без дубликатов
        if(this->isValueExists(key)) return;

        if(index > this->values.size() - 1) {
            this->put(key, value);
        } else {
            this->values.insert(
                this->values.cbegin() + index,
                std::make_pair(key, Element(value)));
        }
    }
    void insert(const size_t index, const std::string& key, const char* value)
    { this->insert(index, key, std::string(value)); }
    void insert(const size_t index, const std::string& key, const Json* value)
    {
        //без дубликатов
        if(this->isValueExists(key)) return;

        if(index > this->values.size() - 1) {
            this->put(key, value);
        } else {
            this->values.insert(
                this->values.cbegin() + index,
                std::make_pair(key, Element(value)));
        }
    }
    void insert(const size_t index, const std::string& key, const Array* value)
    {
        //без дубликатов
        if(this->isValueExists(key)) return;

        if(index > this->values.size() - 1) {
            this->put(key, value);
        } else {
            this->values.insert(
                this->values.cbegin() + index,
                std::make_pair(key, Element(value)));
        }
    }

    void insert(JVector::iterator iterator, const std::string& key, const double value)
    {
        //без дубликатов
        if(this->isValueExists(key)) return;

        this->values.insert(iterator, std::make_pair(key, Element(value)));
    }
    void insert(JVector::iterator iterator, const std::string& key, const bool value)
    {
        //без дубликатов
        if(this->isValueExists(key)) return;

        this->values.insert(iterator, std::make_pair(key, Element(value)));
    }
    void insert(JVector::iterator iterator, const std::string& key, const std::string value)
    {
        //без дубликатов
        if(this->isValueExists(key)) return;

        this->values.insert(iterator, std::make_pair(key, Element(value)));
    }
    void insert(JVector::iterator iterator, const std::string& key, const char* value)
    { insert(iterator, key, std::string(value)); }
    void insert(JVector::iterator iterator, const std::string& key, const Json* value)
    {
        //без дубликатов
        if(this->isValueExists(key)) return;

        this->values.insert(iterator, std::make_pair(key, Element(value)));
    }
    void insert(JVector::iterator iterator, const std::string& key, const Array* value)
    {
        //без дубликатов
        if(this->isValueExists(key)) return;

        this->values.insert(iterator, std::make_pair(key, Element(value)));
    }

//TODO:    insertBefore(std::string key, key, value)
//TODO:    insertBefore(std::string key, <key, value>{...})
//TODO:    insertAfter(std::string key, key, value)
//TODO:    insertAfter(std::string key, <key, value>{...})

    void erase(const size_t index)
    {
        if(index > this->values.size() - 1) return;

        this->values.erase(this->values.cbegin() + index);
    }
    void erase(const JVector::iterator iterator)
    { this->values.erase(this->values.cbegin()); }
    void erase(const JVector::iterator begin, const JVector::iterator end)
    { this->values.erase(begin, end); }
    void erase(const std::string& key)
    {
        bool flag = false;
        size_t index;
        for(index = 0; index < this->size(); index++) {
            if(this->values[index].first == key) {
                flag = true;
                break;
            }
        }

        if(flag) this->values.erase(this->values.cbegin() + index);
    }
    void erase(const std::vector<std::string>& keys)
    {
        for(const std::string &key : keys)
            this->erase(key);
    }
}; ///class Json

static ValueType CheckValue(std::string& value);
static bool CheckDouble(std::string& value);
static bool CheckBool(std::string& value);
static bool CheckString(std::string& value);
static bool CheckJson(std::string& value);
static bool CheckArray(std::string& value);
static bool ParseJson(const std::string& str, Json* json);
static bool ParseArray(const std::string& str, Array* array);

class DoubleElement : BaseElement {
public:
    double value;

    DoubleElement(){}
    DoubleElement(const double& d) : value(d){};
    ~DoubleElement(){}

    std::string to_string(int16_t tabulation_level = 0)
        { return utils::ToString(value); }
};
class BoolElement : BaseElement {
public:
    bool value;

    BoolElement(){}
    BoolElement(const bool& b) : value(b){};
    ~BoolElement(){}

    std::string to_string(int16_t tabulation_level = 0)
        { return value ? "true" : "false"; }
};
class StringElement : BaseElement {
public:
    std::string value;

    StringElement(){}
    StringElement(const std::string& s) : value(s){};
    ~StringElement(){}

    std::string to_string(int16_t tabulation_level = 0)
        { return "\"" + value + "\""; }
};
class JsonElement : BaseElement {
public:
    Json value;

    JsonElement(){}
    JsonElement(const Json& j) : value(j){};
    ~JsonElement(){}

    std::string to_string(int16_t tabulation_level = 0)
        { return value.to_string(tabulation_level); }
};
class ArrayElement : BaseElement {
public:
    Array value;

    ArrayElement(){}
    ArrayElement(const Array& a) : value(a){};
    ~ArrayElement(){}

    std::string to_string(int16_t tabulation_level = 0)
        { return value.to_string(tabulation_level); }
};

} /// namespace json

#endif // JSON_H
