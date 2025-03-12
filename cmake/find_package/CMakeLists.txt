cmake_minimum_required(VERSION 3.5)

# Эти переменные нужны, чтобы файлы библиотеки были сконфигурированы в конкретной директории
set(SIMPLE_API_OUTPUT_LIB_DIR "${CMAKE_CURRENT_LIST_DIR}/lib")
set(SIMPLE_API_OUTPUT_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include")

# Обязательно, т.к. CMakeLists.txt корневой для данной сборки
project(SimpleAPI_library_builder)

if(CMAKE_BUILD_TYPE STREQUAL "RELEASE")
    add_subdirectory(
        "${CMAKE_CURRENT_LIST_DIR}/../../SimpleAPI"
        "${CMAKE_CURRENT_LIST_DIR}/build/SimpleAPI"
    )
else()
    add_subdirectory(
        "${CMAKE_CURRENT_LIST_DIR}/../../SimpleAPI"
        "${CMAKE_CURRENT_LIST_DIR}/build/SimpleAPId"
    )
endif()
