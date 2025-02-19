cmake_minimum_required(VERSION 3.5)

set(SIMPLE_API_MAIN_DIR ${CMAKE_CURRENT_LIST_DIR})
set(SIMPLE_API_CMAKE_GENERATED_DIR ${CMAKE_CURRENT_LIST_DIR}/build-cmake-cache)

include(${SIMPLE_API_MAIN_DIR}/utils.cmake)

if(CMAKE_BUILD_TYPE STREQUAL Debug )
    message("${Yellow}DEBUG mode enabled${Reset}")
    add_compile_definitions(__DEBUG__)
endif()

project(SimpleAPI_main LANGUAGES CXX)
print_project_name(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_TESTS_UNIT        "Google tests enabled"          on)
option(ENABLE_TEST_LIBRARY_APP  "Base lib testing app enabled"  off)
option(ENABLE_TEST_JSON_APP     "Json testing app enabled"      off)
option(ENABLE_TEST_SOCKETS_APP  "Sockets testing app enabled"   off)
option(ENABLE_TEST_COLORS_APP   "Color choiser app enabled"     off)
option(ENABLE_TEST_SECURITY_APP "(De)Chiphering app enabled"    off)

#library
add_subdirectory(SimpleAPI ${SIMPLE_API_CMAKE_GENERATED_DIR})


#TESTS =================================================================
if(ENABLE_TESTS_UNIT)
    #unit tests
    add_subdirectory(tests_unit)
endif()
unset(ENABLE_TESTS_UNIT CACHE)

#эти тесты предполагают подключение уже сгенерированной библиотеки
add_subdirectory(tests_complex)
#================================================================= TESTS
