cmake_minimum_required(VERSION 3.0)

project(Unformat)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Functional test
add_executable(unformat_test unformat.h unformat_test.cpp)

# Benchmark
if (CMAKE_VERSION VERSION_LESS 3.2)
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
else()
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
endif()

include(DownloadProject/DownloadProject.cmake)
download_project(PROJ                benchmark
                 GIT_REPOSITORY      https://github.com/google/benchmark.git
                 GIT_TAG             v1.3.0
                 ${UPDATE_DISCONNECTED_IF_AVAILABLE}
)

add_subdirectory(${benchmark_SOURCE_DIR} ${benchmark_BINARY_DIR})
add_executable(unformat_benchmark unformat.h unformat_benchmark.cpp)
target_link_libraries(unformat_benchmark benchmark)