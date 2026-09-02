# Static Config

## Об идее
В ходе разработки и тестирования возможностей библиотеки SimpleAPI была сформирована идея, что класс 
Config позволяет многое, но упускает иногда главное для программиста, работающего с файлами конфигураций. 
А именно - подсветку доступных переменных при обращении к объекту Config.

Проблема исходит из того, что объект Config формируется на ходу, то есть ему совершенно неважно, что вы 
дадите ему на вход.

В связи с этим появилась концепция, что программист-пользователь библиотеки может через макросы описать 
необходимые ему типы и названия полей структуры, а библиотека берёт на себя её заполнение и валидацию 
средствами класса Config и паттерн X-macro.

На данный момент эта технология поддерживает:
- одиночные примитивы (строки, числа, bool, char)
- вложенные структуры, описанные через эту же технологию
- перечисления (enum) - при этом в конечном файле конфига хранится строковое представление поля enum
- STL-контейнеры: 
    - std::vector<T>
    - std::list<T>
    - std::forward_list<T>
    - std::deque<T>
    - std::set<T>
    - std::multiset<T>
    - std::unordered_set<T>
    - std::unordered_multiset<T>
    - std::queue<T>
    - std::priority_queue<T>
    - std::stack<T>
    - std::array<T, size>
    - std::bitset<size>
- ассоциативные STL-контейнеры (K = строка, число или bool):
    - std::map<K, T>
    - std::multimap<K, T>
    - std::unordered_map<K, T>
    - std::unordered_multimap<K, T>

## Макросы описания
### X-Macro
Данный макрос целиком описывается пользователем, но суть в следующем - каждый из описанных ниже макросов 
принимает на вход определённое количество параметров. Не все из них обязательные. Это как с вызовом функций,
у которых переменное количество параметров.

Опциональные параметры отмечены знаком "(O)".

### SAPI_REGISTER_ENUM(EnumName, X_MACRO_FOR_CURRENT_ENUM, UnderlyingType(O))
Параметры X-Macro:
- element
- value

std::string ToString(value)
void FromString(input_str, out_value)
std::string EnumPossibleVariants(value)

### SAPI_REGISTER_CONFIG(StructName, X_MACRO_FOR_CURRENT_STRUCT_FIELDS)
Параметры X-Macro:
- type
- name
- default_value
- lambda
- prefix_comment
- suffix_comment

## Примеры использования

