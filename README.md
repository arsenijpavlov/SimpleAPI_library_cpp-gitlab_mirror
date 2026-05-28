# SimpleAPI library

**SimpleAPI** - это библиотека для разработчиков C++ под Linux. Для совместимости с большинством старых проектов используется стандарт C++11.

Проект предоставляет инструменты для работы с JSON, INI, _XML(TODO)_ и _YAML(TODO)_ с добавлением возможности указания пользовательских комментариев внутри файлов конфигурации. А также некоторые инструменты для быстрого развёртывания UDP сокетных соединений с возможностью фрагментации сообщений и _шифрования(TODO)_.

Ознакомиться более подробно с проектом можно прочитав [документацию](doc)

Примеры кода:
- [взаимодействие с классом Config](tests_unit/test_element.cpp)
- [работа с объектами в формате Json](tests_unit/test_json.cpp)
- [работа с массивами в формате Json](tests_unit/test_json_array.cpp)
- [работа с форматом Ini](tests_unit/test_json_ini.cpp)
- [сервер UDP](tests_integration/Network/Network_server_programtest/main.cpp)
- [клиент UDP](tests_integration/Network/Network_client_programtest/main.cpp)

