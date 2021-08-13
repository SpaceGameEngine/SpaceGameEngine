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
#include "Bootstrap.h"
#include "Platform.hpp"
#include "SGEStringForward.h"
#ifdef SGE_WINDOWS
#include <Windows.h>
#endif

#ifdef SGE_WINDOWS
static HMODULE g_CommonModule;
#endif

SpaceGameEngine::Bootstrap::Bootstrap()
{
#ifdef SGE_WINDOWS
#ifdef SGE_X86
#ifdef SGE_DEBUG
	g_CommonModule = LoadLibrary(SGE_TSTR(".\\Binary\\Common\\x86\\Debug\\Common.dll"));
#else
	g_CommonModule = LoadLibrary(SGE_TSTR(".\\Binary\\Common\\x86\\Release\\Common.dll"));
#endif
#elif defined(SGE_X64)
#ifdef SGE_DEBUG
	g_CommonModule = LoadLibrary(SGE_TSTR(".\\Binary\\Common\\x64\\Debug\\Common.dll"));
#else
	g_CommonModule = LoadLibrary(SGE_TSTR(".\\Binary\\Common\\x64\\Release\\Common.dll"));
#endif
#else
	static_assert(false, "Platform macro error!");
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
#ifdef SGE_WINDOWS
	if (g_CommonModule == 0)
	{
		DebugBreak();
	}
	else
	{
		FreeLibrary(g_CommonModule);
	}
#endif
}
