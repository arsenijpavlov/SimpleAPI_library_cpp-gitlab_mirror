cmake_minimum_required(VERSION 3.5)

project(project_Client LANGUAGES CXX)
message("project: project_unit_tests")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/../bin")

#Google test libs
add_subdirectory(googletest-1.15.0)
add_subdirectory(simple_test)

function (create_test test_group_name)
    file(GLOB test_group_sources
        "${test_group_name}"
        "*.h"
    )
    string(REPLACE ".cpp" "" test_name ${test_group_name})
    string(REGEX REPLACE ".*/" "" test_name ${test_name})

    add_executable(${test_name})
    target_sources(${test_name} PUBLIC ${test_group_sources})
    target_include_directories(${test_name} PUBLIC "${PROJECT_SOURCE_DIR}/../SimpleAPI")
    target_link_libraries(${test_name} PUBLIC SimpleAPI gtest gmock)
endfunction()

file(GLOB TEST_LIST "${CMAKE_CURRENT_LIST_DIR}/test_*.cpp")
foreach(TEST ${TEST_LIST})
    create_test(${TEST})
endforeach()

#===============================================
#test Json
if(ENABLE_JSON_TEST_APP)
    add_subdirectory(Json_programtest)
endif()
unset(ENABLE_JSON_TEST_APP CACHE)

#test sockets
if(ENABLE_SOCKETS_TEST_APP)
    file(GLOB NETWORK_DIRS ${CMAKE_CURRENT_LIST_DIR}/Network*_programtest)
    foreach(NETWORK_DIR ${NETWORK_DIRS})
        add_subdirectory(${NETWORK_DIR})
    endforeach()
endif()
unset(ENABLE_SOCKETS_TEST_APP CACHE)

#test colors
if(ENABLE_COLORS_TEST_APP)
    add_subdirectory(Colors_programtest)
endif()
unset(ENABLE_COLORS_TEST_APP CACHE)
#===============================================
