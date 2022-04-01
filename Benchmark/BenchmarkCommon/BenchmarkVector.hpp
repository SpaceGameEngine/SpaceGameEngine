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
#include "Container/Vector.hpp"
#include <vector>
#include <cassert>

void BM_StdVectorCreate(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::vector<int> v;
	}
}

void BM_SgeVectorCreate(benchmark::State& state)
{
	for (auto _ : state)
	{
		SpaceGameEngine::Vector<int> v;
	}
}

BENCHMARK(BM_StdVectorCreate)->Iterations(1000000);
BENCHMARK(BM_SgeVectorCreate)->Iterations(1000000);

inline const int vector_test_size = 1024;

void BM_StdVectorPushBack(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::vector<int> v;
		for (int i = 0; i < vector_test_size; ++i)
		{
			v.push_back(i);
		}
	}
}

void BM_SgeVectorPushBack(benchmark::State& state)
{
	for (auto _ : state)
	{
		SpaceGameEngine::Vector<int> v;
		for (int i = 0; i < vector_test_size; ++i)
		{
			v.PushBack(i);
		}
	}
}

BENCHMARK(BM_StdVectorPushBack)->Iterations(1000000);
BENCHMARK(BM_SgeVectorPushBack)->Iterations(1000000);

void BM_StdVectorInsert(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::vector<int> v{0};
		for (int i = 0; i < vector_test_size; ++i)
		{
			v.insert(v.begin() + 1, i);
		}
	}
}

void BM_SgeVectorInsert(benchmark::State& state)
{
	for (auto _ : state)
	{
		SpaceGameEngine::Vector<int> v{0};
		for (int i = 0; i < vector_test_size; ++i)
		{
			v.Insert(v.GetBegin() + 1, i);
		}
	}
}

BENCHMARK(BM_StdVectorInsert)->Iterations(10000);
BENCHMARK(BM_SgeVectorInsert)->Iterations(10000);

void BM_StdVectorRemove(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::vector<int> v(vector_test_size, 1);
		for (int i = 0; i < vector_test_size - 1; ++i)
		{
			v.erase(v.begin() + 1);
		}
	}
}

void BM_SgeVectorRemove(benchmark::State& state)
{
	for (auto _ : state)
	{
		SpaceGameEngine::Vector<int> v(vector_test_size, 1);
		for (int i = 0; i < vector_test_size - 1; ++i)
		{
			v.Remove(v.GetBegin() + 1);
		}
	}
}

BENCHMARK(BM_StdVectorRemove)->Iterations(10000);
BENCHMARK(BM_SgeVectorRemove)->Iterations(10000);

void BM_StdVectorIteration(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::vector<int> v(vector_test_size, 1);
		int cnt = 0;
		for (auto i = v.begin(); i != v.end(); ++i, ++cnt)
		{
			assert(*i == cnt);
		}
	}
}

void BM_SgeVectorIteration(benchmark::State& state)
{
	for (auto _ : state)
	{
		SpaceGameEngine::Vector<int> v(vector_test_size, 1);
		int cnt = 0;
		for (auto i = v.GetBegin(); i != v.GetEnd(); ++i, ++cnt)
		{
			assert(*i == cnt);
		}
	}
}

BENCHMARK(BM_StdVectorIteration)->Iterations(10000);
BENCHMARK(BM_SgeVectorIteration)->Iterations(10000);