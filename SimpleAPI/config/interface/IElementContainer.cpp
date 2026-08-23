#include "IElementContainer.h"
#include "../Config.h"
#include "../../utils/StringUtils.h"


namespace simpleapi {
namespace tools {

IElementContainer::KeysComments::KeysComments(const KeysComments &other) noexcept
{
    m_comment_str = other.m_comment_str;
}

IElementContainer::KeysComments::KeysComments(KeysComments &&other) noexcept
{
    m_comment_str = other.m_comment_str;
}

IElementContainer::KeysComments &IElementContainer::KeysComments::operator=(const KeysComments &other) noexcept
{
    m_comment_str = other.m_comment_str;
    return *this;
}

IElementContainer::KeysComments &IElementContainer::KeysComments::operator=(KeysComments &&other) noexcept
{
    m_comment_str = other.m_comment_str;
    return *this;
}

IElementContainer::KeysValues::KeysValues(const KeysValues &other) noexcept
{
    m_key            = other.m_key;
    m_ptr_remote_cfg = other.m_ptr_remote_cfg;
}

IElementContainer::KeysValues::KeysValues(KeysValues &&other) noexcept
{
    m_key                  = other.m_key;
    other.m_key            = nullptr;
    m_ptr_remote_cfg       = other.m_ptr_remote_cfg;
    other.m_ptr_remote_cfg = nullptr;
}

IElementContainer::KeysValues &IElementContainer::KeysValues::operator=(const KeysValues &other) noexcept
{
    m_key            = other.m_key;
    m_ptr_remote_cfg = other.m_ptr_remote_cfg;
    return *this;
}

IElementContainer::KeysValues &IElementContainer::KeysValues::operator=(KeysValues &&other) noexcept
{
    m_key                  = other.m_key;
    other.m_key            = nullptr;
    m_ptr_remote_cfg       = other.m_ptr_remote_cfg;
    other.m_ptr_remote_cfg = nullptr;
    return *this;
}

std::vector<std::unique_ptr<IElementContainer::KeysBase>> IElementContainer::CollectKeys(Config &cfg, std::string prefix) noexcept
{
    std::vector<std::unique_ptr<KeysBase>> ret;

    switch(cfg.getType()) {
    case ValueType::eJson: {
        if(!cfg.getPrefixComment().empty())
            ret.push_back(std::unique_ptr<KeysComments>(new KeysComments(cfg.getPrefixComment())));

        for(auto& pair : cfg.getNamedRange()) {
            auto temp = CollectKeys(*pair.second, prefix + "/" + pair.first);
            ret.insert(ret.end(),
                       std::make_move_iterator(temp.begin()),
                       std::make_move_iterator(temp.end()));
        }

        if(!cfg.getSuffixComment().empty())
            ret.push_back(std::unique_ptr<KeysComments>(new KeysComments(cfg.getSuffixComment())));

        break;
    }
    case ValueType::eArray: {
        if(!cfg.getPrefixComment().empty())
            ret.push_back(std::unique_ptr<KeysComments>(new KeysComments(cfg.getPrefixComment())));

        for(auto& cfg_inner : cfg.getRange()) {
            auto temp = CollectKeys(*cfg_inner, prefix);
            ret.insert(ret.end(),
                       std::make_move_iterator(temp.begin()),
                       std::make_move_iterator(temp.end()));
        }

        if(!cfg.getSuffixComment().empty())
            ret.push_back(std::unique_ptr<KeysComments>(new KeysComments(cfg.getSuffixComment())));

        break;
    }
    default: {
        ret.push_back(std::unique_ptr<KeysValues>(new KeysValues(prefix, &cfg)));
        break;
    }
    }

    return ret;
}

std::vector<std::unique_ptr<IElementContainer::KeysBase>> IElementContainer::CollectKeys(Config &cfg, VString prefixes) noexcept
{
    std::vector<std::unique_ptr<KeysBase>> ret;

    std::string prefix;
    if(prefixes.empty()) {
        prefix = "";
    } else {
        for(auto& p : prefixes)
            prefix += p + ((!prefix.empty() && prefix.back() == '/') ? " " : "") + "/";
        // удалить замыкающий слэш
        if(!prefix.empty())
            prefix.pop_back();
    }
    utils::RemoveIllegalSpaces(prefix);

    switch(cfg.getType()) {
    case ValueType::eJson: {
        if(!cfg.getPrefixComment().empty())
            ret.push_back(std::unique_ptr<KeysComments>(new KeysComments(cfg.getPrefixComment())));

        for(auto& pair : cfg.getNamedRange()) {
            auto temp = CollectKeys(*pair.second, VString{prefix, pair.first});
            ret.insert(ret.end(),
                       std::make_move_iterator(temp.begin()),
                       std::make_move_iterator(temp.end()));
        }

        if(!cfg.getSuffixComment().empty())
            ret.push_back(std::unique_ptr<KeysComments>(new KeysComments(cfg.getSuffixComment())));

        break;
    }
    case ValueType::eArray: {
        if(!cfg.getPrefixComment().empty())
            ret.push_back(std::unique_ptr<KeysComments>(new KeysComments(cfg.getPrefixComment())));

        for(auto& cfg_inner : cfg.getRange()) {
            auto temp = CollectKeys(*cfg_inner, VString{prefix});
            ret.insert(ret.end(),
                       std::make_move_iterator(temp.begin()),
                       std::make_move_iterator(temp.end()));
        }

        if(!cfg.getSuffixComment().empty())
            ret.push_back(std::unique_ptr<KeysComments>(new KeysComments(cfg.getSuffixComment())));

        break;
    }
    default: {
        ret.push_back(std::unique_ptr<KeysValues>(new KeysValues(prefix, &cfg)));
        break;
    }
    }

    return ret;
}

} // namespace tools
} // namespace simpleapi
