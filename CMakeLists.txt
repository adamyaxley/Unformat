cmake_minimum_required(VERSION 3.0)

project(Unformat)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(DownloadProject/DownloadProject.cmake)

# Functional test
download_project(PROJ                googletest
                 GIT_REPOSITORY      https://github.com/google/googletest.git
                 GIT_TAG             release-1.8.0
                 ${UPDATE_DISCONNECTED_IF_AVAILABLE}
)
# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
add_executable(unformat_test unformat.h unformat_test.cpp)
target_link_libraries(unformat_test gtest gmock_main)

# Benchmark
if (CMAKE_VERSION VERSION_LESS 3.2)
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
else()
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
endif()

download_project(PROJ                benchmark
                 GIT_REPOSITORY      https://github.com/google/benchmark.git
                 GIT_TAG             v1.3.0
                 ${UPDATE_DISCONNECTED_IF_AVAILABLE}
)

set(BENCHMARK_ENABLE_TESTING OFF)
add_subdirectory(${benchmark_SOURCE_DIR} ${benchmark_BINARY_DIR})
add_executable(unformat_benchmark unformat.h unformat_benchmark.cpp)
target_link_libraries(unformat_benchmark benchmark)