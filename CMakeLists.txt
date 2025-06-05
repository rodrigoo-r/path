include(FetchContent)

cmake_minimum_required(VERSION 3.12)
project(path C)

set(CMAKE_C_STANDARD 11)

add_library(path STATIC path.c path.h)

if(NOT FLUENT_LIBC_RELEASE) # Manually add libraries only if not in release mode
    FetchContent_Declare(
            string_builder
            GIT_REPOSITORY https://github.com/rodrigoo-r/string_builder
            GIT_TAG        master
    )

    FetchContent_MakeAvailable(string_builder)

    target_include_directories(path PRIVATE ${CMAKE_BINARY_DIR}/_deps/string_builder-src)
    target_link_libraries(path PRIVATE string_builder)
endif ()