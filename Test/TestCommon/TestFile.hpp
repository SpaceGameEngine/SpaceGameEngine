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
#include "gtest/gtest.h"
#include "File.h"
#include "Container/Map.hpp"

using namespace SpaceGameEngine;

TEST(Path, NormalizePathStringTest)
{
	ASSERT_EQ(NormalizePathString(SGE_STR("test\\test1.txt")), SGE_STR("test/test1.txt"));
	ASSERT_EQ(NormalizePathString(SGE_STR("test/test1/")), SGE_STR("test/test1"));
	ASSERT_EQ(NormalizePathString(SGE_STR("test///test1//")), SGE_STR("test/test1"));
	ASSERT_EQ(NormalizePathString(SGE_STR("test///test1/\\")), SGE_STR("test/test1"));
	ASSERT_EQ(NormalizePathString(SGE_STR("test//\\/test1/")), SGE_STR("test/test1"));
#ifdef SGE_WINDOWS
	ASSERT_EQ(NormalizePathString(SGE_STR("/")), SGE_STR("."));
	ASSERT_EQ(NormalizePathString(SGE_STR("///")), SGE_STR("."));
	ASSERT_EQ(NormalizePathString(SGE_STR("///\\//\\")), SGE_STR("."));
#elif defined(SGE_POSIX)
	ASSERT_EQ(NormalizePathString(SGE_STR("/")), SGE_STR("/"));
	ASSERT_EQ(NormalizePathString(SGE_STR("///")), SGE_STR("/"));
	ASSERT_EQ(NormalizePathString(SGE_STR("///\\//\\")), SGE_STR("/"));
#else
#error this os has not been supported.
#endif
	ASSERT_EQ(NormalizePathString(SGE_STR("")), SGE_STR("."));
}

TEST(Path, NormalizeAbsolutePathStringTest)
{
#ifdef SGE_WINDOWS
	ASSERT_EQ(NormalizeAbsolutePathString(SGE_STR("C:")), SGE_STR("C:"));
	ASSERT_EQ(NormalizeAbsolutePathString(SGE_STR("C:/test")), SGE_STR("C:/test"));
	ASSERT_EQ(NormalizeAbsolutePathString(SGE_STR("C:/test/..")), SGE_STR("C:"));
	ASSERT_EQ(NormalizeAbsolutePathString(SGE_STR("C:/test/../..")), SGE_STR("C:"));
	ASSERT_EQ(NormalizeAbsolutePathString(SGE_STR("C:/test/../../..")), SGE_STR("C:"));
	ASSERT_EQ(NormalizeAbsolutePathString(SGE_STR("C:/test/./..")), SGE_STR("C:"));
	ASSERT_EQ(NormalizeAbsolutePathString(SGE_STR("C:/test/./test1/..")), SGE_STR("C:/test"));
	ASSERT_EQ(NormalizeAbsolutePathString(SGE_STR("C:/test/test1/..")), SGE_STR("C:/test"));
	ASSERT_EQ(NormalizeAbsolutePathString(SGE_STR("C:/test/test1/../..")), SGE_STR("C:"));
	ASSERT_EQ(NormalizeAbsolutePathString(SGE_STR("C:/test/test1/../.")), SGE_STR("C:/test"));
	ASSERT_EQ(NormalizeAbsolutePathString(SGE_STR("C:/test/test1/././.")), SGE_STR("C:/test/test1"));
#elif defined(SGE_POSIX)
	ASSERT_EQ(NormalizeAbsolutePathString(SGE_STR("/")), SGE_STR("/"));
	ASSERT_EQ(NormalizeAbsolutePathString(SGE_STR("/.")), SGE_STR("/"));
	ASSERT_EQ(NormalizeAbsolutePathString(SGE_STR("/..")), SGE_STR("/"));
	ASSERT_EQ(NormalizeAbsolutePathString(SGE_STR("/../..")), SGE_STR("/"));
	ASSERT_EQ(NormalizeAbsolutePathString(SGE_STR("/./.")), SGE_STR("/"));
	ASSERT_EQ(NormalizeAbsolutePathString(SGE_STR("/../../..")), SGE_STR("/"));
	ASSERT_EQ(NormalizeAbsolutePathString(SGE_STR("/../../../.")), SGE_STR("/"));
	ASSERT_EQ(NormalizeAbsolutePathString(SGE_STR("/../../.././..")), SGE_STR("/"));
	ASSERT_EQ(NormalizeAbsolutePathString(SGE_STR("/../../.././../.")), SGE_STR("/"));
	ASSERT_EQ(NormalizeAbsolutePathString(SGE_STR("/test")), SGE_STR("/test"));
	ASSERT_EQ(NormalizeAbsolutePathString(SGE_STR("/test/..")), SGE_STR("/"));
	ASSERT_EQ(NormalizeAbsolutePathString(SGE_STR("/test/../..")), SGE_STR("/"));
	ASSERT_EQ(NormalizeAbsolutePathString(SGE_STR("/test/../../..")), SGE_STR("/"));
	ASSERT_EQ(NormalizeAbsolutePathString(SGE_STR("/test/./..")), SGE_STR("/"));
	ASSERT_EQ(NormalizeAbsolutePathString(SGE_STR("/test/./test1/..")), SGE_STR("/test"));
	ASSERT_EQ(NormalizeAbsolutePathString(SGE_STR("/test/test1/..")), SGE_STR("/test"));
	ASSERT_EQ(NormalizeAbsolutePathString(SGE_STR("/test/test1/../..")), SGE_STR("/"));
	ASSERT_EQ(NormalizeAbsolutePathString(SGE_STR("/test/test1/../.")), SGE_STR("/test"));
	ASSERT_EQ(NormalizeAbsolutePathString(SGE_STR("/test/test1/././.")), SGE_STR("/test/test1"));
#else
#error this os has not been supported.
#endif
}

TEST(Path, InstanceTest)
{
	Path p1;
	ASSERT_EQ(p1.GetString(), SGE_STR("."));
	Path p2(SGE_STR("test"));
	ASSERT_EQ(p2.GetString(), SGE_STR("test"));
}

TEST(Path, GetSystemPathStringTest)
{
	Path p(SGE_STR("test/test1.txt"));
#ifdef SGE_WINDOWS
	ASSERT_EQ(p.GetSystemPathString(), SGE_STR("test\\test1.txt"));
#elif defined(SGE_POSIX)
	ASSERT_EQ(p.GetSystemPathString(), SGE_STR("test/test1.txt"));
#else
#error this os has not been supported.
#endif
}

TEST(Path, GetStringTest)
{
	Path p(SGE_STR("test/test1.txt"));
	ASSERT_EQ(p.GetString(), SGE_STR("test/test1.txt"));

	Path p2(SGE_STR("test///test2///"));
	ASSERT_EQ(p2.GetString(), SGE_STR("test/test2"));
}

TEST(Path, GetFileNameTest)
{
	ASSERT_EQ(Path(SGE_STR("./TestData")).GetFileName(), SGE_STR("TestData"));
	ASSERT_EQ(Path(SGE_STR("./TestData/TestCommon")).GetFileName(), SGE_STR("TestCommon"));
	ASSERT_EQ(Path(SGE_STR("./TestData/TestCommon/TestFile")).GetFileName(), SGE_STR("TestFile"));
	ASSERT_EQ(Path(SGE_STR("./TestData/TestCommon/TestFile/test1.txt")).GetFileName(), SGE_STR("test1.txt"));
}

TEST(Path, IsAbsoluteOrRelativeTest)
{
#ifdef SGE_WINDOWS
	Path p1(SGE_STR("C:/test"));
	ASSERT_TRUE(p1.IsAbsolute());
	ASSERT_FALSE(p1.IsRelative());
	Path p2(SGE_STR("E:"));
	ASSERT_TRUE(p2.IsAbsolute());
	ASSERT_FALSE(p2.IsRelative());
	Path p3(SGE_STR("test"));
	ASSERT_FALSE(p3.IsAbsolute());
	ASSERT_TRUE(p3.IsRelative());
#elif defined(SGE_POSIX)
	Path p1(SGE_STR("/test"));
	ASSERT_TRUE(p1.IsAbsolute());
	ASSERT_FALSE(p1.IsRelative());
	Path p2(SGE_STR("/"));
	ASSERT_TRUE(p2.IsAbsolute());
	ASSERT_FALSE(p2.IsRelative());
	Path p3(SGE_STR("test"));
	ASSERT_FALSE(p3.IsAbsolute());
	ASSERT_TRUE(p3.IsRelative());
	Path p4(SGE_STR("~"));
	ASSERT_FALSE(p4.IsAbsolute());
	ASSERT_TRUE(p4.IsRelative());
#else
#error this os has not been supported.
#endif
}

TEST(Path, IsRootTest)
{
#ifdef SGE_WINDOWS
	ASSERT_TRUE(Path(SGE_STR("C:")).IsRoot());
	ASSERT_TRUE(Path(SGE_STR("D:")).IsRoot());
	ASSERT_FALSE(Path(SGE_STR("C:/test")).IsRoot());
	ASSERT_FALSE(Path(SGE_STR("test")).IsRoot());
#elif defined(SGE_POSIX)
	ASSERT_TRUE(Path(SGE_STR("/")).IsRoot());
	ASSERT_FALSE(Path(SGE_STR("/test")).IsRoot());
	ASSERT_FALSE(Path(SGE_STR("test")).IsRoot());
#else
#error this os has not been supported.
#endif
}

TEST(Path, IsExistTest)
{
	ASSERT_TRUE(Path(SGE_STR("./TestData/TestCommon/TestFile/test1.txt")).IsExist());
	ASSERT_FALSE(Path(SGE_STR("./TestData/TestCommon/TestFile/test_not_exist.txt")).IsExist());
}

TEST(Path, GetPathTypeTest)
{
	ASSERT_EQ(Path(SGE_STR("./TestData/TestCommon/TestFile/test_not_exist.txt")).GetPathType(), PathType::NotExist);
	ASSERT_EQ(Path(SGE_STR("./TestData/TestCommon/TestFile")).GetPathType(), PathType::Directory);
	ASSERT_EQ(Path(SGE_STR("./TestData/TestCommon/TestFile/test1.txt")).GetPathType(), PathType::File);
}

TEST(Path, GetAbsolutePathTest)
{
	Path p(SGE_STR("test"));
	ASSERT_FALSE(p.IsAbsolute());
	Path ap = p.GetAbsolutePath();
	ASSERT_TRUE(ap.IsAbsolute());
	StdTCout << SGE_STR_TO_TSTR(ap.GetString()).GetData() << std::endl;
#ifdef SGE_WINDOWS
	Path p2(SGE_STR("C:/test"));
	ASSERT_TRUE(p2.IsAbsolute());
	ASSERT_EQ(p2.GetString(), p2.GetAbsolutePath().GetString());
#elif defined(SGE_POSIX)
	Path p2(SGE_STR("/test"));
	ASSERT_TRUE(p2.IsAbsolute());
	ASSERT_EQ(p2.GetString(), p2.GetAbsolutePath().GetString());
#else
#error this os has not been supported.
#endif
}

TEST(Path, GetChildPathTest)
{
	Map<String, PathType> check_map;
	auto result = Path(SGE_STR("./TestData/TestCommon/TestFile")).GetChildPath();
	for (auto iter = result.GetConstBegin(); iter != result.GetConstEnd(); ++iter)
	{
		auto fiter = check_map.Find(iter->m_First.GetFileName());
		ASSERT_EQ(fiter, check_map.GetEnd());
		check_map.Insert(iter->m_First.GetFileName(), iter->m_Second);
	}
	ASSERT_EQ(check_map.GetSize(), 3);
	ASSERT_EQ(check_map[SGE_STR("test1.txt")], PathType::File);
	ASSERT_EQ(check_map[SGE_STR("test2.txt")], PathType::File);
	ASSERT_EQ(check_map[SGE_STR("TestDirectory")], PathType::Directory);
}

TEST(Path, AdditionTest)
{
	ASSERT_EQ((Path(SGE_STR("test")) / Path(SGE_STR("test1.txt"))).GetString(), SGE_STR("test/test1.txt"));
#ifdef SGE_WINDOWS
	ASSERT_EQ((Path(SGE_STR("C:/test")) / Path(SGE_STR("test1.txt"))).GetString(), SGE_STR("C:/test/test1.txt"));
#elif defined(SGE_POSIX)
	ASSERT_EQ((Path(SGE_STR("/test")) / Path(SGE_STR("test1.txt"))).GetString(), SGE_STR("/test/test1.txt"));
#else
#error this os has not been supported.
#endif
}

TEST(Path, EqualnessTest)
{
	ASSERT_EQ(Path(SGE_STR("./test")), Path(SGE_STR("./test")));
	ASSERT_EQ(Path(SGE_STR("test")), Path(SGE_STR("test")));
	ASSERT_NE(Path(SGE_STR("./test")), Path(SGE_STR("test")));
	ASSERT_NE(Path(SGE_STR("test")), Path(SGE_STR("./test")));
}

TEST(Path, GetParentPathTest)
{
	ASSERT_EQ(Path(SGE_STR("./test/test1")).GetParentPath().GetString(), Path(SGE_STR("test")).GetAbsolutePath().GetString());
}

TEST(Path, GetCurrentDirectoryPathTest)
{
	Path p = GetCurrentDirectoryPath();
	ASSERT_TRUE(p.IsAbsolute());
	StdTCout << SGE_STR_TO_TSTR(p.GetString()).GetData() << std::endl;
}

TEST(Path, SetCurrentDirectoryPathTest)
{
	Path pold = GetCurrentDirectoryPath();
	Path p = Path(SGE_STR("./TestData")).GetAbsolutePath();
	SetCurrentDirectoryPath(p);
	ASSERT_EQ(GetCurrentDirectoryPath().GetString(), p.GetString());
	SetCurrentDirectoryPath(pold);
	ASSERT_EQ(GetCurrentDirectoryPath().GetString(), pold.GetString());
}

TEST(Path, GetModuleDirectoryPathTest)
{
	String path_str = GetModuleDirectoryPath().GetString();
	StdTCout << SGE_STR_TO_TSTR(path_str).GetData() << std::endl;
	ASSERT_NE(path_str.Find(SGE_STR("TestCommon"), path_str.GetConstBegin(), path_str.GetConstEnd()), path_str.GetConstEnd());
}