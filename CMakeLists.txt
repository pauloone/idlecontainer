cmake_minimum_required(VERSION 3.18)

project(IDLEContainer)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")


add_executable(IDLEContainer src/cli.cpp)

install(TARGETS IDLEContainer)
