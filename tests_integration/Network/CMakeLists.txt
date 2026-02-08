cmake_minimum_required(VERSION 3.5)

set(SIMPLE_API_MAIN_DIR ${CMAKE_CURRENT_LIST_DIR}/../..)
include(${SIMPLE_API_MAIN_DIR}/utils.cmake)

project(Network_test)


#test sockets
if(ENABLE_TEST_SOCKETS_APP)
    file(GLOB NETWORK_DIRS ${CMAKE_CURRENT_LIST_DIR}/Network*)
    foreach(NETWORK_DIR ${NETWORK_DIRS})
        add_subdirectory(${NETWORK_DIR})
    endforeach()
endif()
