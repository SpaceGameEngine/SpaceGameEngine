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
#include <string>
#include <SGEString.hpp>

void BM_StdStringCreate1(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::string str;
	}
}

void BM_SgeStringCreate1(benchmark::State& state)
{
	for (auto _ : state)
	{
		SpaceGameEngine::String str;
	}
}

BENCHMARK(BM_StdStringCreate1)->Iterations(1000000);
BENCHMARK(BM_SgeStringCreate1)->Iterations(1000000);

void BM_StdStringCreateSmall(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::string str("sso");
	}
}

void BM_SgeStringCreateSmall(benchmark::State& state)
{
	for (auto _ : state)
	{
		SpaceGameEngine::String str(SGE_STR("sso"));
	}
}

BENCHMARK(BM_StdStringCreateSmall)->Iterations(1000000);
BENCHMARK(BM_SgeStringCreateSmall)->Iterations(1000000);

void BM_StdStringCreateMiddle(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::string str("this is a test这是测试");
	}
}

void BM_SgeStringCreateMiddle(benchmark::State& state)
{
	for (auto _ : state)
	{
		SpaceGameEngine::String str(SGE_STR("this is a test这是测试"));
	}
}

BENCHMARK(BM_StdStringCreateMiddle)->Iterations(1000000);
BENCHMARK(BM_SgeStringCreateMiddle)->Iterations(1000000);

void BM_StdStringCreateLarge(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::string str("测试一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890");
	}
}

void BM_SgeStringCreateLarge(benchmark::State& state)
{
	for (auto _ : state)
	{
		SpaceGameEngine::String str(SGE_STR("测试一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890"));
	}
}

BENCHMARK(BM_StdStringCreateLarge)->Iterations(1000000);
BENCHMARK(BM_SgeStringCreateLarge)->Iterations(1000000);

void BM_StdStringGetData(benchmark::State& state)
{
	for (auto _ : state)
	{
		const std::string str("测试一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890");
		auto ptr = str.data();
	}
}

void BM_SgeStringGetData(benchmark::State& state)
{
	for (auto _ : state)
	{
		const SpaceGameEngine::String str(SGE_STR("测试一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890"));
		auto ptr = str.GetData();
	}
}

BENCHMARK(BM_StdStringGetData)->Iterations(1000000);
BENCHMARK(BM_SgeStringGetData)->Iterations(1000000);

void BM_StdStringRead(benchmark::State& state)
{
	for (auto _ : state)
	{
		const std::string str("测试一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890");
		size_t re = 0;
		for (auto i = 0; i < str.size(); i++)
		{
			re += (size_t)str[i];
		}
	}
}

void BM_SgeStringRead(benchmark::State& state)
{
	for (auto _ : state)
	{
		const SpaceGameEngine::String str(SGE_STR("测试一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890"));
		SpaceGameEngine::SizeType re = 0;
		for (auto i = 0; i < str.GetSize(); i++)
		{
			re += (SpaceGameEngine::SizeType)str[i];
		}
	}
}

BENCHMARK(BM_StdStringRead)->Iterations(1000000);
BENCHMARK(BM_SgeStringRead)->Iterations(1000000);

void BM_StdStringAppend(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::string str;
		for (auto i = 0; i < 100; i++)
		{
			str += "t";
		}
	}
}

void BM_SgeStringAppend(benchmark::State& state)
{
	for (auto _ : state)
	{
		SpaceGameEngine::String str;
		for (auto i = 0; i < 100; i++)
		{
			str += SGE_STR("t");
		}
	}
}

BENCHMARK(BM_StdStringAppend)->Iterations(1000000);
BENCHMARK(BM_SgeStringAppend)->Iterations(1000000);

void BM_StdStringInsert(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::string str;
		for (auto i = 0; i < 100; i++)
		{
			str.insert(str.cbegin(), 't');
		}
	}
}

void BM_SgeStringInsert(benchmark::State& state)
{
	for (auto _ : state)
	{
		SpaceGameEngine::String str;
		for (auto i = 0; i < 100; i++)
		{
			str.Insert(str.GetConstBegin(), SGE_STR("t"));
		}
	}
}

BENCHMARK(BM_StdStringInsert)->Iterations(1000000);
BENCHMARK(BM_SgeStringInsert)->Iterations(1000000);

void BM_StdStringFind(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::string str("abcdabcdabccdacdasdfabdbacbasdacasdfsacsdcdddcasddscsabbbdscavbasdfasdfasdfasdfscsdfasseerrrwqezxcaawaabcarabcaba");
		std::string pat("abcda");
		auto re = str.find(pat);
	}
}

void BM_SgeStringFind(benchmark::State& state)
{
	for (auto _ : state)
	{
		SpaceGameEngine::UTF8String str(SGE_U8STR("abcdabcdabccdacdasdfabdbacbasdacasdfsacsdcdddcasddscsabbbdscavbasdfasdfasdfasdfscsdfasseerrrwqezxcaawaabcarabcaba"));
		SpaceGameEngine::UTF8String pat(SGE_U8STR("abcda"));
		auto re = str.Find(pat, str.GetConstBegin(), str.GetConstEnd());
	}
}

BENCHMARK(BM_StdStringFind)->Iterations(1000);
BENCHMARK(BM_SgeStringFind)->Iterations(1000);

void BM_StdStringFind2(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::wstring str(L"abcdabcdabccdacdasdfabdbacbasdacasdfsacsdcdddcasddscsabbbdscavbasdfasdfasdfasdfscsdfasseerrrwqezxcaawaabcarabcaba");
		std::wstring pat(L"abcda");
		auto re = str.find(pat);
	}
}

void BM_SgeStringFind2(benchmark::State& state)
{
	for (auto _ : state)
	{
		SpaceGameEngine::UCS2String str(SGE_STR("abcdabcdabccdacdasdfabdbacbasdacasdfsacsdcdddcasddscsabbbdscavbasdfasdfasdfasdfscsdfasseerrrwqezxcaawaabcarabcaba"));
		SpaceGameEngine::UCS2String pat(SGE_STR("abcda"));
		auto re = str.Find(pat, str.GetConstBegin(), str.GetConstEnd());
	}
}

BENCHMARK(BM_StdStringFind2)->Iterations(1000);
BENCHMARK(BM_SgeStringFind2)->Iterations(1000);