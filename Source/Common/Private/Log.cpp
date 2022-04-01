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
		{
			DeleteFile(Path(iter->m_First));
		}
	}
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

LogWriter<BindConsoleLogWriterCore<FileLogWriterCore>>& SpaceGameEngine::GetDefaultLogWriter()
{
	static GlobalVariable<LogWriter<BindConsoleLogWriterCore<FileLogWriterCore>>> g_DefaultLogWriter(GetDefaultLogDirectoryPath() / Path(SGE_STR("Default")));
	return g_DefaultLogWriter.Get();
}

Logger<BindConsoleLogWriterCore<FileLogWriterCore>>& SpaceGameEngine::GetDefaultLogger()
{
	static GlobalVariable<Logger<BindConsoleLogWriterCore<FileLogWriterCore>>> g_DefaultLogger(GetDefaultLogWriter(), LogLevel::All);
	return g_DefaultLogger.Get();
}
