cmake_minimum_required(VERSION 3.18)

set (CXX_VERSION -std=c++17)

project(IDLEContainer VERSION "0.0.1" LANGUAGES CXX)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

include_directories(libraries/docopt)


add_executable(IDLEContainer src/cli.cpp libraries/docopt/docopt.cpp)

install(TARGETS IDLEContainer)

set (CPACK_PACKAGE_NAME "IDLEContainer")
set (CPACK_GENERATOR "DEB")
set (CPACK_PACKAGE_VERSION ${VERSION})
set (CPACK_DEBIAN_PACKAGE_ARCHITECTURE "x64")
set (CPACK_DEBIAN_PACKAGE_DESCRIPTION "global cpu throttler for containers")
set (CPACK_PACKAGE_CONTACT "pauloone <bob.smith.ano@gmail.com>")
set (CPACK_OUTPUT_FILE_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/pack")

## Install licence
install (FILES "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"
         DESTINATION "/usr/share/doc/${CPACK_PACKAGE_NAME}"
)

include (CPack)