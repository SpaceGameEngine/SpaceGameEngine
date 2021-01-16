/*
Copyright 2019 xsun2001

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#pragma once
#include <vector>
#include "benchmark/benchmark.h"
#include "Memory/Allocators.h"

template<typename AllocatorType>
void BM_AllocationThroughput(benchmark::State& state)
{
	auto size = state.range(0);
	for (auto _ : state)
	{
		void* ptr = SpaceGameEngine::AllocatorWrapper<AllocatorType>::RawNew(size, 4);
	}
	state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * size);
}

BENCHMARK_TEMPLATE(BM_AllocationThroughput, SpaceGameEngine::NativeAllocator)->ThreadRange(1, 16)->RangeMultiplier(2)->Range(1 << 2, 1 << 10);
BENCHMARK_TEMPLATE(BM_AllocationThroughput, SpaceGameEngine::SegregatedFitAllocator)->ThreadRange(1, 16)->RangeMultiplier(2)->Range(1 << 2, 1 << 10);
BENCHMARK_TEMPLATE(BM_AllocationThroughput, SpaceGameEngine::NewSegregatedFitAllocator)->ThreadRange(1, 16)->RangeMultiplier(2)->Range(1 << 3, 1 << 6);
