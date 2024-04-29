#ifndef JSON_H
#define JSON_H

#include "utils.h"
#include <map>
#include <memory>
#include <vector>

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
    double* getNum();
    bool*   getBool();
    std::string* getString();
    Json*   getJson();
    Array*  getArray();
};

// Упорядоченный список значений
class Array {
    std::vector<Element> values;

    bool checkIndexes(const size_t index) {
        if(index + 1 > values.size()) {
            throw "Going beyond Array boundaries";
            return false;
        }
        return true;
    }
public:
    Array(){};
    Array(const Array& array);
    ~Array();

    void push_back(double d);
    void push_back(bool b);
    void push_back(std::string string);
    void push_back(Json& json);
    void push_back(Array& array);

//TODO:    void push_front(double d);
//TODO:    void push_front(bool b);
//TODO:    void push_front(std::string string);
//TODO:    void push_front(Json& json);
//TODO:    void push_front(Array& array);

    ValueType getType(size_t index)         { return this->values[index].first; }
    ValueType getTypeFront(size_t index)    { return getType(0); }
    ValueType getTypeBack(size_t index)     { return getType(this->values.size() - 1); }

    Element getAt(size_t index)             { return this->values[index]; }
    Element getFront()                      { return this->values.front(); }
    Element getBack()                       { return this->values.back(); }

    void popBack()                          { this->values.pop_back(); }
    void clear()                            { this->values.clear(); }

    std::string to_string(int16_t tabulation_level = 0);
    size_t size() { return values.size(); }
    \
    Element operator[](const size_t index) {
        if(this->values.empty()) return {};
        if(!checkIndexes(index)) return {};

        return Element(this->values[index].first, this->values[index].second);
    }
    Element operator[](std::vector<std::string> complex_name);
    Element value(const size_t index)                   { return (*this)[index]; }
    Element value(std::vector<std::string> complex_name){ return (*this)[complex_name]; }

    std::vector<Element>::iterator begin()              { return values.begin(); }
    std::vector<Element>::iterator end()                { return values.end(); }
    std::vector<Element>::const_iterator cbegin() const { return values.begin(); }
    std::vector<Element>::const_iterator cend()   const { return values.end(); }

//TODO:    void insert(size_t index, double value)
//TODO:    void insert(size_t index, bool value)
//TODO:    void insert(size_t index, std::string value)
//TODO:    void insert(size_t index, char* value)
//TODO:    void insert(size_t index, Json value)
//TODO:    void insert(size_t index, Array value)
//TODO:    insert(iterator, ...)
//TODO:    insert(iterator, std::vector<Element>{...})

//TODO:    void erase(size_t index)
//TODO:    void erase(iterator)
//TODO:    void erase(begin, end)
}; /// class Array

// Неупорядоченный список "ключ-значение"
class Json
{
    std::vector<std::pair<std::string, Element>> values;

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
    ~Json();
    bool put(const std::string key, const double value);
    bool put(const std::string key, const bool value);
    bool put(const std::string key, const std::string value);
    bool put(const std::string key, const char* value)      { return this->put(key, std::string(value)); }
    bool put(const std::string key, const Json& value);
    bool put(const std::string key, const Array& value);

    bool add(const std::string key, const double value)     { return this->put(key, value); }
    bool add(const std::string key, const bool value)       { return this->put(key, value); }
    bool add(const std::string key, const std::string value){ return this->put(key, value); }
    bool add(const std::string key, const char* value)      { return this->put(key, value); }
    bool add(const std::string key, const Json& value)      { return this->put(key, value); }
    bool add(const std::string key, const Array& value)     { return this->put(key, value); }

    bool readFile(const std::string path);
    bool writeFile(const std::string path, int16_t tabulation_level = 0);

    std::string to_string(int16_t tabulation_level = 0);
    size_t size()   { return values.size(); }
    std::vector<std::pair<std::string, Element>>::iterator begin()
                    { return values.begin(); };
    std::vector<std::pair<std::string, Element>>::iterator end()
                    { return values.end(); };
    std::vector<std::pair<std::string, Element>>::const_iterator cbegin()
                    const { return values.begin(); };
    std::vector<std::pair<std::string, Element>>::const_iterator cend()
                    const { return values.end(); };

    Element operator[](const size_t index);
    Element operator[](std::string name);
    Element operator[](std::vector<std::string> complex_name);
    Element value(const size_t index)                       { return (*this)[index]; }
    Element value(std::string name)                         { return (*this)[name]; }
    Element value(std::vector<std::string> complex_name)    { return (*this)[complex_name]; }

//TODO:    void insert(size_t index, std::string key, double value)
//TODO:    void insert(size_t index, std::string key, bool value)
//TODO:    void insert(size_t index, std::string key, std::string value)
//TODO:    void insert(size_t index, std::string key, char* value)
//TODO:    void insert(size_t index, std::string key, Json value)
//TODO:    void insert(size_t index, std::string key, Array value)
//TODO:    insert(iterator,  std::string key, <...> value)

//TODO:    insertBefore(std::string key, key, value)
//TODO:    insertBefore(std::string key, <key, value>{...})
//TODO:    insertAfter(std::string key, key, value)
//TODO:    insertAfter(std::string key, <key, value>{...})

//TODO:    void erase(size_t index)
//TODO:    void erase(iterator)
//TODO:    void erase(std::string key)
//TODO:    void erase(std::vector<std::string> keys)
//TODO:    void erase(begin, end)
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
    std::string to_string(int16_t tabulation_level = 0)
        { return utils::ToString(value); }
};
class BoolElement : BaseElement {
public:
    bool value;

    BoolElement(){}
    BoolElement(const bool& b) : value(b){};
    std::string to_string(int16_t tabulation_level = 0)
        { return value ? "true" : "false"; }
};
class StringElement : BaseElement {
public:
    std::string value;

    StringElement(){}
    StringElement(const std::string& s) : value(s){};
    std::string to_string(int16_t tabulation_level = 0)
        { return "\"" + value + "\""; }
};
class JsonElement : BaseElement {
public:
    Json value;

    JsonElement(){}
    JsonElement(const Json& j) : value(j){};
    std::string to_string(int16_t tabulation_level = 0)
        { return value.to_string(tabulation_level); }
};
class Array;
class ArrayElement : BaseElement {
public:
    Array value;

    ArrayElement(){}
    ArrayElement(const Array& a) : value(a){};
    std::string to_string(int16_t tabulation_level = 0)
        { return value.to_string(tabulation_level); }
};

} /// namespace json

#endif // JSON_H
