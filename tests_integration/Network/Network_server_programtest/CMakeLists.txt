cmake_minimum_required(VERSION 3.5)
include(${SIMPLE_API_MAIN_DIR}/utils.cmake)

project(test_Network_Server LANGUAGES CXX)
print_project_name(${PROJECT_NAME})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/../../bin")

file(GLOB ${PROJECT_NAME}_sources
    "*.cpp"
    "*.h"
)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_sources})

target_include_directories(${PROJECT_NAME} PUBLIC "${SIMPLE_API_MAIN_DIR}/SimpleAPI")
target_link_libraries(${PROJECT_NAME} PUBLIC ${SimpleAPI_lib_name})
add_dependencies(${PROJECT_NAME} ${SimpleAPI_lib_name})
