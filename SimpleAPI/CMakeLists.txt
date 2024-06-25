cmake_minimum_required(VERSION 3.5)

if(NOT WIN32)
  string(ASCII 27 Esc)
  set(Reset     "${Esc}[m")
  set(Bold      "${Esc}[1m")
  set(Red       "${Esc}[31m")
  set(Green     "${Esc}[32m")
  set(Yellow    "${Esc}[33m")
  set(Blue      "${Esc}[34m")
  set(Magenta   "${Esc}[35m")
  set(Cyan      "${Esc}[36m")
  set(White     "${Esc}[37m")
endif()

project(project_SimpleAPI_library LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
message("${Bold}project: ${Green}project_SimpleAPI_library${Reset}")

file(GLOB simpleAPI_sources "${PROJECT_SOURCE_DIR}/*.cpp" )
file(GLOB simpleAPI_headers "${PROJECT_SOURCE_DIR}/*.h" )

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/../libs/")
message("CMAKE_LIBRARY_OUTPUT_DIRECTORY: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

add_library(SimpleAPI SHARED)
target_sources(SimpleAPI PRIVATE
    ${simpleAPI_sources}
    ${simpleAPI_headers}
)
target_link_libraries(SimpleAPI PUBLIC -pthread)


execute_process(
    COMMAND bash -c "mkdir -p ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/include && cd $_; \
        cp -vf ${PROJECT_SOURCE_DIR}/*.h ."
    OUTPUT_VARIABLE var
)
message("cp: \n${var}")
