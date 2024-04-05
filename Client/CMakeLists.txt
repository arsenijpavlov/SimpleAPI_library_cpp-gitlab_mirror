cmake_minimum_required(VERSION 3.5)

project(project_Client LANGUAGES CXX)
message("project: project_Client")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/../bin")

file(GLOB client_sources
    "*.cpp"
    "*.h"
)
add_executable(Client)
target_sources(Client PUBLIC ${client_sources})

target_include_directories(Client PUBLIC "${PROJECT_BINARY_DIR}/../libs/include")
target_link_libraries(Client PUBLIC SimpleAPI)
