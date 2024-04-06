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

class Json
{
    std::map<std::string, std::string>      values;
    std::map<std::string, Json>             containers;
//    std::map<std::string, std::vector<int>> arrays;
public:
    Json();
    bool put(std::string key, Json json);
    bool put(std::string key, std::string value);
    bool add(std::string key, Json json)            { this->put(key, json); };
    bool add(std::string key, std::string value)    { this->put(key, value); };
    bool readFile(const std::string path);
    bool writeFile(const std::string path);
    std::string to_string(uint8_t tabulation_level = 0);

private:
    bool ParseJson(const std::vector<std::string>& str, Json* json);
};

} /// namespace json

#endif // JSON_H
