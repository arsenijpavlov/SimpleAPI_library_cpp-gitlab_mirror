cmake_minimum_required(VERSION 3.5)
set(SIMPLE_API_MAIN_DIR ${CMAKE_CURRENT_LIST_DIR}/..)
include(${SIMPLE_API_MAIN_DIR}/utils.cmake)

message("CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(COMPLEX_PROJECT_NAME SimpleAPI)
else()
    set(COMPLEX_PROJECT_NAME SimpleAPId)
endif()
#set(OUTPUT_NAME ${COMPLEX_PROJECT_NAME})

message("SIMPLE_API_STATIC_BUILD: \"${SIMPLE_API_STATIC_BUILD}\"")
if(SIMPLE_API_STATIC_BUILD)
    set(COMPLEX_PROJECT_NAME ${COMPLEX_PROJECT_NAME}_static)
endif()

project(${COMPLEX_PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
print_project_name(${PROJECT_NAME})

message("SIMPLE_API_OUTPUT_LIB_DIR:     ${SIMPLE_API_OUTPUT_LIB_DIR}")
message("SIMPLE_API_OUTPUT_INCLUDE_DIR: ${SIMPLE_API_OUTPUT_INCLUDE_DIR}")

file(GLOB_RECURSE ${PROJECT_NAME}_sources "${PROJECT_SOURCE_DIR}/*.cpp" )
file(GLOB_RECURSE ${PROJECT_NAME}_headers "${PROJECT_SOURCE_DIR}/*.h" )

if(NOT SIMPLE_API_OUTPUT_LIB_DIR)
    string(REPLACE "_static" "" PATH_NAME ${PROJECT_NAME})
    message("new path: \"${PATH_NAME}\"")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/../lib/")
else()
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${SIMPLE_API_OUTPUT_LIB_DIR}")
endif()
message("SimpleAPI output directory: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

if(NOT TARGET ${PROJECT_NAME})
    if(SIMPLE_API_STATIC_BUILD)
        add_library(${PROJECT_NAME} STATIC)
    else()
        add_library(${PROJECT_NAME} SHARED)
    endif()

    target_sources(${PROJECT_NAME} PRIVATE
        ${${PROJECT_NAME}_sources}
        ${${PROJECT_NAME}_headers}
    )
    target_link_libraries(${PROJECT_NAME} PUBLIC -pthread)
    message("creating ${Yellow}${Bold}${PROJECT_NAME}${Reset}")

    message("SIMPLE_API_STATIC_BUILD: ${SIMPLE_API_STATIC_BUILD}")
    if(NOT SIMPLE_API_OUTPUT_INCLUDE_DIR)
        set(SIMPLE_API_OUTPUT_INCLUDE_DIR "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/include")
    endif()

    # упаковка готовой библиотеки(shared) в конечную директорию
    # (скрипт запустится ДО связки скомпилированных файлов в бинарник)
    add_custom_command(TARGET ${PROJECT_NAME}
        PRE_LINK
        COMMAND "${CMAKE_COMMAND}" -P "${SIMPLE_API_MAIN_DIR}/copy_includes.cmake"
                    "${CMAKE_CURRENT_LIST_DIR}" "${SIMPLE_API_OUTPUT_INCLUDE_DIR}"
    )

    # на момент работы с кодом файл должен существовать (в коммит не попадает)
    execute_process(COMMAND bash "${SIMPLE_API_MAIN_DIR}/scripts/check_version_from_git.sh")

    # проверка версии по истории коммитов (скрипт надо запустить ДО компиляции файлов)
    set(VERSION_INFO_FILE "${CMAKE_CURRENT_LIST_DIR}/VersionInfo.h")
    add_custom_command(TARGET ${PROJECT_NAME}
        PRE_LINK
        COMMAND "${CMAKE_COMMAND}" -P "${SIMPLE_API_MAIN_DIR}/generate_version_info.cmake"
    )
    # для CMake отмечаем, что файл появится перед компиляцией (убирает ошибку)
    set_source_files_properties("${VERSION_INFO_FILE}" PROPERTIES GENERATED TRUE)
    # добавляем зависимость сборки от файла, генерируемого внешним скриптом
    target_sources(${PROJECT_NAME} PRIVATE "${VERSION_INFO_FILE}")

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
    set(CPACK_DEBIAN_PACKAGE_VERSION    "0.0.2")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Riddick")
    set(CPACK_PACKAGE_VENDOR            "Riddick")
    include(CPack)
endif()

message("SimpleAPI output include directory: ${SIMPLE_API_OUTPUT_INCLUDE_DIR}")
# если собирали статику, сбросить флаг
unset(SIMPLE_API_STATIC_BUILD)
