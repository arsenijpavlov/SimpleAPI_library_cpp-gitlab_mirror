cmake_minimum_required(VERSION 3.5)
include(${SIMPLE_API_MAIN_DIR}/utils.cmake)

project(Append_library_Test LANGUAGES CXX)
print_project_name(${PROJECT_NAME})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/../bin")

file(GLOB ${PROJECT_NAME}_sources
    "*.cpp"
    "*.h"
)
add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_sources})

#add_subdirectory(${SIMPLE_API_MAIN_DIR}/SimpleAPI ${SIMPLE_API_CMAKE_GENERATED_DIR})
add_dependencies(${PROJECT_NAME} SimpleAPI)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../libSimpleAPI")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../libSimpleAPI/include")
target_link_libraries(${PROJECT_NAME} PUBLIC -lSimpleAPI)

