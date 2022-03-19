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
#include "Log.h"
#include <iostream>
#include <cstdint>
#include <functional>

using namespace SpaceGameEngine;

void SpaceGameEngine::WriteLogToConsole(const Char8* ptr, SizeType size)
{
	SGE_ASSERT(NullPointerError, ptr);
	SGE_ASSERT(InvalidValueError, size, 1, SGE_MAX_MEMORY_SIZE);

	std::cout.write((const char*)ptr, size);
}

SpaceGameEngine::LogWriter::LogWriter(void (*write_func)(const Char8*, SizeType))
	: m_WriteFunction(write_func)
{
	SGE_ASSERT(NullPointerError, write_func);
	m_IsRunning.Store(true, MemoryOrder::Release);
	m_Thread = Thread(std::bind(&LogWriter::Run, this));
}

SpaceGameEngine::LogWriter::~LogWriter()
{
	m_IsRunning.Store(false, MemoryOrder::Release);
	m_Thread.Join();
}

void SpaceGameEngine::LogWriter::Run()
{
	while (m_IsRunning.Load(MemoryOrder::Acquire))
	{
	}
}
