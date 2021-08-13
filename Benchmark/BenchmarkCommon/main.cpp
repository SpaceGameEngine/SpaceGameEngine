#ifdef _WIN32
#pragma comment(lib, "Shlwapi.lib")
#endif

#include <benchmark/benchmark.h>
#include "Bootstrap.h"
#include "BenchmarkFunction.hpp"
#include "BenchmarkVector.hpp"
#include "BenchmarkString.hpp"
#include "BenchmarkMap.hpp"
#include "BenchmarkAllocator.hpp"
#include "BenchmarkHashMap.hpp"
#include "BenchmarkList.hpp"

SpaceGameEngine::Bootstrap bootstrap;

BENCHMARK_MAIN();