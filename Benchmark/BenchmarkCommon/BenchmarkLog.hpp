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

class COMMON_API BenchmarkLogWriterCore
{
public:
	inline void WriteLog(const SpaceGameEngine::Char8* pstr, SpaceGameEngine::SizeType size)
	{
		++m_LogCount;
	}

private:
	SpaceGameEngine::SizeType m_LogCount = 0;
};

void BM_LogWrite(benchmark::State& state)
{
	SpaceGameEngine::LogWriter<BenchmarkLogWriterCore> log_writer;
	SpaceGameEngine::Logger<BenchmarkLogWriterCore> logger(log_writer, SpaceGameEngine::LogLevel::All);
	for (auto _ : state)
	{
		logger.WriteLog(SpaceGameEngine::GetLocalDate(), SGE_DEBUG_INFORMATION, SpaceGameEngine::LogLevel::Debug, SGE_U8STR("This is a benchmark log message."));
	}
}