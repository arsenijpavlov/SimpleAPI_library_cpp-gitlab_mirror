cmake_minimum_required(VERSION 3.5)

project(project_all_tests LANGUAGES CXX)
message("project: project_unit_tests")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/../tests")

#Google test libs
add_subdirectory(googletest-1.15.0)

function (create_test test_group_name)
    file(GLOB test_group_sources
        "${test_group_name}"
        "*.h"
    )
    string(REPLACE ".cpp" "" test_name ${test_group_name})
    string(REGEX REPLACE ".*/" "unit_" test_name ${test_name})

    add_executable(${test_name})
    target_sources(${test_name} PUBLIC ${test_group_sources})
    target_link_libraries(${test_name} PUBLIC gtest gmock)

    target_include_directories(${test_name} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../SimpleAPI")
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_link_libraries(${test_name} PUBLIC SimpleAPI)
    else()
        target_link_libraries(${test_name} PUBLIC SimpleAPId)
    endif()

    add_test(NAME ${test_name}
        COMMAND "${test_name}"
        WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
    )
endfunction()

file(GLOB TEST_LIST "${CMAKE_CURRENT_LIST_DIR}/test_*.cpp")
foreach(TEST ${TEST_LIST})
    create_test(${TEST})
endforeach()
