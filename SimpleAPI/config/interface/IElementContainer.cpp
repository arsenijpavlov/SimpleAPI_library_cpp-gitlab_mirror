#include "IElementContainer.h"
#include "../Config.h"

using namespace simpleapi::tools;

std::vector<IElementContainer::KeysValuesAndComments> IElementContainer::CollectKeysAndComments(Config &cfg, std::string prefix) noexcept
{
    std::vector<KeysValuesAndComments> ret;

    switch(cfg.getType()) {
    case ValueType::eJson: {
        for(auto& pair : cfg.getNamedRange()) {
            auto temp = CollectKeysAndComments(*pair.second, prefix + "/" + pair.first);
            ret.insert(ret.end(), temp.begin(), temp.end());
        }
        break;
    }
    case ValueType::eArray: {
        for(auto& cfg_inner : cfg.getRange()) {
            auto temp = CollectKeysAndComments(*cfg_inner, prefix);
            ret.insert(ret.end(), temp.begin(), temp.end());
        }
        break;
    }
    default: {
        ret.push_back({prefix, &cfg});
        break;
    }
    }

    return ret;
}

std::vector<IElementContainer::KeysValuesAndComments> IElementContainer::CollectKeysAndComments(Config &cfg, VString prefixes) noexcept
{
    std::vector<KeysValuesAndComments> ret;

    std::string prefix;
    if(prefixes.empty()) {
        prefix = "";
    } else {
        for(auto& p : prefixes)
            prefix += p + "/";
        // удалить замыкающий слэш
        if(!prefix.empty())
            prefix.pop_back();
    }

    switch(cfg.getType()) {
    case ValueType::eJson: {
        for(auto& pair : cfg.getNamedRange()) {
            auto temp = CollectKeysAndComments(*pair.second, {prefix, pair.first});
            ret.insert(ret.end(), temp.begin(), temp.end());
        }
        break;
    }
    case ValueType::eArray: {
        for(auto& cfg_inner : cfg.getRange()) {
            auto temp = CollectKeysAndComments(*cfg_inner, {prefix});
            ret.insert(ret.end(), temp.begin(), temp.end());
        }
        break;
    }
    default: {
        ret.push_back({prefix, &cfg});
        break;
    }
    }

    return ret;
}
