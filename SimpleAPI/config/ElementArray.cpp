#include "ElementArray.h"

#include <algorithm>
#include <stdexcept>
#include "../utils/Utils.h"

//предобъявление
#include "Config.h"
#include "ConfigDefines.h"
#include "ElementArray.h"


namespace simpleapi {
namespace tools {


void ElementArray::set_comment(const size_t& index, const Comment &content) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index)->setComment(content);
}

void ElementArray::set_comment(const size_t& index, const std::string &content_before,
                               const std::string &content_after)
{
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index)->setComment(content_before, content_after);
}

void ElementArray::set_prefix_comment(const size_t& index, const std::string &content) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index)->setPrefixComment(content);
}

void ElementArray::set_suffix_comment(const size_t& index, const std::string &content) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index)->setSuffixComment(content);
}

Comment &ElementArray::get_comment(const size_t& index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    return m_values.at(index)->getComment();
}

Comment ElementArray::get_comment(const size_t& index) const {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    return m_values.at(index)->getComment();
}

std::string ElementArray::get_prefix_comment(const size_t& index) const {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    return m_values.at(index)->getPrefixComment();
}

std::string ElementArray::get_suffix_comment(const size_t& index) const {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    return m_values.at(index)->getSuffixComment();
}

//std::string &ElementArray::get_prefix_comment(const size_t& index) {
//    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
//    return m_values.at(index)->getPrefixComment();
//}

//std::string &ElementArray::get_suffix_comment(const size_t& index) {
//    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
//    return m_values.at(index)->getSuffixComment();
//}

void ElementArray::clear_comment(const size_t& index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index)->clearComment();
}

void ElementArray::clear_prefix_comment(const size_t& index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index)->clearPrefixComment();
}

void ElementArray::clear_suffix_comment(const size_t& index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index)->clearSuffixComment();
}

void ElementArray::delete_comment(const size_t& index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index)->deleteComment();
}

void ElementArray::delete_prefix_comment(const size_t& index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index)->deletePrefixComment();
}

void ElementArray::delete_suffix_comment(const size_t& index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    m_values.at(index)->deleteSuffixComment();
}

void ElementArray::append_if_not_contains(const Config &config) noexcept {
    if(!contains(config))
        m_values.push_back(std::make_shared<Config>(config));
}

void ElementArray::append_if_not_contains(Config &&config) noexcept {
    if(!contains(config))
        m_values.push_back(std::make_shared<Config>(std::move(config)));
}

Config &ElementArray::get_front() {
    __CHECK_ARRAY_EMPTY_EXCEPTION__((*this))
    return *m_values.front();
}

Config ElementArray::get_front() const {
    __CHECK_ARRAY_EMPTY_EXCEPTION__((*this))
    return *m_values.front();
}

Config &ElementArray::get_at(const size_t& index) {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    return *m_values[index];
}

Config ElementArray::get_at(const size_t& index) const {
    __CHECK_CONTAINER_INDEX_CORRECT__((*this), index)
    return *m_values[index];
}

Config &ElementArray::get_back() {
    __CHECK_ARRAY_EMPTY_EXCEPTION__((*this))
    return *m_values.back();
}

Config ElementArray::get_back() const {
    __CHECK_ARRAY_EMPTY_EXCEPTION__((*this))
    return *m_values.back();
}

void ElementArray::clear() noexcept {
    clearComment();
    m_values.clear();
}

void ElementArray::insert_front(const VElement &elements) noexcept {
    for(size_t i = 0; i < elements.size(); i++)
        m_values.insert(cbegin() + i, std::make_shared<Config>(elements[i]));
}

void ElementArray::insert_front(VElement &&elements) noexcept {
    for(size_t i = 0; i < elements.size(); i++)
        m_values.insert(cbegin() + i, std::make_shared<Config>(std::move(elements[i])));
}

//если индекс больше существующего - добавить в конец массива
void ElementArray::insert_at(const size_t& index, const VElement &elements) noexcept {
    VElement temp_elements = elements;
    insert_at(index, std::move(temp_elements));
}

//если индекс больше существующего - добавить в конец массива
void ElementArray::insert_at(const size_t& index, VElement &&elements) noexcept {
    size_t counter = 0;
    for(auto &element : elements) {
        if(index+counter < size())
            m_values.insert(m_values.begin() + index + counter, std::make_shared<Config>(std::move(element)));
        else
            m_values.push_back(std::make_shared<Config>(std::move(element)));
        counter++;
    }
}

void ElementArray::insert_back(const VElement &elements) noexcept {
    std::transform(elements.cbegin(), elements.cend(), std::back_inserter(m_values),
                   [](const Config& element){ return std::make_shared<Config>(element); });
}

void ElementArray::insert_back(VElement &&elements) noexcept {
    std::transform(std::make_move_iterator(elements.begin()),
                   std::make_move_iterator(elements.end()),
                   std::back_inserter(m_values),
                   [](const Config& element) {
                       return std::make_shared<Config>(std::move(element));
                   });
}

shared_VElement::iterator ElementArray::insert_at(const shared_VElement::iterator iterator, const Config &value) {
    Config temp = value;
    return insert_at(iterator, std::move(temp));
}

shared_VElement::iterator ElementArray::insert_at(const shared_VElement::iterator iterator, Config &&value) {
    if(iterator >= m_values.begin() && iterator < m_values.end())
        return m_values.insert(iterator, std::make_shared<Config>(std::move(value)));
    else
        return m_values.insert(m_values.end(), std::make_shared<Config>(std::move(value)));
}

void ElementArray::append(const ElementArray &other) noexcept {
    ElementArray array_copy(other);
    append(std::move(array_copy));
}

void ElementArray::append(ElementArray &&other) noexcept {
    for(auto& o : other.m_values)
        insert_back(std::move(*o.get()));
}

void ElementArray::append_null(size_t size) noexcept {
    for(size_t i = size; i > 0; --i)
        m_values.push_back(std::make_shared<Config>());
}

void ElementArray::pop_front() noexcept {
    if(!m_values.empty())
        m_values.erase(cbegin());
}

void ElementArray::pop_at(const size_t& index) noexcept {
    if(m_values.size() > index)
        m_values.erase(cbegin() + index);
}

void ElementArray::pop_back() noexcept {
    if(!m_values.empty())
        m_values.pop_back();
}

Config ElementArray::get_and_pop_front() {
    Config cfg = get_front();
    pop_front();
    return cfg;
}

Config ElementArray::get_and_pop_at(const size_t& index) {
    Config cfg = get_at(index);
    pop_at(index);
    return cfg;
}

Config ElementArray::get_and_pop_back() {
    Config cfg = get_back();
    pop_back();
    return cfg;
}

shared_VElement::iterator ElementArray::pop_at(const shared_VElement::iterator iterator)
{
    if(iterator >= m_values.begin() && iterator < m_values.end())
        return m_values.erase(iterator);
    else
        return m_values.end();
}

bool ElementArray::isEqual(const IElement &other, const bool compare_comments,
                           const bool map_sort_important) const noexcept
{
    bool b1 = true;
    if(compare_comments)
        b1 = isCommentsEqual(other);

    bool b2 = size() == other.size();
    if(b2) {
        const ElementArray& other_array = reinterpret_cast<const ElementArray&>(other);
        for(size_t i = 0; i < size(); i++) {
            if(!m_values[i]->isEqual(*other_array.m_values[i], compare_comments, map_sort_important)) {
                b2 = false;
                break;
            }
        }
    }

    return b1 && b2;
}

//комментарии при поиске не учитываются
bool ElementArray::contains(const Config &config) const noexcept {
    return std::any_of(cbegin(), cend(), [&config](const std::shared_ptr<Config> &value){ return value->isEqual(config); });
}

Config ElementArray::operator[](const size_t &index) const    {
    return get_at(index);
}

bool ElementArray::operator==(const Config &config) const
{
    if(!config.isArray())
        return false;

    if(size() != config.size())
        return false;

    for(size_t i = 0; i < size(); i++) {
        if((*this)[i] != config[i])
            return false;
    }

    return true;
}

std::string ElementArray::toString(const ConfigFormat format, const CommentDesign &design,
                                   const int8_t custom_tabulation_level) const noexcept
{
    switch(format){
    //предполагается, что eONLY_VALUE - это JSON без пробелов и переносов строки
    case ConfigFormat::eONLY_VALUE: return toJsonString(design, -1);
    case ConfigFormat::eJSON:       return toJsonString(design, custom_tabulation_level);
    case ConfigFormat::eINI:        return toIniString(design, custom_tabulation_level);
    case ConfigFormat::eYAML:       return toYamlString(design, custom_tabulation_level);
    case ConfigFormat::eXML:        return toXmlString(design, custom_tabulation_level);
    default:                        return "[UNKNOWN_CONFIG_FORMAT]";
    }
}

std::string ElementArray::toJsonString(const CommentDesign &design, const int8_t custom_tabulation_level) const noexcept
{
    std::string ret;
    const std::string tablulation_str   = utils::RepeatSymToStr('\t', custom_tabulation_level);
    const std::string tablulation_str_1 = utils::RepeatSymToStr('\t', custom_tabulation_level + 1);

    //(для отладки)
    //ret += "custom_tabulation_level:" + std::to_string(custom_tabulation_level) + "\n";

    bool with_spaces = custom_tabulation_level != -1;
    CommentDesign inner_design = design;
    inner_design.is_in_container = true;

    if(with_spaces)
        ret += tablulation_str;
    ret += "[";
    if(with_spaces)
        ret += "\n";

    for(size_t i = 0; i < size(); i++)
    {
        //вывод комментария с рамкой
        if(with_spaces
            && design.with_comments
            && !m_values[i]->getPrefixComment().empty())
        {
            //вернули в исходное состояние после записи КОММЕНТАРИЯ ПОСЛЕ ЗНАЧЕНИЯ
            inner_design.opt_multiline_column_size = design.opt_multiline_column_size;

            if(m_values[i]->getPrefixComment().find('\n' != std::string::npos))
                ret += "\n";

            ret += ToComment(m_values[i]->getPrefixComment(), inner_design, custom_tabulation_level + 1);
            ret += "\n";
        }

        //вывод значения
        if(with_spaces)
            ret += tablulation_str_1;

        std::string temp = m_values[i]->toString(ConfigFormat::eJSON, inner_design,
                                                 (with_spaces ? custom_tabulation_level + 1 : -1));
        if(m_values[i]->isContainer()) {
            ret += utils::RemoveStartTabulations(temp);
        } else {
            if(m_values[i]->isNumber()) {
                ret += temp;
            } else {
                SeparatedLines sl = SeparateWithoutColumned(temp);
                // все строки кроме первой выровнять по первой строке
                for(size_t j = 1; j < sl.lines.size(); j++) {
                    sl.lines[j] = utils::RepeatSymToStr('\t', custom_tabulation_level + 1) + sl.lines[j];
                }

                if(m_values[i]->isString()) //строки ВСЕГДА обрамляются кавычками
                    ret += "\"";

                ret += VStringToString(sl.lines);

                if(m_values[i]->isString()) //строки ВСЕГДА обрамляются кавычками
                    ret += "\"";
            }
        }

        if(i < size() - 1)
            ret += ",";

        //вывод комментария без рамки
        if(!m_values[i]->isContainer()) { //NOTE: контейнеры сами себя описывают
            if(with_spaces
                && inner_design.with_comments
                && !m_values[i]->getSuffixComment().empty())
            {
                //NOTE: (ширина колонки многосторчного комментария после значения не влияет на вывод)
                inner_design.opt_multiline_column_size = 0;

                std::string temp = ToComment(m_values[i]->getSuffixComment(), inner_design, -1);
                const size_t pos = ret.rfind('\n');
                std::string temp__ = (ret.size() > pos +1 ? (ret.substr(pos + 1, ret.size())) : "") + " ";
                utils::SetStringAsOnlySpaces(temp__);
                utils::AddStringForFromLine(temp, 2, temp__);
                ret += " " + temp;
            }
        }

        if(with_spaces)
            ret += "\n";
    }

    if(with_spaces)
        ret += tablulation_str;
    ret += "]";

    //вывод комментария без рамки
    if(with_spaces
        && design.with_comments
        && !design.is_in_container
        && !getSuffixComment().empty())
    {
        //NOTE: (ширина колонки многосторчного комментария после значения не влияет на вывод)
        inner_design.opt_multiline_column_size = 0;

        std::string temp = ToComment(getSuffixComment(), inner_design);
        const size_t pos = ret.rfind('\n');
        std::string temp__ = (ret.size() > pos +1 ? (ret.substr(pos + 1, ret.size())) : "") + " ";
        utils::SetStringAsOnlySpaces(temp__);
        utils::AddStringForFromLine(temp, 2, temp__);
        ret += " " + temp;
    }

    return ret;
}

//метод не рекурсивный для контейнеров!
//TODO: ElementArray::toIniString()
//FIXME: переносы строк
//FIXME: вывод комментариев
/*
 *     Config cfg2;
    cfg2.parseJson("[[\"a\naaa\", b],{c=[d,{e=f}]},g]"); //TODO: кандидат на тест
//    cfg2.parseJson("{a={b=\"c\nccc\",d=e},f:[{g=[h,{i=j}]}]}"); //TODO: кандидат на тест
//    cfg2.setPrefixComment("big preview\ncomment");
//    cfg2.setSuffixComment("big suffix\ncomment");
    std::cout << "----------------------------------" << std::endl;
    std::cout << cfg2.toString(simpleapi::ConfigFormat::eJSON, cfg2.getCommentDesign()) << std::endl;
    std::cout << "----------------------------------" << std::endl;
    std::cout << cfg2.toString(simpleapi::ConfigFormat::eINI, cfg2.getCommentDesign()) << std::endl;
    std::cout << "----------------------------------" << std::endl;
*/

std::string ElementArray::toIniString(const CommentDesign &design, const int8_t custom_tabulation_level) const noexcept
{
    std::string ret;

    auto GetPrefixComment = [&design](const Config cfg) -> std::string {
        return (cfg.getPrefixComment().empty()) ? "" : (ToComment(cfg.getPrefixComment(), design) + "\n");
    };
    auto GetSuffixComment = [&design, &ret](const Config cfg) -> std::string {
        if(cfg.getSuffixComment().empty())
            return "";

        SeparatedLines sl = SeparateWithoutColumned(ToComment(cfg.getSuffixComment(), design));
        //начиная со второй, все строки дополнить пробелами в начале по длине
        // последней строки +1(отделение комментария от значения)
        size_t pos = ret.rfind('\n');
        std::string last_line = (pos == std::string::npos) ? ret
                                                           : (ret.substr(pos + 1, ret.size()));
        size_t end_line_size = utils::GetStringCharCount(last_line) + 1;
        for(size_t i = 0; i < sl.lines.size(); i++) {
            if(i != 0) {
                sl.lines[i] = utils::RepeatSymToStr(' ', end_line_size) + sl.lines[i];
            }
        }

        return (cfg.getSuffixComment().empty()) ? "" : " " + VStringToString(sl.lines);
    };
    auto IsArrayWithPrimitives = [](const Config cfg) -> bool {
        if(!cfg.isIndexContainer())
            return false;
        for(const auto& cfg_inner : cfg.getRange()) {
            if(cfg_inner->isContainer()
                || !cfg_inner->getPrefixComment().empty()
                || !cfg_inner->getSuffixComment().empty()
                || (cfg_inner->isString() && (cfg_inner->getString().find('\n') != std::string::npos
                                              || cfg_inner->getString().size() > 50)))
            {
                return false;
            }
        }
        return true;
    };
    auto AppendMultinlineString = [&ret](const std::string& str) -> void {
        ret += "\"";
        std::string temp_str = str;
        if(temp_str.find('\n') != std::string::npos) {
            SeparatedLines sl = tools::SeparateWithoutColumned(temp_str);
            for(size_t i = 0; i < sl.lines.size(); i++) {
                ret += (i == 0 ? "" : "    ");
                if(i + 1 < sl.lines.size())
                    ret += sl.lines[i] + " \\\n";
                else
                    ret += sl.lines[i];
            }
        } else {
            ret += str;
        }
        ret += "\"";
    };
    auto AppendArrayPrimitives = [&](const std::string& key, Config& cfg) -> void {
        if(!key.empty())
            ret += key + " = ";

        ret += "[";
        for(size_t i = 0; i < cfg.size(); i++) {
            if(cfg.isString())
                ret += "\"";
            ret += cfg[i].toString();
            if(cfg.isString())
                ret += "\"";

            if(i + 1 < cfg.size())
                ret += ", ";
        }
        ret += "]\n";
    };
    auto AppendCollection = [&](const VString& prefixes, Config& cfg) -> void {
        std::vector<KeysValuesAndComments> kvacs = CollectKeysAndComments(cfg, prefixes);
        for(auto& kvac : kvacs) {
            ret += GetPrefixComment(*kvac.remote_cfg);
            if(!kvac.key.empty())
                ret += kvac.key + " = ";
            if(kvac.remote_cfg->isString()) {
                AppendMultinlineString(kvac.remote_cfg->toString());
            } else {
                ret += kvac.remote_cfg->toString();
            }
            ret += GetSuffixComment(*kvac.remote_cfg);
            ret += "\n";
        }
    };

    if(!getPrefixComment().empty()) {
        ret += ToComment(getPrefixComment(), design) + "\n\n\n";
    }

    for(const auto& cfg : m_values) {
        switch(cfg->getType()) {
        case ValueType::eArray: {
            //все массивы первого уровня должны быть преобразованы в безымянный Json
            Config temp = *cfg;
            *cfg = Config(ValueType::eJson);
            cfg->push_back("", temp);
        }
        case ValueType::eJson: {
            ret += GetPrefixComment(*cfg);

            ret += ""; //контейнер без имени

            for(const auto& cfg_inner : cfg->getNamedRange()) {
                if(cfg_inner.second->isContainer()) {
                    if(IsArrayWithPrimitives(*cfg_inner.second))
                    {
                        //если внутри только примитивы без комментариев - вывести их в одну строку (строки длиной <=50)
                        AppendArrayPrimitives(cfg_inner.first, *cfg_inner.second);
                    } else {
                        //нужно собрать все элементы массива и упаковать в общее имя с переходом между уровнями
                        AppendCollection({"", cfg_inner.first}, *cfg_inner.second);
                    }
                } else if(cfg_inner.second->isString()) {
                    ret += GetPrefixComment(*cfg_inner.second);
                    if(!cfg_inner.first.empty())
                        ret += cfg_inner.first + " = ";
                    AppendMultinlineString(cfg_inner.second->toString());
                    ret += GetSuffixComment(*cfg_inner.second);
                    ret += "\n";
                } else {
                    ret += GetPrefixComment(*cfg_inner.second);
                    if(!cfg_inner.first.empty())
                        ret += cfg_inner.first + " = ";
                    ret += cfg_inner.second->toString();
                    ret += GetSuffixComment(*cfg_inner.second);
                    ret += "\n";
                }
            } // for()

            ret += GetSuffixComment(*cfg);
            ret += "\n";

            break;
        }
        default: {
            if(cfg->isArray()) {
                //если внутри только примитивы без комментариев - вывести их в одну строку (строки длиной <=50)
                if(IsArrayWithPrimitives(*cfg)) {
                    AppendArrayPrimitives("", *cfg);
                } else {
                    //нужно собрать все элементы массива и упаковать в общее имя с переходом между уровнями
                    AppendCollection({}, *cfg);
                }
            } else {
                ret += GetPrefixComment(*cfg);
                ret += cfg->toString() + "\n";
                ret += GetSuffixComment(*cfg);
            }
            break;
        }
        }
    } // loop for()

    if(!getSuffixComment().empty()) {
        ret += "\n\n" + ToComment(getSuffixComment(), design);
    }

    return ret;
}

std::string ElementArray::toYamlString(const CommentDesign &design, const int8_t custom_tabulation_level) const noexcept
{
    //TODO (потом): std::string ElementArray::toYamlString()
    return "[TODO]";
}

std::string ElementArray::toXmlString(const CommentDesign &design, const int8_t custom_tabulation_level) const noexcept
{
    //TODO (потом): std::string ElementArray::toXmlString()
    return "[TODO]";
}

bool IsElementArray(const IElement &e) noexcept
{
    return e.getType() == ValueType::eArray;
}

bool IsElementArray(const Config &cfg) noexcept
{
    return cfg.isArray();
}

} // namespace tools
} // namespace simpleapi
