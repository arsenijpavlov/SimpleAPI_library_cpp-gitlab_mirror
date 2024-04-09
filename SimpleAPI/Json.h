#ifndef JSON_H
#define JSON_H

#include <map>
#include <vector>

namespace json {

enum ValueType {
    eNull,

    eNumber,
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

class ArrayElement;
typedef std::vector<ArrayElement> Array;
std::string ToString(Array array, int16_t tabulation_level = 0);

class Json;
class ArrayElement {
private:
    std::string *string;
    double      *number;
    Json        *json;
    Array       *array;
public:
    ValueType type;

    ArrayElement() : string(nullptr), number(nullptr), json(nullptr), array(nullptr), type(eNull)
        {};
    ~ArrayElement();
    std::string getString();
    double      getNumber();
    Json        getJson();
    Array       getArray();
    std::string to_string();
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
