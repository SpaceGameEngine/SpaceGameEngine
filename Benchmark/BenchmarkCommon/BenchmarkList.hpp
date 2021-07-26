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
#include "Container/List.hpp"
#include <list>
#include <cassert>

void BM_StdListCreate(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::list<int> l;
	}
}

void BM_SgeListCreate(benchmark::State& state)
{
	for (auto _ : state)
	{
		SpaceGameEngine::List<int> l;
	}
}

BENCHMARK(BM_StdListCreate)->Iterations(1000000);
BENCHMARK(BM_SgeListCreate)->Iterations(1000000);

inline const int list_test_size = 1024;

void BM_StdListPushBack(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::list<int> l;
		for (int i = 0; i < list_test_size; ++i)
		{
			l.push_back(i);
		}
	}
}

void BM_SgeListPushBack(benchmark::State& state)
{
	for (auto _ : state)
	{
		SpaceGameEngine::List<int> l;
		for (int i = 0; i < list_test_size; ++i)
		{
			l.PushBack(i);
		}
	}
}

BENCHMARK(BM_StdListPushBack)->Iterations(1000000);
BENCHMARK(BM_SgeListPushBack)->Iterations(1000000);

void BM_StdListInsert(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::list<int> l{0};
		for (int i = 0; i < list_test_size; ++i)
		{
			auto iter = l.begin();
			++iter;
			l.insert(iter, i);
		}
	}
}

void BM_SgeListInsert(benchmark::State& state)
{
	for (auto _ : state)
	{
		SpaceGameEngine::List<int> l{0};
		for (int i = 0; i < list_test_size; ++i)
		{
			auto iter = l.GetBegin();
			++iter;
			l.Insert(iter, i);
		}
	}
}

BENCHMARK(BM_StdListInsert)->Iterations(10000);
BENCHMARK(BM_SgeListInsert)->Iterations(10000);

void BM_StdListRemove(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::list<int> l(list_test_size, 1);
		for (int i = 0; i < list_test_size - 1; ++i)
		{
			auto iter = l.begin();
			++iter;
			l.erase(iter);
		}
	}
}

void BM_SgeListRemove(benchmark::State& state)
{
	for (auto _ : state)
	{
		SpaceGameEngine::List<int> l(list_test_size, 1);
		for (int i = 0; i < list_test_size - 1; ++i)
		{
			auto iter = l.GetBegin();
			++iter;
			l.Remove(iter);
		}
	}
}

BENCHMARK(BM_StdListRemove)->Iterations(10000);
BENCHMARK(BM_SgeListRemove)->Iterations(10000);

void BM_StdListIteration(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::list<int> l(list_test_size, 1);
		int cnt = 0;
		for (auto i = l.begin(); i != l.end(); ++i, ++cnt)
		{
			assert(*i, cnt);
		}
	}
}

void BM_SgeListIteration(benchmark::State& state)
{
	for (auto _ : state)
	{
		SpaceGameEngine::List<int> l(list_test_size, 1);
		int cnt = 0;
		for (auto i = l.GetBegin(); i != l.GetEnd(); ++i, ++cnt)
		{
			assert(*i, cnt);
		}
	}
}

BENCHMARK(BM_StdListIteration)->Iterations(10000);
BENCHMARK(BM_SgeListIteration)->Iterations(10000);