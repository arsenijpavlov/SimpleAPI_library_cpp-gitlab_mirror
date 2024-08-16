cmake_minimum_required(VERSION 3.5)

project(project_Client LANGUAGES CXX)
message("project: project_simple_test")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/../bin")

file(GLOB simple_sources
    "*.cpp"
    "*.h"
)
add_executable(Test_simple)
target_sources(Test_simple PUBLIC ${simple_sources})

target_include_directories(Test_simple PUBLIC "${PROJECT_SOURCE_DIR}/../../SimpleAPI")
target_link_libraries(Test_simple PUBLIC SimpleAPI)
