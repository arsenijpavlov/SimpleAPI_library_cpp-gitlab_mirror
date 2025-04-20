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

#library
set(SIMPLE_API_STATIC_BUILD on) #API по идее должен мочь меняться на ходу
add_subdirectory(SimpleAPI)


#TESTS =================================================================
if(ENABLE_TESTS_UNIT)
    enable_testing()
    #unit tests
    add_subdirectory(tests_unit)
endif()
unset(ENABLE_TESTS_UNIT CACHE)
#================================================================= TESTS
