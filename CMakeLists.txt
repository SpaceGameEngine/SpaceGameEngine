cmake_minimum_required(VERSION 3.0.0)
project(SpaceGameEngine VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

## set utf8 runtime environment
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
add_compile_options(/source-charset:utf-8 /source-charset:utf-8)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
add_compile_options(-fexec-charset=UTF-8 -finput-charset=UTF-8)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
add_compile_options(/utf-8)
endif()

# cmake_policy(SET CMP0054 OLD)

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

# Copy Shared Library
function(CopySharedLibrary target)
if(CMAKE_HOST_UNIX)
if(CMAKE_HOST_APPLE)
add_custom_command(TARGET ${target} POST_BUILD
                   COMMAND cp ./lib${target}.dylib ../../)
else()
add_custom_command(TARGET ${target} POST_BUILD
                   COMMAND cp ./lib${target}.so ../../)
endif()
endif()
endfunction(CopySharedLibrary)


# Third Party
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Enable testing of the benchmark library." FORCE)

add_subdirectory(ThirdParty/GoogleTest/source)
add_subdirectory(ThirdParty/GoogleBenchmark/source)
add_subdirectory(ThirdParty/JsonCpp)
add_subdirectory(ThirdParty/TinyXML2)

set(THIRD_PARTY_LIBRARIES JsonCpp TinyXML2)
set(THIRD_PARTY_TEST_LIBRARIES gtest gmock benchmark)

# Game Engine Modules
add_subdirectory(Source/Common)
add_subdirectory(Source/Bootstrap)

# Tests
add_subdirectory(Test/TestCommon)
add_subdirectory(Test/TestModule)

# Benchmarks
add_subdirectory(Benchmark/BenchmarkCommon)