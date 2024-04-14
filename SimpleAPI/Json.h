#ifndef JSON_H
#define JSON_H

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
enum NextReadState {
    eUnknown,

    eKey,
    eValue,
    eColon,
    eComma,
    eJsonStart,
    eJsonEnd,
    eArrayStart,
    eArrayEnd
};
static std::string ToString(const NextReadState state);
static std::string ToString(const ValueType type);
static void ChangeNextState(NextReadState &state, const NextReadState nextState);

class Json;
class BaseElement {
public:
    virtual ~BaseElement(){};
};
typedef std::pair<ValueType, BaseElement*> ArrayElements;

// Упорядоченный список значений
class Array {
    std::vector<ArrayElements> values;
public:
    Array();
    ~Array();
    void push_back(double d);
    void push_back(bool b);
    void push_back(std::string string);
    void push_back(Json json);
    void push_back(Array array);
//    void push_front(const double d);
//    void push_front(const bool b);
//    void push_front(const std::string string);
//    void push_front(const Json json);
//    void push_front(const Array array);
    ValueType getType(size_t index);
    ValueType getFrontType(size_t index)    { return getType(0); }
    ValueType getBackType(size_t index)     { return getType(this->values.size() - 1); }
    void* getAt(size_t index);
    void* getFront()                        { return getAt(0); }
    void* getBack()                         { return getAt(this->values.size() - 1); }
    void* operator[](size_t index);
    std::string to_string(int16_t tabulation_level = 0);
};

// Неупорядоченный список "ключ-значение"
class Json
{
    std::map<std::string, double>       numbers;
    std::map<std::string, bool>         bools;
    std::map<std::string, std::string>  strings;
    std::map<std::string, Json>         jsons;
    std::map<std::string, Array>        arrays;
public:
//    Json();
    ~Json();
    bool put(const std::string key, const double value);
    bool put(const std::string key, const bool value);
    bool put(const std::string key, const std::string value);
    bool put(const std::string key, const Json value);
    bool put(const std::string key, const Array value);
    bool add(const std::string key, const std::string value)    { return this->put(key, value); };
    bool add(const std::string key, const double value)         { return this->put(key, value); };
    bool add(const std::string key, const Json value)           { return this->put(key, value); };
    bool add(const std::string key, const Array value)          { return this->put(key, value); };

    bool readFile(const std::string path);
    bool writeFile(const std::string path);
    std::string to_string(int16_t tabulation_level = 0);
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

    DoubleElement(){};
    DoubleElement(double d) : value(d) {};
};
class BoolElement : BaseElement {
public:
    uint8_t value;

    BoolElement(){};
    BoolElement(bool b) : value(b) {};
};
class StringElement : BaseElement {
public:
    std::string value;

    StringElement(){};
    StringElement(std::string s) : value(s) {};
};
class JsonElement : BaseElement {
public:
    Json value;

    JsonElement(){};
    JsonElement(Json j) : value(j) {};
//    ~JsonElement();
};
class Array;
class ArrayElement : BaseElement {
public:
    Array value;

    ArrayElement(){};
    ArrayElement(Array a) : value(a) {};
//    ~ArrayElement();
};

} /// namespace json

#endif // JSON_H
