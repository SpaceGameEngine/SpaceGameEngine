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
#include "gtest/gtest.h"
#include "File.h"

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

TEST(Path, GetAbsolutePathTest)
{
	Path p(SGE_STR("test"));
	ASSERT_FALSE(p.IsAbsolute());
	Path ap = p.GetAbsolutePath();
	ASSERT_TRUE(ap.IsAbsolute());
	std::wcout << ap.GetString().GetData() << std::endl;
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

TEST(Path, GetCurrentDirectoryPathTest)
{
	Path p = GetCurrentDirectoryPath();
	ASSERT_TRUE(p.IsAbsolute());
	std::wcout << p.GetString().GetData() << std::endl;
}