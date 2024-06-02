/*
Copyright 2024 creatorlxd

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
#include <benchmark/benchmark.h>
#include "MemoryManager.h"

const std::size_t fixed_allocate_size = 4;
const int max_buffer_size = 128;

void BM_StdAllocatorFixedSizeAllocate(benchmark::State& state)
{
	SpaceGameEngine::Byte* buffer[max_buffer_size];
	int size = state.range(0);
	for (auto _ : state)
	{
		for (int i = 0; i < size; i++)
			buffer[i] = new SpaceGameEngine::Byte[fixed_allocate_size];
		for (int i = 0; i < size; i++)
			delete[] buffer[i];
	}
}

void BM_SgeFixedSizeAllocatorFixedSizeAllocate(benchmark::State& state)
{
	static SpaceGameEngine::MemoryManager::FixedSizeAllocator test(fixed_allocate_size, 0xffff, 4);
	void* buffer[max_buffer_size];
	int size = state.range(0);
	for (auto _ : state)
	{
		for (int i = 0; i < size; i++)
			buffer[i] = test.Allocate();
		for (int i = 0; i < size; i++)
			test.Free(buffer[i]);
	}
}

// void BM_SgeMultiThreadBufferedFixedSizeAllocatorFixedSizeAllocate(benchmark::State& state)
//{
//	static SpaceGameEngine::MemoryManager::MultiThreadBufferedFixedSizeAllocator test(fixed_allocate_size, 0xffff, 4);
//	void* buffer[max_buffer_size];
//	int size = state.range(0);
//	for (auto _ : state)
//	{
//		for (int i = 0; i < size; i++)
//			buffer[i] = test.Allocate();
//		for (int i = 0; i < size; i++)
//			test.Free(buffer[i]);
//	}
// }

BENCHMARK(BM_StdAllocatorFixedSizeAllocate)->ThreadRange(1, 8)->RangeMultiplier(2)->Range(1, max_buffer_size);
BENCHMARK(BM_SgeFixedSizeAllocatorFixedSizeAllocate)->ThreadRange(1, 8)->RangeMultiplier(2)->Range(1, max_buffer_size);
// BENCHMARK(BM_SgeMultiThreadBufferedFixedSizeAllocatorFixedSizeAllocate)->ThreadRange(1, 8)->RangeMultiplier(2)->Range(1, max_buffer_size);

template<typename Allocator>
void BM_AllocatorAllocate(benchmark::State& state)
{
	auto size = state.range(0);
	void* buffer[max_buffer_size];
	for (auto _ : state)
	{
		for (int i = 0; i < max_buffer_size; ++i)
			buffer[i] = Allocator::RawNew(size, 4);
		for (int i = 0; i < max_buffer_size; ++i)
			Allocator::RawDelete(buffer[i]);
	}
}

BENCHMARK_TEMPLATE(BM_AllocatorAllocate, SpaceGameEngine::StdAllocator)->ThreadRange(1, 8)->RangeMultiplier(2)->Range(1 << 2, 1 << 10);
BENCHMARK_TEMPLATE(BM_AllocatorAllocate, SpaceGameEngine::MemoryManagerAllocator)->ThreadRange(1, 8)->RangeMultiplier(2)->Range(1 << 2, 1 << 10);