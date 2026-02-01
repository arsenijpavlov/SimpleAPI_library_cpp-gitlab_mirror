cmake_minimum_required(VERSION 3.5)
include(${SIMPLE_API_MAIN_DIR}/utils.cmake)

project(test_Colors LANGUAGES CXX)
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
#set(SimpleAPI_lib_name ${SimpleAPI_lib_name})

set(SIMPLE_API_STATIC_BUILD on)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../SimpleAPI ${CMAKE_CURRENT_BINARY_DIR}/${SimpleAPI_lib_name}_static)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_sources})

target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/../../SimpleAPI")
target_link_libraries(${PROJECT_NAME} PUBLIC -lib${SimpleAPI_lib_name}.a)

add_dependencies(${PROJECT_NAME} ${SimpleAPI_lib_name})
