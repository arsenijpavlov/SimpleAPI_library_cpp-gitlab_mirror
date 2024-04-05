cmake_minimum_required(VERSION 3.5)

project(project_SimpleAPI_main LANGUAGES CXX)

#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_CLIENT "Client for testing enabled" on)
option(ENABLE_SERVER "Server for testing enabled" off)

#library
add_subdirectory(SimpleAPI)

#test client
if(ENABLE_CLIENT)
    add_subdirectory(Client)
endif()

#test server
if(ENABLE_SERVER)
    add_subdirectory(Server)
endif()
