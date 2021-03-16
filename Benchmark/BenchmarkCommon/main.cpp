#ifdef _WIN32
#pragma comment(lib, "Shlwapi.lib")
#endif

#include <benchmark/benchmark.h>
#include "BenchmarkFunction.hpp"
#include "BenchmarkString.hpp"

BENCHMARK_MAIN();