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
#include "CommonAPI.h"
#include "Utility/Utility.hpp"
#include "Concurrent/Atomic.hpp"
#include "Concurrent/Thread.h"
#include "Concurrent/Lock.h"
#include "SGEStringForward.h"
#include "Utility/LockFreeFixedSizeRingBuffer.hpp"
#include "Utility/DebugInformation.h"
#include "Utility/Format.hpp"
#include "Time/Date.h"
#include "Time/TimeCounter.h"
#include "File.h"
#include "GlobalVariable.h"

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{

	template<typename T>
	concept IsLogWriterCore = requires(T t, const Char8* pstr, SizeType size) {
		t.WriteLog(pstr, size);
	};

	class COMMON_API ConsoleLogWriterCore
	{
	public:
		void WriteLog(const Char8* pstr, SizeType size);
	};

	/*!
	@brief Get default log directory path.
	@note When the directory is not existed, the function will create it.
	*/
	COMMON_API Path GetDefaultLogDirectoryPath();

	class COMMON_API FileLogWriterCore : public UncopyableAndUnmovable
	{
	public:
		FileLogWriterCore();
		FileLogWriterCore(const Path& dir_path);

		void WriteLog(const Char8* pstr, SizeType size);

	private:
		void DeleteOldLogFile(const Path& dir_path);

	private:
		UTF8File m_File;
		TimeCounter<Second> m_TimeCounter;
	};

	template<IsLogWriterCore FirstLogWriterCore, IsLogWriterCore SecondLogWriterCore>
	class ProxyPairLogWriterCore
	{
	public:
		inline ProxyPairLogWriterCore(FirstLogWriterCore& first, SecondLogWriterCore& second)
			: m_First(first), m_Second(second)
		{
		}

		inline void WriteLog(const Char8* pstr, SizeType size)
		{
			SGE_ASSERT(NullPointerError, pstr);
			SGE_ASSERT(InvalidValueError, size, 1, SGE_MAX_MEMORY_SIZE);

			m_First.WriteLog(pstr, size);
			m_Second.WriteLog(pstr, size);
		}

	private:
		FirstLogWriterCore& m_First;
		SecondLogWriterCore& m_Second;
	};

	using AllLogWriterCore = ProxyPairLogWriterCore<ConsoleLogWriterCore, FileLogWriterCore>;

	COMMON_API AllLogWriterCore& GetAllLogWriterCore();

	COMMON_API void HandleLogOverflow(const Char8* pstr, SizeType size);

	using DefaultLogWriterCore = ConsoleLogWriterCore;

	inline static constexpr const SizeType LogWriterBufferSize = 4194304;

	template<IsLogWriterCore LogWriterCore = DefaultLogWriterCore>
	class LogWriter : public UncopyableAndUnmovable, public LogWriterCore
	{
	public:
		template<typename... Args>
		inline LogWriter(Args&&... args)
			: LogWriterCore(std::forward<Args>(args)...)
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
			if (!m_Buffer.TryPush(pstr, size))
				HandleLogOverflow(pstr, size);
		}

	private:
		inline void Run()
		{
			while (m_IsRunning.Load(MemoryOrder::Acquire))
			{
				if (!m_Buffer.Pop([this](void* ptr, SizeType size) {
						LogWriterCore::WriteLog((const Char8*)ptr, size);
					}))
					Thread::YieldCurrentThread();
			}
			m_Buffer.Pop([this](void* ptr, SizeType size) {
				LogWriterCore::WriteLog((const Char8*)ptr, size);
			});
		}

	private:
		inline static constexpr const SizeType sm_BufferArraySize = 4;

		Atomic<bool> m_IsRunning;
		Thread m_Thread;
		LockFreeFixedSizeRingBuffer<LogWriterBufferSize, sm_BufferArraySize> m_Buffer;
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
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The LogLevel is invalid.");
		static COMMON_API bool Judge(LogLevelType log_level);
	};

	COMMON_API UTF8String GetLogLevelUTF8String(LogLevelType log_level);

	template<typename T>
	concept IsLogFormatter = requires(const Date& date, const DebugInformation& debug_info, LogLevelType log_level, const UTF8String& str) {
		{
			T::Format(date, debug_info, log_level, str)
		}
		-> std::convertible_to<UTF8String>;
	};

	struct COMMON_API DefaultLogFormatter
	{
		static UTF8String Format(const Date& date, const DebugInformation& debug_info, LogLevelType log_level, const UTF8String& str);
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

#define SGE_LOGGER_DECLARE(api_macro, name)                                                                   \
	api_macro LogWriter<ProxyPairLogWriterCore<AllLogWriterCore, FileLogWriterCore>>& Get##name##LogWriter(); \
	api_macro Logger<ProxyPairLogWriterCore<AllLogWriterCore, FileLogWriterCore>>& Get##name##Logger();

#define SGE_LOGGER_DEFINE(name)                                                                                                                                                      \
	LogWriter<ProxyPairLogWriterCore<AllLogWriterCore, FileLogWriterCore>>& Get##name##LogWriter()                                                                                   \
	{                                                                                                                                                                                \
		static GlobalVariable<FileLogWriterCore> g_##name##FileLogWriterCore(GetDefaultLogDirectoryPath() / Path(SGE_STR(#name)));                                                   \
		static GlobalVariable<LogWriter<ProxyPairLogWriterCore<AllLogWriterCore, FileLogWriterCore>>> g_##name##LogWriter(GetAllLogWriterCore(), g_##name##FileLogWriterCore.Get()); \
		return g_##name##LogWriter.Get();                                                                                                                                            \
	}                                                                                                                                                                                \
	Logger<ProxyPairLogWriterCore<AllLogWriterCore, FileLogWriterCore>>& Get##name##Logger()                                                                                         \
	{                                                                                                                                                                                \
		static GlobalVariable<Logger<ProxyPairLogWriterCore<AllLogWriterCore, FileLogWriterCore>>> g_##name##Logger(Get##name##LogWriter(), LogLevel::All);                          \
		return g_##name##Logger.Get();                                                                                                                                               \
	}

#define SGE_LOG(logger, level, str, ...) logger.WriteLog(SpaceGameEngine::GetLocalDate(), SGE_DEBUG_INFORMATION, level, str, ##__VA_ARGS__);

	extern template class COMMON_API_TEMPLATE_DECLARE ProxyPairLogWriterCore<ConsoleLogWriterCore, FileLogWriterCore>;
	extern template class COMMON_API_TEMPLATE_DECLARE ProxyPairLogWriterCore<FileLogWriterCore, ConsoleLogWriterCore>;
	extern template class COMMON_API_TEMPLATE_DECLARE ProxyPairLogWriterCore<AllLogWriterCore, FileLogWriterCore>;
	extern template class COMMON_API_TEMPLATE_DECLARE ProxyPairLogWriterCore<FileLogWriterCore, AllLogWriterCore>;
	extern template class COMMON_API_TEMPLATE_DECLARE LogWriter<ProxyPairLogWriterCore<AllLogWriterCore, FileLogWriterCore>>;
	extern template class COMMON_API_TEMPLATE_DECLARE LogWriter<ProxyPairLogWriterCore<FileLogWriterCore, AllLogWriterCore>>;
	extern template class COMMON_API_TEMPLATE_DECLARE Logger<ProxyPairLogWriterCore<AllLogWriterCore, FileLogWriterCore>>;
	extern template class COMMON_API_TEMPLATE_DECLARE Logger<ProxyPairLogWriterCore<FileLogWriterCore, AllLogWriterCore>>;

	SGE_LOGGER_DECLARE(COMMON_API, Default);

}

/*!
@}
*/