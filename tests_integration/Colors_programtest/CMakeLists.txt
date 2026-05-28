cmake_minimum_required(VERSION 3.5)

set(SIMPLE_API_MAIN_DIR ${CMAKE_CURRENT_LIST_DIR}/../..)
include(${SIMPLE_API_MAIN_DIR}/utils.cmake)

project(Colors_test LANGUAGES CXX)
print_project_name(${PROJECT_NAME})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/../bin")

file(GLOB ${PROJECT_NAME}_sources
    "*.cpp"
    "*.h"
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(SimpleAPI_lib_name SimpleAPI)
else()
    set(SimpleAPI_lib_name SimpleAPId)
endif()

set(SIMPLE_API_STATIC_BUILD on)
set(SimpleAPI_lib_name ${SimpleAPI_lib_name}_static)
add_subdirectory(${SIMPLE_API_MAIN_DIR}/SimpleAPI ${CMAKE_CURRENT_BINARY_DIR}/${SimpleAPI_lib_name})

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_sources})

target_include_directories(${PROJECT_NAME} PUBLIC "${SIMPLE_API_MAIN_DIR}/SimpleAPI")
target_link_libraries(${PROJECT_NAME} PUBLIC ${SimpleAPI_lib_name})

add_dependencies(${PROJECT_NAME} ${SimpleAPI_lib_name})
