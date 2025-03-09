cmake_minimum_required(VERSION 3.5)
include(${SIMPLE_API_MAIN_DIR}/utils.cmake)

project(Append_library_find_package_Test LANGUAGES CXX)

print_project_name(${PROJECT_NAME})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/../bin")

file(GLOB ${PROJECT_NAME}_sources
    "*.cpp"
    "*.h"
)

#include(${CMAKE_CURRENT_SOURCE_DIR}/../../../SimpleAPI/FindSimpleAPI.cmake)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../SimpleAPI ${CMAKE_CURRENT_BINARY_DIR}/SimpleAPI)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_sources})

#target_link_libraries(${PROJECT_NAME} PUBLIC SimpleAPI)
#target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../../SimpleAPI)
#add_dependencies(${PROJECT_NAME} SimpleAPI)

#include(${SIMPLE_API_MAIN_DIR}/SimpleAPI/FindSimpleAPI.cmake)
#include(${CMAKE_CURRENT_SOURCE_DIR}/../../../SimpleAPI/FindSimpleAPI.cmake)

list(APPEND CMAKE_MODULE_PATH "${SIMPLE_API_MAIN_DIR}/cmake_modules")
find_package(SimpleAPI REQUIRED)

if(SimpleAPI_FOUND)
    message("SimpleAPI found")
else()
    message("SimpleAPI not found")
endif()
