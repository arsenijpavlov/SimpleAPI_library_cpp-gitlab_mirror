cmake_minimum_required(VERSION 3.5)

project(project_Client LANGUAGES CXX)
message("project: project_Test_Client")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/../bin")

file(GLOB client_sources
    "*.cpp"
    "*.h"
)
add_executable(Test_Client)
target_sources(Test_Client PUBLIC ${client_sources})

target_include_directories(Test_Client PUBLIC "${PROJECT_BINARY_DIR}/../libs/include")
target_link_libraries(Test_Client PUBLIC SimpleAPI)
