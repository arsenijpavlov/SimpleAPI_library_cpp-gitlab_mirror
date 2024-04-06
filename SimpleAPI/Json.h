#ifndef JSON_H
#define JSON_H

#include <map>
#include <vector>

namespace json {

enum ValueType {
    eNull,
    eString,
    eNumber,
    eJson,
    eArray
};
enum NextReadState {
    eKey,
    eValue,
    eColon,
    eComma
};
static std::string ToString(const NextReadState state);
static std::string ToString(const ValueType type);
static void ChangeNextState(NextReadState &state, const NextReadState nextState);

class Json;
class ArrayElement {
private:
    std::string                 *string;
    double                      *number;
    Json                        *json;
    std::vector<ArrayElement>   *array;
public:
    ValueType type;

    ArrayElement() : string(nullptr), number(nullptr), json(nullptr), type(eNull) {};
    ~ArrayElement();
    std::string getString();
    double      getNumber();
    Json        getJson();
    std::vector<ArrayElement> getArray();
};

class Json
{
    std::map<std::string, std::string>  values;
    //TODO: add numbers
    std::map<std::string, Json>         containers;
    std::map<std::string, std::vector<ArrayElement>> arrays;
public:
    Json();
    bool put(std::string key, Json json);
    bool put(std::string key, std::string value);
    bool add(std::string key, Json json)            { return this->put(key, json); };
    bool add(std::string key, std::string value)    { return this->put(key, value); };
    bool readFile(const std::string path);
    bool writeFile(const std::string path);
    std::string to_string(int16_t tabulation_level = 0);

private:
    bool ParseJson(const std::vector<std::string>& str, Json* json);
};

} /// namespace json

#endif // JSON_H
