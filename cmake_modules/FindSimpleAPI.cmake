cmake_minimum_required(VERSION 3.5)
#include(${SIMPLE_API_MAIN_DIR}/utils.cmake)

#TODO: поиск библиотеки в стандартном пути
message("FindSimpleAPI.cmake dir is \"${CMAKE_CURRENT_SOURCE_DIR}\"")

find_package(PkgConfig REQUIRED)

if(NOT SimpleAPI_FOUND)
    find_path(SimpleAPI_INCLUDE_DIRS
        NAMES SimpleAPI.h
        PATHS ${SIMPLE_API_MAIN_DIR}/static_SimpleAPI/include

        NO_DEFAULT_PATH
        NO_CMAKE_PATH
        NO_CMAKE_ENVIRONMENT_PATH
    )

    find_library(SimpleAPI_LIBRARIES
        NAMES SimpleAPI
        PATHS ${SIMPLE_API_MAIN_DIR}/static_SimpleAPI

        NO_DEFAULT_PATH
        NO_CMAKE_ENVIRONMENT_PATH
        NO_CMAKE_PATH
    )

    message(STATUS ${SimpleAPI_LIBRARIES})
    message(STATUS ${SimpleAPI_INCLUDE_DIRS})

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(SimpleAPI DEFAULT_MSG SimpleAPI_LIBRARIES SimpleAPI_INCLUDE_DIRS)
    mark_as_advanced(SimpleAPI_LIBRARIES SimpleAPI_INCLUDE_DIRS)

endif(NOT SimpleAPI_FOUND)
