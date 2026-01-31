cmake_minimum_required(VERSION 3.5)
set(SIMPLE_API_MAIN_DIR ${CMAKE_CURRENT_LIST_DIR}/..)
include(${SIMPLE_API_MAIN_DIR}/utils.cmake)

message("CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    project(SimpleAPI LANGUAGES CXX)
else()
    project(SimpleAPId LANGUAGES CXX)
endif()

set(CMAKE_CXX_STANDARD 11)
print_project_name(${PROJECT_NAME})

message("SIMPLE_API_OUTPUT_LIB_DIR:     ${SIMPLE_API_OUTPUT_LIB_DIR}")
message("SIMPLE_API_OUTPUT_INCLUDE_DIR: ${SIMPLE_API_OUTPUT_INCLUDE_DIR}")

file(GLOB_RECURSE ${PROJECT_NAME}_sources "${PROJECT_SOURCE_DIR}/*.cpp" )
file(GLOB_RECURSE ${PROJECT_NAME}_headers "${PROJECT_SOURCE_DIR}/*.h" )

if(NOT SIMPLE_API_OUTPUT_LIB_DIR)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/../lib${PROJECT_NAME}/")
else()
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${SIMPLE_API_OUTPUT_LIB_DIR}")
endif()
message("SimpleAPI output directory: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")


if(SIMPLE_API_STATIC_BUILD)
    add_library(${PROJECT_NAME} STATIC)
else()
    add_library(${PROJECT_NAME} SHARED)
endif()
unset(SIMPLE_API_STATIC_BUILD)

target_sources(${PROJECT_NAME} PRIVATE
    ${${PROJECT_NAME}_sources}
    ${${PROJECT_NAME}_headers}
)
target_link_libraries(${PROJECT_NAME} PUBLIC -pthread)

if(SIMPLE_API_OUTPUT_INCLUDE_DIR)
    # упаковка готовой библиотеки в конечную директорию
    add_custom_command(TARGET ${PROJECT_NAME}
        PRE_LINK
        COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_LIST_DIR}/../copy_includes.cmake"
                    "${CMAKE_CURRENT_LIST_DIR}" "${SIMPLE_API_OUTPUT_INCLUDE_DIR}"
    )
    message("SimpleAPI output include directory: ${SIMPLE_API_OUTPUT_INCLUDE_DIR}")
else()
    # упаковка готовой библиотеки в конечную директорию
    add_custom_command(TARGET ${PROJECT_NAME}
        PRE_LINK
        COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_LIST_DIR}/../copy_includes.cmake"
                    "${CMAKE_CURRENT_LIST_DIR}" "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/include"
    )
    message("SimpleAPI output include directory: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
endif()

#----------------------------
#TODO: ВРЕМЕННОЕ ХРАНЕНИЕ
target_sources(${PROJECT_NAME} PRIVATE pre_WIKI.md)
#----------------------------

install(TARGETS ${PROJECT_NAME}
    DESTINATION /opt/SimpleAPI
)
install(DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/include"
    DESTINATION /opt/SimpleAPI
)

string(TOLOWER ${PROJECT_NAME} PACKAGE_NAME)
set(CPACK_GENERATOR                 "DEB")
set(CPACK_DEBIAN_FILE_NAME          DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_DEPENDS    "")
set(CPACK_DEBIAN_PACKAGE_NAME       ${PACKAGE_NAME})
set(CPACK_DEBIAN_PACKAGE_VERSION    "0.0.1")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Riddick")
set(CPACK_PACKAGE_VENDOR            "Riddick")
include(CPack)
