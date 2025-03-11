cmake_minimum_required(VERSION 3.5)
set(SIMPLE_API_MAIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")
include(${SIMPLE_API_MAIN_DIR}/utils.cmake)

project(SimpleAPI_complex_tests)

option(ENABLE_TEST_LIBRARY_APP  "Base lib testing app enabled"  on)
option(ENABLE_TEST_JSON_APP     "Json testing app enabled"      off)
option(ENABLE_TEST_SOCKETS_APP  "Sockets testing app enabled"   off)
option(ENABLE_TEST_COLORS_APP   "Color choiser app enabled"     off)
option(ENABLE_TEST_SECURITY_APP "(De)Chiphering app enabled"    off)


#test append library
if(ENABLE_TEST_LIBRARY_APP)
    option(ENABLE_TEST_LIBRARY_AS_ADD_SUBDIRECTORY  "TEST add_subdirectory()"   on)
    option(ENABLE_TEST_LIBRARY_AS_FIND_PACKAGE      "TEST find_package()"       on)

    add_subdirectory(Append_lib_programtest)
endif()
unset(ENABLE_TEST_LIBRARY_APP)

#test Json
if(ENABLE_TEST_JSON_APP)
    add_subdirectory(Json_programtest)
endif()
unset(ENABLE_TEST_JSON_APP)

#test sockets
if(ENABLE_TEST_SOCKETS_APP)
    file(GLOB NETWORK_DIRS ${CMAKE_CURRENT_LIST_DIR}/Network*_programtest)
    foreach(NETWORK_DIR ${NETWORK_DIRS})
        add_subdirectory(${NETWORK_DIR})
    endforeach()
endif()
unset(ENABLE_TEST_SOCKETS_APP)

#test colors
if(ENABLE_TEST_COLORS_APP)
    add_subdirectory(Colors_programtest)
endif()
unset(ENABLE_TEST_COLORS_APP)

#test security
if(ENABLE_TEST_SECURITY_APP)
    add_subdirectory(Security_programtest)
endif()
unset(ENABLE_TEST_SECURITY_APP)
