cmake_minimum_required(VERSION 3.5)

project(project_Client LANGUAGES CXX)
message("project: project_unit_tests")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/../bin")

function (create_test test_group)
    file(GLOB test_group_sources
        "${test_group}.cpp"
        "*.h"
    )
    add_executable(${test_group})
    target_sources(${test_group} PUBLIC ${test_group_sources})
    target_include_directories(${test_group} PUBLIC "${PROJECT_BINARY_DIR}/../lib/include")
    target_link_libraries(${test_group} PUBLIC SimpleAPI gtest gmock)
endfunction()

create_test(TEST_JSON)
create_test(TEST_SOCKETS)
