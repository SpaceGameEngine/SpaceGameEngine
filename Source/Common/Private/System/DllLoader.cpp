﻿/*
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
#include "System/DllLoader.h"
#include "File.h"

#ifdef SGE_WINDOWS
#include "System/AllowWindowsMacro.h"
#endif

bool SpaceGameEngine::EmptyDllNameError::Judge(const String& name)
{
	return name.GetSize() == 0;
}

bool SpaceGameEngine::EmptyDllPathError::Judge(const String& dll_path)
{
	return dll_path.GetSize() == 0;
}

bool SpaceGameEngine::EmptySymbolError::Judge(const String& symbol)
{
	return symbol.GetSize() == 0;
}

bool SpaceGameEngine::LoadDllFailedError::Judge(const DllHandle& re)
{
#if defined(SGE_WINDOWS) || defined(SGE_POSIX)
	return re == 0;
#else
#error "this platform is not supported"
#endif
}

bool SpaceGameEngine::GetAddressFromDllFailedError::Judge(void* re)
{
#if defined(SGE_WINDOWS) || defined(SGE_POSIX)
	return re == nullptr;
#else
#error "this platform is not supported"
#endif
}

bool SpaceGameEngine::UnloadDllFailedError::Judge(int re)
{
#if defined(SGE_WINDOWS)
	return re == 0;
#elif defined(SGE_POSIX)
	return re != 0;
#else
#error "this platform is not supported"
#endif
}

bool SpaceGameEngine::NullDllHandleError::Judge(const DllHandle& handle)
{
#if defined(SGE_WINDOWS) || defined(SGE_POSIX)
	return handle == 0;
#else
#error "this platform is not supported"
#endif
}

SpaceGameEngine::DllHandle SpaceGameEngine::LoadDll(const String& dll_path)
{
	SGE_ASSERT(EmptyDllPathError, dll_path);
#ifdef SGE_WINDOWS
	DllHandle re = LoadLibrary(SGE_STR_TO_TSTR(dll_path).GetData());
	SGE_CHECK(LoadDllFailedError, re);
	return re;
#elif defined(SGE_POSIX)
	DllHandle re = dlopen((const char*)SGE_STR_TO_TSTR(dll_path).GetData(), RTLD_NOW | RTLD_GLOBAL);
	SGE_CHECK(LoadDllFailedError, re);
	return re;
#else
#error "this platform is not supported"
#endif
}

void* SpaceGameEngine::GetAddressFromDll(const DllHandle& handle, const String& symbol)
{
	SGE_ASSERT(NullDllHandleError, handle);
	SGE_ASSERT(EmptySymbolError, symbol);
#ifdef SGE_WINDOWS
	void* re = (void*)GetProcAddress(handle, (char*)UCS2StringToUTF8String(symbol).GetData());
	SGE_CHECK(GetAddressFromDllFailedError, re);
	return re;
#elif defined(SGE_POSIX)
	void* re = dlsym(handle, (const char*)UCS2StringToUTF8String(symbol).GetData());
	SGE_CHECK(GetAddressFromDllFailedError, re);
	return re;
#else
#error "this platform is not supported"
#endif
}

void SpaceGameEngine::UnloadDll(const DllHandle& handle)
{
	SGE_ASSERT(NullDllHandleError, handle);
#ifdef SGE_WINDOWS
	SGE_CHECK(UnloadDllFailedError, FreeLibrary(handle));
#elif defined(SGE_POSIX)
	SGE_CHECK(UnloadDllFailedError, dlclose(handle));
#else
#error "this platform is not supported"
#endif
}

SpaceGameEngine::String SpaceGameEngine::GetDllPath(const String& dll_name)
{
	SGE_ASSERT(EmptyDllNameError, dll_name);
#ifdef SGE_WINDOWS
	String dll_path = (GetProjectDirectoryPath() / Path(SGE_STR("Binary")) / Path(dll_name)).GetAbsolutePath().GetSystemPathString();
#ifdef SGE_X86
	dll_path += SGE_STR("\\x86");
#elif defined SGE_X64
	dll_path += SGE_STR("\\x64");
#else
#error "Platform macro error!"
#endif

#ifdef SGE_DEBUG
	dll_path += SGE_STR("\\Debug\\");
#else
	dll_path += SGE_STR("\\Release\\");
#endif

	dll_path += dll_name;
	dll_path += SGE_STR(".dll");

	return dll_path;
#elif defined(SGE_MACOS)	//CMake support @rpath
	String re(SGE_STR("lib"));
	re += dll_name;
	re += SGE_STR(".dylib");
	return re;
#elif defined(SGE_LINUX)	//CMake support rpath
	String re(SGE_STR("lib"));
	re += dll_name;
	re += SGE_STR(".so");
	return re;
#else
#error "this platform is not supported"
#endif
}
