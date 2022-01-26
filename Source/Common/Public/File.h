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
#include <tchar.h>
#elif defined(SGE_POSIX)
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
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

	struct FindNextFileFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("FindNextFile failed.");
		static COMMON_API bool Judge(DWORD last_error);
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

	struct CreateFileFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("CreateFile failed.");
		static COMMON_API bool Judge(HANDLE handle);
	};

	struct CloseHandleFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("CloseHandle failed.");
		static COMMON_API bool Judge(BOOL re);
	};

	struct GetFileInformationByHandleExFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("GetFileInformationByHandleEx failed.");
		static COMMON_API bool Judge(BOOL re);
	};

	struct DeleteFileFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("DeleteFile failed.");
		static COMMON_API bool Judge(BOOL re);
	};

#include "System/HideWindowsMacro.h"

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

	struct StatFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("stat failed.");
		static COMMON_API bool Judge(int re);
	};

	struct OpenDirFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("opendir failed.");
		static COMMON_API bool Judge(DIR* re);
	};

	struct CloseDirFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("closedir failed.");
		static COMMON_API bool Judge(int re);
	};

	struct CreatFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("creat failed.");
		static COMMON_API bool Judge(int re);
	};

	struct OpenFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("open failed.");
		static COMMON_API bool Judge(int re);
	};

	struct CloseFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("close failed.");
		static COMMON_API bool Judge(int re);
	};

	struct UnlinkFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("unlink failed.");
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

	struct PathNotDirectoryError;

	enum class PathType : UInt8
	{
		NotExist = 0,
		File = 1,
		Directory = 2,
		Link = 3,
		Unknown = 4
	};

	class COMMON_API Path
	{
	public:
		Path();
		Path(const String& path);

		String GetSystemPathString() const;
		String GetString() const;
		String GetFileName() const;

		bool IsAbsolute() const;
		bool IsRelative() const;
		bool IsRoot() const;
		bool IsExist() const;

		PathType GetPathType() const;

		Path GetAbsolutePath() const;
		/*!
		@brief Get the parent path of this path.
		@return The absolute parent path.
		*/
		Path GetParentPath() const;

		/*!
		@brief Get the child paths in Vector<Pair<Path, PathType>> which the Path is the absolute path.
		*/
		Vector<Pair<Path, PathType>> GetChildPath() const;

		/*!
		@brief Visit the child paths.
		@note GetChildPath returns absolute child paths, but this function calls the parameter with the filename.
		@param callable function which adopts (file_name, path_type) as arguments.
		*/
		template<typename Callable>
		inline void VisitChildPath(Callable&& callable) const
		{
			Path apath = GetAbsolutePath();
			SGE_ASSERT(PathNotDirectoryError, apath);
			String astr = apath.GetString();
#ifdef SGE_WINDOWS
			String qstr = astr + SGE_STR("/*");
			WIN32_FIND_DATA find_file_data;
			HANDLE handle = FindFirstFile(SGE_STR_TO_TSTR(qstr).GetData(), &find_file_data);
			SGE_CHECK(FindFirstFileFailError, handle);
			do
			{
				if (_tcscmp(find_file_data.cFileName, SGE_TSTR(".")) == 0 || _tcscmp(find_file_data.cFileName, SGE_TSTR("..")) == 0)
					continue;
				PathType pt = PathType::NotExist;
				if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
					pt = PathType::Link;
				else if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					pt = PathType::Directory;
				else if ((find_file_data.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) || (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_NORMAL))
					pt = PathType::File;
				else
					pt = PathType::Unknown;
				callable(SGE_TSTR_TO_STR(find_file_data.cFileName), pt);
			} while (FindNextFile(handle, &find_file_data));
			SGE_CHECK(FindCloseFailError, FindClose(handle));
			SGE_CHECK(FindNextFileFailError, GetLastError());
#elif defined(SGE_POSIX)
			DIR* pdir = opendir(SGE_STR_TO_TSTR(astr).GetData());
			SGE_CHECK(OpenDirFailError, pdir);
			dirent* pchild = nullptr;
			while (pchild = readdir(pdir))
			{
				if (strcmp(pchild->d_name, SGE_TSTR(".")) == 0 || strcmp(pchild->d_name, SGE_TSTR("..")) == 0)
					continue;
				PathType pt = PathType::NotExist;
				if (pchild->d_type == DT_LNK)
					pt = PathType::Link;
				else if (pchild->d_type == DT_DIR)
					pt = PathType::Directory;
				else if (pchild->d_type == DT_REG)
					pt = PathType::File;
				else
					pt = PathType::Unknown;
				callable(SGE_TSTR_TO_STR(pchild->d_name), pt);
			}
			SGE_CHECK(CloseDirFailError, closedir(pdir));
#else
#error this os has not been supported.
#endif
		}

		Path operator/(const Path& path) const;

		/*!
		@brief Compare the two paths by comparing their inner string content;
		*/
		bool operator==(const Path& path) const;
		/*!
		@brief Compare the two paths by comparing their inner string content;
		*/
		bool operator!=(const Path& path) const;

		bool IsEquivalent(const Path& path) const;

	private:
		String m_Content;
	};

	struct AbsolutePathAdditionError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("Absolute path can not be added to another path.");
		static COMMON_API bool Judge(const Path& path);
	};

	struct PathExistError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("Path has been existed.");
		static COMMON_API bool Judge(const Path& path);
	};

	struct PathNotExistError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("Path does not exist.");
		static COMMON_API bool Judge(const Path& path);
	};

	struct PathNotDirectoryError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("Path is not a directory.");
		static COMMON_API bool Judge(const Path& path);
	};

	struct PathNotFileError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("Path is not a file.");
		static COMMON_API bool Judge(const Path& path);
	};

	COMMON_API Path GetCurrentDirectoryPath();

	COMMON_API void SetCurrentDirectoryPath(const Path& path);

	COMMON_API Path GetModuleDirectoryPath();

	COMMON_API void CreateFile(const Path& path);

	COMMON_API void DeleteFile(const Path& path);
	/*!
	@}
	*/
}