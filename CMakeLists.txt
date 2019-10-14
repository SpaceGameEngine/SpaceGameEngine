cmake_minimum_required(VERSION 3.0.0)
project(SpaceGameEngine VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_policy(SET CMP0054 OLD)

# Doxygen support
if (ENABLE_DOXYGEN)
    find_package(Doxygen
            REQUIRED dot
            OPTIONAL_COMPONENTS mscgen dia)
    SET(ENABLE_DOXYGEN ${DOXYGEN_FOUND})
endif ()

if (ENABLE_DOXYGEN)
    doxygen_add_docs(
            doxygen
            COMMENT "Generate doxygen reference"
    )
    configure_file(Doxyfile Doxyfile.doxygen COPYONLY)
endif ()

# Linux Thread Lib
find_package(Threads REQUIRED)

# Third Party
set(BENCHMARK_ENABLE_TESTING OFF)

add_subdirectory(ThirdParty/GoogleTest/source)
add_subdirectory(ThirdParty/GoogleBenchmark/source)
add_subdirectory(ThirdParty/JsonCpp)
add_subdirectory(ThirdParty/TinyXML2)

set(THIRD_PARTY_LIBRARIES JsonCpp TinyXML2)
set(THIRD_PARTY_TEST_LIBRARIES gtest gmock benchmark)

# Game Engine Modules
add_subdirectory(Source/Common)

# Tests
add_subdirectory(Test/TestCommon)

# Benchmarks
add_subdirectory(Benchmark/BenchmarkCommon)