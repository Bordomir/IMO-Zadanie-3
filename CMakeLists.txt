cmake_minimum_required (VERSION 3.22)

add_compile_options(-Wall -Wextra -Wpedantic -O3)

project ("Modified travelling salesman problem")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SRC_FILES
        "${CMAKE_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_SOURCE_DIR}/include/*.h"
        "${CMAKE_SOURCE_DIR}/include/*.hpp"
)

add_executable(solution ${SRC_FILES})