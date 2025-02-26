cmake_minimum_required(VERSION 3.5)
include(${SIMPLE_API_MAIN_DIR}/utils.cmake)

project(Append_library_add_subdirectory_Test LANGUAGES CXX)
print_project_name(${PROJECT_NAME})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/../bin")

file(GLOB ${PROJECT_NAME}_sources
    "*.cpp"
    "*.h"
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../SimpleAPI ${CMAKE_CURRENT_SOURCE_DIR}/build/SimpleAPI)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_sources})

add_dependencies(${PROJECT_NAME} SimpleAPI)
target_link_libraries(${PROJECT_NAME} PUBLIC SimpleAPI)
