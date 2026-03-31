cmake_minimum_required(VERSION 3.14)
project(NDArrayLab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(ndarray INTERFACE)
target_include_directories(ndarray INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(ndarray INTERFACE cxx_std_17)

find_package(GTest REQUIRED)
if(NOT GTest_FOUND)
    message(FATAL_ERROR "GoogleTest not found. Please install it (e.g., 'brew install googletest' on macOS).")
endif()

add_executable(runTests test/tests.cpp)
target_link_libraries(runTests PRIVATE
    GTest::gtest
    GTest::gtest_main
    Threads::Threads
    ndarray
)
target_include_directories(runTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

include(GoogleTest)
gtest_discover_tests(runTests)

target_compile_options(runTests PRIVATE
    -Wall -Wextra -pedantic
    -O2
)