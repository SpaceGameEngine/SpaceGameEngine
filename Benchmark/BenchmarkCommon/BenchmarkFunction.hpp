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

void BM_StdFunctionCreate(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::function<void()> func = []() {};
	}
}

BENCHMARK(BM_StdFunctionCreate);

void BM_FunctionCreate(benchmark::State& state)
{
	for (auto _ : state)
	{
		SpaceGameEngine::Function<void()> func = []() {};
	}
}

BENCHMARK(BM_FunctionCreate);

void BM_StdFunction1(benchmark::State& state)
{
	auto lambda = [](int a, int b) -> int { return a + b; };
	for (auto _ : state)
	{
		std::function<int(int, int)> func = lambda;
		func(1, 2);
	}
}

BENCHMARK(BM_StdFunction1);

void BM_Function1(benchmark::State& state)
{
	auto lambda = [](int a, int b) -> int { return a + b; };
	for (auto _ : state)
	{
		SpaceGameEngine::Function<int(int, int)> func = lambda;
		func(1, 2);
	}
}

BENCHMARK(BM_Function1);

struct bm_func_class
{
	inline void add(int a, int b)
	{
		result = a + b;
	}
	int result;
};

void BM_StdFunction2(benchmark::State& state)
{
	bm_func_class t;
	for (auto _ : state)
	{
		std::function<void(bm_func_class*, int, int)> func = &bm_func_class::add;
		func(&t, 1, 2);
	}
}

BENCHMARK(BM_StdFunction2);

void BM_Function2(benchmark::State& state)
{
	bm_func_class t;
	for (auto _ : state)
	{
		SpaceGameEngine::Function<void(bm_func_class*, int, int)> func = &bm_func_class::add;
		func(&t, 1, 2);
	}
}

BENCHMARK(BM_Function2);