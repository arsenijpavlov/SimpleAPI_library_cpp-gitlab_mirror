cmake_minimum_required(VERSION 3.5)

# Обязательно, т.к. CMakeLists.txt корневой для данной сборки
project(SimpleAPI_library_builder)

add_subdirectory(
    "${SIMPLE_API_MAIN_DIR}/SimpleAPI"
    "${CMAKE_CURRENT_LIST_DIR}/build/SimpleAPI"
)
