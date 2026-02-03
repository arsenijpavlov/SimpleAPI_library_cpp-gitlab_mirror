cmake_minimum_required(VERSION 3.5)
set(SIMPLE_API_MAIN_DIR "${CMAKE_CURRENT_LIST_DIR}/..")
include(${SIMPLE_API_MAIN_DIR}/utils.cmake)
set(CMAKE_CXX_STANDARD 11)

project(SimpleAPI_Shared_Static LANGUAGES CXX)
print_project_name(${PROJECT_NAME})

add_subdirectory(Shared)

set(SIMPLE_API_STATIC_BUILD on)
add_subdirectory(Static)
