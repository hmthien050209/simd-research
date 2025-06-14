cmake_minimum_required(VERSION 3.20)
project(simd_research VERSION 1.0.0 LANGUAGES CXX)

# Use C++20
set(CMAKE_CXX_STANDARD 20)

# Compile with AVX512
add_compile_options(-mavx512bw -mavx512vl -mavx512f -mavx512dq -Wall -fno-omit-frame-pointer -fsanitize=address -fno-sanitize-recover=all)

# Build flags
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-ggdb")
set(BENCHMARK_LTO ON)

# We're targeting only GCC for now
if (NOT CMAKE_COMPILER_IS_GNUCXX)
    message(FATAL_ERROR "GCC is required to compile simd-research")
endif ()

# We're using C++20, so we should use gcc >=10.0
if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10.0)
    message(FATAL_ERROR "GCC-10.0 is required to compile simd-research, but found ${CMAKE_CXX_COMPILER_VERSION}")
endif ()

# Unit testing
enable_testing()

# Dependencies
find_package(benchmark CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

# Header files
include_directories(${PROJECT_SOURCE_DIR}/include)

# pcg
find_path(PCG_INCLUDE_DIRS NAMES pcg_random.hpp)

# The test executable
add_executable(
        main_test
        src/test.cpp
        src/exam.cpp
)

target_link_libraries(
        main_test
        PRIVATE
        GTest::gtest_main
)

target_include_directories(main_test PRIVATE ${PCG_INCLUDE_DIRS})

include(GoogleTest)
gtest_discover_tests(main_test)

# The benchmark executable
add_executable(
        main_benchmark
        src/benchmark.cpp
        src/exam.cpp
)

target_link_libraries(
        main_benchmark
        PRIVATE
        benchmark::benchmark
)

target_include_directories(main_benchmark PRIVATE ${PCG_INCLUDE_DIRS})
