cmake_minimum_required(VERSION 3.5)

project(project_Client LANGUAGES CXX)
message("project: project_Test_COLORS")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/../bin")

file(GLOB colors_sources
    "*.cpp"
    "*.h"
)
add_executable(Test_COLORS)
target_sources(Test_COLORS PUBLIC ${colors_sources})

target_include_directories(Test_COLORS PUBLIC "${PROJECT_SOURCE_DIR}/../SimpleAPI")
target_link_libraries(Test_COLORS PUBLIC SimpleAPI)
