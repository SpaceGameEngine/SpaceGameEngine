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
#include "File.h"
#include "Container/Stack.hpp"

#ifdef SGE_POSIX
#include <fcntl.h>
#include <filesystem>
#include <unistd.h>
#endif

#ifdef SGE_MACOS
#include <mach-o/dyld.h>
#include <stdlib.h>
#endif

SpaceGameEngine::Path::Path()
	: m_Content(SGE_STR("."))
{
}

SpaceGameEngine::Path::Path(const String& path)
	: m_Content(NormalizePathString(path))
{
	if (IsAbsolute())
		m_Content = NormalizeAbsolutePathString(m_Content);
}

SpaceGameEngine::String SpaceGameEngine::Path::GetSystemPathString() const
{
#ifdef SGE_WINDOWS
	String re;
	for (auto iter = m_Content.GetConstBegin(); iter != m_Content.GetConstEnd(); ++iter)
	{
		if (*iter == SGE_STR('/'))
			re += SGE_STR('\\');
		else
			re += *iter;
	}
	return re;
#elif defined(SGE_POSIX)
	return m_Content;
#else
#error this os has not been supported.
#endif
}

SpaceGameEngine::String SpaceGameEngine::Path::GetString() const
{
	return m_Content;
}

SpaceGameEngine::String SpaceGameEngine::Path::GetFileName() const
{
	Path apath = GetAbsolutePath();
	if (apath.IsRoot())
		return String();
	String strbuf = apath.GetString();
	return String(strbuf.ReverseFind(SGE_STR("/"), strbuf.GetConstBegin(), strbuf.GetConstEnd()) + 1, strbuf.GetConstEnd());
}

bool SpaceGameEngine::Path::IsAbsolute() const
{
#ifdef SGE_WINDOWS
	if (m_Content.GetSize() < 2)
		return false;
	if (m_Content[1] != SGE_STR(':'))
		return false;
#elif defined(SGE_POSIX)
	if (m_Content.GetSize() < 1)
		return false;
	if (m_Content[0] != SGE_STR('/'))
		return false;
#else
#error this os has not been supported.
#endif
	return true;
}

bool SpaceGameEngine::Path::IsRelative() const
{
	return !IsAbsolute();
}

bool SpaceGameEngine::Path::IsRoot() const
{
#ifdef SGE_WINDOWS
	if (m_Content.GetSize() != 2)
		return false;
	if (m_Content[1] != SGE_STR(':'))
		return false;
	return true;
#elif defined(SGE_POSIX)
	return m_Content == SGE_STR("/");
#else
#error this os has not been supported.
#endif
}

bool SpaceGameEngine::Path::IsExist() const
{
	String astr = GetAbsolutePath().GetString();
#ifdef SGE_WINDOWS
	WIN32_FIND_DATA find_file_data;
	HANDLE handle = FindFirstFile(SGE_STR_TO_TSTR(astr).GetData(), &find_file_data);
	bool re = false;
	if (handle != INVALID_HANDLE_VALUE)
	{
		SGE_CHECK(FindCloseFailError, FindClose(handle));
		re = true;
	}
	return re;
#elif defined(SGE_POSIX)
	return access((const char*)SGE_STR_TO_TSTR(astr).GetData(), F_OK) == 0;
#else
#error this os has not been supported.
#endif
}

SpaceGameEngine::PathType SpaceGameEngine::Path::GetPathType() const
{
	String astr = GetAbsolutePath().GetString();
	PathType re = PathType::NotExist;
#ifdef SGE_WINDOWS
	WIN32_FIND_DATA find_file_data;
	HANDLE handle = FindFirstFile(SGE_STR_TO_TSTR(astr).GetData(), &find_file_data);

	if (handle != INVALID_HANDLE_VALUE)
	{
		SGE_CHECK(FindCloseFailError, FindClose(handle));
		if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
			re = PathType::Link;
		else if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			re = PathType::Directory;
		else if ((find_file_data.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) || (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_NORMAL))
			re = PathType::File;
		else
			re = PathType::Unknown;
	}
#elif defined(SGE_POSIX)
	struct stat buf;
	int stat_re = stat((const char*)SGE_STR_TO_TSTR(astr).GetData(), &buf);
	if (stat_re == 0)
	{
		if (S_ISLNK(buf.st_mode))
			re = PathType::Link;
		else if (S_ISDIR(buf.st_mode))
			re = PathType::Directory;
		else if (S_ISREG(buf.st_mode))
			re = PathType::File;
		else
			re = PathType::Unknown;
	}
#else
#error this os has not been supported.
#endif
	return re;
}

SpaceGameEngine::Path SpaceGameEngine::Path::GetAbsolutePath() const
{
	if (IsRelative())
	{
#ifdef SGE_WINDOWS
		const SizeType buf_size = 4096;
		TChar out_buffer[buf_size];
		memset(out_buffer, 0, sizeof(out_buffer));
		SGE_CHECK(GetFullPathNameFailError, GetFullPathName(SGE_STR_TO_TSTR(m_Content).GetData(), buf_size, out_buffer, NULL), buf_size);
		return Path(SGE_TSTR_TO_STR(out_buffer));
#elif defined(SGE_POSIX)
		return Path(GetCurrentDirectoryPath() / (*this));
#else
#error this os has not been supported.
#endif
	}
	else
		return m_Content;
}

SpaceGameEngine::Path SpaceGameEngine::Path::GetParentPath() const
{
	Path re = GetAbsolutePath();
	if (re.IsRoot())
		return re;
	for (auto iter = re.m_Content.GetConstReverseBegin(); iter != re.m_Content.GetConstReverseEnd(); ++iter)
	{
		if (*iter == SGE_STR('/'))
		{
			re.m_Content.Remove(re.m_Content.GetConstReverseBegin(), ++iter);
			break;
		}
	}
	return re;
}

SpaceGameEngine::Vector<SpaceGameEngine::Pair<SpaceGameEngine::Path, SpaceGameEngine::PathType>> SpaceGameEngine::Path::GetChildPath() const
{
	Path apath = GetAbsolutePath();
	SGE_ASSERT(PathNotDirectoryError, apath);
	String astr = apath.GetString();
	Vector<Pair<Path, PathType>> re;
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
		re.EmplaceBack(Pair<Path, PathType>(Path(astr + SGE_STR("/") + SGE_TSTR_TO_STR(find_file_data.cFileName)), pt));
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
		re.EmplaceBack(Pair<Path, PathType>(Path(astr + SGE_STR("/") + SGE_TSTR_TO_STR((const char8_t*)pchild->d_name)), pt));
	}
	SGE_CHECK(CloseDirFailError, closedir(pdir));
#else
#error this os has not been supported.
#endif
	return re;
}

SpaceGameEngine::Path SpaceGameEngine::Path::operator/(const Path& path) const
{
	SGE_ASSERT(AbsolutePathAdditionError, path);
	return Path(m_Content + SGE_STR('/') + path.m_Content);
}

bool SpaceGameEngine::Path::operator==(const Path& path) const
{
	return m_Content == path.m_Content;
}

bool SpaceGameEngine::Path::operator!=(const Path& path) const
{
	return m_Content != path.m_Content;
}

bool SpaceGameEngine::Path::IsEquivalent(const Path& path) const
{
	if ((!IsExist()) || (!path.IsExist()))
		return false;
	String astr1 = GetAbsolutePath().GetString();
	String astr2 = path.GetAbsolutePath().GetString();
#ifdef SGE_WINDOWS
#include "System/AllowWindowsMacro.h"
	FILE_ID_INFO id1, id2;

	HANDLE handle1 = CreateFile(SGE_STR_TO_TSTR(astr1).GetData(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	SGE_CHECK(CreateFileFailError, handle1);
	SGE_CHECK(GetFileInformationByHandleExFailError, GetFileInformationByHandleEx(handle1, FileIdInfo, &id1, sizeof(FILE_ID_INFO)));
	SGE_CHECK(CloseHandleFailError, CloseHandle(handle1));

	HANDLE handle2 = CreateFile(SGE_STR_TO_TSTR(astr2).GetData(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	SGE_CHECK(CreateFileFailError, handle2);
	SGE_CHECK(GetFileInformationByHandleExFailError, GetFileInformationByHandleEx(handle2, FileIdInfo, &id2, sizeof(FILE_ID_INFO)));
	SGE_CHECK(CloseHandleFailError, CloseHandle(handle2));

	return memcmp(&id1, &id2, sizeof(FILE_ID_INFO)) == 0;
#include "System/HideWindowsMacro.h"
#elif defined(SGE_POSIX)
	struct stat sbuf1, sbuf2;
	SGE_CHECK(StatFailError, stat((const char*)SGE_STR_TO_TSTR(astr1).GetData(), &sbuf1));
	SGE_CHECK(StatFailError, stat((const char*)SGE_STR_TO_TSTR(astr2).GetData(), &sbuf2));
	return (sbuf1.st_dev == sbuf2.st_dev) && (sbuf1.st_ino == sbuf2.st_ino);
#else
#error this os has not been supported.
#endif
}

namespace
{
	using namespace SpaceGameEngine;

	void WalkThroughPath(const Vector<String>& dirs, SizeType idx, Stack<SizeType, DefaultAllocator, Vector>& stack)
	{
		if (idx == dirs.GetSize())
			return;
		if (dirs[idx] == SGE_STR(".."))
		{
			if (stack.GetSize())
				stack.Pop();
		}
		else if (dirs[idx] != SGE_STR("."))
			stack.Push(idx);
		WalkThroughPath(dirs, idx + 1, stack);
	}
}

SpaceGameEngine::String SpaceGameEngine::NormalizePathString(const String& path)
{
	if (path.GetSize() == 0)
		return String(SGE_STR("."));
	String tmp;
	for (auto iter = path.GetConstBegin(); iter != path.GetConstEnd(); ++iter)
	{
		if (*iter == SGE_STR('\\'))
			tmp += SGE_STR('/');
		else
			tmp += *iter;
	}
	String re;
	SizeType slash_cnt = 0;
	for (auto iter = tmp.GetConstBegin(); iter != tmp.GetConstEnd(); ++iter)
	{
		if (*iter == SGE_STR('/'))
			slash_cnt += 1;
		else
		{
			if (slash_cnt)
			{
				re += SGE_STR('/');
				slash_cnt = 0;
			}
			re += *iter;
		}
	}
	if (slash_cnt)
	{
		if (re.GetSize() == 0)
#ifdef SGE_WINDOWS
			return String(SGE_STR("."));
#elif defined(SGE_POSIX)
			return String(SGE_STR("/"));
#else
#error this os has not been supported.
#endif
	}
	return re;
}

SpaceGameEngine::String SpaceGameEngine::NormalizeAbsolutePathString(const String& path)
{
	Vector<String> dirs = Split(path, String(SGE_STR("/")));
	Stack<SizeType, DefaultAllocator, Vector> stack;
	WalkThroughPath(dirs, 1, stack);
#ifdef SGE_WINDOWS
	String re(dirs[0]);
	for (auto iter = stack.GetImplement().GetConstBegin(); iter != stack.GetImplement().GetConstEnd(); ++iter)
	{
		re += SGE_STR('/');
		re += dirs[*iter];
	}
#elif defined(SGE_POSIX)
	String re;
	for (auto iter = stack.GetImplement().GetConstBegin(); iter != stack.GetImplement().GetConstEnd(); ++iter)
	{
		re += SGE_STR('/');
		re += dirs[*iter];
	}
	if (re.GetSize() == 0)
		re += SGE_STR('/');
#else
#error this os has not been supported.
#endif

	return re;
}

SpaceGameEngine::Path SpaceGameEngine::GetCurrentDirectoryPath()
{
	const SizeType buf_size = 4096;
	TChar out_buffer[buf_size];
	memset(out_buffer, 0, sizeof(out_buffer));
#ifdef SGE_WINDOWS
	SGE_CHECK(GetCurrentDirectoryFailError, GetCurrentDirectory(buf_size, out_buffer), buf_size);
#elif defined(SGE_POSIX)
	SGE_CHECK(GetCWDFailError, getcwd((char*)out_buffer, buf_size));
#else
#error this os has not been supported.
#endif
	return Path(SGE_TSTR_TO_STR(out_buffer));
}

void SpaceGameEngine::SetCurrentDirectoryPath(const Path& path)
{
	Path apath = path.GetAbsolutePath();
	SGE_ASSERT(PathNotExistError, apath);
#ifdef SGE_WINDOWS
	SGE_CHECK(SetCurrentDirectoryFailError, SetCurrentDirectory(SGE_STR_TO_TSTR(apath.GetString()).GetData()));
#elif defined(SGE_POSIX)
	SGE_CHECK(ChDirFailError, chdir((const char*)SGE_STR_TO_TSTR(apath.GetString()).GetData()));
#else
#error this os has not been supported.
#endif
}

SpaceGameEngine::Path SpaceGameEngine::GetModuleDirectoryPath()
{
	const SizeType buf_size = 4096;
	TChar out_buffer[buf_size];
	memset(out_buffer, 0, sizeof(out_buffer));
#ifdef SGE_WINDOWS
	SGE_CHECK(GetModuleFileNameFailError, GetModuleFileName(NULL, out_buffer, buf_size), buf_size);
#elif defined(SGE_LINUX)
	SizeType re_size = readlink("/proc/self/exe", (char*)out_buffer, buf_size);
	SGE_CHECK(ReadLinkFailError, re_size);
	out_buffer[re_size] = SGE_TSTR('\0');
#elif defined(SGE_MACOS)
	uint32_t buf_size2 = buf_size;
	TChar out_buffer2[buf_size];
	memset(out_buffer2, 0, sizeof(out_buffer2));
	SGE_CHECK(NSGetExecutablePathFailError, _NSGetExecutablePath((char*)out_buffer2, &buf_size2));
	out_buffer2[buf_size2] = SGE_TSTR('\0');
	SGE_CHECK(RealPathFailError, realpath((char*)out_buffer2, (char*)out_buffer));
#else
#error this os has not been supported.
#endif
	return Path(SGE_TSTR_TO_STR(out_buffer)).GetParentPath();
}

void SpaceGameEngine::CreateFile(const Path& path)
{
	SGE_ASSERT(PathExistError, path);
	SGE_ASSERT(PathNotExistError, path.GetParentPath());
	SGE_ASSERT(PathNotDirectoryError, path.GetParentPath());
	String astr = path.GetAbsolutePath().GetString();

#ifdef SGE_WINDOWS
#include "System/AllowWindowsMacro.h"
	HANDLE handle = CreateFile(SGE_STR_TO_TSTR(astr).GetData(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	SGE_CHECK(CreateFileFailError, handle);
	SGE_CHECK(CloseHandleFailError, CloseHandle(handle));
#include "System/HideWindowsMacro.h"
#elif defined(SGE_POSIX)
	int fd = creat((const char*)SGE_STR_TO_TSTR(astr).GetData(), S_IRWXU | S_IRWXG | S_IRWXO);
	SGE_CHECK(CreatFailError, fd);
	SGE_CHECK(CloseFailError, close(fd));
#else
#error this os has not been supported.
#endif
}

void SpaceGameEngine::DeleteFile(const Path& path)
{
	SGE_ASSERT(PathNotExistError, path);
	SGE_ASSERT(PathNotFileError, path);
	String astr = path.GetAbsolutePath().GetString();

#ifdef SGE_WINDOWS
#include "System/AllowWindowsMacro.h"
	SGE_CHECK(DeleteFileFailError, DeleteFile(SGE_STR_TO_TSTR(astr).GetData()));
#include "System/HideWindowsMacro.h"
#elif defined(SGE_POSIX)
	SGE_CHECK(UnlinkFailError, unlink((const char*)SGE_STR_TO_TSTR(astr).GetData()));
#else
#error this os has not been supported.
#endif
}

void SpaceGameEngine::CopyFile(const Path& dst, const Path& src, bool can_overwrite)
{
	SGE_ASSERT(PathNotExistError, src);
	SGE_ASSERT(PathNotFileError, src);
	SGE_ASSERT(PathNotExistError, dst.GetParentPath());
	SGE_ASSERT(PathNotDirectoryError, dst.GetParentPath());
	if (can_overwrite)
	{
		if (dst.IsExist())
			SGE_ASSERT(PathNotFileError, dst);
	}
	else
		SGE_ASSERT(PathExistError, dst);
	String dstr = dst.GetAbsolutePath().GetString();
	String sstr = src.GetAbsolutePath().GetString();
#ifdef SGE_WINDOWS
#include "System/AllowWindowsMacro.h"
	SGE_CHECK(CopyFileFailError, CopyFile(SGE_STR_TO_TSTR(sstr).GetData(), SGE_STR_TO_TSTR(dstr).GetData(), !can_overwrite));
#include "System/HideWindowsMacro.h"
#elif defined(SGE_POSIX)
	//no portable solution for unix platform to copy file, so use STL
	SGE_CHECK(STLCopyFileFailError, std::filesystem::copy_file(std::filesystem::path((const char*)SGE_STR_TO_UTF8(sstr).GetData()), std::filesystem::path((const char*)SGE_STR_TO_UTF8(dstr).GetData()), (can_overwrite ? std::filesystem::copy_options::overwrite_existing : std::filesystem::copy_options::none)));
#else
#error this os has not been supported.
#endif
}

void SpaceGameEngine::MoveFile(const Path& dst, const Path& src, bool can_overwrite)
{
	SGE_ASSERT(PathNotExistError, src);
	SGE_ASSERT(PathNotFileError, src);
	SGE_ASSERT(PathNotExistError, dst.GetParentPath());
	SGE_ASSERT(PathNotDirectoryError, dst.GetParentPath());
	if (can_overwrite)
	{
		if (dst.IsExist())
			SGE_ASSERT(PathNotFileError, dst);
	}
	else
		SGE_ASSERT(PathExistError, dst);
	String dstr = dst.GetAbsolutePath().GetString();
	String sstr = src.GetAbsolutePath().GetString();
#ifdef SGE_WINDOWS
#include "System/AllowWindowsMacro.h"
	SGE_CHECK(MoveFileExFailError, MoveFileEx(SGE_STR_TO_TSTR(sstr).GetData(), SGE_STR_TO_TSTR(dstr).GetData(), MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH | (can_overwrite ? MOVEFILE_REPLACE_EXISTING : 0)));
#include "System/HideWindowsMacro.h"
#elif defined(SGE_POSIX)
	if (!can_overwrite)
		SGE_CHECK(PathExistError, dst);
	SGE_CHECK(RenameFailError, rename((const char*)SGE_STR_TO_TSTR(sstr).GetData(), (const char*)SGE_STR_TO_TSTR(dstr).GetData()));
#else
#error this os has not been supported.
#endif
}

void SpaceGameEngine::CreateDirectory(const Path& path)
{
	SGE_ASSERT(PathExistError, path);
	SGE_ASSERT(PathNotExistError, path.GetParentPath());
	SGE_ASSERT(PathNotDirectoryError, path.GetParentPath());

	String astr = path.GetAbsolutePath().GetString();

#ifdef SGE_WINDOWS
#include "System/AllowWindowsMacro.h"
	SGE_CHECK(CreateDirectoryFailError, CreateDirectory(SGE_STR_TO_TSTR(astr).GetData(), NULL));
#include "System/HideWindowsMacro.h"
#elif defined(SGE_POSIX)
	SGE_CHECK(MkdirFailError, mkdir((const char*)SGE_STR_TO_TSTR(astr).GetData(), S_IRWXU | S_IRWXG | S_IRWXO));
#else
#error this os has not been supported.
#endif
}

void SpaceGameEngine::DeleteDirectory(const Path& path)
{
	SGE_ASSERT(PathNotExistError, path);
	SGE_ASSERT(PathNotDirectoryError, path);
	String astr = path.GetAbsolutePath().GetString();
	auto children_paths = path.GetChildPath();

	if (children_paths.GetSize())
	{
		for (auto iter = children_paths.GetConstBegin(); iter != children_paths.GetConstEnd(); ++iter)
		{
			SGE_CHECK(PathNotFileOrDirectoryError, iter->m_Second);
			if (iter->m_Second == PathType::File)
				DeleteFile(iter->m_First);
			else if (iter->m_Second == PathType::Directory)
				DeleteDirectory(iter->m_First);
		}
	}

#ifdef SGE_WINDOWS
	SGE_CHECK(RemoveDirectoryFailError, RemoveDirectory(SGE_STR_TO_TSTR(astr).GetData()));
#elif defined(SGE_POSIX)
	SGE_CHECK(RmdirFailError, rmdir((const char*)SGE_STR_TO_TSTR(astr).GetData()));
#else
#error this os has not been supported.
#endif
}

void SpaceGameEngine::CopyDirectory(const Path& dst, const Path& src, bool can_overwrite)
{
	SGE_ASSERT(PathNotExistError, src);
	SGE_ASSERT(PathNotDirectoryError, src);
	SGE_ASSERT(PathNotExistError, dst.GetParentPath());
	SGE_ASSERT(PathNotDirectoryError, dst.GetParentPath());
	if (can_overwrite)
	{
		if (dst.IsExist())
			SGE_ASSERT(PathNotDirectoryError, dst);
	}
	else
		SGE_ASSERT(PathExistError, dst);
	String dstr = dst.GetAbsolutePath().GetString();
	String sstr = src.GetAbsolutePath().GetString();

	if (dst.IsExist())
		DeleteDirectory(dst);
	CreateDirectory(dst);
	src.VisitChildPath([&](const String& file_name, PathType ptype) {
		SGE_CHECK(PathNotFileOrDirectoryError, ptype);
		if (ptype == PathType::File)
			CopyFile(dst / Path(file_name), src / Path(file_name), can_overwrite);
		else if (ptype == PathType::Directory)
			CopyDirectory(dst / Path(file_name), src / Path(file_name), can_overwrite);
	});
}

void SpaceGameEngine::MoveDirectory(const Path& dst, const Path& src, bool can_overwrite)
{
	SGE_ASSERT(PathNotExistError, src);
	SGE_ASSERT(PathNotDirectoryError, src);
	SGE_ASSERT(PathNotExistError, dst.GetParentPath());
	SGE_ASSERT(PathNotDirectoryError, dst.GetParentPath());
	if (can_overwrite)
	{
		if (dst.IsExist())
			SGE_ASSERT(PathNotDirectoryError, dst);
	}
	else
		SGE_ASSERT(PathExistError, dst);
	String dstr = dst.GetAbsolutePath().GetString();
	String sstr = src.GetAbsolutePath().GetString();

	if (dst.IsExist())
		DeleteDirectory(dst);
	CreateDirectory(dst);
	src.VisitChildPath([&](const String& file_name, PathType ptype) {
		SGE_CHECK(PathNotFileOrDirectoryError, ptype);
		if (ptype == PathType::File)
			MoveFile(dst / Path(file_name), src / Path(file_name), can_overwrite);
		else if (ptype == PathType::Directory)
			MoveDirectory(dst / Path(file_name), src / Path(file_name), can_overwrite);
	});
	DeleteDirectory(src);
}

#ifdef SGE_WINDOWS
bool SpaceGameEngine::GetFullPathNameFailError::Judge(DWORD re, SizeType buf_size)
{
	return re == 0 || re >= buf_size;
}

bool SpaceGameEngine::GetCurrentDirectoryFailError::Judge(DWORD re, SizeType buf_size)
{
	return re == 0 || re >= buf_size;
}

bool SpaceGameEngine::GetModuleFileNameFailError::Judge(DWORD re, SizeType buf_size)
{
	return re == 0 || re >= buf_size;
}

bool SpaceGameEngine::FindFirstFileFailError::Judge(HANDLE handle)
{
	return handle == INVALID_HANDLE_VALUE;
}

bool SpaceGameEngine::FindNextFileFailError::Judge(DWORD last_error)
{
	return last_error != ERROR_NO_MORE_FILES;
}

bool SpaceGameEngine::FindCloseFailError::Judge(BOOL re)
{
	return re == 0;
}

bool SpaceGameEngine::SetCurrentDirectoryFailError::Judge(BOOL re)
{
	return re == 0;
}

bool SpaceGameEngine::CreateFileFailError::Judge(HANDLE handle)
{
	return handle == INVALID_HANDLE_VALUE;
}

bool SpaceGameEngine::CloseHandleFailError::Judge(BOOL re)
{
	return re == 0;
}

bool SpaceGameEngine::GetFileInformationByHandleExFailError::Judge(BOOL re)
{
	return re == 0;
}

bool SpaceGameEngine::DeleteFileFailError::Judge(BOOL re)
{
	return re == 0;
}

bool SpaceGameEngine::CopyFileFailError::Judge(BOOL re)
{
	return re == 0;
}

bool SpaceGameEngine::MoveFileExFailError::Judge(BOOL re)
{
	return re == 0;
}

bool SpaceGameEngine::CreateDirectoryFailError::Judge(BOOL re)
{
	return re == 0;
}

bool SpaceGameEngine::RemoveDirectoryFailError::Judge(BOOL re)
{
	return re == 0;
}

bool SpaceGameEngine::FlushFileBuffersFailError::Judge(BOOL re)
{
	return re == 0;
}

bool SpaceGameEngine::ReadFileFailError::Judge(BOOL re)
{
	return re == 0 && GetLastError() != ERROR_IO_PENDING;
}

bool SpaceGameEngine::WriteFileFailError::Judge(BOOL re)
{
	return re == 0 && GetLastError() != ERROR_IO_PENDING;
}

bool SpaceGameEngine::SetFilePointerExFailError::Judge(BOOL re)
{
	return re == 0;
}

bool SpaceGameEngine::GetFileSizeExFailError::Judge(BOOL re)
{
	return re == 0;
}

bool SpaceGameEngine::SetEndOfFileFailError::Judge(BOOL re)
{
	return re == 0;
}
#elif defined(SGE_POSIX)
bool SpaceGameEngine::GetCWDFailError::Judge(char* re)
{
	return re == NULL;
}

bool SpaceGameEngine::ChDirFailError::Judge(int re)
{
	return re == -1;
}

bool SpaceGameEngine::StatFailError::Judge(int re)
{
	return re == -1;
}

bool SpaceGameEngine::OpenDirFailError::Judge(DIR* re)
{
	return re == NULL;
}

bool SpaceGameEngine::CloseDirFailError::Judge(int re)
{
	return re == -1;
}

bool SpaceGameEngine::CreatFailError::Judge(int re)
{
	return re == -1;
}

bool SpaceGameEngine::OpenFailError::Judge(int re)
{
	return re == -1;
}

bool SpaceGameEngine::CloseFailError::Judge(int re)
{
	return re == -1;
}

bool SpaceGameEngine::UnlinkFailError::Judge(int re)
{
	return re == -1;
}

bool SpaceGameEngine::STLCopyFileFailError::Judge(bool re)
{
	return re == false;
}

bool SpaceGameEngine::RenameFailError::Judge(int re)
{
	return re == -1;
}

bool SpaceGameEngine::MkdirFailError::Judge(int re)
{
	return re == -1;
}

bool SpaceGameEngine::RmdirFailError::Judge(int re)
{
	return re == -1;
}

bool SpaceGameEngine::FsyncFailError::Judge(int re)
{
	return re == -1;
}

bool SpaceGameEngine::ReadFailError::Judge(int re)
{
	return re == -1;
}

bool SpaceGameEngine::WriteFailError::Judge(int re)
{
	return re == -1;
}

bool SpaceGameEngine::LSeekFailError::Judge(int re)
{
	return re == -1;
}

bool SpaceGameEngine::FStatFailError::Judge(int re)
{
	return re == -1;
}

bool SpaceGameEngine::FTruncateFailError::Judge(int re)
{
	return re == -1;
}
#endif

#ifdef SGE_LINUX
bool SpaceGameEngine::ReadLinkFailError::Judge(ssize_t re)
{
	return re == -1;
}
#elif defined(SGE_MACOS)
bool SpaceGameEngine::NSGetExecutablePathFailError::Judge(int re)
{
	return re != 0;
}

bool SpaceGameEngine::RealPathFailError::Judge(char* re)
{
	return re == NULL;
}
#endif

bool SpaceGameEngine::AbsolutePathAdditionError::Judge(const Path& path)
{
	return path.IsAbsolute();
}

bool SpaceGameEngine::PathExistError::Judge(const Path& path)
{
	return path.IsExist();
}

bool SpaceGameEngine::PathNotExistError::Judge(const Path& path)
{
	return !path.IsExist();
}

bool SpaceGameEngine::PathNotDirectoryError::Judge(const Path& path)
{
	return path.GetPathType() != PathType::Directory;
}

bool SpaceGameEngine::PathNotFileError::Judge(const Path& path)
{
	return path.GetPathType() != PathType::File;
}

bool SpaceGameEngine::PathNotFileOrDirectoryError::Judge(const Path& path)
{
	auto ptype = path.GetPathType();
	return ptype != PathType::File && ptype != PathType::Directory;
}

bool SpaceGameEngine::PathNotFileOrDirectoryError::Judge(PathType ptype)
{
	return ptype != PathType::File && ptype != PathType::Directory;
}

FileIOMode SpaceGameEngine::operator|(const FileIOMode& m1, const FileIOMode& m2)
{
	return FileIOMode((UInt8)m1 | (UInt8)m2);
}

FileIOMode SpaceGameEngine::operator&(const FileIOMode& m1, const FileIOMode& m2)
{
	return FileIOMode((UInt8)m1 & (UInt8)m2);
}

bool SpaceGameEngine::InvalidFileIOModeError::Judge(FileIOMode mode)
{
	return (UInt8)mode > 7;
}

bool SpaceGameEngine::InvalidFilePositionOriginError::Judge(FilePositionOrigin origin)
{
	return (UInt8)origin > 2;
}

SpaceGameEngine::BinaryFile::BinaryFile()
#ifdef SGE_WINDOWS
	: m_Handle(NULL), m_Mode(FileIOMode::Unknown), m_IsReadFinished(true)
#elif defined(SGE_POSIX)
	: m_Handle(-1), m_Mode(FileIOMode::Unknown), m_IsReadFinished(true)
#else
#error this os has not been supported.
#endif
{
}

SpaceGameEngine::BinaryFile::BinaryFile(const Path& path, FileIOMode mode)
	: m_Mode(mode), m_IsReadFinished((UInt8)(mode & FileIOMode::Read) ? false : true)
{
	SGE_ASSERT(InvalidFileIOModeError, mode);
	SGE_ASSERT(FileIOModeUnknownError, mode);
	if (mode == FileIOMode::Read)
		SGE_ASSERT(PathNotExistError, path);
	if (path.IsExist())
		SGE_ASSERT(PathNotFileError, path);
	String astr = path.GetAbsolutePath().GetString();
#ifdef SGE_WINDOWS
#include "System/AllowWindowsMacro.h"
	m_Handle = CreateFile(SGE_STR_TO_TSTR(astr).GetData(), ((UInt8)(mode & FileIOMode::Read) ? GENERIC_READ : 0) | ((UInt8)(mode & FileIOMode::Write) ? GENERIC_WRITE : 0), FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, ((mode == FileIOMode::Read) ? OPEN_EXISTING : OPEN_ALWAYS), FILE_ATTRIBUTE_NORMAL, NULL);
	SGE_CHECK(CreateFileFailError, m_Handle);
#include "System/HideWindowsMacro.h"
#elif defined(SGE_POSIX)
	int oflag = 0;
	if ((UInt8)(mode & FileIOMode::Read))
	{
		if ((UInt8)(mode & FileIOMode::Write))
			oflag = O_RDWR;
		else
			oflag = O_RDONLY;
	}
	else
		oflag = O_WRONLY;
	if (mode != FileIOMode::Read)
		oflag |= O_CREAT;
	m_Handle = open((const char*)SGE_STR_TO_TSTR(astr).GetData(), oflag, S_IRWXU | S_IRWXG | S_IRWXO);
	SGE_CHECK(OpenFailError, m_Handle);
#else
#error this os has not been supported.
#endif
	if ((mode & FileIOMode::Append) == FileIOMode::Append)
		MoveFilePosition(FilePositionOrigin::End, 0);
}

SpaceGameEngine::BinaryFile::~BinaryFile()
{
#ifdef SGE_WINDOWS
	if (m_Handle)
	{
		//if ((UInt8)(m_Mode & FileIOMode::Write))
		//	SGE_CHECK(FlushFileBuffersFailError, FlushFileBuffers(m_Handle));
		SGE_CHECK(CloseHandleFailError, CloseHandle(m_Handle));
	}
#elif defined(SGE_POSIX)
	if (m_Handle > -1)
	{
		//if ((UInt8)(m_Mode & FileIOMode::Write))
		//	SGE_CHECK(FsyncFailError, fsync(m_Handle));
		SGE_CHECK(CloseFailError, close(m_Handle));
	}
#else
#error this os has not been supported.
#endif
}

void SpaceGameEngine::BinaryFile::Open(const Path& path, FileIOMode mode)
{
	SGE_ASSERT(FileHandleOccupiedError, m_Handle);
	SGE_ASSERT(InvalidFileIOModeError, mode);
	SGE_ASSERT(FileIOModeUnknownError, mode);
	if (mode == FileIOMode::Read)
		SGE_ASSERT(PathNotExistError, path);
	if (path.IsExist())
		SGE_ASSERT(PathNotFileError, path);
	String astr = path.GetAbsolutePath().GetString();
	m_Mode = mode;
	m_IsReadFinished = ((UInt8)(mode & FileIOMode::Read) ? false : true);
#ifdef SGE_WINDOWS
#include "System/AllowWindowsMacro.h"
	m_Handle = CreateFile(SGE_STR_TO_TSTR(astr).GetData(), ((UInt8)(mode & FileIOMode::Read) ? GENERIC_READ : 0) | ((UInt8)(mode & FileIOMode::Write) ? GENERIC_WRITE : 0), FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, ((mode == FileIOMode::Read) ? OPEN_EXISTING : OPEN_ALWAYS), FILE_ATTRIBUTE_NORMAL, NULL);
	SGE_CHECK(CreateFileFailError, m_Handle);
#include "System/HideWindowsMacro.h"
#elif defined(SGE_POSIX)
	int oflag = 0;
	if ((UInt8)(mode & FileIOMode::Read))
	{
		if ((UInt8)(mode & FileIOMode::Write))
			oflag = O_RDWR;
		else
			oflag = O_RDONLY;
	}
	else
		oflag = O_WRONLY;
	if (mode != FileIOMode::Read)
		oflag |= O_CREAT;
	m_Handle = open((const char*)SGE_STR_TO_TSTR(astr).GetData(), oflag, S_IRWXU | S_IRWXG | S_IRWXO);
	SGE_CHECK(OpenFailError, m_Handle);
#else
#error this os has not been supported.
#endif
	if ((mode & FileIOMode::Append) == FileIOMode::Append)
		MoveFilePosition(FilePositionOrigin::End, 0);
}

void SpaceGameEngine::BinaryFile::Close()
{
	SGE_ASSERT(FileHandleReleasedError, m_Handle);
#ifdef SGE_WINDOWS
	//if ((UInt8)(m_Mode & FileIOMode::Write))
	//	SGE_CHECK(FlushFileBuffersFailError, FlushFileBuffers(m_Handle));
	SGE_CHECK(CloseHandleFailError, CloseHandle(m_Handle));
	m_Handle = NULL;
#elif defined(SGE_POSIX)
	//if ((UInt8)(m_Mode & FileIOMode::Write))
	//	SGE_CHECK(FsyncFailError, fsync(m_Handle));
	SGE_CHECK(CloseFailError, close(m_Handle));
	m_Handle = -1;
#else
#error this os has not been supported.
#endif
}

void SpaceGameEngine::BinaryFile::Flush()
{
	SGE_ASSERT(FileHandleReleasedError, m_Handle);
	SGE_ASSERT(FileIOModeNotWriteError, m_Mode);
#ifdef SGE_WINDOWS
	SGE_CHECK(FlushFileBuffersFailError, FlushFileBuffers(m_Handle));
#elif defined(SGE_POSIX)
	SGE_CHECK(FsyncFailError, fsync(m_Handle));
#else
#error this os has not been supported.
#endif
}

SizeType SpaceGameEngine::BinaryFile::Read(void* pdst, SizeType size)
{
	SGE_ASSERT(FileHandleReleasedError, m_Handle);
	SGE_ASSERT(FileIOModeNotReadError, m_Mode);
	SGE_ASSERT(NullPointerError, pdst);
	SGE_ASSERT(InvalidValueError, size, 1, SGE_MAX_MEMORY_SIZE);
#ifdef SGE_WINDOWS
	DWORD buf = 0;
	SGE_CHECK(ReadFileFailError, ReadFile(m_Handle, pdst, size, &buf, NULL));
	if (buf != size)
		m_IsReadFinished = true;
	else
		m_IsReadFinished = false;
	return buf;
#elif defined(SGE_POSIX)
	ssize_t re = read(m_Handle, pdst, size);
	SGE_CHECK(ReadFailError, re);
	if (re != size)
		m_IsReadFinished = true;
	else
		m_IsReadFinished = false;
	return re;
#else
#error this os has not been supported.
#endif
}

SizeType SpaceGameEngine::BinaryFile::Write(const void* psrc, SizeType size)
{
	SGE_ASSERT(FileHandleReleasedError, m_Handle);
	SGE_ASSERT(FileIOModeNotWriteError, m_Mode);
	SGE_ASSERT(NullPointerError, psrc);
	SGE_ASSERT(InvalidValueError, size, 1, SGE_MAX_MEMORY_SIZE);
#ifdef SGE_WINDOWS
	DWORD buf = 0;
	SGE_CHECK(WriteFileFailError, WriteFile(m_Handle, psrc, size, &buf, NULL));
	return buf;
#elif defined(SGE_POSIX)
	ssize_t re = write(m_Handle, psrc, size);
	SGE_CHECK(WriteFailError, re);
	return re;
#else
#error this os has not been supported.
#endif
}

namespace
{
	using namespace SpaceGameEngine;
#ifdef SGE_WINDOWS
	DWORD GetSystemFilePositionOrigin(FilePositionOrigin origin)
	{
		if (origin == FilePositionOrigin::Begin)
			return FILE_BEGIN;
		else if (origin == FilePositionOrigin::Current)
			return FILE_CURRENT;
		else if (origin == FilePositionOrigin::End)
			return FILE_END;
	}
#elif defined(SGE_POSIX)
	int GetSystemFilePositionOrigin(FilePositionOrigin origin)
	{
		if (origin == FilePositionOrigin::Begin)
			return SEEK_SET;
		else if (origin == FilePositionOrigin::Current)
			return SEEK_CUR;
		else if (origin == FilePositionOrigin::End)
			return SEEK_END;
	}
#endif
}

Int64 SpaceGameEngine::BinaryFile::MoveFilePosition(FilePositionOrigin origin, Int64 offset)
{
	SGE_ASSERT(FileHandleReleasedError, m_Handle);
	SGE_ASSERT(InvalidFilePositionOriginError, origin);
	if (origin == FilePositionOrigin::Begin)
		SGE_ASSERT(InvalidValueError, offset, 0, INT64_MAX);
	if (origin == FilePositionOrigin::End)
		SGE_ASSERT(InvalidValueError, offset, -INT64_MAX, 0);
#ifdef SGE_WINDOWS
	LARGE_INTEGER buf;
	LARGE_INTEGER input;
	input.QuadPart = offset;
	SGE_CHECK(SetFilePointerExFailError, SetFilePointerEx(m_Handle, input, &buf, GetSystemFilePositionOrigin(origin)));
	return buf.QuadPart;
#elif defined(SGE_POSIX)
	off_t re = lseek(m_Handle, offset, GetSystemFilePositionOrigin(origin));
	SGE_CHECK(LSeekFailError, re);
	return re;
#else
#error this os has not been supported.
#endif
}

SizeType SpaceGameEngine::BinaryFile::GetFileSize() const
{
	SGE_ASSERT(FileIOModeNotReadError, m_Mode);
#ifdef SGE_WINDOWS
	LARGE_INTEGER re;
	SGE_CHECK(GetFileSizeExFailError, GetFileSizeEx(m_Handle, &re));
	return re.QuadPart;
#elif defined(SGE_POSIX)
	struct stat buf;
	SGE_CHECK(FStatFailError, fstat(m_Handle, &buf));
	return buf.st_size;
#else
#error this os has not been supported.
#endif
}

void SpaceGameEngine::BinaryFile::SetFileSize(SizeType size)
{
	SGE_ASSERT(FileIOModeNotWriteError, m_Mode);
	SGE_ASSERT(InvalidValueError, static_cast<Int64>(size), MoveFilePosition(FilePositionOrigin::Current, 0), INT64_MAX);
#ifdef SGE_WINDOWS
	Int64 fp = MoveFilePosition(FilePositionOrigin::Current, 0);
	MoveFilePosition(FilePositionOrigin::Begin, size);
	SGE_CHECK(SetEndOfFileFailError, SetEndOfFile(m_Handle));
	MoveFilePosition(FilePositionOrigin::Begin, fp);
#elif defined(SGE_POSIX)
	SGE_CHECK(FTruncateFailError, ftruncate(m_Handle, static_cast<Int64>(size)));
#else
#error this os has not been supported.
#endif
}

FileIOMode SpaceGameEngine::BinaryFile::GetFileIOMode() const
{
	return m_Mode;
}

bool SpaceGameEngine::BinaryFile::IsReadFinished() const
{
	return m_IsReadFinished;
}

SpaceGameEngine::BinaryFile::operator bool() const
{
	return !m_IsReadFinished;
}

bool SpaceGameEngine::FileHandleOccupiedError::Judge(FileHandle handle)
{
#ifdef SGE_WINDOWS
	return handle != NULL;
#elif defined(SGE_POSIX)
	return handle != -1;
#else
#error this os has not been supported.
#endif
}

bool SpaceGameEngine::FileHandleReleasedError::Judge(FileHandle handle)
{
#ifdef SGE_WINDOWS
	return handle == NULL;
#elif defined(SGE_POSIX)
	return handle == -1;
#else
#error this os has not been supported.
#endif
}

bool SpaceGameEngine::FileIOModeUnknownError::Judge(FileIOMode mode)
{
	return mode == FileIOMode::Unknown;
}

bool SpaceGameEngine::FileIOModeNotReadError::Judge(FileIOMode mode)
{
	return (UInt8)(mode & FileIOMode::Read) == 0;
}

bool SpaceGameEngine::FileIOModeNotWriteError::Judge(FileIOMode mode)
{
	return (UInt8)(mode & FileIOMode::Write) == 0;
}

SpaceGameEngine::FileCore<Char16, UCS2Trait>::FileCore()
	: BinaryFile(), m_HasBomHeader(false), m_Endian(GetSystemEndian())
{
}

SpaceGameEngine::FileCore<Char16, UCS2Trait>::FileCore(const Path& path, FileIOMode mode)
	: BinaryFile(path, mode)
{
	if ((UInt8)(mode & FileIOMode::Read))
		ReadBomHeader();
	else
	{
		m_HasBomHeader = false;
		m_Endian = GetSystemEndian();
	}
}

void SpaceGameEngine::FileCore<Char16, UCS2Trait>::Open(const Path& path, FileIOMode mode)
{
	BinaryFile::Open(path, mode);
	if ((UInt8)(mode & FileIOMode::Read))
		ReadBomHeader();
	else
	{
		m_HasBomHeader = false;
		m_Endian = GetSystemEndian();
	}
}

bool SpaceGameEngine::FileCore<Char16, UCS2Trait>::IsHasBomHeader() const
{
	return m_HasBomHeader;
}

void SpaceGameEngine::FileCore<Char16, UCS2Trait>::SetHasBomHeader(bool val)
{
	SGE_ASSERT(FileIOModeNotReadError, m_Mode);
	SGE_ASSERT(FileIOModeNotWriteError, m_Mode);

	if (m_HasBomHeader != val)
	{
		if (m_HasBomHeader)
			RemoveBomHeader();
		else
			AddBomHeader();
		m_HasBomHeader = val;
	}
}

Endian SpaceGameEngine::FileCore<Char16, UCS2Trait>::GetEndian() const
{
	return m_Endian;
}

void SpaceGameEngine::FileCore<Char16, UCS2Trait>::SetEndian(Endian endian)
{
	SGE_ASSERT(FileIOModeNotWriteError, m_Mode);
	SGE_ASSERT(InvalidEndianError, endian);

	if (m_Endian != endian)
	{
		ChangeFileEndian(endian, m_Endian);
		m_Endian = endian;
	}
}

Pair<Char16, bool> SpaceGameEngine::FileCore<Char16, UCS2Trait>::ReadChar()
{
	SGE_ASSERT(FileIOModeNotReadError, m_Mode);

	Char16 re = 0;
	SizeType read_size = Read(&re, sizeof(re));
	if (read_size)
	{
		if (m_Endian != GetSystemEndian())
			ChangeEndian(re, GetSystemEndian(), m_Endian);
		return Pair<Char16, bool>(re, true);
	}
	else
		return Pair<Char16, bool>(0, false);
}

bool SpaceGameEngine::FileCore<Char16, UCS2Trait>::WriteChar(Char16 c)
{
	SGE_ASSERT(FileIOModeNotWriteError, m_Mode);

	if (m_Endian != GetSystemEndian())
		ChangeEndian(c, m_Endian, GetSystemEndian());
	if (Write(&c, sizeof(c)) == sizeof(c))
		return true;
	else
		return false;
}

Int64 SpaceGameEngine::FileCore<Char16, UCS2Trait>::Seek(FilePositionOrigin origin, Int64 offset)
{
	SGE_ASSERT(InvalidFilePositionOriginError, origin);
	if (origin == FilePositionOrigin::Begin)
		SGE_ASSERT(InvalidValueError, offset, 0, INT64_MAX);
	if (origin == FilePositionOrigin::End)
		SGE_ASSERT(InvalidValueError, offset, -INT64_MAX, 0);

	if (m_HasBomHeader)
	{
		if (origin == FilePositionOrigin::Begin)
			offset += 2;
		Int64 re = MoveFilePosition(origin, offset);
		if (re < 2)
			return 0;
		else
			return re - 2;
	}
	else
		return MoveFilePosition(origin, offset);
}

void SpaceGameEngine::FileCore<Char16, UCS2Trait>::ReadBomHeader()
{
	SGE_ASSERT(FileIOModeNotReadError, m_Mode);

	UInt8 bom[2] = {0, 0};
	SGE_CHECK(InvalidUCS2FileSizeError, Read(bom, sizeof(bom)));
	if (bom[0] == 0xff && bom[1] == 0xfe)
	{
		m_Endian = Endian::Little;
		m_HasBomHeader = true;
	}
	else if (bom[0] == 0xfe && bom[1] == 0xff)
	{
		m_Endian = Endian::Big;
		m_HasBomHeader = true;
	}
	else
	{
		m_Endian = GetSystemEndian();	 //can not judge, so use system endian
		m_HasBomHeader = false;
		MoveFilePosition(FilePositionOrigin::Begin, 0);
	}
}

void SpaceGameEngine::FileCore<Char16, UCS2Trait>::AddBomHeader()
{
	SGE_ASSERT(FileIOModeNotReadError, m_Mode);
	SGE_ASSERT(FileIOModeNotWriteError, m_Mode);

	UInt8 bom[2] = {0, 0};
	if (m_Endian == Endian::Little)
	{
		bom[0] = 0xff;
		bom[1] = 0xfe;
	}
	else	//big endian
	{
		bom[0] = 0xfe;
		bom[1] = 0xff;
	}
	SizeType size = GetFileSize();
	SGE_CHECK(InvalidUCS2FileSizeError, size);
	Int64 fp = MoveFilePosition(FilePositionOrigin::Current, 0);
	MoveFilePosition(FilePositionOrigin::Begin, 0);
	if (size)
	{
		Char16* pbuf = (Char16*)DefaultAllocator::RawNew(size, alignof(Char16));
		Read(pbuf, size);
		MoveFilePosition(FilePositionOrigin::Begin, 0);
		SetFileSize(sizeof(bom) + size);
		Write(bom, sizeof(bom));
		Write(pbuf, size);
		DefaultAllocator::RawDelete(pbuf);
	}
	else
		Write(bom, sizeof(bom));
	MoveFilePosition(FilePositionOrigin::Begin, fp + 2);
}

void SpaceGameEngine::FileCore<Char16, UCS2Trait>::RemoveBomHeader()
{
	SGE_ASSERT(FileIOModeNotReadError, m_Mode);
	SGE_ASSERT(FileIOModeNotWriteError, m_Mode);

	SizeType size = GetFileSize();
	SGE_CHECK(InvalidUCS2FileSizeError, size);
	Int64 fp = MoveFilePosition(FilePositionOrigin::Current, 0);
	if (size > 2)
	{
		size -= 2;
		MoveFilePosition(FilePositionOrigin::Begin, 2);
		Char16* pbuf = (Char16*)DefaultAllocator::RawNew(size, alignof(Char16));
		Read(pbuf, size);
		MoveFilePosition(FilePositionOrigin::Begin, 0);
		SetFileSize(size);
		Write(pbuf, size);
		DefaultAllocator::RawDelete(pbuf);
	}
	else
	{
		MoveFilePosition(FilePositionOrigin::Begin, 0);
		SetFileSize(0);
	}
	if (fp > 1)
		MoveFilePosition(FilePositionOrigin::Begin, fp - 2);
	else
		MoveFilePosition(FilePositionOrigin::Begin, 0);
}

void SpaceGameEngine::FileCore<Char16, UCS2Trait>::ChangeFileEndian(Endian dst, Endian src)
{
	SGE_ASSERT(FileIOModeNotReadError, m_Mode);
	SGE_ASSERT(FileIOModeNotWriteError, m_Mode);
	SGE_ASSERT(InvalidEndianError, dst);
	SGE_ASSERT(InvalidEndianError, src);

	SizeType size = GetFileSize();
	SGE_CHECK(InvalidUCS2FileSizeError, size);
	if (size)
	{
		Int64 fp = MoveFilePosition(FilePositionOrigin::Current, 0);
		MoveFilePosition(FilePositionOrigin::Begin, 0);
		Char16* pbuf = (Char16*)DefaultAllocator::RawNew(size, alignof(Char16));
		Read(pbuf, size);
		for (SizeType i = 0; i < size / sizeof(Char16); ++i)
		{
			ChangeEndian(pbuf[i], dst, src);
		}
		MoveFilePosition(FilePositionOrigin::Begin, 0);
		SetFileSize(size);
		Write(pbuf, size);
		DefaultAllocator::RawDelete(pbuf);
		MoveFilePosition(FilePositionOrigin::Begin, fp);
	}
}

bool SpaceGameEngine::InvalidUCS2FileSizeError::Judge(SizeType size)
{
	return size % 2 == 1;
}

SpaceGameEngine::FileCore<Char8, UTF8Trait>::FileCore()
	: BinaryFile(), m_HasBomHeader(false)
{
}

SpaceGameEngine::FileCore<Char8, UTF8Trait>::FileCore(const Path& path, FileIOMode mode)
	: BinaryFile(path, mode)
{
	if ((UInt8)(mode & FileIOMode::Read))
		ReadBomHeader();
	else
		m_HasBomHeader = false;
}

void SpaceGameEngine::FileCore<Char8, UTF8Trait>::Open(const Path& path, FileIOMode mode)
{
	BinaryFile::Open(path, mode);
	if ((UInt8)(mode & FileIOMode::Read))
		ReadBomHeader();
	else
		m_HasBomHeader = false;
}

bool SpaceGameEngine::FileCore<Char8, UTF8Trait>::IsHasBomHeader() const
{
	return m_HasBomHeader;
}

void SpaceGameEngine::FileCore<Char8, UTF8Trait>::SetHasBomHeader(bool val)
{
	SGE_ASSERT(FileIOModeNotReadError, m_Mode);
	SGE_ASSERT(FileIOModeNotWriteError, m_Mode);

	if (m_HasBomHeader != val)
	{
		if (m_HasBomHeader)
			RemoveBomHeader();
		else
			AddBomHeader();
		m_HasBomHeader = val;
	}
}

Char8* SpaceGameEngine::FileCore<Char8, UTF8Trait>::ReadChar(Char8* pc)
{
	SGE_ASSERT(FileIOModeNotReadError, m_Mode);
	SGE_ASSERT(NullPointerError, pc);

	SizeType read_size = Read(pc, sizeof(Char8));
	if (read_size)
	{
		SizeType left_size = StringImplement::GetMultipleByteCharSize<Char8, UTF8Trait>(pc) - 1;
		if (left_size)
			Read(pc + 1, left_size * sizeof(Char8));
		using _InvalidMultipleByteCharError = StringImplement::InvalidMultipleByteCharError<Char8, UTF8Trait>;
		SGE_CHECK(_InvalidMultipleByteCharError, pc);
		return pc + 1 + left_size;
	}
	else
		return nullptr;
}

const Char8* SpaceGameEngine::FileCore<Char8, UTF8Trait>::WriteChar(const Char8* pc)
{
	SGE_ASSERT(FileIOModeNotWriteError, m_Mode);
	SGE_ASSERT(NullPointerError, pc);
	using _InvalidMultipleByteCharError = StringImplement::InvalidMultipleByteCharError<Char8, UTF8Trait>;
	SGE_ASSERT(_InvalidMultipleByteCharError, pc);

	SizeType mchar_size = StringImplement::GetMultipleByteCharSize<Char8, UTF8Trait>(pc);
	if (Write(pc, mchar_size * sizeof(Char8)) == mchar_size * sizeof(Char8))
		return pc + mchar_size;
	else
		return nullptr;
}

Int64 SpaceGameEngine::FileCore<Char8, UTF8Trait>::Seek(FilePositionOrigin origin, Int64 offset)
{
	SGE_ASSERT(InvalidFilePositionOriginError, origin);
	if (origin == FilePositionOrigin::Begin)
		SGE_ASSERT(InvalidValueError, offset, 0, INT64_MAX);
	if (origin == FilePositionOrigin::End)
		SGE_ASSERT(InvalidValueError, offset, -INT64_MAX, 0);

	if (m_HasBomHeader)
	{
		if (origin == FilePositionOrigin::Begin)
			offset += 3;
		Int64 re = MoveFilePosition(origin, offset);
		if (re < 3)
			return 0;
		else
			return re - 3;
	}
	else
		return MoveFilePosition(origin, offset);
}

void SpaceGameEngine::FileCore<Char8, UTF8Trait>::ReadBomHeader()
{
	SGE_ASSERT(FileIOModeNotReadError, m_Mode);

	UInt8 bom[3] = {0, 0, 0};
	Read(bom, sizeof(bom));
	if (bom[0] == 0xef && bom[1] == 0xbb && bom[2] == 0xbf)
		m_HasBomHeader = true;
	else
	{
		m_HasBomHeader = false;
		MoveFilePosition(FilePositionOrigin::Begin, 0);
	}
}

void SpaceGameEngine::FileCore<Char8, UTF8Trait>::AddBomHeader()
{
	SGE_ASSERT(FileIOModeNotReadError, m_Mode);
	SGE_ASSERT(FileIOModeNotWriteError, m_Mode);

	UInt8 bom[3] = {0xef, 0xbb, 0xbf};
	SizeType size = GetFileSize();
	Int64 fp = MoveFilePosition(FilePositionOrigin::Current, 0);
	MoveFilePosition(FilePositionOrigin::Begin, 0);
	if (size)
	{
		Char16* pbuf = (Char16*)DefaultAllocator::RawNew(size, alignof(Char16));
		Read(pbuf, size);
		MoveFilePosition(FilePositionOrigin::Begin, 0);
		SetFileSize(sizeof(bom) + size);
		Write(bom, sizeof(bom));
		Write(pbuf, size);
		DefaultAllocator::RawDelete(pbuf);
	}
	else
		Write(bom, sizeof(bom));
	MoveFilePosition(FilePositionOrigin::Begin, fp + 3);
}

void SpaceGameEngine::FileCore<Char8, UTF8Trait>::RemoveBomHeader()
{
	SGE_ASSERT(FileIOModeNotReadError, m_Mode);
	SGE_ASSERT(FileIOModeNotWriteError, m_Mode);

	SizeType size = GetFileSize();
	Int64 fp = MoveFilePosition(FilePositionOrigin::Current, 0);
	if (size > 3)
	{
		size -= 3;
		MoveFilePosition(FilePositionOrigin::Begin, 3);
		Char16* pbuf = (Char16*)DefaultAllocator::RawNew(size, alignof(Char16));
		Read(pbuf, size);
		MoveFilePosition(FilePositionOrigin::Begin, 0);
		SetFileSize(size);
		Write(pbuf, size);
		DefaultAllocator::RawDelete(pbuf);
	}
	else
	{
		MoveFilePosition(FilePositionOrigin::Begin, 0);
		SetFileSize(0);
	}
	if (fp > 2)
		MoveFilePosition(FilePositionOrigin::Begin, fp - 3);
	else
		MoveFilePosition(FilePositionOrigin::Begin, 0);
}

bool SpaceGameEngine::UnknownFileLineBreakError::Judge(FileLineBreak flb)
{
	return flb == FileLineBreak::Unknown;
}

FileLineBreak SpaceGameEngine::GetSystemFileLineBreak()
{
#ifdef SGE_WINDOWS
	return FileLineBreak::CRLF;
//#elif defined(SGE_MACOS)
//	return FileLineBreak::CR;
//#elif defined(SGE_LINUX)
//	return FileLineBreak::LF;
#elif defined(SGE_UNIX)
	return FileLineBreak::LF;
#else
#error this os has not been supported.
#endif
}