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

    find_library(SimpleAPId_LIBRARIES
        NAMES SimpleAPId
                SimpleAPId_static
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
    find_SimpleAPId()

    if(NOT SimpleAPId_FOUND)
        set(BUILD_DIR "${CMAKE_CURRENT_LIST_DIR}/build")
        set(BUILD_DIR_STATIC "${CMAKE_CURRENT_LIST_DIR}/build_static")
        make_directory(${BUILD_DIR})
        make_directory(${CMAKE_CURRENT_LIST_DIR}/lib)
        make_directory(${CMAKE_CURRENT_LIST_DIR}/lib/include)

        execute_process(
            COMMAND nproc
            OUTPUT_VARIABLE N_CORES
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        #message("N_CORES: \"${N_CORES}\"")

        # обязательно в два раздельных вызова, иначе не работает
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

        find_SimpleAPId()

        include(FindPackageHandleStandardArgs)
        find_package_handle_standard_args(SimpleAPId DEFAULT_MSG SimpleAPId_LIBRARIES SimpleAPId_INCLUDE_DIRS)
        mark_as_advanced(SimpleAPId_LIBRARIES SimpleAPId_INCLUDE_DIRS)
    endif(NOT SimpleAPId_FOUND)
endif(NOT SimpleAPId_FOUND)
