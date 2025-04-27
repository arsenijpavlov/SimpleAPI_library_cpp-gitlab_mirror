#ifndef ELEMENT_ARRAY_H
#define ELEMENT_ARRAY_H

#include "IElement.h"
#include "ConfigDefines.h"


//предобъявление
class ElementJson;

class ElementArray : public IElement {
protected:
    VElement m_values;

public:
    ElementArray() noexcept                                     { init(); }
    template<typename ... Types>
    ElementArray(const Types... args) noexcept {
        init();
        for(const IElement el : {IElement(args)...}) push_back(el);
    }
    ElementArray(const std::string& string, const ConfigFormat format,
                 const bool enable_comments = false) noexcept;
    ~ElementArray() noexcept                                    {}

private:
    void init() noexcept                                        { m_type = ValueType::eArray; }
public:

    //PARSING -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    void        parseArray(const std::string& string, const bool enable_comment = false,
                            const ConfigFormat config_format = ConfigFormat::eJSON, CommentDesign* design = nullptr);
    void        parseJsonArray(const std::string& string, const bool enable_comment = false,
                            CommentDesign* design = nullptr);
    void        parseIniArray(const std::string& string, const bool enable_comment = false,
                            CommentDesign* design = nullptr);
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- PARSING

    //PRINTING =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //для рекурсивного вызова, без комментариев, в одну строку
    std::string toString(const ConfigFormat format = ConfigFormat::eJSON,
                          const int8_t tabulation_level = 0) const noexcept override;
    std::string toJsonString(const int8_t tabulation_level = 0) const noexcept;
    std::string toIniString(const int8_t tabulation_level = 0) const noexcept;

    //для рекурсивного вызова, с использованием комментариев
    std::string toString(const ConfigFormat format, const CommentDesign &design,
                          const int8_t tabulation_level = 0) const noexcept override;
    std::string toJsonString(const CommentDesign &design,
                               const int8_t tabulation_level = 0) const noexcept;
    std::string toIniString(const CommentDesign &design,
                              const int8_t tabulation_level = 0) const noexcept;
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= PRINTING

    //COMMENTS =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    void        add_comment(const size_t index, const Comment& content);
    void        add_comment(const size_t index, const std::string& content_before,
                           const std::string& content_after);
    void        add_prefix_comment(const size_t index, const std::string& content);
    void        add_suffix_comment(const size_t index, const std::string& content);

    Comment&        get_comment(const size_t index);
    Comment         get_comment(const size_t index) const;
    std::string&    get_prefix_comment(const size_t index);
    std::string     get_prefix_comment(const size_t index) const;
    std::string&    get_suffix_comment(const size_t index);
    std::string     get_suffix_comment(const size_t index) const;

    //NOTE: noexcept, потому что неправильный индекс просто пропустит действие
    void        clear_comment(const size_t index) noexcept;
    void        clear_prefix_comment(const size_t index) noexcept;
    void        clear_suffix_comment(const size_t index) noexcept;
    void        delete_comment(const size_t index) noexcept;
    void        delete_prefix_comment(const size_t index) noexcept;
    void        delete_suffix_comment(const size_t index) noexcept;
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= COMMENTS

    ElementArray&   clear() noexcept;
    bool            contains(const size_t& index) const noexcept
                                                                { return m_values.size() > index; }
    bool            isEmpty() const noexcept                    { return m_values.empty(); }
    bool            isEqual(const IElement& other, const bool compare_comments = false) const noexcept override;
    size_t          size() const noexcept                       { return m_values.size(); }

    //TYPES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    ValueType   get_type_front();
    ValueType   get_type_at(const size_t index);
    ValueType   get_type_back();
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- TYPES

    //SETTERS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //NOTE: если индекс больше количества вложенных элементов, то добавятся в конец
    ElementArray&   append(const IElement& element) noexcept    { return push_back(element); }
    ElementArray&   appendArray(const ElementArray& array) noexcept;
    ElementArray&   push_front(const IElement& element) noexcept;
    ElementArray&   push_at(const IElement& element, const size_t index);
    ElementArray&   push_back(const IElement& element) noexcept;
                    __ONLY_ALLOWED_TYPES__(T)
    ElementArray&   insert(const size_t index, const T& value) noexcept {
                        if(index > m_values.size() - 1) m_values.push_back(value);
                        else                            m_values.insert(m_values.cbegin() + index, IElement(value));
                        return *this;
                    }
                    __ONLY_ALLOWED_TYPES__(T)
    ElementArray&   insert(const VElement::iterator& iterator, const T& value) {
                        m_values.insert(iterator, value);
                        return *this;
                    }
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- SETTERS

    //GETTERS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    IElement&    get_value(const size_t index);
    IElement&    get_value(const VString& complex_key);
    IElement&    get_value(const std::vector<size_t>& complex_key);
    IElement&    get_front()                                    { return m_values.front(); }
    IElement&    get_at(const size_t index)                     { return get_value(index); }
    IElement&    get_back()                                     { return m_values.back(); }
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- GETTERS

    //DELETERS =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    ElementArray&   pop_front();
    ElementArray&   pop_at(const size_t index);
    ElementArray&   pop_back();
    ElementArray&   erase(const size_t index);
    ElementArray&   erase(const VElement::iterator& iterator);
    ElementArray&   erase(const VElement::iterator& begin, const VElement::iterator& end);
    ElementArray&   erase(const std::vector<size_t> indexes);
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= DELETERS

    //OPERATORS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    void            operator=(const VElement& other) noexcept;
    ElementArray&   operator<<(const IElement& element) noexcept{ return push_back(element); }
    IElement&       operator[](const size_t index)              { return get_value(index); }
    IElement&       operator[](const VString& complex_key)      { return get_value(complex_key); }
                    template<std::size_t SIZE>
    IElement&       operator[](const std::array<std::string, SIZE>& complex_key) {
                        VString complex_key_vec;
                        complex_key_vec.reserve(SIZE);
                        std::copy(complex_key.begin() + 1, complex_key.end(), complex_key_vec.begin());
                        return (*this)[complex_key_vec];
                    }
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= OPERATORS

    VElement::iterator       begin() noexcept                   { return m_values.begin(); }
    VElement::iterator       end() noexcept                     { return m_values.end(); }
    VElement::const_iterator cbegin() const noexcept            { return m_values.cbegin(); }
    VElement::const_iterator cend() const noexcept              { return m_values.cend(); }
};

bool IsElementArray(const std::string& str, const ConfigFormat format = ConfigFormat::eJSON) noexcept;
bool IsElementJsonArray(const std::string& str) noexcept;
bool IsElementIniArray(const std::string& str) noexcept;
bool IsElementArray(const IElement& e) noexcept;

#endif // ELEMENT_ARRAY_H
