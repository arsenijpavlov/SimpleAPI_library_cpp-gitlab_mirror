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
        PATHS ${CMAKE_CURRENT_LIST_DIR}/include

        NO_DEFAULT_PATH
        NO_CMAKE_PATH
        NO_CMAKE_ENVIRONMENT_PATH
    )

    find_library(SimpleAPI_LIBRARIES
        NAMES SimpleAPI
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
    find_SimpleAPI()

    if(NOT SimpleAPI_FOUND)
        set(BUILD_DIR "${CMAKE_CURRENT_LIST_DIR}/build")
        make_directory(${BUILD_DIR})
        make_directory(${CMAKE_CURRENT_LIST_DIR}/lib)
        make_directory(${CMAKE_CURRENT_LIST_DIR}/include)

        # обязательно в два раздельных вызова, иначе не работает
        execute_process(
            WORKING_DIRECTORY ${BUILD_DIR}
            COMMAND "${CMAKE_COMMAND}" -DCMAKE_BUILD_TYPE=RELEASE -DSIMPLE_API_STATIC_BUILD="on" ${CMAKE_CURRENT_LIST_DIR}
        )
        execute_process(
            WORKING_DIRECTORY ${BUILD_DIR}
            COMMAND "${CMAKE_COMMAND}" --build ${BUILD_DIR}
        )
        if(EXISTS ${BUILD_DIR}/SimpleAPI/libSimpleAPI.a)
            file(COPY ${BUILD_DIR}/SimpleAPI/libSimpleAPI.a
                DESTINATION ${CMAKE_CURRENT_LIST_DIR}/lib
            )
        endif()

        find_SimpleAPI()

        include(FindPackageHandleStandardArgs)
        find_package_handle_standard_args(SimpleAPI DEFAULT_MSG SimpleAPI_LIBRARIES SimpleAPI_INCLUDE_DIRS)
        mark_as_advanced(SimpleAPI_LIBRARIES SimpleAPI_INCLUDE_DIRS)
    endif(NOT SimpleAPI_FOUND)
endif(NOT SimpleAPI_FOUND)
