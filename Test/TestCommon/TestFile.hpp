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
#include <fstream>
#include <iostream>
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

TEST(Path, VisitChildPathTest)
{
	Map<String, PathType> check_map;
	Path(SGE_STR("./TestData/TestCommon/TestFile")).VisitChildPath([&](const String& file_name, PathType pt) {
		auto fiter = check_map.Find(file_name);
		ASSERT_EQ(fiter, check_map.GetEnd());
		check_map.Insert(file_name, pt);
	});
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

TEST(Path, IsEquivalentTest)
{
	ASSERT_FALSE(Path(SGE_STR("./TestData/TestCommon/TestFile/not_exist.txt")).IsEquivalent(Path(SGE_STR("./TestData/TestCommon/TestFile/test1.txt"))));
	ASSERT_FALSE(Path(SGE_STR("./TestData/TestCommon/TestFile/test1.txt")).IsEquivalent(Path(SGE_STR("./TestData/TestCommon/TestFile/not_exist.txt"))));
	ASSERT_FALSE(Path(SGE_STR("./TestData/TestCommon/TestFile/test1.txt")).IsEquivalent(Path(SGE_STR("./TestData/TestCommon/TestFile/test2.txt"))));
	ASSERT_FALSE(Path(SGE_STR("./TestData/TestCommon/TestFile/test1.txt")).IsEquivalent(Path(SGE_STR("./TestData/TestCommon/TestFile/TestDirectory"))));
	ASSERT_TRUE(Path(SGE_STR("./TestData/TestCommon/TestFile/test1.txt")).IsEquivalent(Path(SGE_STR("./TestData/TestCommon/TestFile/test1.txt"))));
	ASSERT_TRUE(Path(SGE_STR("./TestData/TestCommon/TestFile/test2.txt")).IsEquivalent(Path(SGE_STR("./TestData/TestCommon/TestFile/test2.txt"))));
	ASSERT_TRUE(Path(SGE_STR("./TestData/TestCommon/TestFile/TestDirectory")).IsEquivalent(Path(SGE_STR("./TestData/TestCommon/TestFile/TestDirectory"))));
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

TEST(Path, CreateAndDeleteFileTest)
{
	Path p(SGE_STR("./TestData/TestCommon/TestFile/test_c_d.txt"));
	ASSERT_FALSE(p.IsExist());
	CreateFile(p);
	ASSERT_TRUE(p.IsExist());
	DeleteFile(p);
	ASSERT_FALSE(p.IsExist());
}

TEST(Path, CopyFileTest)
{
	Path src(SGE_STR("./TestData/TestCommon/TestFile/test_cp_src.txt"));
	Path dst(SGE_STR("./TestData/TestCommon/TestFile/test_cp_dst.txt"));
	ASSERT_FALSE(src.IsExist());
	std::ofstream test_cp_w(SGE_STR_TO_UTF8(src.GetString()).GetData());
	test_cp_w << 1024;
	test_cp_w.close();
	ASSERT_TRUE(src.IsExist());

	CopyFile(dst, src, false);

	ASSERT_TRUE(src.IsExist());
	ASSERT_TRUE(dst.IsExist());

	int num = 0;
	std::ifstream test_cp_i_src(SGE_STR_TO_UTF8(src.GetString()).GetData());
	test_cp_i_src >> num;
	ASSERT_EQ(num, 1024);
	test_cp_i_src.close();
	num = 0;
	std::ifstream test_cp_i_dst(SGE_STR_TO_UTF8(dst.GetString()).GetData());
	test_cp_i_dst >> num;
	ASSERT_EQ(num, 1024);
	test_cp_i_dst.close();

	DeleteFile(src);
	ASSERT_FALSE(src.IsExist());
	DeleteFile(dst);
	ASSERT_FALSE(dst.IsExist());
}

TEST(Path, CopyFileOverwriteTest)
{
	Path src(SGE_STR("./TestData/TestCommon/TestFile/test_cp_ow_src.txt"));
	Path dst(SGE_STR("./TestData/TestCommon/TestFile/test_cp_ow_dst.txt"));
	ASSERT_FALSE(src.IsExist());
	std::ofstream test_cp_w(SGE_STR_TO_UTF8(src.GetString()).GetData());
	test_cp_w << 1024;
	test_cp_w.close();
	ASSERT_TRUE(src.IsExist());

	ASSERT_FALSE(dst.IsExist());
	std::ofstream test_cp_w2(SGE_STR_TO_UTF8(dst.GetString()).GetData());
	test_cp_w2 << 2048;
	test_cp_w2.close();
	ASSERT_TRUE(dst.IsExist());

	CopyFile(dst, src, true);

	ASSERT_TRUE(src.IsExist());
	ASSERT_TRUE(dst.IsExist());

	int num = 0;
	std::ifstream test_cp_i_src(SGE_STR_TO_UTF8(src.GetString()).GetData());
	test_cp_i_src >> num;
	ASSERT_EQ(num, 1024);
	test_cp_i_src.close();
	num = 0;
	std::ifstream test_cp_i_dst(SGE_STR_TO_UTF8(dst.GetString()).GetData());
	test_cp_i_dst >> num;
	ASSERT_EQ(num, 1024);
	test_cp_i_dst.close();

	DeleteFile(src);
	ASSERT_FALSE(src.IsExist());
	DeleteFile(dst);
	ASSERT_FALSE(dst.IsExist());
}

TEST(Path, MoveFileTest)
{
	Path src(SGE_STR("./TestData/TestCommon/TestFile/test_mv_src.txt"));
	Path dst(SGE_STR("./TestData/TestCommon/TestFile/test_mv_dst.txt"));
	ASSERT_FALSE(src.IsExist());
	std::ofstream test_mv_w(SGE_STR_TO_UTF8(src.GetString()).GetData());
	test_mv_w << 1024;
	test_mv_w.close();
	ASSERT_TRUE(src.IsExist());

	MoveFile(dst, src, false);

	ASSERT_FALSE(src.IsExist());
	ASSERT_TRUE(dst.IsExist());

	int num = 0;
	std::ifstream test_mv_i_dst(SGE_STR_TO_UTF8(dst.GetString()).GetData());
	test_mv_i_dst >> num;
	ASSERT_EQ(num, 1024);
	test_mv_i_dst.close();

	DeleteFile(dst);
	ASSERT_FALSE(dst.IsExist());
}

TEST(Path, MoveFileOverwriteTest)
{
	Path src(SGE_STR("./TestData/TestCommon/TestFile/test_mv_ow_src.txt"));
	Path dst(SGE_STR("./TestData/TestCommon/TestFile/test_mv_ow_dst.txt"));
	ASSERT_FALSE(src.IsExist());
	std::ofstream test_mv_w(SGE_STR_TO_UTF8(src.GetString()).GetData());
	test_mv_w << 1024;
	test_mv_w.close();
	ASSERT_TRUE(src.IsExist());

	ASSERT_FALSE(dst.IsExist());
	std::ofstream test_mv_w2(SGE_STR_TO_UTF8(dst.GetString()).GetData());
	test_mv_w2 << 2048;
	test_mv_w2.close();
	ASSERT_TRUE(dst.IsExist());

	MoveFile(dst, src, true);

	ASSERT_FALSE(src.IsExist());
	ASSERT_TRUE(dst.IsExist());

	int num = 0;
	std::ifstream test_mv_i_dst(SGE_STR_TO_UTF8(dst.GetString()).GetData());
	test_mv_i_dst >> num;
	ASSERT_EQ(num, 1024);
	test_mv_i_dst.close();

	DeleteFile(dst);
	ASSERT_FALSE(dst.IsExist());
}

TEST(Path, CreateAndDeleteDirectoryTest)
{
	Path test_dir1(SGE_STR("./TestData/TestCommon/TestFile/test_dir"));
	ASSERT_FALSE(test_dir1.IsExist());
	CreateDirectory(test_dir1);
	ASSERT_TRUE(test_dir1.IsExist());

	Path test_file1 = test_dir1 / Path(SGE_STR("test_dir_file1.txt"));
	ASSERT_FALSE(test_file1.IsExist());
	CreateFile(test_file1);
	ASSERT_TRUE(test_file1.IsExist());

	std::ofstream test_file1_output(SGE_STR_TO_UTF8(test_file1.GetAbsolutePath().GetString()).GetData());
	test_file1_output << "test_file1";
	test_file1_output.close();

	Path test_dir2 = test_dir1 / Path(SGE_STR("test_dir2"));
	ASSERT_FALSE(test_dir2.IsExist());
	CreateDirectory(test_dir2);
	ASSERT_TRUE(test_dir2.IsExist());

	Path test_file2 = test_dir2 / Path(SGE_STR("test_dir_file2.txt"));
	ASSERT_FALSE(test_file2.IsExist());
	CreateFile(test_file2);
	ASSERT_TRUE(test_file2.IsExist());

	std::ofstream test_file2_output(SGE_STR_TO_UTF8(test_file2.GetAbsolutePath().GetString()).GetData());
	test_file2_output << "test_file2";
	test_file2_output.close();

	ASSERT_EQ(test_dir1.GetChildPath().GetSize(), 2);
	ASSERT_EQ(test_dir2.GetChildPath().GetSize(), 1);
	ASSERT_TRUE(test_file1.GetParentPath().IsEquivalent(test_dir1));
	ASSERT_TRUE(test_dir2.GetParentPath().IsEquivalent(test_dir1));
	ASSERT_TRUE(test_file2.GetParentPath().IsEquivalent(test_dir2));

	DeleteDirectory(test_dir1);
	ASSERT_FALSE(test_dir1.IsExist());
	ASSERT_FALSE(test_dir2.IsExist());
	ASSERT_FALSE(test_file1.IsExist());
	ASSERT_FALSE(test_file2.IsExist());
}

TEST(Path, CopyDirectoryTest)
{
	Path test_src_dir(SGE_STR("./TestData/TestCommon/TestFile/test_dir_cp_src"));
	ASSERT_FALSE(test_src_dir.IsExist());
	CreateDirectory(test_src_dir);
	ASSERT_TRUE(test_src_dir.IsExist());
	Path test_src_file = test_src_dir / Path(SGE_STR("test_dir_cp_file.txt"));
	ASSERT_FALSE(test_src_file.IsExist());
	CreateFile(test_src_file);
	ASSERT_TRUE(test_src_file.IsExist());
	std::ofstream test_src_file_output(SGE_STR_TO_UTF8(test_src_file.GetAbsolutePath().GetString()).GetData());
	test_src_file_output << 256;
	test_src_file_output.close();
	Path test_src_dir2 = test_src_dir / Path(SGE_STR("test_dir2"));
	ASSERT_FALSE(test_src_dir2.IsExist());
	CreateDirectory(test_src_dir2);
	ASSERT_TRUE(test_src_dir2.IsExist());
	Path test_src_file2 = test_src_dir2 / Path(SGE_STR("test_file2.txt"));
	ASSERT_FALSE(test_src_file2.IsExist());
	CreateFile(test_src_file2);
	ASSERT_TRUE(test_src_file2.IsExist());

	Path test_dst_dir(SGE_STR("./TestData/TestCommon/TestFile/test_dir_cp_dst"));
	Path test_dst_file = test_dst_dir / Path(SGE_STR("test_dir_cp_file.txt"));
	Path test_dst_dir2 = test_dst_dir / Path(SGE_STR("test_dir2"));
	Path test_dst_file2 = test_dst_dir2 / Path(SGE_STR("test_file2.txt"));
	ASSERT_FALSE(test_dst_dir.IsExist());
	ASSERT_FALSE(test_dst_file.IsExist());
	ASSERT_FALSE(test_dst_dir2.IsExist());
	ASSERT_FALSE(test_dst_file2.IsExist());

	CopyDirectory(test_dst_dir, test_src_dir, false);

	ASSERT_TRUE(test_src_dir.IsExist());
	ASSERT_TRUE(test_src_file.IsExist());
	ASSERT_TRUE(test_src_dir2.IsExist());
	ASSERT_TRUE(test_src_file2.IsExist());
	ASSERT_TRUE(test_dst_dir.IsExist());
	ASSERT_TRUE(test_dst_file.IsExist());
	ASSERT_TRUE(test_dst_dir2.IsExist());
	ASSERT_TRUE(test_dst_file2.IsExist());

	std::ifstream test_dst_file_input(SGE_STR_TO_UTF8(test_dst_file.GetAbsolutePath().GetString()).GetData());
	int num = 0;
	test_dst_file_input >> num;
	ASSERT_EQ(num, 256);
	test_dst_file_input.close();

	DeleteDirectory(test_src_dir);
	ASSERT_FALSE(test_src_dir.IsExist());
	ASSERT_FALSE(test_src_file.IsExist());
	ASSERT_FALSE(test_src_dir2.IsExist());
	ASSERT_FALSE(test_src_file2.IsExist());
	DeleteDirectory(test_dst_dir);
	ASSERT_FALSE(test_dst_dir.IsExist());
	ASSERT_FALSE(test_dst_file.IsExist());
	ASSERT_FALSE(test_dst_dir2.IsExist());
	ASSERT_FALSE(test_dst_file2.IsExist());
}

TEST(Path, CopyDirectoryOverwriteTest)
{
	Path test_src_dir(SGE_STR("./TestData/TestCommon/TestFile/test_dir_cp_ow_src"));
	ASSERT_FALSE(test_src_dir.IsExist());
	CreateDirectory(test_src_dir);
	ASSERT_TRUE(test_src_dir.IsExist());
	Path test_src_file = test_src_dir / Path(SGE_STR("test_dir_cp_file.txt"));
	ASSERT_FALSE(test_src_file.IsExist());
	CreateFile(test_src_file);
	ASSERT_TRUE(test_src_file.IsExist());
	std::ofstream test_src_file_output(SGE_STR_TO_UTF8(test_src_file.GetAbsolutePath().GetString()).GetData());
	test_src_file_output << 256;
	test_src_file_output.close();
	Path test_src_dir2 = test_src_dir / Path(SGE_STR("test_dir2"));
	ASSERT_FALSE(test_src_dir2.IsExist());
	CreateDirectory(test_src_dir2);
	ASSERT_TRUE(test_src_dir2.IsExist());
	Path test_src_file2 = test_src_dir2 / Path(SGE_STR("test_file2.txt"));
	ASSERT_FALSE(test_src_file2.IsExist());
	CreateFile(test_src_file2);
	ASSERT_TRUE(test_src_file2.IsExist());

	Path test_dst_dir(SGE_STR("./TestData/TestCommon/TestFile/test_dir_cp_ow_dst"));
	Path test_dst_file = test_dst_dir / Path(SGE_STR("test_dir_cp_file.txt"));
	Path test_dst_dir2 = test_dst_dir / Path(SGE_STR("test_dir2"));
	Path test_dst_file2 = test_dst_dir2 / Path(SGE_STR("test_file2.txt"));
	ASSERT_FALSE(test_dst_dir.IsExist());
	ASSERT_FALSE(test_dst_file.IsExist());
	ASSERT_FALSE(test_dst_dir2.IsExist());
	ASSERT_FALSE(test_dst_file2.IsExist());
	CreateDirectory(test_dst_dir);
	CreateFile(test_dst_file);
	ASSERT_TRUE(test_dst_dir.IsExist());
	ASSERT_TRUE(test_dst_file.IsExist());
	std::ofstream test_dst_file_output(SGE_STR_TO_UTF8(test_dst_file.GetAbsolutePath().GetString()).GetData());
	test_dst_file_output << 1024;
	test_dst_file_output.close();

	CopyDirectory(test_dst_dir, test_src_dir, true);

	ASSERT_TRUE(test_src_dir.IsExist());
	ASSERT_TRUE(test_src_file.IsExist());
	ASSERT_TRUE(test_src_dir2.IsExist());
	ASSERT_TRUE(test_src_file2.IsExist());
	ASSERT_TRUE(test_dst_dir.IsExist());
	ASSERT_TRUE(test_dst_file.IsExist());
	ASSERT_TRUE(test_dst_dir2.IsExist());
	ASSERT_TRUE(test_dst_file2.IsExist());

	std::ifstream test_dst_file_input(SGE_STR_TO_UTF8(test_dst_file.GetAbsolutePath().GetString()).GetData());
	int num = 0;
	test_dst_file_input >> num;
	ASSERT_EQ(num, 256);
	test_dst_file_input.close();

	DeleteDirectory(test_src_dir);
	ASSERT_FALSE(test_src_dir.IsExist());
	ASSERT_FALSE(test_src_file.IsExist());
	ASSERT_FALSE(test_src_dir2.IsExist());
	ASSERT_FALSE(test_src_file2.IsExist());
	DeleteDirectory(test_dst_dir);
	ASSERT_FALSE(test_dst_dir.IsExist());
	ASSERT_FALSE(test_dst_file.IsExist());
	ASSERT_FALSE(test_dst_dir2.IsExist());
	ASSERT_FALSE(test_dst_file2.IsExist());
}

TEST(Path, MoveDirectoryTest)
{
	Path test_src_dir(SGE_STR("./TestData/TestCommon/TestFile/test_dir_mv_src"));
	ASSERT_FALSE(test_src_dir.IsExist());
	CreateDirectory(test_src_dir);
	ASSERT_TRUE(test_src_dir.IsExist());
	Path test_src_file = test_src_dir / Path(SGE_STR("test_dir_mv_file.txt"));
	ASSERT_FALSE(test_src_file.IsExist());
	CreateFile(test_src_file);
	ASSERT_TRUE(test_src_file.IsExist());
	std::ofstream test_src_file_output(SGE_STR_TO_UTF8(test_src_file.GetAbsolutePath().GetString()).GetData());
	test_src_file_output << 256;
	test_src_file_output.close();
	Path test_src_dir2 = test_src_dir / Path(SGE_STR("test_dir2"));
	ASSERT_FALSE(test_src_dir2.IsExist());
	CreateDirectory(test_src_dir2);
	ASSERT_TRUE(test_src_dir2.IsExist());
	Path test_src_file2 = test_src_dir2 / Path(SGE_STR("test_file2.txt"));
	ASSERT_FALSE(test_src_file2.IsExist());
	CreateFile(test_src_file2);
	ASSERT_TRUE(test_src_file2.IsExist());

	Path test_dst_dir(SGE_STR("./TestData/TestCommon/TestFile/test_dir_mv_dst"));
	Path test_dst_file = test_dst_dir / Path(SGE_STR("test_dir_mv_file.txt"));
	Path test_dst_dir2 = test_dst_dir / Path(SGE_STR("test_dir2"));
	Path test_dst_file2 = test_dst_dir2 / Path(SGE_STR("test_file2.txt"));
	ASSERT_FALSE(test_dst_dir.IsExist());
	ASSERT_FALSE(test_dst_file.IsExist());
	ASSERT_FALSE(test_dst_dir2.IsExist());
	ASSERT_FALSE(test_dst_file2.IsExist());

	MoveDirectory(test_dst_dir, test_src_dir, false);

	ASSERT_FALSE(test_src_dir.IsExist());
	ASSERT_FALSE(test_src_file.IsExist());
	ASSERT_FALSE(test_src_dir2.IsExist());
	ASSERT_FALSE(test_src_file2.IsExist());
	ASSERT_TRUE(test_dst_dir.IsExist());
	ASSERT_TRUE(test_dst_file.IsExist());
	ASSERT_TRUE(test_dst_dir2.IsExist());
	ASSERT_TRUE(test_dst_file2.IsExist());

	std::ifstream test_dst_file_input(SGE_STR_TO_UTF8(test_dst_file.GetAbsolutePath().GetString()).GetData());
	int num = 0;
	test_dst_file_input >> num;
	ASSERT_EQ(num, 256);
	test_dst_file_input.close();

	DeleteDirectory(test_dst_dir);
	ASSERT_FALSE(test_dst_dir.IsExist());
	ASSERT_FALSE(test_dst_file.IsExist());
	ASSERT_FALSE(test_dst_dir2.IsExist());
	ASSERT_FALSE(test_dst_file2.IsExist());
}

TEST(Path, MoveDirectoryOverwriteTest)
{
	Path test_src_dir(SGE_STR("./TestData/TestCommon/TestFile/test_dir_mv_ow_src"));
	ASSERT_FALSE(test_src_dir.IsExist());
	CreateDirectory(test_src_dir);
	ASSERT_TRUE(test_src_dir.IsExist());
	Path test_src_file = test_src_dir / Path(SGE_STR("test_dir_mv_file.txt"));
	ASSERT_FALSE(test_src_file.IsExist());
	CreateFile(test_src_file);
	ASSERT_TRUE(test_src_file.IsExist());
	std::ofstream test_src_file_output(SGE_STR_TO_UTF8(test_src_file.GetAbsolutePath().GetString()).GetData());
	test_src_file_output << 256;
	test_src_file_output.close();
	Path test_src_dir2 = test_src_dir / Path(SGE_STR("test_dir2"));
	ASSERT_FALSE(test_src_dir2.IsExist());
	CreateDirectory(test_src_dir2);
	ASSERT_TRUE(test_src_dir2.IsExist());
	Path test_src_file2 = test_src_dir2 / Path(SGE_STR("test_file2.txt"));
	ASSERT_FALSE(test_src_file2.IsExist());
	CreateFile(test_src_file2);
	ASSERT_TRUE(test_src_file2.IsExist());

	Path test_dst_dir(SGE_STR("./TestData/TestCommon/TestFile/test_dir_mv_ow_dst"));
	Path test_dst_file = test_dst_dir / Path(SGE_STR("test_dir_mv_file.txt"));
	Path test_dst_dir2 = test_dst_dir / Path(SGE_STR("test_dir2"));
	Path test_dst_file2 = test_dst_dir2 / Path(SGE_STR("test_file2.txt"));
	ASSERT_FALSE(test_dst_dir.IsExist());
	ASSERT_FALSE(test_dst_file.IsExist());
	ASSERT_FALSE(test_dst_dir2.IsExist());
	ASSERT_FALSE(test_dst_file2.IsExist());
	CreateDirectory(test_dst_dir);
	CreateFile(test_dst_file);
	ASSERT_TRUE(test_dst_dir.IsExist());
	ASSERT_TRUE(test_dst_file.IsExist());
	std::ofstream test_dst_file_output(SGE_STR_TO_UTF8(test_dst_file.GetAbsolutePath().GetString()).GetData());
	test_dst_file_output << 1024;
	test_dst_file_output.close();

	MoveDirectory(test_dst_dir, test_src_dir, true);

	ASSERT_FALSE(test_src_dir.IsExist());
	ASSERT_FALSE(test_src_file.IsExist());
	ASSERT_FALSE(test_src_dir2.IsExist());
	ASSERT_FALSE(test_src_file2.IsExist());
	ASSERT_TRUE(test_dst_dir.IsExist());
	ASSERT_TRUE(test_dst_file.IsExist());
	ASSERT_TRUE(test_dst_dir2.IsExist());
	ASSERT_TRUE(test_dst_file2.IsExist());

	std::ifstream test_dst_file_input(SGE_STR_TO_UTF8(test_dst_file.GetAbsolutePath().GetString()).GetData());
	int num = 0;
	test_dst_file_input >> num;
	ASSERT_EQ(num, 256);
	test_dst_file_input.close();

	DeleteDirectory(test_dst_dir);
	ASSERT_FALSE(test_dst_dir.IsExist());
	ASSERT_FALSE(test_dst_file.IsExist());
	ASSERT_FALSE(test_dst_dir2.IsExist());
	ASSERT_FALSE(test_dst_file2.IsExist());
}

TEST(BinaryFile, InstanceTest)
{
	BinaryFile file1;
}

TEST(BinaryFile, ConstructionAndDestructionTest)
{
	{
		BinaryFile file1(Path(SGE_STR("./TestData/TestCommon/TestFile/test1.txt")), FileIOMode::Read);
		BinaryFile file2(Path(SGE_STR("./TestData/TestCommon/TestFile/test2.txt")), FileIOMode::Write);
		BinaryFile file3(Path(SGE_STR("./TestData/TestCommon/TestFile/test_app.txt")), FileIOMode::Append);
	}
	DeleteFile(Path(SGE_STR("./TestData/TestCommon/TestFile/test_app.txt")));
}

TEST(BinaryFile, OpenTest)
{
	{
		BinaryFile file1;
		file1.Open(Path(SGE_STR("./TestData/TestCommon/TestFile/test1.txt")), FileIOMode::Read);
		BinaryFile file2;
		file2.Open(Path(SGE_STR("./TestData/TestCommon/TestFile/test2.txt")), FileIOMode::Write);
		BinaryFile file3;
		file3.Open(Path(SGE_STR("./TestData/TestCommon/TestFile/test_app.txt")), FileIOMode::Append);
	}
	DeleteFile(Path(SGE_STR("./TestData/TestCommon/TestFile/test_app.txt")));
}

TEST(BinaryFile, CloseTest)
{
	BinaryFile file1(Path(SGE_STR("./TestData/TestCommon/TestFile/test1.txt")), FileIOMode::Read);
	file1.Close();
	file1.Open(Path(SGE_STR("./TestData/TestCommon/TestFile/test2.txt")), FileIOMode::Write);
	file1.Close();
	file1.Open(Path(SGE_STR("./TestData/TestCommon/TestFile/test_app.txt")), FileIOMode::Append);
	file1.Close();
	DeleteFile(Path(SGE_STR("./TestData/TestCommon/TestFile/test_app.txt")));
}