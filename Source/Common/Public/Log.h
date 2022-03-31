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
#include "Utility/DebugInformation.h"
#include "Utility/Format.hpp"
#include "Time/Date.h"

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

	using DefaultLogWriterCore = ConsoleLogWriterCore;

	template<IsLogWriterCore LogWriterCore = DefaultLogWriterCore>
	class LogWriter : public UncopyableAndUnmovable, public LogWriterCore
	{
	public:
		inline LogWriter()
			: LogWriterCore(), m_CurrentIndex(0), m_WriteIndex(0)
		{
			m_IsRunning.Store(true, MemoryOrder::Release);
			m_Thread = Thread(std::bind(&LogWriter::Run, this));
		}

		template<typename... Args>
		inline LogWriter(Args&&... args)
			: LogWriterCore(std::forward<Args>(args)...), m_CurrentIndex(0), m_WriteIndex(0)
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
					LogWriterCore::WriteLog((const Char8*)(m_Buffers[m_WriteIndex].GetData()), m_Buffers[m_WriteIndex].GetSize());
					m_Buffers[m_WriteIndex].Clear();
				}
			}
			if (m_CurrentIndex == m_WriteIndex && m_Buffers[m_CurrentIndex].GetSize() > 0)
				m_CurrentIndex = (m_CurrentIndex + 1) % sm_BufferArraySize;
			for (; m_WriteIndex != m_CurrentIndex; m_WriteIndex = (m_WriteIndex + 1) % sm_BufferArraySize)
				LogWriterCore::WriteLog((const Char8*)(m_Buffers[m_WriteIndex].GetData()), m_Buffers[m_WriteIndex].GetSize());
		}

	private:
		inline static constexpr const SizeType sm_BufferArraySize = 4;

		Atomic<bool> m_IsRunning;
		Thread m_Thread;
		FixedSizeBuffer<LogWriterBufferSize> m_Buffers[sm_BufferArraySize];
		SizeType m_CurrentIndex;
		SizeType m_WriteIndex;
		Mutex m_Mutex;
		Condition m_Condition;
	};

	using LogLevelType = UInt8;
	namespace LogLevel
	{
		inline constexpr const LogLevelType Error = 0;
		inline constexpr const LogLevelType Exception = 1;
		inline constexpr const LogLevelType Warning = 2;
		inline constexpr const LogLevelType Information = 3;
		inline constexpr const LogLevelType Debug = 4;

		inline constexpr const LogLevelType All = 4;
	}

	struct InvalidLogLevelError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("The LogLevel is invalid.");
		static COMMON_API bool Judge(LogLevelType log_level);
	};

	COMMON_API UTF8String GetLogLevelUTF8String(LogLevelType log_level);

	template<typename T>
	concept IsLogFormatter = requires(const Date& date, const DebugInformation& debug_info, LogLevelType log_level, const UTF8String& str)
	{
		{
			T::Format(date, debug_info, log_level, str)
		}
		->std::convertible_to<UTF8String>;
	};

	struct DefaultLogFormatter
	{
		inline static UTF8String Format(const Date& date, const DebugInformation& debug_info, LogLevelType log_level, const UTF8String& str)
		{
			return SpaceGameEngine::Format(UTF8String(SGE_U8STR("{:4}-{:2}-{:2} {:2}:{:2}:{:2} {}:{}:{} {} {}\n")), date.m_Year, date.m_Month, date.m_Day, date.m_Hour, date.m_Minute, date.m_Second, SGE_TSTR_TO_UTF8(debug_info.m_pFileName), SGE_TSTR_TO_UTF8(debug_info.m_pFunctionName), debug_info.m_LineNumber, GetLogLevelUTF8String(log_level), str);
		}
	};

	template<IsLogWriterCore LogWriterCore = DefaultLogWriterCore, IsLogFormatter LogFormatter = DefaultLogFormatter>
	class Logger : public UncopyableAndUnmovable
	{
	public:
		inline Logger(LogWriter<LogWriterCore>& log_writer, LogLevelType log_level = LogLevel::All)
			: m_LogWriter(log_writer), m_LogLevel(log_level)
		{
		}

		inline void WriteLog(const Date& date, const DebugInformation& debug_info, LogLevelType log_level, const UTF8String& str)
		{
			if (log_level > m_LogLevel)
				return;
			else
			{
				UTF8String result = LogFormatter::Format(date, debug_info, log_level, str);
				m_LogWriter.WriteLog(result.GetData(), result.GetNormalSize());
			}
		}

		template<typename... Args>
		inline void WriteLog(const Date& date, const DebugInformation& debug_info, LogLevelType log_level, const UTF8String& str, Args&&... args)
		{
			if (log_level > m_LogLevel)
				return;
			else
			{
				UTF8String result = LogFormatter::Format(date, debug_info, log_level, Format(str, std::forward<Args>(args)...));
				m_LogWriter.WriteLog(result.GetData(), result.GetNormalSize());
			}
		}

	private:
		LogWriter<LogWriterCore>& m_LogWriter;
		LogLevelType m_LogLevel;
	};

	/*!
	@}
	*/
}