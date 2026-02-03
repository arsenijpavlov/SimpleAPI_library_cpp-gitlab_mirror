cmake_minimum_required(VERSION 3.5)
include(${SIMPLE_API_MAIN_DIR}/utils.cmake)

project(Append_library_find_package_Test LANGUAGES CXX)

print_project_name(${PROJECT_NAME})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/../../bin")

file(GLOB ${PROJECT_NAME}_sources
    "*.cpp"
    "*.h"
)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_sources})

list(APPEND CMAKE_MODULE_PATH "${SIMPLE_API_MAIN_DIR}/cmake/find_package")

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    find_package(SimpleAPI REQUIRED)
    if(SimpleAPI_FOUND)
        target_link_libraries(${PROJECT_NAME} PUBLIC SimpleAPI)
        target_include_directories(${PROJECT_NAME} PUBLIC ${SimpleAPI_INCLUDE_DIRS})
    else()
        message(FATAL "SimpleAPI not found")
    endif(SimpleAPI_FOUND)
else()
    find_package(SimpleAPId REQUIRED)
    if(SimpleAPId_FOUND)
        target_link_libraries(${PROJECT_NAME} PUBLIC SimpleAPId)
        target_include_directories(${PROJECT_NAME} PUBLIC ${SimpleAPId_INCLUDE_DIRS})
    else()
        message(FATAL "SimpleAPId not found")
    endif(SimpleAPId_FOUND)
endif()
