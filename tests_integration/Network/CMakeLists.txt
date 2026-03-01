cmake_minimum_required(VERSION 3.5)

set(SIMPLE_API_MAIN_DIR ${CMAKE_CURRENT_LIST_DIR}/../..)
include(${SIMPLE_API_MAIN_DIR}/utils.cmake)

project(Network_test)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(SimpleAPI_lib_name SimpleAPI)
else()
    set(SimpleAPI_lib_name SimpleAPId)
endif()

add_subdirectory(${SIMPLE_API_MAIN_DIR}/SimpleAPI ${CMAKE_CURRENT_BINARY_DIR}/${SimpleAPI_lib_name})

#test sockets
file(GLOB NETWORK_DIRS ${CMAKE_CURRENT_LIST_DIR}/Network*)
foreach(NETWORK_DIR ${NETWORK_DIRS})
    add_subdirectory(${NETWORK_DIR})
endforeach()
