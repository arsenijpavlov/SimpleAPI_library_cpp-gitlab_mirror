cmake_minimum_required(VERSION 3.5)

project(project_Client LANGUAGES CXX)
message("project: project_unit_tests")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/../bin")

file(GLOB unit_tests_sources
    "*.cpp"
    "*.h"
)
add_executable(unit_tests)
target_sources(unit_tests PUBLIC ${unit_tests_sources})

target_include_directories(unit_tests PUBLIC "${PROJECT_BINARY_DIR}/../lib/include")

target_link_libraries(unit_tests PUBLIC SimpleAPI gtest gmock)
