cmake_minimum_required(VERSION 3.12)
project(gate LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

include_directories(${PROJECT_SOURCE_DIR})

add_compile_options(
        -Wall
        -Wextra
        -Wno-implicit-fallthrough
        -fPIC
)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-march=native -O3)
else ()
    add_compile_options(-O2)
endif()

add_library(gate SHARED src/gate.c src/vector.c src/gate.h src/vector.h)

add_executable(example example.c)
target_link_libraries(example PRIVATE gate)