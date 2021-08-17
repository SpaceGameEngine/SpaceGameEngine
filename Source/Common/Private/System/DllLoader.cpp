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
#include "..\..\Public\System\DllLoader.h"

bool SpaceGameEngine::EmptyDllNameError::Judge(const String& name)
{
	return name.GetSize() == 0;
}

bool SpaceGameEngine::EmptySymbolError::Judge(const String& symbol)
{
	return symbol.GetSize() == 0;
}

bool SpaceGameEngine::LoadDllFailedError::Judge(const DllHandle& re)
{
#if defined(SGE_WINDOWS) || defined(SGE_UNIX)
	return re == 0;
#else
#error "this platform is not supported"
#endif
}

bool SpaceGameEngine::GetAddressFromDllFailedError::Judge(void* re)
{
#if defined(SGE_WINDOWS) || defined(SGE_UNIX)
	return re == nullptr;
#else
#error "this platform is not supported"
#endif
}

bool SpaceGameEngine::UnloadDllFailedError::Judge(int re)
{
#if defined(SGE_WINDOWS)
	return re == 0;
#elif defined(SGE_UNIX)
	return re != 0;
#else
#error "this platform is not supported"
#endif
}

bool SpaceGameEngine::NullDllHandleError::Judge(const DllHandle& handle)
{
#if defined(SGE_WINDOWS) || defined(SGE_UNIX)
	return handle == 0;
#else
#error "this platform is not supported"
#endif
}

SpaceGameEngine::DllHandle SpaceGameEngine::LoadDll(const String& dll)
{
	SGE_ASSERT(EmptyDllNameError, dll);
#ifdef SGE_WINDOWS
	DllHandle re = LoadLibrary(SGE_UCS2_TO_TSTR(dll).GetData());
	SGE_CHECK(LoadDllFailedError, re);
	return re;
#elif defined(SGE_UNIX)
	//todo
#else
#error "this platform is not supported"
#endif
}

void* SpaceGameEngine::GetAddressFromDll(const DllHandle& handle, const String& symbol)
{
	SGE_ASSERT(NullDllHandleError, handle);
	SGE_ASSERT(EmptySymbolError, symbol);
#ifdef SGE_WINDOWS
	void* re = (void*)GetProcAddress(handle, UCS2StringToUTF8String(symbol).GetData());
	SGE_CHECK(GetAddressFromDllFailedError, re);
	return re;
#elif defined(SGE_UNIX)
	//todo
#else
#error "this platform is not supported"
#endif
}

void SpaceGameEngine::UnloadDll(const DllHandle& handle)
{
	SGE_ASSERT(NullDllHandleError, handle);
#ifdef SGE_WINDOWS
	SGE_CHECK(UnloadDllFailedError, FreeLibrary(handle));
#elif defined(SGE_UNIX)
	//todo
#else
#error "this platform is not supported"
#endif
}
