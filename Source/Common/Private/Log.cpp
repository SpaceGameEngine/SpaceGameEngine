/*
Copyright 2026 creatorlxd

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
#include "Container/Map.hpp"
#include <iostream>
#include <functional>

using namespace SpaceGameEngine;

void SpaceGameEngine::ConsoleLogWriterCore::WriteLog(const Char8* pstr, SizeType size)
{
	SGE_ASSERT(NullPointerError, pstr);
	SGE_ASSERT(InvalidValueError, size, 1, SGE_MAX_MEMORY_SIZE);

	std::cout.write((const char*)pstr, size);
}

Path SpaceGameEngine::GetDefaultLogDirectoryPath()
{
	Path p = GetProjectDirectoryPath() / Path(SGE_STR("Log"));
	if (!p.IsExist())
		CreateDirectory(p);
	return p;
}

SpaceGameEngine::FileLogWriterCore::FileLogWriterCore()
{
	Path dir_path = GetDefaultLogDirectoryPath();
	DeleteOldLogFile(dir_path);
	Date date = GetLocalDate();
	m_File.Open(dir_path / Path(Format(String(SGE_STR("{:4}-{:2}-{:2}_{:2}-{:2}-{:2}.log")), date.m_Year, date.m_Month, date.m_Day, date.m_Hour, date.m_Minute, date.m_Second)), FileIOMode::Read | FileIOMode::Write);
	m_File.SetHasBomHeader(true);
	m_TimeCounter.Start();
}

SpaceGameEngine::FileLogWriterCore::FileLogWriterCore(const Path& dir_path)
{
	SGE_ASSERT(PathNotExistError, dir_path.GetParentPath());
	SGE_ASSERT(PathNotDirectoryError, dir_path.GetParentPath());

	if (!dir_path.IsExist())
		CreateDirectory(dir_path);
	DeleteOldLogFile(dir_path);
	Date date = GetLocalDate();
	m_File.Open(dir_path / Path(Format(String(SGE_STR("{:4}-{:2}-{:2}_{:2}-{:2}-{:2}.log")), date.m_Year, date.m_Month, date.m_Day, date.m_Hour, date.m_Minute, date.m_Second)), FileIOMode::Read | FileIOMode::Write);
	m_File.SetHasBomHeader(true);
	m_TimeCounter.Start();
}

void SpaceGameEngine::FileLogWriterCore::WriteLog(const Char8* pstr, SizeType size)
{
	SGE_ASSERT(NullPointerError, pstr);
	SGE_ASSERT(InvalidValueError, size, 1, SGE_MAX_MEMORY_SIZE);

	m_File.Write(pstr, size);
	m_TimeCounter.Tick();
	if (m_TimeCounter.GetDeltaTime() >= 2)
		m_File.Flush();
}

void SpaceGameEngine::FileLogWriterCore::DeleteOldLogFile(const Path& dir_path)
{
	SGE_ASSERT(PathNotExistError, dir_path);
	SGE_ASSERT(PathNotDirectoryError, dir_path);

	Map<String, bool> map;
	dir_path.VisitChildPath([&map, &dir_path](const String& filename, PathType ptype) -> void {
		if (ptype == PathType::File)
			map.Insert(dir_path.GetAbsolutePath().GetString() + SGE_STR("/") + filename, true);
	});
	if (map.GetSize() > 4)
	{
		auto iter = map.GetConstBegin();
		for (SizeType i = 0; i < map.GetSize() - 4; ++i, ++iter)
			DeleteFile(Path(iter->m_First));
	}
}

AllLogWriterCore& SpaceGameEngine::GetAllLogWriterCore()
{
	static GlobalVariable<ConsoleLogWriterCore> g_AllConsoleLogWriterCore;
	static GlobalVariable<FileLogWriterCore> g_AllFileLogWriterCore(GetDefaultLogDirectoryPath() / Path(SGE_STR("All")));
	static GlobalVariable<AllLogWriterCore> g_AllLogWriterCore(g_AllConsoleLogWriterCore.Get(), g_AllFileLogWriterCore.Get());
	return g_AllLogWriterCore.Get();
}

void SpaceGameEngine::Detail::NotifyLogOverflowToStdErr(const Char8* pstr, SizeType size)
{
	std::cerr << "Log overflow" << std::endl;
}

SpaceGameEngine::Detail::LogWriterBuffer::LogWriterBuffer(LogWriterCore& log_writer_core)
	: m_LogWriterCore(log_writer_core)
{
	LogWriter::GetSingleton().RegisterBuffer(*this);
}

SpaceGameEngine::Detail::LogWriterBuffer::~LogWriterBuffer()
{
	if (!m_IsLogWriterReleased)
	{
		while (m_Content.GetSize())
			Thread::YieldCurrentThread();	 // wait for the buffer to be empty
		LogWriter::GetSingleton().UnregisterBuffer(*this);
	}
}

void SpaceGameEngine::Detail::LogWriterBuffer::Push(const Char8* pstr, SizeType size)
{
	SGE_ASSERT(NullPointerError, pstr);
	SGE_ASSERT(InvalidValueError, size, 1, m_Content.GetFreeSize());
	if (!m_Content.TryPush(pstr, size))
		HandleLogOverflow(pstr, size);
}

bool SpaceGameEngine::Detail::LogWriterBuffer::Pop()
{
	return m_Content.Pop(BufferSize, [this](void* ptr, SizeType size) {
		m_LogWriterCore.WriteLog((Char8*)ptr, size);
	});
}

void SpaceGameEngine::Detail::LogWriterBuffer::OnLogWriterReleased()
{
	m_IsLogWriterReleased = true;
}

void SpaceGameEngine::Detail::LogWriterBuffer::HandleLogOverflow(const Char8* pstr, SizeType size)
{
	NotifyLogOverflowToStdErr(pstr, size);
}

SpaceGameEngine::Detail::LogWriter::LogWriter()
{
	m_Thread = Thread(std::bind(&LogWriter::Run, this));
}

SpaceGameEngine::Detail::LogWriter::~LogWriter()
{
	m_IsRunning.Store(false, MemoryOrder::Release);
	m_Thread.Join();
	RecursiveLock lock(m_mutex);
	for (auto iter = m_Buffers.GetBegin(); iter != m_Buffers.GetEnd(); ++iter)
		(*iter)->OnLogWriterReleased();
}

void SpaceGameEngine::Detail::LogWriter::Run()
{
	while (m_IsRunning.Load(MemoryOrder::Acquire))
	{
		RecursiveLock lock(m_mutex);
		bool did_popped = false;
		for (auto iter = m_Buffers.GetBegin(); iter != m_Buffers.GetEnd(); ++iter)
			did_popped |= (*iter)->Pop();
		if (!did_popped)
			Thread::YieldCurrentThread();
	}
}

void SpaceGameEngine::Detail::LogWriter::RegisterBuffer(LogWriterBuffer& buffer)
{
	RecursiveLock lock(m_mutex);
	m_Buffers.Insert(&buffer);
}

void SpaceGameEngine::Detail::LogWriter::UnregisterBuffer(LogWriterBuffer& buffer)
{
	RecursiveLock lock(m_mutex);
	m_Buffers.RemoveByValue(&buffer);
}

bool SpaceGameEngine::InvalidLogLevelError::Judge(LogLevelType log_level)
{
	return log_level > LogLevel::All;
}

UTF8String SpaceGameEngine::GetLogLevelUTF8String(LogLevelType log_level)
{
	SGE_ASSERT(InvalidLogLevelError, log_level);

	switch (log_level)
	{
	case LogLevel::Error:
		return SGE_U8STR("ERROR");
	case LogLevel::Exception:
		return SGE_U8STR("EXCEPTION");
	case LogLevel::Warning:
		return SGE_U8STR("WARNING");
	case LogLevel::Information:
		return SGE_U8STR("INFORMATION");
	case LogLevel::Debug:
		return SGE_U8STR("DEBUG");
	default:
		return SGE_U8STR("ERROR_LEVEL");
	}
}

UTF8String SpaceGameEngine::DefaultLogFormatter::Format(const Date& date, const DebugInformation& debug_info, LogLevelType log_level, const UTF8String& str)
{
	return SpaceGameEngine::Format(UTF8String(SGE_U8STR("{:4}-{:2}-{:2} {:2}:{:2}:{:2} {}:{}:{} {} {}\n")), date.m_Year, date.m_Month, date.m_Day, date.m_Hour, date.m_Minute, date.m_Second, SGE_TSTR_TO_UTF8(debug_info.m_pFileName), SGE_TSTR_TO_UTF8(debug_info.m_pFunctionName), debug_info.m_LineNumber, GetLogLevelUTF8String(log_level), str);
}

namespace SpaceGameEngine
{
	template class COMMON_API_TEMPLATE_DEFINE ProxyPairLogWriterCore<ConsoleLogWriterCore, FileLogWriterCore>;
	template class COMMON_API_TEMPLATE_DEFINE ProxyPairLogWriterCore<FileLogWriterCore, ConsoleLogWriterCore>;
	template class COMMON_API_TEMPLATE_DEFINE ProxyPairLogWriterCore<AllLogWriterCore, FileLogWriterCore>;
	template class COMMON_API_TEMPLATE_DEFINE ProxyPairLogWriterCore<FileLogWriterCore, AllLogWriterCore>;
	template class COMMON_API_TEMPLATE_DEFINE Logger<>;

	SGE_LOGGER_DEFINE(Default);
}
