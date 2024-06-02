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

#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_JSON_TEST     "Testing JSon enabled"      off)
option(ENABLE_CLIENT_TEST   "Testing Client enabled"    off)
option(ENABLE_SERVER_TEST   "Testing Server enabled"    on)

#library
add_subdirectory(SimpleAPI)

#test Json
if(ENABLE_JSON_TEST)
    add_subdirectory(Test_Json)
endif()
unset(ENABLE_CLIENT CACHE)

#test client
if(ENABLE_CLIENT_TEST)
    add_subdirectory(Test_Client)
endif()
unset(ENABLE_CLIENT CACHE)

#test server
if(ENABLE_SERVER_TEST)
    add_subdirectory(Test_Server)
endif()
unset(ENABLE_SERVER CACHE)
