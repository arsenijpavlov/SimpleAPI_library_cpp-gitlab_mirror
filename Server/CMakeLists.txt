cmake_minimum_required(VERSION 3.5)

project(project_Server LANGUAGES CXX)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/../bin")

file(GLOB server_sources
    "*.cpp"
    "*.h"
)
add_executable(Server)
target_sources(Server PUBLIC ${server_sources})

target_include_directories(Server PUBLIC "${PROJECT_BINARY_DIR}/../libs/include")
target_link_libraries(Server PUBLIC SimpleAPI)
