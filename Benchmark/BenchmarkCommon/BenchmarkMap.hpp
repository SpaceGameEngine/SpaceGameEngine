/*
Copyright 2021 creatorlxd

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
#include <map>
#include <cassert>
#include "Container/Map.hpp"

void BM_StdMapCreate(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::map<int, double> m;
	}
}

void BM_SgeRedBlackTreeCreate(benchmark::State& state)
{
	for (auto _ : state)
	{
		SpaceGameEngine::MapImplement::RedBlackTree<int, double> m;
	}
}

BENCHMARK(BM_StdMapCreate)->Iterations(1000000);
BENCHMARK(BM_SgeRedBlackTreeCreate)->Iterations(1000000);

int bm_map_seq[] = {63, 82, 17, 99, 3, 26, 31, 59, 74, 23, 6, 94, 38, 36, 23, 19, 5, 64, 52, 17, 53, 21, 78, 2, 95};
constexpr const int bm_map_seq_size = sizeof(bm_map_seq) / sizeof(int);

void BM_StdMapInsert(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::map<int, double> m;
		for (int i = 0; i < bm_map_seq_size; i++)
		{
			m.insert(std::make_pair(bm_map_seq[i], (double)bm_map_seq[i]));
		}
	}
}

void BM_SgeRedBlackTreeInsert(benchmark::State& state)
{
	for (auto _ : state)
	{
		SpaceGameEngine::MapImplement::RedBlackTree<int, double> m;
		for (int i = 0; i < bm_map_seq_size; i++)
		{
			m.Insert(bm_map_seq[i], (double)bm_map_seq[i]);
		}
	}
}

BENCHMARK(BM_StdMapInsert)->Iterations(1000000);
BENCHMARK(BM_SgeRedBlackTreeInsert)->Iterations(1000000);

void BM_StdMapFind(benchmark::State& state)
{
	std::map<int, double> m;
	for (int i = 0; i < bm_map_seq_size; i++)
	{
		m.insert(std::make_pair(bm_map_seq[i], (double)bm_map_seq[i]));
	}
	for (auto _ : state)
	{
		for (int i = 0; i < bm_map_seq_size; i++)
		{
			auto re = m.find(bm_map_seq[i]);
			assert(re->second == (double)bm_map_seq[i]);
		}
	}
}

void BM_SgeRedBlackTreeFind(benchmark::State& state)
{
	SpaceGameEngine::MapImplement::RedBlackTree<int, double> m;
	for (int i = 0; i < bm_map_seq_size; i++)
	{
		m.Insert(bm_map_seq[i], (double)bm_map_seq[i]);
	}
	for (auto _ : state)
	{
		for (int i = 0; i < bm_map_seq_size; i++)
		{
			auto re = m.FindValueByKey(bm_map_seq[i]);
			assert(*re == (double)bm_map_seq[i]);
		}
	}
}

BENCHMARK(BM_StdMapFind)->Iterations(1000000);
BENCHMARK(BM_SgeRedBlackTreeFind)->Iterations(1000000);

void BM_StdMapRemove(benchmark::State& state)
{
	std::map<int, double> m;
	for (int i = 0; i < bm_map_seq_size; i++)
	{
		m.insert(std::make_pair(bm_map_seq[i], (double)bm_map_seq[i]));
	}
	for (auto _ : state)
	{
		for (int i = 0; i < bm_map_seq_size; i++)
		{
			m.erase(bm_map_seq[i]);
		}
	}
}

void BM_SgeRedBlackTreeRemove(benchmark::State& state)
{
	SpaceGameEngine::MapImplement::RedBlackTree<int, double> m;
	for (int i = 0; i < bm_map_seq_size; i++)
	{
		m.Insert(bm_map_seq[i], (double)bm_map_seq[i]);
	}
	for (auto _ : state)
	{
		for (int i = 0; i < bm_map_seq_size; i++)
		{
			m.RemoveByKey(bm_map_seq[i]);
		}
	}
}

BENCHMARK(BM_StdMapRemove)->Iterations(1000000);
BENCHMARK(BM_SgeRedBlackTreeRemove)->Iterations(1000000);