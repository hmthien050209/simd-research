cmake_minimum_required(VERSION 3.20)
project(simd_research VERSION 1.0.0 LANGUAGES CXX)

# Use C++20
set(CMAKE_CXX_STANDARD 20)

# Compile with AVX2
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2")

# Build flags
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall -O2")

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
find_package(fmt CONFIG REQUIRED)
find_package(benchmark CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

# Header files
include_directories(${PROJECT_SOURCE_DIR}/include)

# pcg
find_path(PCG_INCLUDE_DIRS NAMES pcg_random.hpp)

# The main executable
add_executable(main
        src/main.cpp
        src/exam.cpp
        src/exec_timer.cpp
)

target_include_directories(main PRIVATE ${PCG_INCLUDE_DIRS})
