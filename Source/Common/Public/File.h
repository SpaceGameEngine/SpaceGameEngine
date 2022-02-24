﻿/*
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
#include "Utility/Utility.hpp"
#include "Utility/Endian.h"
#ifdef SGE_WINDOWS
#include <Windows.h>
#include <tchar.h>
#include "System/HideWindowsMacro.h"
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

	struct CopyFileFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("CopyFile failed.");
		static COMMON_API bool Judge(BOOL re);
	};

	struct MoveFileExFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("MoveFileEx failed.");
		static COMMON_API bool Judge(BOOL re);
	};

	struct CreateDirectoryFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("CreateDirectory failed.");
		static COMMON_API bool Judge(BOOL re);
	};

	struct RemoveDirectoryFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("RemoveDirectory failed.");
		static COMMON_API bool Judge(BOOL re);
	};

	struct FlushFileBuffersFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("FlushFileBuffers failed.");
		static COMMON_API bool Judge(BOOL re);
	};

	struct ReadFileFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("ReadFile failed.");
		static COMMON_API bool Judge(BOOL re);
	};

	struct WriteFileFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("WriteFile failed.");
		static COMMON_API bool Judge(BOOL re);
	};

	struct SetFilePointerExFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("SetFilePointerEx failed.");
		static COMMON_API bool Judge(BOOL re);
	};

	struct GetFileSizeExFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("GetFileSizeEx failed.");
		static COMMON_API bool Judge(BOOL re);
	};

	struct SetEndOfFileFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("SetEndOfFile failed.");
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

	struct STLCopyFileFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("STL copy_file failed.");
		static COMMON_API bool Judge(bool re);
	};

	struct RenameFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("rename failed.");
		static COMMON_API bool Judge(int re);
	};

	struct MkdirFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("mkdir failed.");
		static COMMON_API bool Judge(int re);
	};

	struct RmdirFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("rmdir failed.");
		static COMMON_API bool Judge(int re);
	};

	struct FsyncFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("fsync failed.");
		static COMMON_API bool Judge(int re);
	};

	struct ReadFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("read failed.");
		static COMMON_API bool Judge(int re);
	};

	struct WriteFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("write failed.");
		static COMMON_API bool Judge(int re);
	};

	struct LSeekFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("lseek failed.");
		static COMMON_API bool Judge(int re);
	};

	struct FStatFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("fstat failed.");
		static COMMON_API bool Judge(int re);
	};

	struct FTruncateFailError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("ftruncate failed.");
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

	struct PathNotFileOrDirectoryError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("Path is not a file or a directory.");
		static COMMON_API bool Judge(const Path& path);
		static COMMON_API bool Judge(PathType ptype);
	};

	COMMON_API Path GetCurrentDirectoryPath();

	COMMON_API void SetCurrentDirectoryPath(const Path& path);

	COMMON_API Path GetModuleDirectoryPath();

	/*!
	@warning the path's parent directory must be existed.
	*/
	COMMON_API void CreateFile(const Path& path);

	COMMON_API void DeleteFile(const Path& path);

	/*!
	@warning the dst's parent directory must be existed.
	*/
	COMMON_API void CopyFile(const Path& dst, const Path& src, bool can_overwrite = true);

	/*!
	@warning the dst's parent directory must be existed.
	*/
	COMMON_API void MoveFile(const Path& dst, const Path& src, bool can_overwrite = true);

	/*!
	@warning the path's parent directory must be existed.
	*/
	COMMON_API void CreateDirectory(const Path& path);

	COMMON_API void DeleteDirectory(const Path& path);

	/*!
	@warning the dst's parent directory must be existed.
	*/
	COMMON_API void CopyDirectory(const Path& dst, const Path& src, bool can_overwrite = true);

	/*!
	@warning the dst's parent directory must be existed.
	*/
	COMMON_API void MoveDirectory(const Path& dst, const Path& src, bool can_overwrite = true);

#ifdef SGE_WINDOWS
	using FileHandle = HANDLE;
#elif defined(SGE_POSIX)
	using FileHandle = int;
#else
#error this os has not been supported.
#endif

	enum class FileIOMode : UInt8
	{
		Unknown = 0,
		Read = 1,
		Write = 2,
		Append = 6
	};

	COMMON_API FileIOMode operator|(const FileIOMode& m1, const FileIOMode& m2);
	COMMON_API FileIOMode operator&(const FileIOMode& m1, const FileIOMode& m2);

	struct InvalidFileIOModeError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("FileIOMode is invalid.");
		static COMMON_API bool Judge(FileIOMode mode);
	};

	enum class FilePositionOrigin
	{
		Begin = 0,
		Current = 1,
		End = 2
	};

	struct InvalidFilePositionOriginError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("FilePositionOrigin is invalid.");
		static COMMON_API bool Judge(FilePositionOrigin origin);
	};

	class COMMON_API BinaryFile : public UncopyableAndUnmovable
	{
	public:
		BinaryFile();
		BinaryFile(const Path& path, FileIOMode mode);
		~BinaryFile();

		void Open(const Path& path, FileIOMode mode);
		void Close();
		void Flush();
		SizeType Read(void* pdst, SizeType size);
		SizeType Write(const void* psrc, SizeType size);
		Int64 MoveFilePosition(FilePositionOrigin origin, Int64 offset);
		SizeType GetFileSize() const;
		/*!
		@warning the size must >= current file position
		*/
		void SetFileSize(SizeType size);

	protected:
		FileHandle m_Handle;
		FileIOMode m_Mode;
	};

	struct FileHandleOccupiedError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("FileHandle is occupied.");
		static COMMON_API bool Judge(FileHandle handle);
	};

	struct FileHandleReleasedError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("FileHandle is released.");
		static COMMON_API bool Judge(FileHandle handle);
	};

	struct FileIOModeUnknownError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("FileIOMode is unknown.");
		static COMMON_API bool Judge(FileIOMode mode);
	};

	struct FileIOModeNotReadError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("FileIOMode is not Read.");
		static COMMON_API bool Judge(FileIOMode mode);
	};

	struct FileIOModeNotWriteError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("FileIOMode is not Write.");
		static COMMON_API bool Judge(FileIOMode mode);
	};

	template<typename T, typename Trait = CharTrait<T>>
	class FileCore
	{
	};

	template<>
	class COMMON_API FileCore<Char16, UCS2Trait> : public BinaryFile
	{
	public:
		using CharType = Char16;
		using ValueType = Char16;
		using ConstValueType = const Char16;
		using ValueTrait = UCS2Trait;

		FileCore();
		FileCore(const Path& path, FileIOMode mode);

		void Open(const Path& path, FileIOMode mode);

		bool IsHasBomHeader() const;
		void SetHasBomHeader(bool val);

		Endian GetEndian() const;
		void SetEndian(Endian endian);

		Char16 ReadChar();
		void WriteChar(Char16 c);

		/*!
		@note When the file has bom header, the offset is not as same as the
		MoveFilePosition's offset, but the offset starting from the bom header's
		end, when the origin is FilePositionOrigin::Begin.
		@return the offset starting from the bom header's end if the file has
		bom header.
		*/
		Int64 Seek(FilePositionOrigin origin, Int64 offset);

	private:
		void ReadBomHeader();

		void AddBomHeader();
		void RemoveBomHeader();

		void ChangeFileEndian(Endian dst, Endian src);

	private:
		bool m_HasBomHeader;
		Endian m_Endian;
	};

	struct InvalidUCS2FileSizeError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("The UCS2File's size is invalid.");
		static COMMON_API bool Judge(SizeType size);
	};

	template<>
	class COMMON_API FileCore<char, UTF8Trait> : public BinaryFile
	{
	public:
		using CharType = char;
		using ValueType = char*;
		using ConstValueType = const char*;
		using ValueTrait = UTF8Trait;

		FileCore();
		FileCore(const Path& path, FileIOMode mode);

		void Open(const Path& path, FileIOMode mode);

		bool IsHasBomHeader() const;
		void SetHasBomHeader(bool val);

		void ReadChar(char* pc);
		void WriteChar(const char* pc);

		/*!
		@note When the file has bom header, the offset is not as same as the
		MoveFilePosition's offset, but the offset starting from the bom header's
		end, when the origin is FilePositionOrigin::Begin.
		@return the offset starting from the bom header's end if the file has
		bom header.
		*/
		Int64 Seek(FilePositionOrigin origin, Int64 offset);

	private:
		void ReadBomHeader();

		void AddBomHeader();
		void RemoveBomHeader();

	private:
		bool m_HasBomHeader;
	};
	/*!
	@}
	*/
}