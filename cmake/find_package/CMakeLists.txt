cmake_minimum_required(VERSION 3.5)

# Эти переменные нужны, чтобы файлы библиотеки были сконфигурированы в конкретной директории
set(SIMPLE_API_OUTPUT_LIB_DIR "${CMAKE_CURRENT_LIST_DIR}/lib")
set(SIMPLE_API_OUTPUT_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/lib/include")

# Обязательно, т.к. CMakeLists.txt корневой для данной сборки
project(SimpleAPI_library_builder)

add_subdirectory(
    "${CMAKE_CURRENT_LIST_DIR}/../../SimpleAPI"
    "${CMAKE_CURRENT_LIST_DIR}/build/SimpleAPI"
)

set(SIMPLE_API_STATIC_BUILD on)
add_subdirectory(
    "${CMAKE_CURRENT_LIST_DIR}/../../SimpleAPI"
    "${CMAKE_CURRENT_LIST_DIR}/build/SimpleAPI_static"
)
