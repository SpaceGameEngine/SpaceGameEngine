/*
Copyright 2022 creatorlxd

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
#include <unordered_map>
#include <cassert>
#include "Container/HashMap.hpp"

void BM_StdUnorderedMapCreate(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::unordered_map<int, double> m;
	}
}

void BM_SgeHashMapCreate(benchmark::State& state)
{
	for (auto _ : state)
	{
		SpaceGameEngine::HashMap<int, double> m;
	}
}

BENCHMARK(BM_StdUnorderedMapCreate)->Iterations(1000000);
BENCHMARK(BM_SgeHashMapCreate)->Iterations(1000000);

inline int bm_hashmap_seq[] = {63, 82, 17, 99, 3, 26, 31, 59, 74, 23, 6, 94, 38, 36, 23, 19, 5, 64, 52, 17, 53, 21, 78, 2, 95};
inline constexpr const int bm_hashmap_seq_size = sizeof(bm_hashmap_seq) / sizeof(int);

void BM_StdUnorderedMapInsert(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::unordered_map<int, double> m;
		for (int i = 0; i < bm_hashmap_seq_size; i++)
		{
			m.insert(std::make_pair(bm_hashmap_seq[i], (double)bm_hashmap_seq[i]));
		}
	}
}

void BM_SgeHashMapInsert(benchmark::State& state)
{
	for (auto _ : state)
	{
		SpaceGameEngine::HashMap<int, double> m;
		for (int i = 0; i < bm_hashmap_seq_size; i++)
		{
			m.Insert(bm_hashmap_seq[i], (double)bm_hashmap_seq[i]);
		}
	}
}

BENCHMARK(BM_StdUnorderedMapInsert)->Iterations(1000000);
BENCHMARK(BM_SgeHashMapInsert)->Iterations(1000000);

void BM_StdUnorderedMapFind(benchmark::State& state)
{
	std::unordered_map<int, double> m;
	for (int i = 0; i < bm_hashmap_seq_size; i++)
	{
		m.insert(std::make_pair(bm_hashmap_seq[i], (double)bm_hashmap_seq[i]));
	}
	for (auto _ : state)
	{
		for (int i = 0; i < bm_hashmap_seq_size; i++)
		{
			auto re = m.find(bm_hashmap_seq[i]);
			assert(re->second == (double)bm_hashmap_seq[i]);
		}
	}
}

void BM_SgeHashMapFind(benchmark::State& state)
{
	SpaceGameEngine::HashMap<int, double> m;
	for (int i = 0; i < bm_hashmap_seq_size; i++)
	{
		m.Insert(bm_hashmap_seq[i], (double)bm_hashmap_seq[i]);
	}
	for (auto _ : state)
	{
		for (int i = 0; i < bm_hashmap_seq_size; i++)
		{
			auto re = m.Find(bm_hashmap_seq[i]);
			assert(re->m_Second == (double)bm_hashmap_seq[i]);
		}
	}
}

BENCHMARK(BM_StdUnorderedMapFind)->Iterations(1000000);
BENCHMARK(BM_SgeHashMapFind)->Iterations(1000000);

void BM_StdUnorderedMapRemove(benchmark::State& state)
{
	std::unordered_map<int, double> m;
	for (int i = 0; i < bm_hashmap_seq_size; i++)
	{
		m.insert(std::make_pair(bm_hashmap_seq[i], (double)bm_hashmap_seq[i]));
	}
	for (auto _ : state)
	{
		for (int i = 0; i < bm_hashmap_seq_size; i++)
		{
			m.erase(bm_hashmap_seq[i]);
		}
	}
}

void BM_SgeHashMapRemove(benchmark::State& state)
{
	SpaceGameEngine::HashMap<int, double> m;
	for (int i = 0; i < bm_hashmap_seq_size; i++)
	{
		m.Insert(bm_hashmap_seq[i], (double)bm_hashmap_seq[i]);
	}
	for (auto _ : state)
	{
		for (int i = 0; i < bm_hashmap_seq_size; i++)
		{
			m.RemoveByKey(bm_hashmap_seq[i]);
		}
	}
}

BENCHMARK(BM_StdUnorderedMapRemove)->Iterations(1000000);
BENCHMARK(BM_SgeHashMapRemove)->Iterations(1000000);