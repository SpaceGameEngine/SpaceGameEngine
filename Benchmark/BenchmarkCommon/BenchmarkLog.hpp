/*
Copyright 2025 creatorlxd

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
#include "Log.h"
#include <cassert>
#include <iostream>
#include <atomic>

class BenchmarkLogWriterCore
{
public:
	inline void WriteLog(const SpaceGameEngine::Char8* pstr, SpaceGameEngine::SizeType size)
	{
		++m_LogCount;
	}

	inline SpaceGameEngine::SizeType GetLogCount() const
	{
		return m_LogCount;
	}

private:
	std::atomic<SpaceGameEngine::SizeType> m_LogCount = 0;
};

inline SpaceGameEngine::Logger<BenchmarkLogWriterCore>& GetBenchmarkLogger()
{
	static SpaceGameEngine::LogWriter<BenchmarkLogWriterCore> g_log_writer;
	static SpaceGameEngine::Logger<BenchmarkLogWriterCore> g_logger(g_log_writer, SpaceGameEngine::LogLevel::All);
	return g_logger;
}

void BM_LogWrite(benchmark::State& state)
{
	auto& logger = GetBenchmarkLogger();
	for (auto _ : state)
	{
		SGE_LOG(logger, SpaceGameEngine::LogLevel::Information, SGE_U8STR("This is a benchmark log message."));
	}
}

BENCHMARK(BM_LogWrite)->ThreadRange(1, 8)->Iterations(1000000);