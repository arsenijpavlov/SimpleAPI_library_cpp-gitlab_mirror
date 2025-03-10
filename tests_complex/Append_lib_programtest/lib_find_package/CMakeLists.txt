cmake_minimum_required(VERSION 3.5)
include(${SIMPLE_API_MAIN_DIR}/utils.cmake)

project(Append_library_find_package_Test LANGUAGES CXX)

print_project_name(${PROJECT_NAME})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/../bin")

file(GLOB ${PROJECT_NAME}_sources
    "*.cpp"
    "*.h"
)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_sources})

list(APPEND CMAKE_MODULE_PATH "${SIMPLE_API_MAIN_DIR}/cmake_modules")
find_package(SimpleAPI REQUIRED)

if(SimpleAPI_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${SimpleAPI_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${SimpleAPI_INCLUDE_DIRS})
else()
    #TODO: собрать библиотеку и провести поиск заново
endif(SimpleAPI_FOUND)
