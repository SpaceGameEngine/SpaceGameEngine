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
#include "Memory/SegregatedFitAllocator.h"

// ----------

void BM_AllocFixedSize_malloc(benchmark::State& state)
{
	auto size = state.range(0);
	for (auto _ : state)
	{
		void* ptr = _mm_malloc(size, 4);
	}
	state.SetBytesProcessed(
		static_cast<int64_t>(state.iterations()) * size);
}

void BM_AllocFixedSize_stdnew(benchmark::State& state)
{
	auto size = state.range(0);
	for (auto _ : state)
	{
		void* ptr = new char[size];
	}
	state.SetBytesProcessed(
		static_cast<int64_t>(state.iterations()) * size);
}

void BM_AllocFixedSize_FixedSizeAllocator(benchmark::State& state)
{
	auto size = state.range(0);
	SpaceGameEngine::SegregatedFitAllocator::LockedFixedSizeAllocator allocator(size, 4096, 4);
	for (auto _ : state)
	{
		void* ptr = allocator.Allocate();
	}
	state.SetBytesProcessed(
		static_cast<int64_t>(state.iterations()) * size);
}

template<class AllocatorType>
void BM_AllocFixedSize_SGEAllocator(benchmark::State& state)
{
	auto size = state.range(0);
	AllocatorType allocator;
	for (auto _ : state)
	{
		void* ptr = allocator.RawNew(size, 4);
	}
	state.SetBytesProcessed(
		static_cast<int64_t>(state.iterations()) * size);
}

BENCHMARK(BM_AllocFixedSize_malloc)->RangeMultiplier(2)->Range(1 << 2, 1 << 10);
BENCHMARK(BM_AllocFixedSize_stdnew)->RangeMultiplier(2)->Range(1 << 2, 1 << 10);
BENCHMARK(BM_AllocFixedSize_FixedSizeAllocator)->RangeMultiplier(2)->Range(1 << 2, 1 << 10);
BENCHMARK_TEMPLATE(BM_AllocFixedSize_SGEAllocator, SpaceGameEngine::StdAllocator)->RangeMultiplier(2)->Range(1 << 2, 1 << 10);
BENCHMARK_TEMPLATE(BM_AllocFixedSize_SGEAllocator, SpaceGameEngine::MemoryManagerAllocator)->RangeMultiplier(2)->Range(1 << 2, 1 << 10);

// ----------

// ----------



// ----------