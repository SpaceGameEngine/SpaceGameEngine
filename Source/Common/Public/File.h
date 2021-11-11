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
#elif defined(SGE_POSIX)
#include <unistd.h>
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
		static COMMON_API bool Judge(DWORD re, SizeType buf_size);
	};

	struct GetCurrentDirectoryFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("GetCurrentDirectory failed.");
		static COMMON_API bool Judge(DWORD re, SizeType buf_size);
	};

	struct GetModuleFileNameFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("GetModuleFileName failed.");
		static COMMON_API bool Judge(DWORD re, SizeType buf_size);
	};

	struct FindFirstFileFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("FindFirstFile failed.");
		static COMMON_API bool Judge(HANDLE handle);
	};

	struct FindCloseFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("FindClose failed.");
		static COMMON_API bool Judge(BOOL re);
	};

	struct SetCurrentDirectoryFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("SetCurrentDirectory failed.");
		static COMMON_API bool Judge(BOOL re);
	};
#elif defined(SGE_POSIX)
	struct GetCWDFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("getcwd failed.");
		static COMMON_API bool Judge(char* re);
	};

	struct ChDirFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("chdir failed.");
		static COMMON_API bool Judge(int re);
	};
#endif

#ifdef SGE_LINUX
	struct ReadLinkFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("readlink failed.");
		static COMMON_API bool Judge(ssize_t re);
	};
#elif defined(SGE_MACOS)
	struct NSGetExecutablePathFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("NSGetExecutablePath failed.");
		static COMMON_API bool Judge(int re);
	};

	struct RealPathFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("NSGetExecutablePath failed.");
		static COMMON_API bool Judge(char* re);
	};
#endif

	COMMON_API String NormalizePathString(const String& path);

	/*!
	@brief Make the absolute path without "." and "..".
	@warning The parameter path must be a normalized absolute path string.
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
		bool IsRoot() const;
		bool IsExist() const;

		Path GetAbsolutePath() const;
		/*!
		@brief Get the parent path of this path.
		@return The absolute parent path.
		*/
		Path GetParentPath() const;

		Path operator/(const Path& path) const;

		/*!
		@brief Compare the two paths by comparing their inner string content;
		*/
		bool operator==(const Path& path) const;
		/*!
		@brief Compare the two paths by comparing their inner string content;
		*/
		bool operator!=(const Path& path) const;

	private:
		String m_Content;
	};

	struct AbsolutePathAdditionError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("Absolute path can not be added to another path.");
		static COMMON_API bool Judge(const Path& path);
	};

	struct PathNotExistError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("Path does not exist.");
		static COMMON_API bool Judge(const Path& path);
	};

	COMMON_API Path GetCurrentDirectoryPath();

	COMMON_API void SetCurrentDirectoryPath(const Path& path);

	COMMON_API Path GetModuleDirectoryPath();

	/*!
	@}
	*/
}