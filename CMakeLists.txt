cmake_minimum_required(VERSION 3.5)

if(NOT WIN32)
  string(ASCII 27 Esc)
  set(Reset     "${Esc}[m")
  set(Bold      "${Esc}[1m")
  set(Red       "${Esc}[31m")
  set(Green     "${Esc}[32m")
  set(Yellow    "${Esc}[33m")
  set(Blue      "${Esc}[34m")
  set(Magenta   "${Esc}[35m")
  set(Cyan      "${Esc}[36m")
  set(White     "${Esc}[37m")
endif()

if(CMAKE_BUILD_TYPE STREQUAL Debug )
    message("${Yellow}Debug mode enabled${Reset}")
    add_compile_definitions(__DEBUG__)
endif()

project(project_SimpleAPI_main LANGUAGES CXX)
message("${Bold}project: ${Green}project_SimpleAPI_main${Reset}")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(UNIT_TESTS               "Google tests enabled"          on)
option(ENABLE_JSON_TEST_APP     "Json testing app enabled"      on)
option(ENABLE_SOCKETS_TEST_APP  "Sockets testing app enabled"   off)
option(ENABLE_COLORS_TEST_APP   "Color choiser app enabled"     off)

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
    #Google test libs
    add_subdirectory(googletest-1.15.0)

    #unit tests
    add_subdirectory(tests)
endif()
unset(UNIT_TESTS CACHE)
