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
#include "CommonAPI.h"
#include "Utility/Utility.hpp"
#include "Concurrent/Atomic.hpp"
#include "Concurrent/Thread.h"
#include "Concurrent/Lock.h"
#include "SGEStringForward.h"
#include "Utility/FixedSizeBuffer.hpp"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	template<typename T>
	concept IsLogWriterCore = requires(T t, const Char8* pstr, SizeType size)
	{
		t.WriteLog(pstr, size);
	};

	class COMMON_API ConsoleLogWriterCore
	{
	public:
		void WriteLog(const Char8* pstr, SizeType size);
	};

	template<IsLogWriterCore LogWriterCore = ConsoleLogWriterCore>
	class LogWriter : public UncopyableAndUnmovable
	{
	public:
		inline LogWriter()
		{
			m_IsRunning.Store(true, MemoryOrder::Release);
			m_Thread = Thread(std::bind(&LogWriter::Run, this));
		}

		inline ~LogWriter()
		{
			m_IsRunning.Store(false, MemoryOrder::Release);
			m_Thread.Join();
		}

	private:
		inline void Run()
		{
			while (m_IsRunning.Load(MemoryOrder::Acquire))
			{
			}
		}

	private:
		LogWriterCore m_LogWriterCore;
		Atomic<bool> m_IsRunning;
		Thread m_Thread;
	};

	/*!
	@}
	*/
}