cmake_minimum_required(VERSION 3.5)

set(SIMPLE_API_OUTPUT_LIB_DIR "${CMAKE_CURRENT_LIST_DIR}/lib")
set(SIMPLE_API_OUTPUT_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include")

project(SimpleAPI_library_builder)

set(SIMPLE_API_STATIC_BUILD on)
add_subdirectory(
    "${CMAKE_CURRENT_LIST_DIR}/../SimpleAPI"
    "${CMAKE_CURRENT_LIST_DIR}/build-cache/SimpleAPI"
)
