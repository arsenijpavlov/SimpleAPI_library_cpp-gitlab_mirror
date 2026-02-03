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

function(find_SimpleAPId)
    find_path(SimpleAPId_INCLUDE_DIRS
        NAMES SimpleAPI.h
        PATHS ${CMAKE_CURRENT_LIST_DIR}/lib/include

        NO_DEFAULT_PATH
        NO_CMAKE_PATH
        NO_CMAKE_ENVIRONMENT_PATH
    )

    # shared
    find_library(SimpleAPId_LIBRARIES
        NAMES SimpleAPId
        PATHS ${CMAKE_CURRENT_LIST_DIR}/lib

        NO_DEFAULT_PATH
        NO_CMAKE_ENVIRONMENT_PATH
        NO_CMAKE_PATH
    )
    # static
    find_library(SimpleAPId_STATIC_LIBRARIES
        NAMES SimpleAPId_static
        PATHS ${CMAKE_CURRENT_LIST_DIR}/lib

        NO_DEFAULT_PATH
        NO_CMAKE_ENVIRONMENT_PATH
        NO_CMAKE_PATH
    )

    message(STATUS ${SimpleAPId_LIBRARIES})
    message(STATUS ${SimpleAPId_INCLUDE_DIRS})
endfunction()

unset(SimpleAPId_FOUND)
find_package(PkgConfig REQUIRED)

if(NOT SimpleAPId_FOUND)
    find_SimpleAPId() # первичный поиск (библиотека уже собрана)

    if(NOT SimpleAPId_FOUND) # если библиотека не собрана - собрать
        set(BUILD_DIR "${CMAKE_CURRENT_LIST_DIR}/build")
        set(BUILD_DIR_STATIC "${CMAKE_CURRENT_LIST_DIR}/build_static")
        make_directory(${BUILD_DIR})
        make_directory(${CMAKE_CURRENT_LIST_DIR}/lib)
        make_directory(${CMAKE_CURRENT_LIST_DIR}/lib/include)

        execute_process(
            COMMAND nproc
            OUTPUT_VARIABLE N_CORES
            OUTPUT_STRIP_TRAILING_WHITESPACE # удалит лишний перенос строки
        )
        #message("N_CORES: \"${N_CORES}\"")

        # (динамика) обязательно в два раздельных вызова, иначе не работает --------------------------
        execute_process(
            WORKING_DIRECTORY ${BUILD_DIR}
            COMMAND "${CMAKE_COMMAND}"
                    -DCMAKE_BUILD_TYPE=Debug
                    ${CMAKE_CURRENT_LIST_DIR}
        )
        execute_process(
            WORKING_DIRECTORY ${BUILD_DIR}
            COMMAND "${CMAKE_COMMAND}"
                    --build ${BUILD_DIR}
                    -- "-j${N_CORES}" # -j подаётся не в CMake, а уже непосредственно утилите сборки
        )
        # --------------------------------------------------------------------------------------------

        # (статика) обязательно в два раздельных вызова, иначе не работает ---------------------------
        execute_process(
            WORKING_DIRECTORY ${BUILD_DIR_STATIC}
            COMMAND "${CMAKE_COMMAND}"
                    -DCMAKE_BUILD_TYPE=Debug
                    -DSIMPLE_API_STATIC_BUILD=on
                    ${CMAKE_CURRENT_LIST_DIR}
        )
        execute_process(
            WORKING_DIRECTORY ${BUILD_DIR_STATIC}
            COMMAND "${CMAKE_COMMAND}"
                    --build ${BUILD_DIR_STATIC}
                    -- "-j${N_CORES}" # -j подаётся не в CMake, а уже непосредственно утилите сборки
        )
        if(EXISTS ${BUILD_DIR_STATIC}/SimpleAPI/libSimpleAPId_static.a)
            file(COPY ${BUILD_DIR_STATIC}/SimpleAPI/libSimpleAPId_static.a
                DESTINATION ${CMAKE_CURRENT_LIST_DIR}/lib
            )
        endif()
        # --------------------------------------------------------------------------------------------

        find_SimpleAPId() # вторичный поиск, если не было найдено до этого

    endif(NOT SimpleAPId_FOUND)
endif(NOT SimpleAPId_FOUND)

set(SHARED_LIB_NAME SimpleAPId)
if(SimpleAPId_LIBRARIES AND NOT TARGET ${SHARED_LIB_NAME})
    add_library(${SHARED_LIB_NAME} SHARED IMPORTED)
    set_target_properties(${SHARED_LIB_NAME} PROPERTIES
        IMPORTED_LOCATION "${SimpleAPId_LIBRARIES}"
        INTERFACE_INCLUDE_DIRECTORIES "${SimpleAPId_INCLUDE_DIRS}"
    )
endif()

set(STATIC_LIB_NAME SimpleAPId_static)
if(SimpleAPId_STATIC_LIBRARIES AND NOT TARGET ${STATIC_LIB_NAME})
    add_library(${STATIC_LIB_NAME} STATIC IMPORTED)
    set_target_properties(${STATIC_LIB_NAME} PROPERTIES
        IMPORTED_LOCATION "${SimpleAPId_STATIC_LIBRARIES}"
        INTERFACE_INCLUDE_DIRECTORIES "${SimpleAPId_INCLUDE_DIRS}"
    )
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    SimpleAPId
    DEFAULT_MSG
    SimpleAPId_LIBRARIES SimpleAPId_INCLUDE_DIRS
)
if(SimpleAPId_FOUND)
    mark_as_advanced(SimpleAPId_LIBRARIES SimpleAPId_INCLUDE_DIRS)
endif()
