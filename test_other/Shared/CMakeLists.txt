cmake_minimum_required(VERSION 3.5)

project(Test_Shared LANGUAGES CXX)
print_project_name(${PROJECT_NAME})

add_executable(${PROJECT_NAME} main.cpp)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(LIB_NAME SimpleAPI)
else()
    set(LIB_NAME SimpleAPId)
endif()
add_subdirectory(${SIMPLE_API_MAIN_DIR}/SimpleAPI ${CMAKE_CURRENT_BINARY_DIR}/SimpleAPI)

target_link_libraries(${PROJECT_NAME} PUBLIC ${LIB_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${SIMPLE_API_MAIN_DIR}/SimpleAPI)
add_dependencies(${PROJECT_NAME} ${LIB_NAME})
