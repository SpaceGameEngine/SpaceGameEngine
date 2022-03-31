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
#include <iostream>
#include <functional>

using namespace SpaceGameEngine;

void SpaceGameEngine::ConsoleLogWriterCore::WriteLog(const Char8* pstr, SizeType size)
{
	SGE_ASSERT(NullPointerError, pstr);
	SGE_ASSERT(InvalidValueError, size, 1, SGE_MAX_MEMORY_SIZE);

	std::cout.write((const char*)pstr, size);
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
