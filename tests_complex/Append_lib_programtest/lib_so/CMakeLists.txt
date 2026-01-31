cmake_minimum_required(VERSION 3.5)
include(${SIMPLE_API_MAIN_DIR}/utils.cmake)

project(Append_library_lib_so_Test LANGUAGES CXX)
print_project_name(${PROJECT_NAME})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/../bin")

file(GLOB ${PROJECT_NAME}_sources
    "*.cpp"
    "*.h"
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(BUILD_STATE_NAME "libSimpleAPI.so")
else()
    set(BUILD_STATE_NAME "libSimpleAPId.so")
endif()

# Предварительная сборка библиотеки, чтобы было, куда обращаться
# Предполагаем, что если есть файл библиотеки, то есть и заголовочные файлы рядом
if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/lib/${BUILD_STATE_NAME}")
    set(BUILD_DIR "${CMAKE_CURRENT_LIST_DIR}/SimpleAPI_for_test/build")
    make_directory(${BUILD_DIR})
    make_directory(${CMAKE_CURRENT_LIST_DIR}/lib)
    make_directory(${CMAKE_CURRENT_LIST_DIR}/lib/include)

    # обязательно в два раздельных вызова, иначе не работает
    execute_process(
        WORKING_DIRECTORY ${BUILD_DIR}
        COMMAND "${CMAKE_COMMAND}"
                -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                -DSIMPLE_API_MAIN_DIR=${SIMPLE_API_MAIN_DIR}
                -DSIMPLE_API_OUTPUT_LIB_DIR=${CMAKE_CURRENT_LIST_DIR}/lib
                -DSIMPLE_API_OUTPUT_INCLUDE_DIR=${CMAKE_CURRENT_LIST_DIR}/lib/include
                ${CMAKE_CURRENT_LIST_DIR}/SimpleAPI_for_test
    )
    execute_process(
        WORKING_DIRECTORY ${BUILD_DIR}
        COMMAND "${CMAKE_COMMAND}" --build ${BUILD_DIR}
    )
endif()

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_sources})

target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/lib/${BUILD_STATE_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/lib/include/)
