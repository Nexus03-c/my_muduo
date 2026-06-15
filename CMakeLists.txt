cmake_minimum_required(VERSION 2.6)
project(my_muduo)

# set(CXX_FlAGS 
# -g
# -std=c11
# )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++17")

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
message(STATUS "PROJECT_BINARY_DIR: ${PROJECT_BINARY_DIR}")
include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(source/base)
add_subdirectory(source/net)
add_subdirectory(source/example)