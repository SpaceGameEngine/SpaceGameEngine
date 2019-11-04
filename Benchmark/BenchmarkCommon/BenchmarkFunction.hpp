/*
Copyright 2019 creatorlxd

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
#include <functional>
#include <Function.hpp>

// ----------

void BM_StdFunctionCreate(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::function<void()> func = []() {};
	}
}

void BM_SgeFunctionCreate(benchmark::State& state)
{
	for (auto _ : state)
	{
		SpaceGameEngine::Function<void()> func = []() {};
	}
}

BENCHMARK(BM_StdFunctionCreate)->Iterations(1000000);
BENCHMARK(BM_SgeFunctionCreate)->Iterations(1000000);

// ----------

int bm_add(int a, int b)
{
	return a + b;
}

void BM_DirectInvoke1(benchmark::State& state)
{
	for (auto _ : state)
	{
		bm_add(1, 2);
	}
}

auto stdFunction1 = &bm_add;

void BM_StdFunction1(benchmark::State& state)
{
	for (auto _ : state)
	{
		stdFunction1(1, 1);
	}
}

SpaceGameEngine::Function<int(int, int)> sgeFunc1 = &bm_add;

void BM_SgeFunction1(benchmark::State& state)
{
	for (auto _ : state)
	{
		sgeFunc1(1, 2);
	}
}

BENCHMARK(BM_DirectInvoke1)->Iterations(1000000);
BENCHMARK(BM_StdFunction1)->Iterations(1000000);
BENCHMARK(BM_SgeFunction1)->Iterations(1000000);

// ----------

struct bm_func_class
{
	void add(int a, int b)
	{
		result = a + b;
	}
	int result;
};

bm_func_class bm_test_obj;

void BM_DirectInvoke2(benchmark::State& state)
{
	for (auto _ : state)
	{
		bm_test_obj.add(1, 2);
	}
}

std::function<void(bm_func_class*, int, int)> stdFunc2 = &bm_func_class::add;

void BM_StdFunction2(benchmark::State& state)
{
	for (auto _ : state)
	{
		stdFunc2(&bm_test_obj, 1, 2);
	}
}

SpaceGameEngine::Function<void(bm_func_class*, int, int)> sgeFunc2 = &bm_func_class::add;

void BM_SgeFunction2(benchmark::State& state)
{
	bm_func_class t;
	for (auto _ : state)
	{

		sgeFunc2(&bm_test_obj, 1, 2);
	}
}

BENCHMARK(BM_DirectInvoke2)->Iterations(1000000);
BENCHMARK(BM_StdFunction2)->Iterations(1000000);
BENCHMARK(BM_SgeFunction2)->Iterations(1000000);

// ----------