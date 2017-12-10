cmake_minimum_required(VERSION 3.0)

project(Unformat)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Functional test
add_executable(unformat_test unformat.h unformat_test.cpp)
