﻿#ifdef _WIN32
#pragma comment(lib, "Shlwapi.lib")
#endif

#include <benchmark/benchmark.h>
#include "BenchmarkFunction.hpp"
#include "BenchmarkString.hpp"
#include "BenchmarkMap.hpp"
#include "BenchmarkAllocator.hpp"
#include "BenchmarkHashMap.hpp"

BENCHMARK_MAIN();