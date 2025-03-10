# SimpleAPI library

## Getting started

```bash
git clone <actual URL>
cd simpleapi_library
mkdir -p <build_dir> && cd $_
cmake ../
make -j$(nproc)
```

## Description
TODO

## Usage
You can use the library in your project as follows:
1. Build them and move library and include files to target directory;
2. Use CMake add_subdirectory(), ex.:
    ```CMake
    add_subdirectory(<PATH_TO_SIMPLE_API_DIR> <PATH_TO_CMAKE_CACHE>)
    target_link_libraries(<TARGET_NAME> PUBLIC SimpleAPI)
    target_include_directories(<TARGET_NAME> PUBLIC <PATH_TO_SIMPLE_API_DIR>)
    ```
3. Use CMake find_package(), ex.:
    ```CMake
    set(SIMPLE_API_MAIN_DIR <PATH_TO_SIMPLE_API_DIR>)
    list(APPEND CMAKE_MODULE_PATH "${SIMPLE_API_MAIN_DIR}/cmake_modules")
    find_package(SimpleAPI REQUIRED)
    
    if(SimpleAPI_FOUND)
        target_link_libraries(<TARGET_NAME> PUBLIC ${SimpleAPI_LIBRARIES})
        target_include_directories(<TARGET_NAME> PUBLIC ${SimpleAPI_INCLUDE_DIRS})
    endif(SimpleAPI_FOUND)
    ```