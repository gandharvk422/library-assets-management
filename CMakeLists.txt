cmake_minimum_required(VERSION 3.16)
project(LibraryAssetsManagement VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(library_assets
    src/main.cpp
    src/Asset.cpp
    src/Library.cpp
)

target_include_directories(library_assets PRIVATE src)

if(MSVC)
    target_compile_options(library_assets PRIVATE /W4)
else()
    target_compile_options(library_assets PRIVATE -Wall -Wextra -Wpedantic)
endif()
