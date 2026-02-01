cmake_minimum_required(VERSION 3.5)
set(SIMPLE_API_MAIN_DIR "${CMAKE_CURRENT_LIST_DIR}/..")
include(${SIMPLE_API_MAIN_DIR}/utils.cmake)
set(CMAKE_CXX_STANDARD 11)

add_subdirectory(Shared)

set(SIMPLE_API_STATIC_BUILD on)
add_subdirectory(Static)
