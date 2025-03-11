cmake_minimum_required(VERSION 3.5)
include(${SIMPLE_API_MAIN_DIR}/utils.cmake)

if(ENABLE_TEST_LIBRARY_AS_ADD_SUBDIRECTORY)
    add_subdirectory(lib_add_subdirectory)
endif(ENABLE_TEST_LIBRARY_AS_ADD_SUBDIRECTORY)
unset(ENABLE_TEST_LIBRARY_AS_ADD_SUBDIRECTORY CACHE)

if(ENABLE_TEST_LIBRARY_AS_FIND_PACKAGE)
    add_subdirectory(lib_find_package)
endif(ENABLE_TEST_LIBRARY_AS_FIND_PACKAGE)
unset(ENABLE_TEST_LIBRARY_AS_FIND_PACKAGE CACHE)

#TODO: add_subdirectory(lib_find_module)
#NOTE: add_subdirectory(lib_GitSubmodule) #CMAKE::FETCH_CONTENT(since 3.11)

if(ENABLE_TEST_LIBRARY_AS_SO)
    add_subdirectory(lib_so)
endif(ENABLE_TEST_LIBRARY_AS_SO)
unset(ENABLE_TEST_LIBRARY_AS_SO CACHE)
