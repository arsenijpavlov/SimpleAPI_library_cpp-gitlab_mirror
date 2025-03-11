cmake_minimum_required(VERSION 3.5)

function(find_SimpleAPI)
    find_path(SimpleAPI_INCLUDE_DIRS
        NAMES SimpleAPI.h
        PATHS ${CMAKE_CURRENT_LIST_DIR}/include

        NO_DEFAULT_PATH
        NO_CMAKE_PATH
        NO_CMAKE_ENVIRONMENT_PATH
    )

    find_library(SimpleAPI_LIBRARIES
        NAMES SimpleAPI
        PATHS ${CMAKE_CURRENT_LIST_DIR}/lib                     #.so/.dll
        PATHS ${CMAKE_CURRENT_LIST_DIR}/build-cache/SimpleAPI   #.a

        NO_DEFAULT_PATH
        NO_CMAKE_ENVIRONMENT_PATH
        NO_CMAKE_PATH
    )

    message(STATUS ${SimpleAPI_LIBRARIES})
    message(STATUS ${SimpleAPI_INCLUDE_DIRS})
endfunction()

unset(SimpleAPI_FOUND)
find_package(PkgConfig REQUIRED)

if(NOT SimpleAPI_FOUND)
    find_SimpleAPI()

    if(NOT SimpleAPI_FOUND)
        make_directory("${CMAKE_CURRENT_LIST_DIR}/build-cache")
        execute_process(
            WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/build-cache"
            COMMAND "${CMAKE_COMMAND}" ${CMAKE_CURRENT_LIST_DIR}
            OUTPUT_VARIABLE res_var
        )
#        message("[res_var_1]: ${res_var} [/res_var_1]")

        execute_process(
            WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/build-cache"
            COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_CURRENT_LIST_DIR}/build-cache"
            OUTPUT_VARIABLE res_var
        )
#        message("[res_var_2]: ${res_var} [/res_var_2]")

        find_SimpleAPI()

        include(FindPackageHandleStandardArgs)
        find_package_handle_standard_args(SimpleAPI DEFAULT_MSG SimpleAPI_LIBRARIES SimpleAPI_INCLUDE_DIRS)
        mark_as_advanced(SimpleAPI_LIBRARIES SimpleAPI_INCLUDE_DIRS)
    endif(NOT SimpleAPI_FOUND)
endif(NOT SimpleAPI_FOUND)
