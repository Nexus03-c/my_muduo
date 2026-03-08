cmake_minimum_required(VERSION 2.6)
project(my_muduo)

set(CXX_FlAGS 
-g
-std=c11
)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
message(STATUS "PROJECT_BINARY_DIR: ${PROJECT_BINARY_DIR}")
include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(source/base)
add_subdirectory(source/net)