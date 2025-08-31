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
#include "Container/HashSet.hpp"
#include "Container/Concurrent/LockFreeSCSPFixedSizeByteRingBuffer.hpp"
#include "SGEStringForward.h"
#include "Utility/DebugInformation.h"
#include "Utility/Format.hpp"
#include "Utility/Singleton.hpp"
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
	class COMMON_API LogWriterCore
	{
	public:
		virtual ~LogWriterCore() = default;
		virtual void WriteLog(const Char8* pstr, SizeType size) = 0;
	};

	template<typename T>
	concept IsLogWriterCore = std::derived_from<T, LogWriterCore>;

	class COMMON_API ConsoleLogWriterCore : public LogWriterCore
	{
	public:
		void WriteLog(const Char8* pstr, SizeType size) override;
	};

	/*!
	@brief Get default log directory path.
	@note When the directory is not existed, the function will create it.
	*/
	COMMON_API Path GetDefaultLogDirectoryPath();

	class COMMON_API FileLogWriterCore : public LogWriterCore, public UncopyableAndUnmovable
	{
	public:
		FileLogWriterCore();
		FileLogWriterCore(const Path& dir_path);

		void WriteLog(const Char8* pstr, SizeType size) override;

	private:
		void DeleteOldLogFile(const Path& dir_path);

	private:
		UTF8File m_File;
		TimeCounter<Second> m_TimeCounter;
	};

	template<IsLogWriterCore FirstLogWriterCore, IsLogWriterCore SecondLogWriterCore>
	class ProxyPairLogWriterCore : public LogWriterCore
	{
	public:
		inline ProxyPairLogWriterCore(FirstLogWriterCore& first, SecondLogWriterCore& second)
			: m_First(first), m_Second(second)
		{
		}

		inline void WriteLog(const Char8* pstr, SizeType size) override
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

	using DefaultLogWriterCore = ConsoleLogWriterCore;

	namespace Detail
	{
		COMMON_API void NotifyLogOverflowToStdErr(const Char8* pstr, SizeType size);

		class LogWriter;

		class COMMON_API LogWriterBuffer : public UncopyableAndUnmovable
		{
		public:
			LogWriterBuffer(LogWriterCore& log_writer_core);

			~LogWriterBuffer();

			/*!
			@brief Write log to the buffer.
			@note Should be called from the producer thread.
			*/
			void Push(const Char8* pstr, SizeType size);

			/*!
			@brief Pop log from the buffer and write them to the log writer core.
			@note Should be called from the consumer thread.
			@return Have popped or not.
			*/
			bool Pop();

			void OnLogWriterReleased();

		private:
			void HandleLogOverflow(const Char8* pstr, SizeType size);

		private:
			inline static constexpr const SizeType sm_BufferSize = 4194304;

			bool m_IsLogWriterReleased = false;
			LogWriterCore& m_LogWriterCore;
			LockFreeSCSPFixedSizeByteRingBuffer<sm_BufferSize> m_Content;
		};

		class COMMON_API LogWriter : public UncopyableAndUnmovable,
									 public Singleton<LogWriter>
		{
		private:
			LogWriter();

		public:
			friend DefaultAllocator;
			friend LogWriterBuffer;

			~LogWriter();

		private:
			void Run();

			void RegisterBuffer(LogWriterBuffer& buffer);

			void UnregisterBuffer(LogWriterBuffer& buffer);

		private:
			Atomic<bool> m_IsRunning = true;
			Thread m_Thread;
			Mutex m_mutex;
			HashSet<LogWriterBuffer*> m_Buffers;
		};
	}

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

	template<IsLogFormatter LogFormatter = DefaultLogFormatter>
	class Logger : public UncopyableAndUnmovable
	{
	public:
		inline Logger(LogWriterCore& log_writer_core, LogLevelType log_level = LogLevel::All)
			: m_Buffer(log_writer_core), m_LogLevel(log_level)
		{
		}

		inline void WriteLog(const Date& date, const DebugInformation& debug_info, LogLevelType log_level, const UTF8String& str)
		{
			if (log_level > m_LogLevel)
				return;
			else
			{
				UTF8String result = LogFormatter::Format(date, debug_info, log_level, str);
				m_Buffer.Push(result.GetData(), result.GetNormalSize());
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
				m_Buffer.Push(result.GetData(), result.GetNormalSize());
			}
		}

	private:
		Detail::LogWriterBuffer m_Buffer;
		LogLevelType m_LogLevel;
	};

#define SGE_LOGGER_DECLARE(api_macro, name) \
	api_macro Logger<>& Get##name##Logger();

#define SGE_LOGGER_DEFINE(name)                                                                                                                                               \
	Logger<>& Get##name##Logger()                                                                                                                                             \
	{                                                                                                                                                                         \
		static GlobalVariable<FileLogWriterCore> g_##name##FileLogWriterCore(GetDefaultLogDirectoryPath() / Path(SGE_STR(#name)));                                            \
		static GlobalVariable<ProxyPairLogWriterCore<AllLogWriterCore, FileLogWriterCore>> g_##name##LogWriterCore(GetAllLogWriterCore(), g_##name##FileLogWriterCore.Get()); \
		thread_local Logger<> t_##name##Logger(g_##name##LogWriterCore.Get());                                                                                                \
		return t_##name##Logger;                                                                                                                                              \
	}

#define SGE_LOG(logger, level, str, ...) logger.WriteLog(SpaceGameEngine::GetLocalDate(), SGE_DEBUG_INFORMATION, level, str, ##__VA_ARGS__);

	extern template class COMMON_API_TEMPLATE_DECLARE ProxyPairLogWriterCore<ConsoleLogWriterCore, FileLogWriterCore>;
	extern template class COMMON_API_TEMPLATE_DECLARE ProxyPairLogWriterCore<FileLogWriterCore, ConsoleLogWriterCore>;
	extern template class COMMON_API_TEMPLATE_DECLARE ProxyPairLogWriterCore<AllLogWriterCore, FileLogWriterCore>;
	extern template class COMMON_API_TEMPLATE_DECLARE ProxyPairLogWriterCore<FileLogWriterCore, AllLogWriterCore>;
	extern template class COMMON_API_TEMPLATE_DECLARE Logger<>;

	SGE_LOGGER_DECLARE(COMMON_API, Default);

}

/*!
@}
*/