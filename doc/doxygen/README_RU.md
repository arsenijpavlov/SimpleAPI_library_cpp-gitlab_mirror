# Установка Doxygen на Ubuntu

```bash
sudo apt install doxygen graphviz
```

# Создание документации

Перейдите в корневую директорию проекта SimpleAPI и запустите следующую команду
```bash
doxygen Doxyfile_RU
```

# Проблемы отображения

Браузер Google Chrome по умолчанию некорректно отображает страницы, если на них есть скрипты JavaScript. Варинтов решения несколько.

## Поднять локальный встроенный сервер через Python

Перейдите в корневую директорию проекта SimpleAPI и запустите следующую команду
```bash
cd doc/doxygen/ru/html
python3 -m http.server 8000
```

После этого в браузере появится документация проекта по адресу `127.0.0.1:8000`

## Отключить правило Google Chrome

Нужно запускать Google Chrome с дополнительным флагом
```bash
google-chrome --allow-file-access-from-files
```

## Отключить встроенный поисковик внутри документации Doxygen

В файле `Doxyfile_RU` выставить следующие значения:
```ini
SEARCHENGINE           = NO
SERVER_BASED_SEARCH    = NO
```

