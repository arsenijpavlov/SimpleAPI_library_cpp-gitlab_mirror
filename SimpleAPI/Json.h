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
    virtual std::string to_string(uint8_t tabultation_level) = 0;
};
//typedef std::pair<ValueType, BaseElement*> ArrayElements; //C
using Element = std::pair<ValueType, BaseElement*>;   //C++

// Упорядоченный список значений
class Array {
    std::vector<Element> values;
public:
    Array(){};
    Array(const Array& array);
    ~Array();
    void push_back(double d);
    void push_back(bool b);
    void push_back(std::string string);
    void push_back(Json json);
    void push_back(Array& array);
//TODO:    void push_front(const double d);
//TODO:    void push_front(const bool b);
//TODO:    void push_front(const std::string string);
//TODO:    void push_front(const Json json);
//TODO:    void push_front(const Array array);
    ValueType getType(size_t index);
    ValueType getFrontType(size_t index)    { return getType(0); }
    ValueType getBackType(size_t index)     { return getType(this->values.size() - 1); }
    void* getAt(size_t index);
    void* getFront()                        { return getAt(0); }
    void* getBack()                         { return getAt(this->values.size() - 1); }
//TODO:    void popFront();
//TODO:    void popBack();
    void* operator[](size_t index);
    std::string to_string(int16_t tabulation_level = 0);
    size_t size() { return values.size(); }
};

// Неупорядоченный список "ключ-значение"
class Json
{
//    std::map<std::string, double>       numbers;
//    std::map<std::string, bool>         bools;
//    std::map<std::string, std::string>  strings;
//    std::map<std::string, Json>         jsons;
//    std::map<std::string, Array>        arrays;
    std::map<std::string, Element> values;
public:
//    Json();
    Json(const Json& json);
    ~Json();
    bool put(const std::string key, const double value);
    bool put(const std::string key, const bool value);
    bool put(const std::string key, const std::string value);
    bool put(const std::string key, const Json value);
    bool put(const std::string key, const Array& value);
    bool add(const std::string key, const std::string value)    { return this->put(key, value); };
    bool add(const std::string key, const bool value)           { return this->put(key, value); };
    bool add(const std::string key, const double value)         { return this->put(key, value); };
    bool add(const std::string key, const Json value)           { return this->put(key, value); };
    bool add(const std::string key, const Array& value)         { return this->put(key, value); };

    bool readFile(const std::string path);
    bool writeFile(const std::string path);
    std::string to_string(int16_t tabulation_level = 0);
    size_t size() {
//        return numbers.size() + bools.size() + strings.size()
//               + jsons.size() + arrays.size();
        return values.size();
    }
    std::map<std::string, Element>::iterator begin(){ return values.begin(); };
    std::map<std::string, Element>::iterator end()  { return values.end(); };
    std::map<std::string, Element>::const_iterator cbegin() const { return values.begin(); };
    std::map<std::string, Element>::const_iterator cend()   const { return values.end(); };
};

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
    DoubleElement(double d) : value(d) {};
    std::string to_string(uint8_t tabulation_level = 0)
        { return utils::ToString(value); }
};
class BoolElement : BaseElement {
public:
    uint8_t value;

    BoolElement(){}
    BoolElement(bool b) : value(b) {}
    std::string to_string(uint8_t tabulation_level = 0)
        { return ((bool)value) ? "true" : "false"; }
};
class StringElement : BaseElement {
public:
    std::string value;

    StringElement(){}
    StringElement(std::string s) : value(s) {}
    std::string to_string(uint8_t tabulation_level = 0)
        { return "\"" + value + "\""; }
};
class JsonElement : BaseElement {
public:
    Json value;

    JsonElement(){}
    JsonElement(Json j) : value(j) {}
    std::string to_string(uint8_t tabulation_level = 0)
        { return value.to_string(tabulation_level); }
};
class Array;
class ArrayElement : BaseElement {
public:
    Array value;

    ArrayElement(){}
    ArrayElement(Array a) : value(a) {}
    std::string to_string(uint8_t tabulation_level = 0)
        { return value.to_string(tabulation_level); }
};

} /// namespace json

#endif // JSON_H
