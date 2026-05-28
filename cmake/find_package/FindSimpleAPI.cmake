#########################################################################
# Данный файл используется для корректного и лёгкого добавления
# библиотеки в Ваш проект.
#
# Что делает скрипт ниже:
#   1. пробует найти файл динамической библиотеки формата (.so/.dll) или
#       или её статический вариант (.a/.lib)
#   2. пробует найти заголовочные файлы для библиотеки
#       (в данном случае файл SimpleAPI.h уже ссылается на все остальные)
#   3. если пункты [1] и [2] не дали положительного результата, будет
#       произведена сборка библиотеки (файлы будут созданы в этом же
#       каталоге и его производных), а после будет выполнен повторный
#       поиск из пунктов [1] и [2]
#########################################################################

cmake_minimum_required(VERSION 3.5)

#TODO: установка библиотеки в систему

function(find_SimpleAPI)
    find_path(SimpleAPI_INCLUDE_DIRS
        NAMES SimpleAPI.h
        PATHS ${CMAKE_CURRENT_LIST_DIR}/lib/include

        NO_DEFAULT_PATH
        NO_CMAKE_PATH
        NO_CMAKE_ENVIRONMENT_PATH
    )

    # shared
    find_library(SimpleAPI_LIBRARIES
        NAMES SimpleAPI
        PATHS ${CMAKE_CURRENT_LIST_DIR}/lib

        NO_DEFAULT_PATH
        NO_CMAKE_ENVIRONMENT_PATH
        NO_CMAKE_PATH
    )
    # static
    find_library(SimpleAPI_STATIC_LIBRARIES
        NAMES SimpleAPI_static
        PATHS ${CMAKE_CURRENT_LIST_DIR}/lib

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
    find_SimpleAPI() # первичный поиск (библиотека уже собрана)

    if(NOT SimpleAPI_FOUND) # если библиотека не собрана - собрать
        message("SimpleAPI not found, need build it")

        set(BUILD_DIR "${CMAKE_CURRENT_LIST_DIR}/build")
        make_directory(${BUILD_DIR})
        make_directory(${CMAKE_CURRENT_LIST_DIR}/lib)
        make_directory(${CMAKE_CURRENT_LIST_DIR}/lib/include)

        execute_process(
            COMMAND nproc
            OUTPUT_VARIABLE N_CORES
            OUTPUT_STRIP_TRAILING_WHITESPACE # удалит лишний перенос строки
        )
        #message("N_CORES: \"${N_CORES}\"")

        # обязательно в два раздельных вызова, иначе не работает --------------------------
        execute_process(
            WORKING_DIRECTORY ${BUILD_DIR}
            COMMAND rm -rf
        )
        execute_process(
            WORKING_DIRECTORY ${BUILD_DIR}
            COMMAND "${CMAKE_COMMAND}"
                    -DCMAKE_BUILD_TYPE=Release
                    -S ${CMAKE_CURRENT_LIST_DIR}
                    -B ${BUILD_DIR}
        )
        execute_process(
            WORKING_DIRECTORY ${BUILD_DIR}
            COMMAND "${CMAKE_COMMAND}"
                    --build ${BUILD_DIR}
                    -- "-j${N_CORES}" # -j подаётся не в CMake, а уже непосредственно утилите сборки
        )
        # --------------------------------------------------------------------------------------------

        if(EXISTS ${BUILD_DIR}/SimpleAPI_static/libSimpleAPI_static.a)
            file(COPY ${BUILD_DIR}/SimpleAPI_static/libSimpleAPI_static.a
                DESTINATION ${CMAKE_CURRENT_LIST_DIR}/lib
            )
        endif()
        # --------------------------------------------------------------------------------------------

        find_SimpleAPI() # вторичный поиск, если не было найдено до этого

    endif(NOT SimpleAPI_FOUND)
endif(NOT SimpleAPI_FOUND)

set(SHARED_LIB_NAME SimpleAPI)
if(SimpleAPI_LIBRARIES AND NOT TARGET ${SHARED_LIB_NAME})
    add_library(${SHARED_LIB_NAME} SHARED IMPORTED)
    set_target_properties(${SHARED_LIB_NAME} PROPERTIES
        IMPORTED_LOCATION "${SimpleAPI_LIBRARIES}"
        INTERFACE_INCLUDE_DIRECTORIES "${SimpleAPI_INCLUDE_DIRS}"
    )
endif()

#set(STATIC_LIB_NAME SimpleAPI_static)
#if(SimpleAPI_STATIC_LIBRARIES AND NOT TARGET ${STATIC_LIB_NAME})
#    add_library(${STATIC_LIB_NAME} STATIC IMPORTED)
#    set_target_properties(${STATIC_LIB_NAME} PROPERTIES
#        IMPORTED_LOCATION "${SimpleAPI_STATIC_LIBRARIES}"
#        INTERFACE_INCLUDE_DIRECTORIES "${SimpleAPI_INCLUDE_DIRS}"
#    )
#endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    SimpleAPI
    DEFAULT_MSG
    SimpleAPI_LIBRARIES SimpleAPI_INCLUDE_DIRS
)
if(SimpleAPI_FOUND)
    mark_as_advanced(SimpleAPI_LIBRARIES SimpleAPI_INCLUDE_DIRS)
endif()
