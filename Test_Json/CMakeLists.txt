cmake_minimum_required(VERSION 3.5)

project(project_Client LANGUAGES CXX)
message("project: project_Test_Json")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/../bin")

file(GLOB json_sources
    "*.cpp"
    "*.h"
)
add_executable(Test_Json)
target_sources(Test_Json PUBLIC ${json_sources})

target_include_directories(Test_Json PUBLIC "${PROJECT_SOURCE_DIR}/../SimpleAPI")
target_link_libraries(Test_Json PUBLIC SimpleAPI)
