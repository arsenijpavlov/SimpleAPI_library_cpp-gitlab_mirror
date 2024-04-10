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
//template<typename >
class Array {
    /*std::unique_ptr<*/std::tuple<double, bool, std::string, Json, Array> values;
public:
    Array() : values(nullptr) {}
    ~Array() {}
    bool push_back(const double& d);
    bool push_back(const bool& b);
    bool push_back(const std::string string);
    bool push_back(const Json& json);
    bool push_back(const Array& array);

    std::string to_string(Array array, int16_t tabulation_level = 0);
};

class Json
{
    std::map<std::string, std::string>  strings;
    std::map<std::string, double>       numbers;
    std::map<std::string, Json>         jsons;
    std::map<std::string, Array>        arrays;
public:
    Json();
    bool put(std::string key, std::string value);
    bool put(std::string key, double value);
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
static bool CheckString(std::string& value);
static bool CheckJson(std::string& value);
static bool CheckArray(std::string& value);
static bool ParseJson(const std::string& str, Json* json);
static bool ParseArray(const std::string& str, Array* array);

} /// namespace json

#endif // JSON_H
