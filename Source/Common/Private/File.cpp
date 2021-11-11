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
	Path apath = GetAbsolutePath();
#ifdef SGE_WINDOWS
	WIN32_FIND_DATA find_file_data;
	HANDLE handle = FindFirstFile(SGE_STR_TO_TSTR(apath.GetString()).GetData(), &find_file_data);
	bool re = false;
	if (handle != INVALID_HANDLE_VALUE)
	{
		SGE_CHECK(FindCloseFailError, FindClose(handle));
		re = true;
	}
	return re;
#elif defined(SGE_POSIX)
	return access(SGE_STR_TO_TSTR(apath.GetString()).GetData(), F_OK) == 0;
#else
#error this os has not been supported.
#endif
}

SpaceGameEngine::Path SpaceGameEngine::Path::GetAbsolutePath() const
{
	if (IsRelative())
	{
#ifdef SGE_WINDOWS
		const SizeType buf_size = 4096;
		TChar out_buffer[buf_size] = SGE_TSTR("");
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

COMMON_API SpaceGameEngine::String SpaceGameEngine::NormalizePathString(const String& path)
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
	TChar out_buffer[buf_size] = SGE_TSTR("");
#ifdef SGE_WINDOWS
	SGE_CHECK(GetCurrentDirectoryFailError, GetCurrentDirectory(buf_size, out_buffer), buf_size);
#elif defined(SGE_POSIX)
	SGE_CHECK(GetCWDFailError, getcwd(out_buffer, buf_size));
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
	SGE_CHECK(ChDirFailError, chdir(SGE_STR_TO_TSTR(apath.GetString()).GetData()));
#else
#error this os has not been supported.
#endif
}

SpaceGameEngine::Path SpaceGameEngine::GetModuleDirectoryPath()
{
	const SizeType buf_size = 4096;
	TChar out_buffer[buf_size] = SGE_TSTR("");
#ifdef SGE_WINDOWS
	SGE_CHECK(GetModuleFileNameFailError, GetModuleFileName(NULL, out_buffer, buf_size), buf_size);
#elif defined(SGE_LINUX)
	SizeType re_size = readlink("/proc/self/exe", out_buffer, buf_size);
	SGE_CHECK(ReadLinkFailError, re_size);
	out_buffer[re_size] = SGE_TSTR('\0');
#elif defined(SGE_MACOS)
	uint32_t buf_size2 = buf_size;
	TChar out_buffer2[buf_size] = SGE_TSTR("");
	SGE_CHECK(NSGetExecutablePathFailError, _NSGetExecutablePath(out_buffer2, &buf_size2));
	out_buffer2[buf_size2] = SGE_TSTR('\0');
	SGE_CHECK(RealPathFailError, realpath(out_buffer2, out_buffer));
#else
#error this os has not been supported.
#endif
	return Path(SGE_TSTR_TO_STR(out_buffer)).GetParentPath();
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

bool SpaceGameEngine::FindCloseFailError::Judge(BOOL re)
{
	return re == 0;
}

bool SpaceGameEngine::SetCurrentDirectoryFailError::Judge(BOOL re)
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

bool SpaceGameEngine::PathNotExistError::Judge(const Path& path)
{
	return !path.IsExist();
}
