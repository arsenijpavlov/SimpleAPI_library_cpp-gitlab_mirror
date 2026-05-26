# 3. Класс Config

Используется как основной класс, с которым взаимодействует пользователь библиотеки.

Конструкторы Config позволяют создавать объекты Config на основе разрешённых выбранным стандартом значений (long double, bool, std::string, const char*, Config(ValueType::eJson), Config(ValueType::eArray), **TODO: Config(ValueType::XML)**, **TODO: Config(ValueType::YAML)**).

**Используйте файл Config.h как источник информации о доступных методах. Обращайте внимание на отметки API_ в конце объявлений методов**

**Следующие списки могут быть устаревшими!**

## Методы работы с комментариями
```c++
Config& setComment(const Comment& content) noexcept;
Config& setComment(const std::string &content_before, const std::string &content_after) noexcept;
Config& setPrefixComment(const std::string& content) noexcept;
Config& setSuffixComment(const std::string& content) noexcept;

Comment& getComment() noexcept;
Comment getComment() const noexcept;
std::string getPrefixComment() const noexcept;
std::string getSuffixComment() const noexcept;

Config& clearComment() noexcept;
Config& clearPrefixComment() noexcept;
Config& clearSuffixComment() noexcept;
Config& deleteComment() noexcept;
Config& deletePrefixComment() noexcept;
Config& deleteSuffixComment() noexcept;

CommentDesign& getCommentDesign() noexcept;
CommentDesign getCommentDesign() const noexcept;
Config& setCommentDesign(const CommentDesign &design) noexcept;
Config& clearCommentDesign() noexcept;
```

## Методы работы с комментариями элементов внутри контейнеров (имя функции пишется через знак подчёркивания _)
```c++
Config& set_comment(const size_t& index, const Comment &content);
Config& set_comment(const std::string& key, const Comment &content);

Config& set_comment(const size_t& index, const std::string &content_before, const std::string &content_after);
Config& set_comment(const std::string& key, const std::string &content_before, const std::string &content_after);

Config& set_prefix_comment(const size_t& index, const std::string &content);
Config& set_prefix_comment(const std::string& key, const std::string &content);

Config& set_suffix_comment(const size_t& index, const std::string &content);
Config& set_suffix_comment(const std::string& key, const std::string &content);

Comment& get_comment(const size_t& index);
Comment& get_comment(const std::string& key);

Comment get_comment(const size_t& index) const;
Comment get_comment(const std::string& key) const;

std::string get_prefix_comment(const size_t& index) const;
std::string get_prefix_comment(const std::string& key) const;

std::string get_suffix_comment(const size_t& index) const;
std::string get_suffix_comment(const std::string& key) const;

Config& clear_comment(const size_t& index);
Config& clear_comment(const std::string& key);

Config& clear_prefix_comment(const size_t& index);
Config& clear_prefix_comment(const std::string& key);

Config& clear_suffix_comment(const size_t& index);
Config& clear_suffix_comment(const std::string& key);

Config& delete_comment(const size_t& index);
Config& delete_comment(const std::string& key);

Config& delete_prefix_comment(const size_t& index);
Config& delete_prefix_comment(const std::string& key);

Config& delete_suffix_comment(const size_t& index);
Config& delete_suffix_comment(const std::string& key);
```

## Методы установки значения
```c++
Config& setValue() noexcept;
Config& setValue(std::nullptr_t) noexcept;
Config& setValue(const Config& other) noexcept;
Config& setValue(Config&& other) noexcept;
Config& setValue(const tools::IElement& other) noexcept;
Config& setValue(tools::IElement&& other) noexcept;
Config& setValue(const bool other) noexcept;

Config& setValue(const long double& other) noexcept;
Config& setValue(long double&& other) noexcept;
__ONLY_NUMBER_TYPES__(T)
Config& setValue(const T& other) noexcept;
__ONLY_NUMBER_TYPES__(T)
Config& setValue(T&& other) noexcept; 

Config& setValue(const std::string& other) noexcept;
Config& setValue(std::string&& other) noexcept;
__ONLY_STRING_TYPES__(T)
Config& setValue(const T& other) noexcept;
__ONLY_STRING_TYPES__(T)
Config& setValue(T&& other) noexcept;

Config& setValue(const tools::ElementArray& other) noexcept;
Config& setValue(tools::ElementArray&& other) noexcept;
Config& setValue(const tools::ElementJson& other) noexcept;
Config& setValue(tools::ElementJson&& other) noexcept;

// вложенные контейнеры (используют insert_*() ниже, но возвращают этот же базовый элемент)
Config& set(const std::string& key, const Config& value);
Config& set(const std::string& key, Config&& value);
Config& set(const size_t& index, const Config& value);
Config& set(const size_t& index, Config&& value);
Config& set(const size_t& index, const std::string& key, const Config& value);
Config& set(const size_t& index, const std::string& key, Config&& value);
```

## Методы получения реализации значений
```c++
bool& getBool();
bool getBool() const;
long double& getNumber();
long double getNumber() const;
std::string& getString();
std::string getString() const;
bool error() const noexcept;
std::string getError() const noexcept;

// вложенные контейнеры
// @complex_key - список индексов/ключей:   -> el[1]["k1"][2]
Config& get_front();
Config get_front() const;

Config& get_at(const size_t& index);
Config get_at(const size_t& index) const;
Config& get_at(const std::string& key);
Config get_at(const std::string& key) const;
Config& get_at(const std::vector<OnlySizetOrString>& complex_key);
Config get_at(const std::vector<OnlySizetOrString>& complex_key) const;
//фикс для вызова через {}
Config& get_at(const std::initializer_list<OnlySizetOrString>& complex_key);
Config get_at(const std::initializer_list<OnlySizetOrString>& complex_key) const;
  
Config& get_back();
Config get_back() const;

bool& get_front_bool();
bool get_front_bool() const;
long double& get_front_number();
long double get_front_number() const;
std::string& get_front_string();
std::string get_front_string() const;

bool& get_bool_at(const size_t& index);
bool get_bool_at(const size_t& index) const;
bool& get_bool_at(const std::string& key);
bool get_bool_at(const std::string& key) const;
bool& get_bool_at(const std::vector<OnlySizetOrString>& complex_key);
bool get_bool_at(const std::vector<OnlySizetOrString>& complex_key) const;
//фикс для вызова через {}
bool& get_bool_at(const std::initializer_list<OnlySizetOrString>& complex_key);
bool get_bool_at(const std::initializer_list<OnlySizetOrString>& complex_key) const;

long double& get_number_at(const size_t& index);
long double get_number_at(const size_t& index) const;
long double& get_number_at(const std::string& key);
long double get_number_at(const std::string& key) const;
long double& get_number_at(const std::vector<OnlySizetOrString>& complex_key);
long double get_number_at(const std::vector<OnlySizetOrString>& complex_key) const;
//фикс для вызова через {}
long double& get_number_at(const std::initializer_list<OnlySizetOrString>& complex_key);
long double get_number_at(const std::initializer_list<OnlySizetOrString>& complex_key) const;

std::string& get_string_at(const size_t& index);
std::string get_string_at(const size_t& index) const;
std::string& get_string_at(const std::string& key);
std::string get_string_at(const std::string& key) const;
std::string& get_string_at(const std::vector<OnlySizetOrString>& complex_key);
std::string get_string_at(const std::vector<OnlySizetOrString>& complex_key) const;
//фикс для вызова через {}
std::string& get_string_at(const std::initializer_list<OnlySizetOrString>& complex_key);
std::string get_string_at(const std::initializer_list<OnlySizetOrString>& complex_key) const;

bool& get_bool_back();
bool get_bool_back() const;
long double& get_number_back();
long double get_number_back() const;
std::string& get_string_back();
std::string get_string_back() const;

## Методы очистки значений
//здесь просто сброс комментариев, сброс до значения по умолчанию
Config& clear() noexcept;
//очистка контейнера, главные комментарии и CommentDesign не сбрасывается
Config& clearContainer();
```

## Методы добавления значений в контейнер (выдаст exception если тип неподходящий)
```c++
Config& insert_front(const Config& other);
Config& insert_front(Config&& other);
Config& insert_front(const std::string& key, const Config& other);
Config& insert_front(const std::string& key, Config&& other);

Config& insert_at(const size_t& index, const Config& other);
Config& insert_at(const size_t& index, Config&& other);
Config& insert_at(const std::string& key, const Config& other);
Config& insert_at(const std::string& key, Config&& other);
Config& insert_at(const size_t& index, const std::string& key, const Config& other);
Config& insert_at(const size_t& index, const std::string& key, Config&& other);

Config& insert_at(const shared_VElement::iterator iterator, const Config& other);
Config& insert_at(const shared_VElement::iterator iterator, Config&& other);

Config& insert_at(const shared_VPairElement::iterator iterator, const std::string& key, const Config& other);
Config& insert_at(const shared_VPairElement::iterator iterator, const std::string& key, Config&& other);

Config& insert_back(const Config& other);
Config& insert_back(Config&& other);
Config& insert_back(const std::string& key, const Config& other);
Config& insert_back(const std::string& key, Config&& other);

//если путь не существовал - будут созданы все необходимые array и json, чтобы путь был достижим
//если указанный путь уже занят, то элемент по пути должен быть преобразован в array
Config& insert_back_force(const VString& keys, const Config& other) noexcept;
Config& insert_back_force(const VString& keys, Config&& other) noexcept;

Config& insert_before(const std::string& before_key, const std::string& key, const Config& other);
Config& insert_before(const std::string& before_key, const std::string& key, Config&& other);
Config& insert_after(const std::string& after_key, const std::string& key, const Config& other);
Config& insert_after(const std::string& after_key, const std::string& key, Config&& other);

Config& push_front(const Config& other);
Config& push_front(Config&& other);
Config& push_front(const std::string& key, const Config& other);
Config& push_front(const std::string& key, Config&& other);

Config& push_at(const size_t& index, const Config& other);
Config& push_at(const size_t& index, Config&& other);
Config& push_at(const std::string& key, const Config& other);
Config& push_at(const std::string& key, Config&& other);

Config& push_back(const Config& other);
Config& push_back(Config&& other);
Config& push_back(const std::string& key, const Config& other);
Config& push_back(const std::string& key, Config&& other);

Config& push_back_force(const VString& keys, const Config& other) noexcept;
Config& push_back_force(const VString& keys, Config&& other) noexcept;

Config& push_before(const std::string& before_key, const std::string& key, const Config& other);
Config& push_before(const std::string& before_key, const std::string& key, Config&& other);
Config& push_after(const std::string& after_key, const std::string& key, const Config& other);
Config& push_after(const std::string& after_key, const std::string& key, Config&& other);

//обход explicit
__ONLY_ALLOWED_TYPES__(T)
Config& push_front(const T& other);
__ONLY_ALLOWED_TYPES__(T)
Config& push_front(T&& other);
__ONLY_ALLOWED_TYPES__(T)
Config& push_front(const std::string& key, const T& other);
__ONLY_ALLOWED_TYPES__(T)
Config& push_front(const std::string& key, T&& other);
__ONLY_ALLOWED_TYPES__(T)
Config& push_at(const size_t& index, const T& other);
__ONLY_ALLOWED_TYPES__(T)
Config& push_at(const size_t& index, T&& other);
__ONLY_ALLOWED_TYPES__(T)
Config& push_at(const std::string& key, const T& other);
__ONLY_ALLOWED_TYPES__(T)
Config& push_at(const std::string& key, T&& other);
__ONLY_ALLOWED_TYPES__(T)
Config& push_back(const T& other);
__ONLY_ALLOWED_TYPES__(T)
Config& push_back(T&& other);
__ONLY_ALLOWED_TYPES__(T)
Config& push_back(const std::string& key, const T& other);
__ONLY_ALLOWED_TYPES__(T)
Config& push_back(const std::string& key, T&& other);

__ONLY_ALLOWED_TYPES__(T)
Config& push_after(const std::string& after_key, const std::string& key, const T& other);
__ONLY_ALLOWED_TYPES__(T)
Config& push_after(const std::string& after_key, const std::string& key, T&& other);

//добавить существующий список к другому списку (только при совместимости списков)
Config& append(const Config& config);
Config& append(Config&& config);
```

## Методы удаления значений из контейнеров (выдаст exception если тип неподходящий)
```c++
Config& erase_front();
Config& erase_at(const size_t& index);
Config& erase_at(const std::string& key);
Config& erase_back();

Config& erase_at(const shared_VElement::iterator iterator);
Config& erase_at(const shared_VPairElement::iterator iterator);

Config& pop_front();
Config& pop_at(const std::string& key);
Config& pop_at(const size_t& index);
Config& pop_back();

Config& pop_at(const shared_VElement::iterator iterator);
Config& pop_at(const shared_VPairElement::iterator iterator);

Config get_and_pop_front();
Config get_and_pop_at(const size_t& index);
Config get_and_pop_at(const std::string& key);
Config get_and_pop_back();
```

## Методы получения информации о параметрах объекта
```c++
ValueType getType() const noexcept;
bool isNull() const noexcept;
bool isBool() const noexcept;
bool isNumber() const noexcept;
bool isString() const noexcept;
bool isArray() const noexcept;
bool isJson() const noexcept;
bool isContainer() const noexcept;
bool isIndexContainer() const noexcept;
bool isMapContainer() const noexcept;

bool isEqual(const Config& other, const bool compare_comments = false, const bool map_sort_important = false) const noexcept; 
bool isEqual(const tools::IElement& other, const bool compare_comments = false, const bool map_sort_important = false) const noexcept;
bool isEqual(const bool other) const noexcept;
bool isEqual(std::nullptr_t) const noexcept;
bool isEqual(const long double& other) const noexcept;
bool isEqual(const std::string& other) const noexcept;

//для контейнеров вернёт количество элементов
//для строк вернёт количество символов
//для null вернёт 0
//для всех остальных значений вернёт 1
size_t size() const noexcept;

bool isEmpty() const noexcept;
bool containsValue(const Config& config) const noexcept;
__ONLY_ALLOWED_TYPES_WITHOUT_CONFIG__(T)
bool containsValue(const T& other)
  
bool containsKey(const std::string& key) const noexcept;
```

## Операторы
```c++
Config& operator=(const Config& other) noexcept;
Config& operator=(Config&& other) noexcept;
Config& operator=(const tools::IElement& other) noexcept;
Config& operator=(tools::IElement&& other) noexcept;
Config& operator=(std::nullptr_t) noexcept;
Config& operator=(const bool other) noexcept;
__ONLY_NUMBER_TYPES__(T)
Config& operator=(const T& other) noexcept;
Config& operator=(long double&& other) noexcept;
__ONLY_STRING_TYPES__(T)
Config& operator=(const T& other) noexcept;
__ONLY_STRING_TYPES__(T)
Config& operator=(T&& other) noexcept;

/* WARNING: комментарии при сравнении не учитываются!
* Учитывание комментариев только при вызове isEqual(<object>, true) */
bool operator==(const Config& other) const;
bool operator==(const tools::IElement& other) const;
bool operator==(const bool other) const;
bool operator==(std::nullptr_t) const;
__ONLY_NUMBER_TYPES__(T)
bool operator==(const T& other) const;
__ONLY_STRING_TYPES__(T)
bool operator==(const T& other) const;

bool operator!=(const Config& other) const;
bool operator!=(const tools::IElement& other) const;
bool operator!=(const bool other) const;
bool operator!=(std::nullptr_t) const;
__ONLY_NUMBER_TYPES__(T)
bool operator!=(const T& other) const;
bool operator!=(const std::string& other) const;

//числа, контейнеры(размер), строки(длина в видимых символах)
bool operator>(const Config& other) const;
bool operator>=(const Config& other) const;
bool operator<(const Config& other) const;
bool operator<=(const Config& other) const;

//контейнеры
Config& operator[](const size_t& index);
Config operator[](const size_t& index) const;
Config& operator[](const std::string& key);
Config operator[](const std::string& key) const;

Config& operator[](const std::vector<OnlySizetOrString>& complex_key);
Config operator[](const std::vector<OnlySizetOrString>& complex_key) const;
//фикс для вызова через {}
Config& operator[](const std::initializer_list<OnlySizetOrString>& complex_key);
Config operator[](const std::initializer_list<OnlySizetOrString>& complex_key) const;
```

## Методы для работы через итераторы (только индексные массивы):
```c++
Range getRange();
const Range getRange() const;
shared_VElement::iterator array_begin();
shared_VElement::const_iterator array_cbegin() const;
shared_VElement::iterator array_end();
shared_VElement::const_iterator array_cend() const;
```

## Методы для работы через итераторы (только массивы ключ-значение):
```c++
MapRange getNamedRange();
const MapRange getNamedRange() const;
shared_VPairElement::iterator map_begin();
shared_VPairElement::const_iterator map_cbegin() const;
shared_VPairElement::iterator map_end();
shared_VPairElement::const_iterator map_cend() const;
```

## Методы вывода в строку
```c++
//вывод без комментариев, "tabulation_level == -1" => запись в одну строку
std::string toString(const ConfigFormat format = ConfigFormat::eONLY_VALUE, const CommentDesign &design = {}, const int8_t tabulation_level = 0) const noexcept;
//для совместимости с STL
friend std::ostream& operator<<(std::ostream& os, const Config& config) noexcept;
friend std::ostream& operator<<(std::ostream& os, const tools::IElement& config) noexcept;
```

## Методы чтения файлов конфигов:
```c++
//return - получившийся распаршенный корневой элемент, ElementNull если не удалось чтение
bool readFile(const std::string& file_path, const ConfigFormat format,
const CommentDesign &design =  ) noexcept;
bool readFileJson(const std::string& file_path, const CommentDesign &design =  ) noexcept;
bool readFileIni(const std::string& file_path, const CommentDesign &design =  ) noexcept;
```

## Методы записи файлов конфигов:
```c++
//return - удалось записать файл или нет
bool writeFile(const std::string& file_path, const ConfigFormat format,
const CommentDesign &design = {},
const int8_t custom_tabulation_level = 0) noexcept;
bool writeFileJson(const std::string& file_path, const CommentDesign &design = {},
const int8_t custom_tabulation_level = 0) noexcept;
bool writeFileIni(const std::string& file_path, const CommentDesign &design = {},
const int8_t custom_tabulation_level = 0) noexcept;
```

## Методы для чтения конфигов из входной строки:
```c++
bool parse(const std::string& content, const ConfigFormat format,
const CommentDesign &design =  ) noexcept;
bool parseJson(const std::string& content, const CommentDesign &design =  ) noexcept;
bool parseIni(const std::string& content, const CommentDesign &design =  ) noexcept;
bool parseYaml(const std::string& content, const CommentDesign &design =  ) noexcept;
bool parseXml(const std::string& content, const CommentDesign &design =  ) noexcept;
```

## Статичные функции чтения файлов
```c++
Config ReadFile(const std::string& file_path, const ConfigFormat format, const CommentDesign &design = {}) noexcept;
Config ReadFileJson(const std::string& file_path, const CommentDesign &design = {}) noexcept;
Config ReadFileIni(const std::string& file_path, const CommentDesign &design = {}) noexcept;
```

## Статичные функции записи файлов
```c++
//return - удалось записать файл или нет
bool WriteFile(const Config& config, const std::string& file_path, const ConfigFormat format, const CommentDesign &design = {}, const uint8_t custom_tabulation_level = 0) noexcept;
bool WriteFileJson(const Config& config, const std::string& file_path, const CommentDesign &design = {}, const uint8_t custom_tabulation_level = 0) noexcept;
bool WriteFileIni(const Config& config, const std::string& file_path, const CommentDesign &design = {}, const uint8_t custom_tabulation_level = 0) noexcept;
```

## Статичные функции чтения конфигов из входной строки
```c++
Config Parse(const std::string& content, const ConfigFormat format, const CommentDesign &design = {}) noexcept;
Config ParseJson(const std::string& content, const CommentDesign &design = {});  noexcept;
Config ParseIni(const std::string& content, const CommentDesign &design = {});   noexcept;
```
