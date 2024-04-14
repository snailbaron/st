cmake_minimum_required(VERSION 3.24)

project(st)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_COLOR_DIAGNOSTICS TRUE)

add_subdirectory(deps)

if(MSVC)
    add_compile_options(/W4 /WX)
    add_compile_definitions(NOMINMAX)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

add_subdirectory(src)