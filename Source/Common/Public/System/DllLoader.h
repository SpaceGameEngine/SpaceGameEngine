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
#pragma once
#include "SGEString.hpp"
#ifdef SGE_WINDOWS
#include <Windows.h>
#elif defined(SGE_POSIX)
#include <dlfcn.h>
#else
#error "this platform is not supported"
#endif

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

#ifdef SGE_WINDOWS
	using DllHandle = HMODULE;
#elif defined(SGE_POSIX)
	using DllHandle = void*;
#else
#error "this platform is not supported"
#endif

	struct EmptyDllNameError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("Dll's name is empty.");
		static COMMON_API bool Judge(const String& name);
	};

	struct EmptyDllPathError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("Dll's path is empty.");
		static COMMON_API bool Judge(const String& dll_path);
	};

	struct EmptySymbolError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("Symbol's string is empty.");
		static COMMON_API bool Judge(const String& symbol);
	};

	struct LoadDllFailedError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("Loading dll is failed.");
		static COMMON_API bool Judge(const DllHandle& re);
	};

	struct GetAddressFromDllFailedError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("Getting address from dll is failed.");
		static COMMON_API bool Judge(void* re);
	};

	struct UnloadDllFailedError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("Unloading dll is failed.");
		static COMMON_API bool Judge(int re);
	};

	struct NullDllHandleError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("Dll's handle is null.");
		static COMMON_API bool Judge(const DllHandle& handle);
	};

	COMMON_API DllHandle LoadDll(const String& dll_path);

	COMMON_API void* GetAddressFromDll(const DllHandle& handle, const String& symbol);

	COMMON_API void UnloadDll(const DllHandle& handle);

	/*!
	@brief get the dll's relative path which conform with the project rule.
	@param dll_name the name of the dll(without any prefix or suffix).
	*/
	COMMON_API String GetDllPath(const String& dll_name);

#ifdef SGE_WINDOWS
#include "System/HideWindowsMacro.h"
#endif

	/*!
	@}
	*/
}