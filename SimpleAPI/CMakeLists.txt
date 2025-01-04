cmake_minimum_required(VERSION 3.5)
include(${SIMPLE_API_MAIN_DIR}/utils.cmake)

project(SimpleAPI LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
print_project_name(${PROJECT_NAME})

file(GLOB_RECURSE ${PROJECT_NAME}_sources "${PROJECT_SOURCE_DIR}/*.cpp" )
file(GLOB_RECURSE ${PROJECT_NAME}_headers "${PROJECT_SOURCE_DIR}/*.h" )

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/../output_lib/")
message("CMAKE_LIBRARY_OUTPUT_DIRECTORY: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

add_library(${PROJECT_NAME} SHARED)
target_sources(${PROJECT_NAME} PRIVATE
    ${${PROJECT_NAME}_sources}
    ${${PROJECT_NAME}_headers}
)
target_link_libraries(${PROJECT_NAME} PUBLIC -pthread)

make_directory(${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/include)

foreach(header ${${PROJECT_NAME}_headers})
    string(APPEND all_headers " '${header}'")

#    message("preview: ${header}")
    string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR}/ "" header ${header})
#    message("current: ${header}")

    string(REGEX REPLACE "/.*\.h" "" header_dir ${header})
    if(NOT ${header_dir} MATCHES "\.h")
        message("dir: ${header_dir}")
        make_directory("${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/include/${header_dir}")
    endif()

#    execute_process(
#        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${header}" "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/include/${header_dir}"
#        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${header}" "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}include/${header}"
#        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/${header}" "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/include/${header_dir}/${header}"
#    )
endforeach()
message("all_headers: ${Yellow}${all_headers}${Reset}")


#add_custom_command(TARGET ${PROJECT_NAME}
#    PRE_BUILD
#    COMMAND "${CMAKE_COMMAND}" -E copy_if_different ${all_headers} "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/include"
#    COMMAND ${CMAKE_COMMAND} -E copy ${all_headers} "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/include/${header_dir}/${header}"
#)
