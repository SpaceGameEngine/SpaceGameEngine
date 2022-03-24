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

	inline static constexpr const SizeType LogWriterBufferSize = 4194304;

#if defined(SGE_WINDOWS) && defined(SGE_MSVC) && defined(SGE_USE_DLL)
	template class COMMON_API FixedSizeBuffer<LogWriterBufferSize>;
#endif

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
			: m_CurrentIndex(0), m_WriteIndex(0)
		{
			m_IsRunning.Store(true, MemoryOrder::Release);
			m_Thread = Thread(std::bind(&LogWriter::Run, this));
		}

		inline ~LogWriter()
		{
			m_IsRunning.Store(false, MemoryOrder::Release);
			m_Thread.Join();
		}

		inline void WriteLog(const Char8* pstr, SizeType size)
		{
			SGE_ASSERT(NullPointerError, pstr);
			SGE_ASSERT(InvalidValueError, size, 1, LogWriterBufferSize);
			RecursiveLock locker(m_Mutex);
			locker.Lock();
			if (m_Buffers[m_CurrentIndex].GetFreeSize() >= size)
				m_Buffers[m_CurrentIndex].Append(pstr, size);
			else
			{
				if (((m_CurrentIndex + 1) % sm_BufferArraySize) == m_WriteIndex)
				{
					m_Buffers[m_CurrentIndex].Clear();
					m_Buffers[m_CurrentIndex].Append(SGE_U8STR("Log overflow"), 12);
				}
				else
				{
					m_CurrentIndex = (m_CurrentIndex + 1) % sm_BufferArraySize;
					m_Buffers[m_CurrentIndex].Append(pstr, size);
					m_Condition.NodifyAll();
				}
			}
		}

		inline LogWriterCore& GetLogWriterCore()
		{
			return m_LogWriterCore;
		}

		inline const LogWriterCore& GetLogWriterCore() const
		{
			return m_LogWriterCore;
		}

	private:
		inline void Run()
		{
			while (m_IsRunning.Load(MemoryOrder::Acquire))
			{
				RecursiveLock locker(m_Mutex);
				locker.Lock();
				if (m_CurrentIndex == m_WriteIndex)
					m_Condition.WaitFor(locker, MakeTimeDuration<Second, TimeType>(4));
				if (m_CurrentIndex == m_WriteIndex && m_Buffers[m_CurrentIndex].GetSize() > 0)
					m_CurrentIndex = (m_CurrentIndex + 1) % sm_BufferArraySize;
				locker.Unlock();

				for (; m_WriteIndex != m_CurrentIndex; m_WriteIndex = (m_WriteIndex + 1) % sm_BufferArraySize)
				{
					m_LogWriterCore.WriteLog((const Char8*)(m_Buffers[m_WriteIndex].GetData()), m_Buffers[m_WriteIndex].GetSize());
					m_Buffers[m_WriteIndex].Clear();
				}
			}
			if (m_CurrentIndex == m_WriteIndex && m_Buffers[m_CurrentIndex].GetSize() > 0)
				m_CurrentIndex = (m_CurrentIndex + 1) % sm_BufferArraySize;
			for (; m_WriteIndex != m_CurrentIndex; m_WriteIndex = (m_WriteIndex + 1) % sm_BufferArraySize)
				m_LogWriterCore.WriteLog((const Char8*)(m_Buffers[m_WriteIndex].GetData()), m_Buffers[m_WriteIndex].GetSize());
		}

	private:
		inline static constexpr const SizeType sm_BufferArraySize = 4;

		LogWriterCore m_LogWriterCore;
		Atomic<bool> m_IsRunning;
		Thread m_Thread;
		FixedSizeBuffer<LogWriterBufferSize> m_Buffers[sm_BufferArraySize];
		SizeType m_CurrentIndex;
		SizeType m_WriteIndex;
		Mutex m_Mutex;
		Condition m_Condition;
	};

	/*!
	@}
	*/
}