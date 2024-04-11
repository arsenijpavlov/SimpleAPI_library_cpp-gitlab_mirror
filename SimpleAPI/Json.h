#ifndef JSON_H
#define JSON_H

#include <map>
#include <memory>
#include <vector>
#include <tuple>

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

    eJsonStart,
    eKey,
    eValue,
    eColon,
    eComma,
    eJsonEnd
};
static std::string ToString(const NextReadState state);
static std::string ToString(const ValueType type);
static void ChangeNextState(NextReadState &state, const NextReadState nextState);

class Json;
//Массив - это упорядоченный список элементов
typedef std::pair<ValueType, void*> ArrayElement;
class Array {
    std::vector<ArrayElement> values;
public:
    Array();
    ~Array();
    void push_back(const double d);
    void push_back(const bool b);
    void push_back(const std::string string);
    void push_back(const Json json);
    void push_back(const Array array);
    void* getAt(size_t index);
    void* operator[](size_t index);
    std::string to_string(int16_t tabulation_level = 0);
};

class Json
{
    std::map<std::string, double>       numbers;
    std::map<std::string, bool>         bools;
    std::map<std::string, std::string>  strings;
    std::map<std::string, Json>         jsons;
    std::map<std::string, Array>        arrays;
public:
    Json();
    bool put(std::string key, double value);
    bool put(std::string key, bool value);
    bool put(std::string key, std::string value);
    bool put(std::string key, Json value);
    bool put(std::string key, Array value);
    bool add(std::string key, std::string value)    { return this->put(key, value); };
    bool add(std::string key, double value)         { return this->put(key, value); };
    bool add(std::string key, Json value)           { return this->put(key, value); };
    bool add(std::string key, Array value)          { return this->put(key, value); };

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

} /// namespace json

#endif // JSON_H
