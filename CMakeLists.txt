cmake_minimum_required(VERSION 3.18)
project(IDLEContainer VERSION 0.0.1 LANGUAGES CXX)

#============================================================================
# options
#============================================================================
option(WITH_COV "Build tests with coverage" OFF)

#============================================================================
# Internal compiler options
#============================================================================
set (CXX_VERSION -std=c++20)
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -fconcepts")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -fconcepts")



#============================================================================
# Folders
#============================================================================
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

#============================================================================
# Libraries
#============================================================================
include_directories(libraries/docopt)
set (LIBRARIES_SRC libraries/docopt/docopt.cpp)

include (FindCURL)

include_directories(libraries/nlohmann_json/include)

#============================================================================
# Targets
#============================================================================

file (GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(IDLEContainer ${SOURCES} ${LIBRARIES_SRC})

target_link_libraries(
  IDLEContainer
  ${CURL_LIBRARIES}
)

#============================================================================
# Install
#============================================================================

install(TARGETS IDLEContainer)

## Install licence
install (FILES "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"
         DESTINATION "/usr/share/doc/${CPACK_PACKAGE_NAME}"
)

#============================================================================
# CPack
#============================================================================

set (CPACK_PACKAGE_NAME "IDLEContainer")
set (CPACK_GENERATOR "DEB")
set (CPACK_PACKAGE_VERSION ${VERSION})
set (CPACK_DEBIAN_PACKAGE_ARCHITECTURE "x64")
set (CPACK_DEBIAN_PACKAGE_DESCRIPTION "global cpu throttler for containers")
set (CPACK_PACKAGE_CONTACT "pauloone <bob.smith.ano@gmail.com>")
set (CPACK_OUTPUT_FILE_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/pack")


include (CPack)

#============================================================================
# tests
#============================================================================
set(CTEST_BINARY_DIRECTORY ${PROJECT_BINARY_DIR}/tests)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  tests
  tests/main_tests.cpp
)
target_link_libraries(
  tests
  gtest_main
  gcov
)
target_compile_options (tests PUBLIC -O0 -fprofile-arcs -ftest-coverage -DUNITTESTS=True)

include(GoogleTest)
gtest_discover_tests(tests)
