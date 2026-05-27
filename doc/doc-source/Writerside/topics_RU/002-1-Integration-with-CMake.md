# 2.1. Интеграция с CMake

Минимальная версия CMake для сборки библиотеки 3.5.

# Библиотеку можно добавить несколькими способами:

## Как самостоятельный CMake проект

```bash
cd <SimpleAPI_dir>
mkdir -p build
cd build
cmake [-DCMAKE_BUILD_TYPE=Debug] .. # флаг -DCMAKE_BUILD_TYPE указывается опционально
make -j$(nproc)
```

В директории build появится директория lib с собранной библиотекой в формате .so и директорией include со всеми заголовочными файлами библиотеки.

## Как часть другого CMake проекта (add_subdirectory)

В составе вызывающего CMakeLists.txt следует указать следующий код CMake:

```cmake
set(SIMPLE_API_MAIN_DIR ${CMAKE_CURRENT_LIST_DIR}/../..) # указать путь до базовой директории библиотеки
include(${SIMPLE_API_MAIN_DIR}/utils.cmake) # для доступа к макросам библиотеки
...
# код для определения типа сборки
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(SimpleAPI_lib_name SimpleAPI)
else()
    set(SimpleAPI_lib_name SimpleAPId)
endif()

add_subdirectory(${SIMPLE_API_MAIN_DIR}/SimpleAPI ${CMAKE_CURRENT_BINARY_DIR}/${SimpleAPI_lib_name})

# код для создания таргета основного приложения
add_executable(...)
...

# предполагается, что имя таргета совпадает с именем проекта (PROJECT_NAME)
# добавление зависимости от библиотеки для созданного таргета
target_link_libraries(${PROJECT_NAME} PUBLIC ${SimpleAPI_lib_name})
target_include_directories(${PROJECT_NAME} PUBLIC ${SIMPLE_API_MAIN_DIR}/SimpleAPI)
add_dependencies(${PROJECT_NAME} ${SimpleAPI_lib_name})
```

## Через использование технологии find_package

Для использования этого варианта должна быть установлена библиотека `pkg-config`:
```bash
sudo apt-get install pkg-config # Ubuntu
```

Пример кода CMake
```cmake
set(SIMPLE_API_MAIN_DIR ${CMAKE_CURRENT_LIST_DIR}/../..) # указать путь до базовой директории библиотеки
include(${SIMPLE_API_MAIN_DIR}/utils.cmake) # для доступа к макросам библиотеки

# код для создания таргета основного приложения
add_executable(...)
...

# предполагается, что имя таргета совпадает с именем проекта (PROJECT_NAME)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    find_package(SimpleAPI REQUIRED)
    if(SimpleAPI_FOUND)
        target_link_libraries(${PROJECT_NAME} PUBLIC SimpleAPI)
        target_include_directories(${PROJECT_NAME} PUBLIC ${SimpleAPI_INCLUDE_DIRS})
    else()
        message(FATAL "SimpleAPI not found")
    endif(SimpleAPI_FOUND)
else()
    find_package(SimpleAPId REQUIRED)
    if(SimpleAPId_FOUND)
        target_link_libraries(${PROJECT_NAME} PUBLIC SimpleAPId)
        target_include_directories(${PROJECT_NAME} PUBLIC ${SimpleAPId_INCLUDE_DIRS})
    else()
        message(FATAL "SimpleAPId not found")
    endif(SimpleAPId_FOUND)
endif()
```


