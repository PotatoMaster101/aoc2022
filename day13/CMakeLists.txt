cmake_minimum_required(VERSION 3.25)
project(day13)

set(CMAKE_CXX_STANDARD 20)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/input.txt ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
add_executable(day13 main.cpp)
