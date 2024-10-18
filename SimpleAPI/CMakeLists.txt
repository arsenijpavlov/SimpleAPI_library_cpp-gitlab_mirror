cmake_minimum_required(VERSION 3.5)
include(${SIMPLE_API_MAIN_DIR}/utils.cmake)

project(SimpleAPI LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
print_project_name(${PROJECT_NAME})

file(GLOB_RECURSE ${PROJECT_NAME}_sources "${PROJECT_SOURCE_DIR}/*.cpp" )
file(GLOB_RECURSE ${PROJECT_NAME}_headers "${PROJECT_SOURCE_DIR}/*.h" )

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/../lib/")
message("CMAKE_LIBRARY_OUTPUT_DIRECTORY: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

add_library(${PROJECT_NAME} SHARED)
target_sources(${PROJECT_NAME} PRIVATE
    ${${PROJECT_NAME}_sources}
    ${${PROJECT_NAME}_headers}
)
target_link_libraries(${PROJECT_NAME} PUBLIC -pthread)


execute_process(
    COMMAND bash -c "mkdir -p ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/include && cd $_; \
        cp -vf ${PROJECT_SOURCE_DIR}/*.h ."
    OUTPUT_VARIABLE var
)
message("cp: \n${var}")
