cmake_minimum_required(VERSION 3.5)

message("----Start package SimplaAPI includes----")

if(${CMAKE_ARGC} LESS 5)
    message(FATAL "not found sources_library_dir or output_library_dir (CMAKE_ARGC(${CMAKE_ARGC}))")
endif()

if(UNIX)
    string(REPLACE "//" "/" CMAKE_ARGV3 ${CMAKE_ARGV3})
    string(REPLACE "//" "/" CMAKE_ARGV4 ${CMAKE_ARGV4})
endif()
# CMAKE_ARGV3 - sources_library_dir
message("sources: \"${CMAKE_ARGV3}\"")
# CMAKE_ARGV4 - output_library_dir
message("output: \"${CMAKE_ARGV4}\"")


make_directory(${CMAKE_ARGV4})

file(GLOB_RECURSE headers_list "${CMAKE_ARGV3}/**/*.h" )
foreach(header ${headers_list})
    string(APPEND all_headers " '${header}'")

    message("preview: ${header}")
    string(REPLACE ${CMAKE_ARGV3}/ "" header ${header})
    message("current: ${header}")

    string(REGEX REPLACE "/.*\.h" "" header_dir ${header})
    if(NOT ${header_dir} MATCHES "\.h")
        message("dir: [${header_dir}]")
        make_directory("${CMAKE_ARGV4}/${header_dir}")
    endif()

#    message("\"${CMAKE_ARGV3}/${header}\" -> \"${CMAKE_ARGV4}/${header}\"")

    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_ARGV3}/${header}" "${CMAKE_ARGV4}/${header_dir}"
    )
endforeach()
message("all_headers: ${all_headers}")

message("----------------- done -----------------")
