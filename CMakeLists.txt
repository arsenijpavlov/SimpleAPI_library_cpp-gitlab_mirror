cmake_minimum_required(VERSION 3.5)

set(SIMPLE_API_MAIN_DIR ${CMAKE_CURRENT_LIST_DIR})
include(${SIMPLE_API_MAIN_DIR}/utils.cmake)

if(CMAKE_BUILD_TYPE STREQUAL Debug )
    message("${Yellow}DEBUG mode enabled${Reset}")
    add_compile_definitions(__DEBUG__)
endif()

project(SimpleAPI_main LANGUAGES CXX)
print_project_name(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(UNIT_TESTS               "Google tests enabled"          on)
option(ENABLE_JSON_TEST_APP     "Json testing app enabled"      on)
option(ENABLE_SOCKETS_TEST_APP  "Sockets testing app enabled"   on)
option(ENABLE_COLORS_TEST_APP   "Color choiser app enabled"     on)

#library
add_subdirectory(SimpleAPI)

#test Json
if(ENABLE_JSON_TEST_APP)
    add_subdirectory(Test_Json)
endif()
unset(ENABLE_JSON_TEST_APP CACHE)

#test sockets
if(ENABLE_SOCKETS_TEST_APP)
    add_subdirectory(Test_Client)
    add_subdirectory(Test_Server)
endif()
unset(ENABLE_SOCKETS_TEST_APP CACHE)

#test colors
if(ENABLE_COLORS_TEST_APP)
    add_subdirectory(Test_COLORS)
endif()
unset(ENABLE_COLORS_TEST_APP CACHE)

#TESTS
if(UNIT_TESTS)
    #unit tests
    add_subdirectory(tests)
endif()
unset(UNIT_TESTS CACHE)
