#include "IElementContainer.h"
#include "../Config.h"


std::vector<simpleapi::tools::IElementContainer::KeysValuesAndComments> simpleapi::tools::IElementContainer::CollectKeysAndComments(Config &cfg, std::string prefix) noexcept
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
