cmake_minimum_required(VERSION 3.5)
include(${SIMPLE_API_MAIN_DIR}/utils.cmake)

project(SimpleAPI LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
print_project_name(${PROJECT_NAME})

file(GLOB_RECURSE ${PROJECT_NAME}_sources "${PROJECT_SOURCE_DIR}/*.cpp" )
file(GLOB_RECURSE ${PROJECT_NAME}_headers "${PROJECT_SOURCE_DIR}/*.h" )

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/../lib${PROJECT_NAME}/")
message("CMAKE_LIBRARY_OUTPUT_DIRECTORY: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

add_library(${PROJECT_NAME} SHARED)
target_sources(${PROJECT_NAME} PRIVATE
    ${${PROJECT_NAME}_sources}
    ${${PROJECT_NAME}_headers}
)
target_link_libraries(${PROJECT_NAME} PUBLIC -pthread)

# упаковка готовой библиотеки в конечную директорию
add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_LIST_DIR}/../copy_includes.cmake"
                "${CMAKE_CURRENT_LIST_DIR}" "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/include"
)
