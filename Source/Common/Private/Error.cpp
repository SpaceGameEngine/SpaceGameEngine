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
#include "Error.h"
#include "SGEString.hpp"
#include <iostream>

#if defined(SGE_DEBUG) && defined(SGE_WINDOWS)
#include <Windows.h>
#endif

SpaceGameEngine::DebugInformation::DebugInformation(const Char* file_name, const Char* func_name, UInt32 line_number)
{
	m_pFileName = file_name;
	m_pFunctionName = func_name;
	m_LineNumber = line_number;
}

void SpaceGameEngine::ThrowError(const Char* error_msg, DebugInformation debug_info)
{
/*!@todo add unix compitable verson*/
#ifdef SGE_WINDOWS
	StdString output_msg = SGE_STR("error happend in ");
	output_msg += debug_info.m_pFileName;
	output_msg += SGE_STR(" ");
	output_msg += debug_info.m_pFunctionName;
	output_msg += SGE_STR(" ");
	output_msg += StdToString(debug_info.m_LineNumber);
	output_msg += SGE_STR(" : ");
	output_msg += error_msg;
	/*!@todo use sge's output like log or messagebox*/
	StdCout << output_msg << std::endl;
#endif
#if defined(SGE_DEBUG) && defined(SGE_WINDOWS)
	DebugBreak();
#endif
	abort();
}

bool SpaceGameEngine::NullPointerError::Judge(const void* ptr)
{
	return ptr == nullptr;
}

bool SpaceGameEngine::InvalidSizeError::Judge(SizeType size, SizeType min_size, SizeType max_size)
{
	return !(size >= min_size && size <= max_size);
}
