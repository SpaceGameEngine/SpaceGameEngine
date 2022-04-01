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
#include "Bootstrap.h"
#include "Platform.hpp"
#include "SGEStringForward.h"
#include "CommonAPI.h"
#if defined(SGE_WINDOWS) && defined(SGE_MSVC) && defined(SGE_USE_DLL)
#include <Windows.h>
#endif

#if defined(SGE_WINDOWS) && defined(SGE_MSVC) && defined(SGE_USE_DLL)
static HMODULE g_CommonModule;
#endif

SpaceGameEngine::Bootstrap::Bootstrap()
{
#if defined(SGE_WINDOWS) && defined(SGE_MSVC) && defined(SGE_USE_DLL)
	const SizeType buf_size = 4096;
	TChar out_buffer[buf_size];
	memset(out_buffer, 0, sizeof(out_buffer));
	auto re_size = GetModuleFileName(NULL, out_buffer, buf_size);
	if (re_size == 0 || re_size > buf_size)
	{
		DWORD error_code = GetLastError();
		DebugBreak();
	}
	StdTString path(out_buffer);
	auto slash_idx = path.rfind(SGE_TSTR('\\'));
	path = path.substr(0, slash_idx);
	/*!
	@todo use ProjectManager to make this function configable and flexiable(use code generation to make config).
	*/
	path += SGE_TSTR("\\..\\..\\..\\..\\Binary\\Common");
#ifdef SGE_X86
#ifdef SGE_DEBUG
	path += SGE_TSTR("\\x86\\Debug\\Common.dll");
#else
	path += SGE_TSTR("\\x86\\Release\\Common.dll");
#endif
#elif defined(SGE_X64)
#ifdef SGE_DEBUG
	path += SGE_TSTR("\\x64\\Debug\\Common.dll");
#else
	path += SGE_TSTR("\\x64\\Release\\Common.dll");
#endif
#else
#error "Platform macro error!"
#endif
#ifdef SGE_USE_WIDE_CHAR
	g_CommonModule = LoadLibrary(path.c_str());
#else
	g_CommonModule = LoadLibrary((const char*)(path.c_str()));
#endif
	if (g_CommonModule == 0)
	{
		DWORD error_code = GetLastError();
		DebugBreak();
	}
#endif
}

SpaceGameEngine::Bootstrap::~Bootstrap()
{
#if defined(SGE_WINDOWS) && defined(SGE_MSVC) && defined(SGE_USE_DLL)
	if (g_CommonModule == 0)
	{
		DWORD error_code = GetLastError();
		DebugBreak();
	}
	else
	{
		FreeLibrary(g_CommonModule);
	}
#endif
}
