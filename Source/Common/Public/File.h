/*
Copyright 2025 creatorlxd

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
#include "Utility/LineBreak.h"
#ifdef SGE_WINDOWS
#include <Windows.h>
#include <tchar.h>
#include "System/HideWindowsMacro.h"
#elif defined(SGE_POSIX)
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#endif

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{

#ifdef SGE_WINDOWS
	struct GetFullPathNameFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("GetFullPathName failed.");
		static COMMON_API bool Judge(DWORD re, SizeType buf_size);
	};

	struct GetCurrentDirectoryFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("GetCurrentDirectory failed.");
		static COMMON_API bool Judge(DWORD re, SizeType buf_size);
	};

	struct GetModuleFileNameFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("GetModuleFileName failed.");
		static COMMON_API bool Judge(DWORD re, SizeType buf_size);
	};

	struct FindFirstFileFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("FindFirstFile failed.");
		static COMMON_API bool Judge(HANDLE handle);
	};

	struct FindNextFileFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("FindNextFile failed.");
		static COMMON_API bool Judge(DWORD last_error);
	};

	struct FindCloseFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("FindClose failed.");
		static COMMON_API bool Judge(BOOL re);
	};

	struct SetCurrentDirectoryFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("SetCurrentDirectory failed.");
		static COMMON_API bool Judge(BOOL re);
	};

	struct CreateFileFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("CreateFile failed.");
		static COMMON_API bool Judge(HANDLE handle);
	};

	struct CloseHandleFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("CloseHandle failed.");
		static COMMON_API bool Judge(BOOL re);
	};

	struct GetFileInformationByHandleExFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("GetFileInformationByHandleEx failed.");
		static COMMON_API bool Judge(BOOL re);
	};

	struct DeleteFileFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("DeleteFile failed.");
		static COMMON_API bool Judge(BOOL re);
	};

	struct CopyFileFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("CopyFile failed.");
		static COMMON_API bool Judge(BOOL re);
	};

	struct MoveFileExFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("MoveFileEx failed.");
		static COMMON_API bool Judge(BOOL re);
	};

	struct CreateDirectoryFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("CreateDirectory failed.");
		static COMMON_API bool Judge(BOOL re);
	};

	struct RemoveDirectoryFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("RemoveDirectory failed.");
		static COMMON_API bool Judge(BOOL re);
	};

	struct FlushFileBuffersFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("FlushFileBuffers failed.");
		static COMMON_API bool Judge(BOOL re);
	};

	struct ReadFileFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("ReadFile failed.");
		static COMMON_API bool Judge(BOOL re);
	};

	struct WriteFileFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("WriteFile failed.");
		static COMMON_API bool Judge(BOOL re);
	};

	struct SetFilePointerExFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("SetFilePointerEx failed.");
		static COMMON_API bool Judge(BOOL re);
	};

	struct GetFileSizeExFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("GetFileSizeEx failed.");
		static COMMON_API bool Judge(BOOL re);
	};

	struct SetEndOfFileFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("SetEndOfFile failed.");
		static COMMON_API bool Judge(BOOL re);
	};

#elif defined(SGE_POSIX)
	struct GetCWDFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("getcwd failed.");
		static COMMON_API bool Judge(char* re);
	};

	struct ChDirFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("chdir failed.");
		static COMMON_API bool Judge(int re);
	};

	struct StatFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("stat failed.");
		static COMMON_API bool Judge(int re);
	};

	struct OpenDirFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("opendir failed.");
		static COMMON_API bool Judge(DIR* re);
	};

	struct CloseDirFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("closedir failed.");
		static COMMON_API bool Judge(int re);
	};

	struct CreatFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("creat failed.");
		static COMMON_API bool Judge(int re);
	};

	struct OpenFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("open failed.");
		static COMMON_API bool Judge(int re);
	};

	struct CloseFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("close failed.");
		static COMMON_API bool Judge(int re);
	};

	struct UnlinkFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("unlink failed.");
		static COMMON_API bool Judge(int re);
	};

	struct STLCopyFileFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("STL copy_file failed.");
		static COMMON_API bool Judge(bool re);
	};

	struct RenameFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("rename failed.");
		static COMMON_API bool Judge(int re);
	};

	struct MkdirFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("mkdir failed.");
		static COMMON_API bool Judge(int re);
	};

	struct RmdirFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("rmdir failed.");
		static COMMON_API bool Judge(int re);
	};

	struct FsyncFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("fsync failed.");
		static COMMON_API bool Judge(int re);
	};

	struct ReadFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("read failed.");
		static COMMON_API bool Judge(int re);
	};

	struct WriteFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("write failed.");
		static COMMON_API bool Judge(int re);
	};

	struct LSeekFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("lseek failed.");
		static COMMON_API bool Judge(int re);
	};

	struct FStatFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("fstat failed.");
		static COMMON_API bool Judge(int re);
	};

	struct FTruncateFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("ftruncate failed.");
		static COMMON_API bool Judge(int re);
	};
#endif

#ifdef SGE_LINUX
	struct ReadLinkFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("readlink failed.");
		static COMMON_API bool Judge(ssize_t re);
	};
#elif defined(SGE_MACOS)
	struct NSGetExecutablePathFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("NSGetExecutablePath failed.");
		static COMMON_API bool Judge(int re);
	};

	struct RealPathFailError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("NSGetExecutablePath failed.");
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
			DIR* pdir = opendir((const char*)SGE_STR_TO_TSTR(astr).GetData());
			SGE_CHECK(OpenDirFailError, pdir);
			dirent* pchild = nullptr;
			while (pchild = readdir(pdir))
			{
				if (strcmp(pchild->d_name, (const char*)SGE_TSTR(".")) == 0 || strcmp(pchild->d_name, (const char*)SGE_TSTR("..")) == 0)
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
				callable(SGE_TSTR_TO_STR((const char8_t*)pchild->d_name), pt);
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
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("Absolute path can not be added to another path.");
		static COMMON_API bool Judge(const Path& path);
	};

	struct PathExistError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("Path has been existed.");
		static COMMON_API bool Judge(const Path& path);
	};

	struct PathNotExistError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("Path does not exist.");
		static COMMON_API bool Judge(const Path& path);
	};

	struct PathNotDirectoryError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("Path is not a directory.");
		static COMMON_API bool Judge(const Path& path);
	};

	struct PathNotFileError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("Path is not a file.");
		static COMMON_API bool Judge(const Path& path);
	};

	struct PathNotFileOrDirectoryError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("Path is not a file or a directory.");
		static COMMON_API bool Judge(const Path& path);
		static COMMON_API bool Judge(PathType ptype);
	};

	COMMON_API Path GetCurrentDirectoryPath();

	COMMON_API void SetCurrentDirectoryPath(const Path& path);

	COMMON_API Path GetModuleDirectoryPath();

	/*!
	@todo use ProjectManager to make this function configable and flexiable(use code generation to make config).
	*/
	COMMON_API Path GetProjectDirectoryPath();

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
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("FileIOMode is invalid.");
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
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("FilePositionOrigin is invalid.");
		static COMMON_API bool Judge(FilePositionOrigin origin);
	};

	struct IncompleteWriteError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("Write is incomplete.");
		static COMMON_API bool Judge(SizeType real, SizeType wish);
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
		void Write(const void* psrc, SizeType size);
		Int64 MoveFilePosition(FilePositionOrigin origin, Int64 offset);
		SizeType GetFileSize() const;
		/*!
		@warning the size must >= current file position
		*/
		void SetFileSize(SizeType size);
		FileIOMode GetFileIOMode() const;

		bool IsReadFinished() const;
		explicit operator bool() const;

		bool IsOpen() const;

	protected:
		FileHandle m_Handle;
		FileIOMode m_Mode;
		bool m_IsReadFinished;
		bool m_IsOpen;
	};

	struct FileHandleOccupiedError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("FileHandle is occupied.");
		static COMMON_API bool Judge(FileHandle handle);
	};

	struct FileHandleReleasedError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("FileHandle is released.");
		static COMMON_API bool Judge(FileHandle handle);
	};

	struct FileIOModeUnknownError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("FileIOMode is unknown.");
		static COMMON_API bool Judge(FileIOMode mode);
	};

	struct FileIOModeNotReadError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("FileIOMode is not Read.");
		static COMMON_API bool Judge(FileIOMode mode);
	};

	struct FileIOModeNotWriteError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("FileIOMode is not Write.");
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

		Pair<Char16, bool> ReadChar();
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
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("The UCS2File's size is invalid.");
		static COMMON_API bool Judge(SizeType size);
	};

	template<>
	class COMMON_API FileCore<Char8, UTF8Trait> : public BinaryFile
	{
	public:
		using CharType = Char8;
		using ValueType = Char8*;
		using ConstValueType = const Char8*;
		using ValueTrait = UTF8Trait;

		FileCore();
		FileCore(const Path& path, FileIOMode mode);

		void Open(const Path& path, FileIOMode mode);

		bool IsHasBomHeader() const;
		void SetHasBomHeader(bool val);

		Char8* ReadChar(Char8* pc);
		const Char8* WriteChar(const Char8* pc);

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

	template<typename T, typename Trait = CharTrait<T>>
	class File : public FileCore<T, Trait>
	{
	public:
		using CharType = typename FileCore<T, Trait>::CharType;
		using ValueType = typename FileCore<T, Trait>::ValueType;
		using ConstValueType = typename FileCore<T, Trait>::ConstValueType;
		using ValueTrait = typename FileCore<T, Trait>::ValueTrait;

		inline File()
			: m_LineBreak(LineBreak::Unknown), FileCore<T, Trait>()
		{
		}

		inline File(const Path& path, FileIOMode mode)
			: FileCore<T, Trait>(path, mode)
		{
			if ((UInt8)(mode & FileIOMode::Read))
				ReadLineBreak();
			else
				m_LineBreak = GetSystemLineBreak();
		}

		inline void Open(const Path& path, FileIOMode mode)
		{
			FileCore<T, Trait>::Open(path, mode);
			if ((UInt8)(mode & FileIOMode::Read))
				ReadLineBreak();
			else
				m_LineBreak = GetSystemLineBreak();
		}

		inline LineBreak GetLineBreak() const
		{
			return m_LineBreak;
		}

		inline void SetLineBreak(LineBreak lb)
		{
			SGE_ASSERT(FileIOModeNotReadError, BinaryFile::m_Mode);
			SGE_ASSERT(FileIOModeNotWriteError, BinaryFile::m_Mode);
			SGE_ASSERT(UnknownLineBreakError, lb);
			if (lb != m_LineBreak)
			{
				Int64 fpos = FileCore<T, Trait>::Seek(FilePositionOrigin::Current, 0);
				FileCore<T, Trait>::Seek(FilePositionOrigin::Begin, 0);
				StringCore<T, Trait> str_buffer;
				Int64 fpos_offset = 0;
				if constexpr (!Trait::IsMultipleByte)
				{
					Pair<T, bool> buffer = FileCore<T, Trait>::ReadChar();
					if (m_LineBreak != LineBreak::CRLF)
					{
						while (buffer.m_Second)
						{
							if (SpaceGameEngine::GetLineBreak<T, Trait>(buffer.m_First, buffer.m_First) != m_LineBreak)
								str_buffer += buffer.m_First;
							else
							{
								str_buffer += GetLineBreakString<T, Trait>(lb);
								if (lb == LineBreak::CRLF && fpos >= FileCore<T, Trait>::Seek(FilePositionOrigin::Current, 0))
									fpos_offset += 1;
							}
							buffer = FileCore<T, Trait>::ReadChar();
						}
					}
					else
					{
						while (buffer.m_Second)
						{
							if (SpaceGameEngine::GetLineBreak<T, Trait>(buffer.m_First, buffer.m_First) == LineBreak::CR)
							{
								auto buffer2 = FileCore<T, Trait>::ReadChar();
								if (buffer2.m_Second && SpaceGameEngine::GetLineBreak<T, Trait>(buffer.m_First, buffer2.m_First) == LineBreak::CRLF)
								{
									str_buffer += GetLineBreakString<T, Trait>(lb);
									if (fpos >= FileCore<T, Trait>::Seek(FilePositionOrigin::Current, 0))
										fpos_offset -= 1;
									buffer = FileCore<T, Trait>::ReadChar();
								}
								else
								{
									str_buffer += buffer.m_First;
									buffer = buffer2;
								}
							}
							else
							{
								str_buffer += buffer.m_First;
								buffer = FileCore<T, Trait>::ReadChar();
							}
						}
					}
				}
				else
				{
					T buffer[Trait::MaxMultipleByteSize];
					if (m_LineBreak != LineBreak::CRLF)
					{
						while ((memset(buffer, 0, sizeof(buffer)), FileCore<T, Trait>::ReadChar(buffer)))
						{
							if (SpaceGameEngine::GetLineBreak<T, Trait>(buffer, buffer) != m_LineBreak)
								str_buffer.Insert(str_buffer.GetConstEnd(), (T*)buffer, (T*)buffer + StringImplement::GetMultipleByteCharSize<T, Trait>(buffer));
							else
							{
								str_buffer += GetLineBreakString<T, Trait>(lb);
								if (lb == LineBreak::CRLF && fpos >= FileCore<T, Trait>::Seek(FilePositionOrigin::Current, 0))
									fpos_offset += 1;
							}
						}
					}
					else
					{
						bool is_read = (memset(buffer, 0, sizeof(buffer)), FileCore<T, Trait>::ReadChar(buffer));
						while (is_read)
						{
							if (SpaceGameEngine::GetLineBreak<T, Trait>(buffer, buffer) == LineBreak::CR)
							{
								T buffer2[Trait::MaxMultipleByteSize];
								if ((is_read = FileCore<T, Trait>::ReadChar(buffer2)) && SpaceGameEngine::GetLineBreak<T, Trait>(buffer, buffer2) == LineBreak::CRLF)
								{
									str_buffer += GetLineBreakString<T, Trait>(lb);
									if (fpos >= FileCore<T, Trait>::Seek(FilePositionOrigin::Current, 0))
										fpos_offset -= 1;
									is_read = (memset(buffer, 0, sizeof(buffer)), FileCore<T, Trait>::ReadChar(buffer));
								}
								else
								{
									str_buffer.Insert(str_buffer.GetConstEnd(), (T*)buffer, (T*)buffer + StringImplement::GetMultipleByteCharSize<T, Trait>(buffer));
									memcpy(buffer, buffer2, sizeof(buffer));
								}
							}
							else
							{
								str_buffer.Insert(str_buffer.GetConstEnd(), (T*)buffer, (T*)buffer + StringImplement::GetMultipleByteCharSize<T, Trait>(buffer));
								is_read = (memset(buffer, 0, sizeof(buffer)), FileCore<T, Trait>::ReadChar(buffer));
							}
						}
					}
				}
				if (str_buffer.GetNormalSize())
				{
					FileCore<T, Trait>::Seek(FilePositionOrigin::Begin, 0);
					BinaryFile::SetFileSize(str_buffer.GetNormalSize() * sizeof(T));
					BinaryFile::Write(str_buffer.GetData(), str_buffer.GetNormalSize() * sizeof(T));
				}
				FileCore<T, Trait>::Seek(FilePositionOrigin::Begin, fpos + fpos_offset);
				m_LineBreak = lb;
			}
		}

		inline StringCore<T, Trait> ReadLine()
		{
			SGE_ASSERT(FileIOModeNotReadError, BinaryFile::m_Mode);
			StringCore<T, Trait> re;
			if constexpr (!Trait::IsMultipleByte)
			{
				Pair<T, bool> buffer = FileCore<T, Trait>::ReadChar();
				if (m_LineBreak != LineBreak::CRLF)
				{
					while (buffer.m_Second && SpaceGameEngine::GetLineBreak<T, Trait>(buffer.m_First, buffer.m_First) != m_LineBreak)
					{
						re += buffer.m_First;
						buffer = FileCore<T, Trait>::ReadChar();
					}
				}
				else
				{
					while (buffer.m_Second)
					{
						if (SpaceGameEngine::GetLineBreak<T, Trait>(buffer.m_First, buffer.m_First) == LineBreak::CR)
						{
							auto buffer2 = FileCore<T, Trait>::ReadChar();
							if (buffer2.m_Second && SpaceGameEngine::GetLineBreak<T, Trait>(buffer.m_First, buffer2.m_First) == LineBreak::CRLF)
								break;
							else
							{
								re += buffer.m_First;
								buffer = buffer2;
							}
						}
						else
						{
							re += buffer.m_First;
							buffer = FileCore<T, Trait>::ReadChar();
						}
					}
				}
			}
			else
			{
				T buffer[Trait::MaxMultipleByteSize];
				if (m_LineBreak != LineBreak::CRLF)
				{
					while ((memset(buffer, 0, sizeof(buffer)), FileCore<T, Trait>::ReadChar(buffer)) && SpaceGameEngine::GetLineBreak<T, Trait>(buffer, buffer) != m_LineBreak)
						re.Insert(re.GetConstEnd(), (T*)buffer, (T*)buffer + StringImplement::GetMultipleByteCharSize<T, Trait>(buffer));
				}
				else
				{
					bool is_read = (memset(buffer, 0, sizeof(buffer)), FileCore<T, Trait>::ReadChar(buffer));
					while (is_read)
					{
						if (SpaceGameEngine::GetLineBreak<T, Trait>(buffer, buffer) == LineBreak::CR)
						{
							T buffer2[Trait::MaxMultipleByteSize];
							if ((is_read = FileCore<T, Trait>::ReadChar(buffer2)) && SpaceGameEngine::GetLineBreak<T, Trait>(buffer, buffer2) == LineBreak::CRLF)
								break;
							else
							{
								re.Insert(re.GetConstEnd(), (T*)buffer, (T*)buffer + StringImplement::GetMultipleByteCharSize<T, Trait>(buffer));
								memcpy(buffer, buffer2, sizeof(buffer));
							}
						}
						else
						{
							re.Insert(re.GetConstEnd(), (T*)buffer, (T*)buffer + StringImplement::GetMultipleByteCharSize<T, Trait>(buffer));
							is_read = (memset(buffer, 0, sizeof(buffer)), FileCore<T, Trait>::ReadChar(buffer));
						}
					}
				}
			}
			return re;
		}

		inline StringCore<T, Trait> ReadWord()
		{
			SGE_ASSERT(FileIOModeNotReadError, BinaryFile::m_Mode);
			StringCore<T, Trait> re;
			if constexpr (!Trait::IsMultipleByte)
			{
				Pair<T, bool> buffer = FileCore<T, Trait>::ReadChar();
				while (buffer.m_Second && IsWordSeparatorCharacter<T, Trait>(buffer.m_First))
					buffer = FileCore<T, Trait>::ReadChar();
				while (buffer.m_Second && !IsWordSeparatorCharacter<T, Trait>(buffer.m_First))
				{
					re += buffer.m_First;
					buffer = FileCore<T, Trait>::ReadChar();
				}
			}
			else
			{
				T buffer[Trait::MaxMultipleByteSize];
				bool is_read = (memset(buffer, 0, sizeof(buffer)), FileCore<T, Trait>::ReadChar(buffer));
				while (is_read && IsWordSeparatorCharacter<T, Trait>(buffer))
					is_read = (memset(buffer, 0, sizeof(buffer)), FileCore<T, Trait>::ReadChar(buffer));
				while (is_read && !IsWordSeparatorCharacter<T, Trait>(buffer))
				{
					re.Insert(re.GetConstEnd(), (T*)buffer, (T*)buffer + StringImplement::GetMultipleByteCharSize<T, Trait>(buffer));
					is_read = (memset(buffer, 0, sizeof(buffer)), FileCore<T, Trait>::ReadChar(buffer));
				}
			}
			return re;
		}

		template<typename U>
		inline File& operator>>(U& val)
		{
			SGE_ASSERT(FileIOModeNotReadError, BinaryFile::m_Mode);
			StringCore<T, Trait> str = ReadWord();
			if (str.GetSize())
				val = StringTo<StringCore<T, Trait>, U>(str);
			return *this;
		}

		template<IsAllocator Allocator>
		inline File& operator>>(StringCore<T, Trait, Allocator>& val)
		{
			SGE_ASSERT(FileIOModeNotReadError, BinaryFile::m_Mode);
			StringCore<T, Trait> str = ReadWord();
			if (str.GetSize())
				val = std::move(str);
			return *this;
		}

		inline void WriteString(const StringCore<T, Trait>& str)
		{
			SGE_ASSERT(FileIOModeNotWriteError, BinaryFile::m_Mode);
			for (auto iter = str.GetConstBegin(); iter != str.GetConstEnd(); ++iter)
				FileCore<T, Trait>::WriteChar(*iter);
		}

		template<typename U>
		inline File& operator<<(const U& val)
		{
			SGE_ASSERT(FileIOModeNotWriteError, BinaryFile::m_Mode);
			WriteString(ToString<StringCore<T, Trait>, U>(val));
			return *this;
		}

		template<IsAllocator Allocator>
		inline File& operator<<(StringCore<T, Trait, Allocator>& val)
		{
			SGE_ASSERT(FileIOModeNotWriteError, BinaryFile::m_Mode);
			WriteString(val);
			return *this;
		}

		inline File& operator<<(const EndLineType& endl)
		{
			SGE_ASSERT(FileIOModeNotWriteError, BinaryFile::m_Mode);
			WriteString(GetLineBreakString<T, Trait>(m_LineBreak));
			return *this;
		}

	private:
		inline void ReadLineBreak()
		{
			SGE_ASSERT(FileIOModeNotReadError, BinaryFile::m_Mode);
			if constexpr (!Trait::IsMultipleByte)
			{
				Pair<T, bool> buffer(0, false);
				do
				{
					buffer = FileCore<T, Trait>::ReadChar();
					if (!buffer.m_Second)
						break;
				} while ((m_LineBreak = SpaceGameEngine::GetLineBreak<T, Trait>(buffer.m_First, buffer.m_First)) == LineBreak::Unknown);
				if (m_LineBreak == LineBreak::CR)	 // CRLF judge
				{
					auto buffer2 = FileCore<T, Trait>::ReadChar();
					if (buffer2.m_Second)
						m_LineBreak = SpaceGameEngine::GetLineBreak<T, Trait>(buffer.m_First, buffer2.m_First);
				}
			}
			else
			{
				T buffer[Trait::MaxMultipleByteSize];
				do
				{
					if (!FileCore<T, Trait>::ReadChar(buffer))
						break;
				} while ((m_LineBreak = SpaceGameEngine::GetLineBreak<T, Trait>(buffer, buffer)) == LineBreak::Unknown);
				if (m_LineBreak == LineBreak::CR)	 // CRLF judge
				{
					T buffer2[Trait::MaxMultipleByteSize];
					if (FileCore<T, Trait>::ReadChar(buffer2))
						m_LineBreak = SpaceGameEngine::GetLineBreak<T, Trait>(buffer, buffer2);
				}
			}

			if (((UInt8)(BinaryFile::m_Mode & FileIOMode::Write)) && m_LineBreak == LineBreak::Unknown)	   // write need file line break
				m_LineBreak = GetSystemLineBreak();														   // can not judge, use system file line break
			FileCore<T, Trait>::m_IsReadFinished = false;
			FileCore<T, Trait>::Seek(FilePositionOrigin::Begin, 0);
		}

	private:
		LineBreak m_LineBreak;
	};

	using UCS2File = File<Char16, UCS2Trait>;
	using UTF8File = File<Char8, UTF8Trait>;

	extern template class COMMON_API_TEMPLATE_DECLARE File<Char16, UCS2Trait>;
	extern template class COMMON_API_TEMPLATE_DECLARE File<Char8, UTF8Trait>;

}

/*!
@}
*/