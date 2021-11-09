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
#pragma once
#include "CommonAPI.h"
#include "SGEString.hpp"
#ifdef SGE_WINDOWS
#include <Windows.h>
#endif

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

#ifdef SGE_WINDOWS
	struct GetFullPathNameFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("GetFullPathName failed.");
		static COMMON_API bool Judge(DWORD re);
	};

	struct GetCurrentDirectoryFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("GetCurrentDirectory failed.");
		static COMMON_API bool Judge(DWORD re);
	};
#elif defined(SGE_POSIX)
	struct GetCWDFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("getcwd failed.");
		static COMMON_API bool Judge(char* re);
	};
#endif

	COMMON_API String NormalizePathString(const String& path);

	/*!
	@brief Make the absolute path without "." and "..".
	@warning The parameter path must be a normalized path string.
	*/
	COMMON_API String NormalizeAbsolutePathString(const String& path);

	class COMMON_API Path
	{
	public:
		Path();
		Path(const String& path);

		String GetSystemPathString() const;
		String GetString() const;

		bool IsAbsolute() const;
		bool IsRelative() const;

		Path GetAbsolutePath() const;
		Path GetParentPath() const;

		Path operator/(const Path& path) const;

	private:
		String m_Content;
	};

	struct AbsolutePathAdditionError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("Absolute path can not be added to another path.");
		static COMMON_API bool Judge(const Path& path);
	};

	COMMON_API Path GetCurrentDirectoryPath();

	COMMON_API Path SetCurrentDirectoryPath(const Path& path);

	COMMON_API Path GetModuleDirectoryPath();

	/*!
	@}
	*/
}