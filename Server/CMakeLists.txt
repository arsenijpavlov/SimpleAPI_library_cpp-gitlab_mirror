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

project(project_Server LANGUAGES CXX)
message("${Bold}project: ${Green}project_Server${Reset}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/../bin")

file(GLOB server_sources
    "*.cpp"
    "*.h"
)
add_executable(Server)
target_sources(Server PUBLIC ${server_sources})

target_include_directories(Server PUBLIC "${PROJECT_BINARY_DIR}/../libs/include")
target_link_libraries(Server PUBLIC SimpleAPI)
