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

TEST(BinaryFile, ReadTest)
{
	int test_data[4] = {12, 34, 56, 78};
	Path path(SGE_STR("./TestData/TestCommon/TestFile/testbr.dat"));
	ASSERT_FALSE(path.IsExist());

	std::ofstream b_output(SGE_STR_TO_UTF8(path.GetAbsolutePath().GetString()).GetData(), std::ios::binary);
	b_output.write((const char*)test_data, sizeof(test_data));
	b_output.close();
	ASSERT_TRUE(path.IsExist());

	int test_data_input[4] = {0, 0, 0, 0};
	BinaryFile bf_read(path, FileIOMode::Read);
	ASSERT_EQ(bf_read.Read(test_data_input, sizeof(test_data_input)), sizeof(test_data_input));
	bf_read.Close();

	for (int i = 0; i < 4; ++i)
		ASSERT_EQ(test_data[i], test_data_input[i]);

	DeleteFile(path);
	ASSERT_FALSE(path.IsExist());
}

TEST(BinaryFile, WriteTest)
{
	int test_data[4] = {12, 34, 56, 78};
	Path path(SGE_STR("./TestData/TestCommon/TestFile/testbw.dat"));
	ASSERT_FALSE(path.IsExist());

	BinaryFile bf_write(path, FileIOMode::Write);
	ASSERT_EQ(bf_write.Write(test_data, sizeof(test_data)), sizeof(test_data));
	bf_write.Close();
	ASSERT_TRUE(path.IsExist());

	int test_data_input[4] = {0, 0, 0, 0};
	std::ifstream b_input(SGE_STR_TO_UTF8(path.GetAbsolutePath().GetString()).GetData(), std::ios::binary);
	b_input.read((char*)test_data_input, sizeof(test_data_input));
	b_input.close();

	for (int i = 0; i < 4; ++i)
		ASSERT_EQ(test_data[i], test_data_input[i]);

	DeleteFile(path);
	ASSERT_FALSE(path.IsExist());
}

TEST(BinaryFile, FlushTest)
{
	int test_data[4] = {12, 34, 56, 78};
	Path path(SGE_STR("./TestData/TestCommon/TestFile/testbf.dat"));
	ASSERT_FALSE(path.IsExist());

	BinaryFile bf_flush(path, FileIOMode::Write);
	ASSERT_EQ(bf_flush.Write(test_data, sizeof(test_data)), sizeof(test_data));
	bf_flush.Flush();
	bf_flush.Close();
	ASSERT_TRUE(path.IsExist());

	int test_data_input[4] = {0, 0, 0, 0};
	std::ifstream b_input(SGE_STR_TO_UTF8(path.GetAbsolutePath().GetString()).GetData(), std::ios::binary);
	b_input.read((char*)test_data_input, sizeof(test_data_input));
	b_input.close();

	for (int i = 0; i < 4; ++i)
		ASSERT_EQ(test_data[i], test_data_input[i]);

	DeleteFile(path);
	ASSERT_FALSE(path.IsExist());
}

TEST(BinaryFile, ReadWriteTest)
{
	int test_data[4] = {12, 34, 56, 78};
	Path path(SGE_STR("./TestData/TestCommon/TestFile/testbrw.dat"));
	ASSERT_FALSE(path.IsExist());

	std::ofstream b_output(SGE_STR_TO_UTF8(path.GetAbsolutePath().GetString()).GetData(), std::ios::binary);
	b_output.write((const char*)test_data, sizeof(test_data));
	b_output.close();
	ASSERT_TRUE(path.IsExist());

	int test_data_input[4] = {0, 0, 0, 0};
	int test_data_output[4] = {101, 202, 303, 404};
	BinaryFile bf_read_write(path, FileIOMode::Read | FileIOMode::Write);
	ASSERT_EQ(bf_read_write.Read(test_data_input, sizeof(test_data_input)), sizeof(test_data_input));
	ASSERT_EQ(bf_read_write.Write(test_data_output, sizeof(test_data_output)), sizeof(test_data_output));
	bf_read_write.Close();

	for (int i = 0; i < 4; ++i)
		ASSERT_EQ(test_data[i], test_data_input[i]);

	int test_data_check[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	std::ifstream b_input(SGE_STR_TO_UTF8(path.GetAbsolutePath().GetString()).GetData(), std::ios::binary);
	b_input.read((char*)test_data_check, sizeof(test_data_check));
	b_input.close();

	for (int i = 0; i < 4; ++i)
		ASSERT_EQ(test_data_input[i], test_data_check[i]);
	for (int i = 4; i < 8; ++i)
		ASSERT_EQ(test_data_output[i - 4], test_data_check[i]);

	DeleteFile(path);
	ASSERT_FALSE(path.IsExist());
}

TEST(BinaryFile, AppendTest)
{
	int test_data[4] = {12, 34, 56, 78};
	Path path(SGE_STR("./TestData/TestCommon/TestFile/testba.dat"));
	ASSERT_FALSE(path.IsExist());

	std::ofstream b_output(SGE_STR_TO_UTF8(path.GetAbsolutePath().GetString()).GetData(), std::ios::binary);
	b_output.write((const char*)test_data, sizeof(test_data));
	b_output.close();
	ASSERT_TRUE(path.IsExist());

	int test_data_output[4] = {90, 112, 134, 156};
	BinaryFile bf_append(path, FileIOMode::Append);
	ASSERT_EQ(bf_append.Write(test_data_output, sizeof(int) * 2), sizeof(int) * 2);
	bf_append.Close();
	bf_append.Open(path, FileIOMode::Append);	 //test open append
	ASSERT_EQ(bf_append.Write(test_data_output + 2, sizeof(int) * 2), sizeof(int) * 2);
	bf_append.Close();

	int test_data_check[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	std::ifstream b_input(SGE_STR_TO_UTF8(path.GetAbsolutePath().GetString()).GetData(), std::ios::binary);
	b_input.read((char*)test_data_check, sizeof(test_data_check));
	b_input.close();

	for (int i = 0; i < 4; ++i)
		ASSERT_EQ(test_data[i], test_data_check[i]);
	for (int i = 4; i < 8; ++i)
		ASSERT_EQ(test_data_output[i - 4], test_data_check[i]);

	DeleteFile(path);
	ASSERT_FALSE(path.IsExist());
}

TEST(BinaryFile, ReadWriteAppendTest)
{
	int test_data[4] = {12, 34, 56, 78};
	Path path(SGE_STR("./TestData/TestCommon/TestFile/testbrwa.dat"));
	ASSERT_FALSE(path.IsExist());

	std::ofstream b_output(SGE_STR_TO_UTF8(path.GetAbsolutePath().GetString()).GetData(), std::ios::binary);
	b_output.write((const char*)test_data, sizeof(test_data));
	b_output.close();
	ASSERT_TRUE(path.IsExist());

	int test_data_input[4] = {0, 0, 0, 0};
	int test_data_output[4] = {101, 202, 303, 404};
	BinaryFile bf_read_write_append(path, FileIOMode::Read | FileIOMode::Write | FileIOMode::Append);
	ASSERT_EQ(bf_read_write_append.Write(test_data_output, sizeof(test_data_output)), sizeof(test_data_output));
	bf_read_write_append.MoveFilePosition(FilePositionOrigin::Begin, 0);
	ASSERT_EQ(bf_read_write_append.Read(test_data_input, sizeof(test_data_input)), sizeof(test_data_input));
	bf_read_write_append.Close();

	for (int i = 0; i < 4; ++i)
		ASSERT_EQ(test_data[i], test_data_input[i]);

	int test_data_check[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	std::ifstream b_input(SGE_STR_TO_UTF8(path.GetAbsolutePath().GetString()).GetData(), std::ios::binary);
	b_input.read((char*)test_data_check, sizeof(test_data_check));
	b_input.close();

	for (int i = 0; i < 4; ++i)
		ASSERT_EQ(test_data_input[i], test_data_check[i]);
	for (int i = 4; i < 8; ++i)
		ASSERT_EQ(test_data_output[i - 4], test_data_check[i]);

	DeleteFile(path);
	ASSERT_FALSE(path.IsExist());
}

TEST(BinaryFile, MoveFilePositionTest)
{
	int test_data[4] = {12, 34, 56, 78};
	Path path(SGE_STR("./TestData/TestCommon/TestFile/testbfp.dat"));
	ASSERT_FALSE(path.IsExist());

	std::ofstream b_output(SGE_STR_TO_UTF8(path.GetAbsolutePath().GetString()).GetData(), std::ios::binary);
	b_output.write((const char*)test_data, sizeof(test_data));
	b_output.close();
	ASSERT_TRUE(path.IsExist());

	int test_data_input[4] = {0, 0, 0, 0};
	BinaryFile bf(path, FileIOMode::Read);
	Int64 fp = bf.MoveFilePosition(FilePositionOrigin::End, -1 * (Int64)sizeof(int));
	ASSERT_EQ(bf.Read(test_data_input + 3, sizeof(int)), sizeof(int));
	Int64 fp_cur = bf.MoveFilePosition(FilePositionOrigin::Current, 0);
	ASSERT_EQ(fp + sizeof(int), fp_cur);
	ASSERT_EQ(bf.MoveFilePosition(FilePositionOrigin::Begin, fp - 3 * (Int64)sizeof(int)), 0);
	ASSERT_EQ(bf.Read(test_data_input, 3 * sizeof(int)), 3 * sizeof(int));
	bf.Close();

	for (int i = 0; i < 4; ++i)
		ASSERT_EQ(test_data[i], test_data_input[i]);

	DeleteFile(path);
	ASSERT_FALSE(path.IsExist());
}

TEST(BinaryFile, GetFileSizeTest)
{
	int test_data[4] = {12, 34, 56, 78};
	Path path(SGE_STR("./TestData/TestCommon/TestFile/testbfgs.dat"));
	ASSERT_FALSE(path.IsExist());

	std::ofstream b_output(SGE_STR_TO_UTF8(path.GetAbsolutePath().GetString()).GetData(), std::ios::binary);
	b_output.write((const char*)test_data, sizeof(test_data));
	b_output.close();
	ASSERT_TRUE(path.IsExist());

	{
		const BinaryFile bf(path, FileIOMode::Read);
		ASSERT_EQ(bf.GetFileSize(), sizeof(test_data));
	}
	BinaryFile bf(path, FileIOMode::Read);
	bf.MoveFilePosition(FilePositionOrigin::Begin, 2);
	ASSERT_EQ(bf.GetFileSize(), sizeof(test_data));
	ASSERT_EQ(bf.MoveFilePosition(FilePositionOrigin::Current, 0), 2);
	bf.Close();

	DeleteFile(path);
	ASSERT_FALSE(path.IsExist());
}

TEST(BinaryFile, SetFileSizeTest)
{
	int test_data[4] = {12, 34, 56, 78};
	Path path(SGE_STR("./TestData/TestCommon/TestFile/testbfss.dat"));
	ASSERT_FALSE(path.IsExist());

	std::ofstream b_output(SGE_STR_TO_UTF8(path.GetAbsolutePath().GetString()).GetData(), std::ios::binary);
	b_output.write((const char*)test_data, sizeof(test_data));
	b_output.close();
	ASSERT_TRUE(path.IsExist());

	BinaryFile bf(path, FileIOMode::Read | FileIOMode::Write);
	ASSERT_EQ(bf.GetFileSize(), sizeof(test_data));
	bf.MoveFilePosition(FilePositionOrigin::Begin, 2);
	bf.SetFileSize(2 * sizeof(test_data));
	ASSERT_EQ(bf.GetFileSize(), 2 * sizeof(test_data));
	ASSERT_EQ(bf.MoveFilePosition(FilePositionOrigin::Current, 0), 2);
	bf.SetFileSize(2);
	ASSERT_EQ(bf.GetFileSize(), 2);
	ASSERT_EQ(bf.MoveFilePosition(FilePositionOrigin::Current, 0), 2);
	bf.Close();

	DeleteFile(path);
	ASSERT_FALSE(path.IsExist());
}

TEST(BinaryFile, GetFileIOModeTest)
{
	Path path(SGE_STR("./TestData/TestCommon/TestFile/test1.txt"));
	ASSERT_TRUE(path.IsExist());

	BinaryFile bf(path, FileIOMode::Read | FileIOMode::Write);
	ASSERT_EQ(bf.GetFileIOMode(), FileIOMode::Read | FileIOMode::Write);
	bf.Close();

	const BinaryFile cbf(path, FileIOMode::Read);
	ASSERT_EQ(cbf.GetFileIOMode(), FileIOMode::Read);
}

TEST(BinaryFile, IsReadFinishedTest)
{
	int test_data[4] = {12, 34, 56, 78};
	Path path(SGE_STR("./TestData/TestCommon/TestFile/testbrf.dat"));
	ASSERT_FALSE(path.IsExist());

	std::ofstream b_output(SGE_STR_TO_UTF8(path.GetAbsolutePath().GetString()).GetData(), std::ios::binary);
	b_output.write((const char*)test_data, sizeof(test_data));
	b_output.close();
	ASSERT_TRUE(path.IsExist());

	int test_data_input[4] = {0, 0, 0, 0};

	{
		const BinaryFile bf_write(path, FileIOMode::Write);
		ASSERT_TRUE(bf_write.IsReadFinished());
		ASSERT_FALSE(bf_write);
	}

	BinaryFile bf_read(path, FileIOMode::Read);
	ASSERT_EQ(bf_read.Read(test_data_input, sizeof(test_data_input)), sizeof(test_data_input));

	for (int i = 0; i < 4; ++i)
		ASSERT_EQ(test_data[i], test_data_input[i]);

	ASSERT_FALSE(bf_read.IsReadFinished());
	ASSERT_TRUE(bf_read);

	ASSERT_EQ(bf_read.Read(test_data_input, sizeof(test_data_input)), 0);

	ASSERT_TRUE(bf_read.IsReadFinished());
	ASSERT_FALSE(bf_read);

	bf_read.MoveFilePosition(FilePositionOrigin::Begin, 0);
	memset(test_data_input, 0, sizeof(test_data_input));
	ASSERT_EQ(bf_read.Read(test_data_input, sizeof(test_data_input)), sizeof(test_data_input));

	for (int i = 0; i < 4; ++i)
		ASSERT_EQ(test_data[i], test_data_input[i]);

	ASSERT_FALSE(bf_read.IsReadFinished());
	ASSERT_TRUE(bf_read);

	bf_read.Close();

	DeleteFile(path);
	ASSERT_FALSE(path.IsExist());
}

TEST(UCS2FileCore, InstanceTest)
{
	FileCore<Char16, UCS2Trait> file;
}

TEST(UCS2FileCore, OpenTest)
{
	Path p_le(SGE_STR("./TestData/TestCommon/TestFile/test_ucs2_le.txt"));
	ASSERT_FALSE(p_le.IsExist());
	Path p_be(SGE_STR("./TestData/TestCommon/TestFile/test_ucs2_be.txt"));
	ASSERT_FALSE(p_be.IsExist());

	UInt8 bom[2] = {0, 0};
	BinaryFile bf_output(p_le, FileIOMode::Write);
	bom[0] = 0xff;
	bom[1] = 0xfe;
	bf_output.Write(bom, sizeof(bom));
	bf_output.Close();
	ASSERT_TRUE(p_le.IsExist());

	bf_output.Open(p_be, FileIOMode::Write);
	bom[0] = 0xfe;
	bom[1] = 0xff;
	bf_output.Write(bom, sizeof(bom));
	bf_output.Close();
	ASSERT_TRUE(p_be.IsExist());

	{
		FileCore<Char16, UCS2Trait> file(p_le, FileIOMode::Read);
		ASSERT_TRUE(file.IsHasBomHeader());
		ASSERT_EQ(file.GetEndian(), Endian::Little);
	}
	FileCore<Char16, UCS2Trait> file;
	file.Open(p_be, FileIOMode::Read);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetEndian(), Endian::Big);
	file.Close();

	file.Open(Path(SGE_STR("./TestData/TestCommon/TestFile/test1.txt")), FileIOMode::Read);
	ASSERT_FALSE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetEndian(), GetSystemEndian());

	DeleteFile(p_le);
	ASSERT_FALSE(p_le.IsExist());
	DeleteFile(p_be);
	ASSERT_FALSE(p_be.IsExist());
}

TEST(UCS2FileCore, SetHasBomHeaderTest)
{
	Path p_le(SGE_STR("./TestData/TestCommon/TestFile/test_ucs2_le_b.txt"));
	ASSERT_FALSE(p_le.IsExist());
	Path p_be(SGE_STR("./TestData/TestCommon/TestFile/test_ucs2_be_b.txt"));
	ASSERT_FALSE(p_be.IsExist());

	UInt8 bom[2] = {0, 0};
	Char16 test_data[] = SGE_WSTR("test测试");
	BinaryFile bf_output(p_le, FileIOMode::Write);
	bom[0] = 0xff;
	bom[1] = 0xfe;
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_data, sizeof(test_data));
	bf_output.Close();
	ASSERT_TRUE(p_le.IsExist());

	bf_output.Open(p_be, FileIOMode::Write);
	bom[0] = 0xfe;
	bom[1] = 0xff;
	bf_output.Write(bom, sizeof(bom));
	bf_output.Close();
	ASSERT_TRUE(p_be.IsExist());

	Char16 test_input_data[sizeof(test_data) / sizeof(Char16)];
	FileCore<Char16, UCS2Trait> file(p_le, FileIOMode::Read | FileIOMode::Write);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	memset(test_input_data, 0, sizeof(test_input_data));
	ASSERT_EQ(file.Read(test_input_data, sizeof(test_input_data)), sizeof(test_input_data));
	ASSERT_EQ(memcmp(test_data, test_input_data, sizeof(test_data)), 0);
	file.SetHasBomHeader(false);
	ASSERT_FALSE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	file.Close();

	file.Open(p_be, FileIOMode::Read | FileIOMode::Write);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetEndian(), Endian::Big);
	file.SetHasBomHeader(false);
	ASSERT_FALSE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetEndian(), Endian::Big);
	file.Close();

	file.Open(p_le, FileIOMode::Read | FileIOMode::Write);
	ASSERT_FALSE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetEndian(), GetSystemEndian());
	memset(test_input_data, 0, sizeof(test_input_data));
	ASSERT_EQ(file.Read(test_input_data, sizeof(test_input_data)), sizeof(test_input_data));
	ASSERT_EQ(memcmp(test_data, test_input_data, sizeof(test_data)), 0);
	file.SetHasBomHeader(true);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetEndian(), GetSystemEndian());
	file.Close();

	file.Open(p_be, FileIOMode::Read | FileIOMode::Write);
	ASSERT_FALSE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetEndian(), GetSystemEndian());
	file.SetHasBomHeader(true);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetEndian(), GetSystemEndian());
	file.Close();

	file.Open(p_le, FileIOMode::Read);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetEndian(), GetSystemEndian());
	memset(test_input_data, 0, sizeof(test_input_data));
	ASSERT_EQ(file.Read(test_input_data, sizeof(test_input_data)), sizeof(test_input_data));
	ASSERT_EQ(memcmp(test_data, test_input_data, sizeof(test_data)), 0);
	file.Close();

	file.Open(p_be, FileIOMode::Read);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetEndian(), GetSystemEndian());
	file.Close();

	DeleteFile(p_le);
	ASSERT_FALSE(p_le.IsExist());
	DeleteFile(p_be);
	ASSERT_FALSE(p_be.IsExist());
}

TEST(UCS2FileCore, SetEndianTest)
{
	Path p_le(SGE_STR("./TestData/TestCommon/TestFile/test_ucs2_le_e.txt"));
	ASSERT_FALSE(p_le.IsExist());
	Path p_be(SGE_STR("./TestData/TestCommon/TestFile/test_ucs2_be_e.txt"));
	ASSERT_FALSE(p_be.IsExist());

	UInt8 bom[2] = {0, 0};
	UInt8 test_data[2] = {0x12, 0x34};
	UInt8 test_data_rev[2] = {0x34, 0x12};
	BinaryFile bf_output(p_le, FileIOMode::Write);
	bom[0] = 0xff;
	bom[1] = 0xfe;
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_data, sizeof(test_data));
	bf_output.Close();
	ASSERT_TRUE(p_le.IsExist());

	bf_output.Open(p_be, FileIOMode::Write);
	bom[0] = 0xfe;
	bom[1] = 0xff;
	bf_output.Write(bom, sizeof(bom));
	bf_output.Close();
	ASSERT_TRUE(p_be.IsExist());

	UInt8 test_input_data[2] = {0, 0};
	FileCore<Char16, UCS2Trait> file(p_le, FileIOMode::Read | FileIOMode::Write);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	memset(test_input_data, 0, sizeof(test_input_data));
	ASSERT_EQ(file.Read(test_input_data, sizeof(test_input_data)), sizeof(test_input_data));
	ASSERT_EQ(memcmp(test_data, test_input_data, sizeof(test_data)), 0);
	file.SetEndian(Endian::Big);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetEndian(), Endian::Big);
	file.Close();

	file.Open(p_be, FileIOMode::Read | FileIOMode::Write);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetEndian(), Endian::Big);
	file.SetEndian(Endian::Little);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	file.Close();

	file.Open(p_le, FileIOMode::Read | FileIOMode::Write);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetEndian(), Endian::Big);
	memset(test_input_data, 0, sizeof(test_input_data));
	ASSERT_EQ(file.Read(test_input_data, sizeof(test_input_data)), sizeof(test_input_data));
	ASSERT_EQ(memcmp(test_data_rev, test_input_data, sizeof(test_data_rev)), 0);
	file.SetEndian(Endian::Little);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	file.Close();

	file.Open(p_be, FileIOMode::Read | FileIOMode::Write);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	file.SetEndian(Endian::Big);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetEndian(), Endian::Big);
	file.Close();

	file.Open(p_le, FileIOMode::Read);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	memset(test_input_data, 0, sizeof(test_input_data));
	ASSERT_EQ(file.Read(test_input_data, sizeof(test_input_data)), sizeof(test_input_data));
	ASSERT_EQ(memcmp(test_data, test_input_data, sizeof(test_data)), 0);
	file.Close();

	file.Open(p_be, FileIOMode::Read);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetEndian(), Endian::Big);
	file.Close();

	DeleteFile(p_le);
	ASSERT_FALSE(p_le.IsExist());
	DeleteFile(p_be);
	ASSERT_FALSE(p_be.IsExist());
}

TEST(UCS2FileCore, ReadCharTest)
{
	Path p_le(SGE_STR("./TestData/TestCommon/TestFile/test_ucs2_le_r.txt"));
	ASSERT_FALSE(p_le.IsExist());
	Path p_be(SGE_STR("./TestData/TestCommon/TestFile/test_ucs2_be_r.txt"));
	ASSERT_FALSE(p_be.IsExist());

	UInt8 bom[2] = {0, 0};
	Char16 test_data = SGE_WSTR('测');
	BinaryFile bf_output(p_le, FileIOMode::Write);
	bom[0] = 0xff;
	bom[1] = 0xfe;
	bf_output.Write(bom, sizeof(bom));
	ChangeEndian(test_data, Endian::Little, GetSystemEndian());
	bf_output.Write(&test_data, sizeof(test_data));
	bf_output.Close();
	ASSERT_TRUE(p_le.IsExist());

	bf_output.Open(p_be, FileIOMode::Write);
	bom[0] = 0xfe;
	bom[1] = 0xff;
	bf_output.Write(bom, sizeof(bom));
	ChangeEndian(test_data, Endian::Big, Endian::Little);
	bf_output.Write(&test_data, sizeof(test_data));
	bf_output.Close();
	ASSERT_TRUE(p_be.IsExist());

	ChangeEndian(test_data, GetSystemEndian(), Endian::Big);
	ASSERT_EQ(test_data, SGE_WSTR('测'));

	FileCore<Char16, UCS2Trait> file;

	file.Open(p_le, FileIOMode::Read);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	ASSERT_EQ(file.ReadChar(), (Pair<Char16, bool>(test_data, true)));
	file.Close();

	file.Open(p_be, FileIOMode::Read);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetEndian(), Endian::Big);
	ASSERT_EQ(file.ReadChar(), (Pair<Char16, bool>(test_data, true)));
	file.Close();

	DeleteFile(p_le);
	ASSERT_FALSE(p_le.IsExist());
	DeleteFile(p_be);
	ASSERT_FALSE(p_be.IsExist());
}

TEST(UCS2FileCore, WriteCharTest)
{
	Path p_le(SGE_STR("./TestData/TestCommon/TestFile/test_ucs2_le_w.txt"));
	ASSERT_FALSE(p_le.IsExist());
	Path p_be(SGE_STR("./TestData/TestCommon/TestFile/test_ucs2_be_w.txt"));
	ASSERT_FALSE(p_be.IsExist());

	UInt8 bom[2] = {0, 0};
	Char16 test_data = SGE_WSTR('测');
	BinaryFile bf_output(p_le, FileIOMode::Write);
	bom[0] = 0xff;
	bom[1] = 0xfe;
	bf_output.Write(bom, sizeof(bom));
	bf_output.Close();
	ASSERT_TRUE(p_le.IsExist());

	bf_output.Open(p_be, FileIOMode::Write);
	bom[0] = 0xfe;
	bom[1] = 0xff;
	bf_output.Write(bom, sizeof(bom));
	bf_output.Close();
	ASSERT_TRUE(p_be.IsExist());

	FileCore<Char16, UCS2Trait> file;

	file.Open(p_le, FileIOMode::Read | FileIOMode::Write);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	ASSERT_TRUE(file.WriteChar(test_data));
	file.Close();

	file.Open(p_be, FileIOMode::Read | FileIOMode::Write);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetEndian(), Endian::Big);
	ASSERT_TRUE(file.WriteChar(test_data));
	file.Close();

	Char16 test_data_input = 0;
	BinaryFile bf_input(p_le, FileIOMode::Read);
	bf_input.MoveFilePosition(FilePositionOrigin::Begin, 2);
	ASSERT_EQ(bf_input.Read(&test_data_input, sizeof(test_data_input)), sizeof(test_data_input));
	ChangeEndian(test_data, Endian::Little, GetSystemEndian());
	ASSERT_EQ(test_data_input, test_data);
	bf_input.Close();

	bf_input.Open(p_be, FileIOMode::Read);
	bf_input.MoveFilePosition(FilePositionOrigin::Begin, 2);
	ASSERT_EQ(bf_input.Read(&test_data_input, sizeof(test_data_input)), sizeof(test_data_input));
	ChangeEndian(test_data, Endian::Big, Endian::Little);
	ASSERT_EQ(test_data_input, test_data);
	bf_input.Close();

	ChangeEndian(test_data, GetSystemEndian(), Endian::Big);
	ASSERT_EQ(test_data, SGE_WSTR('测'));

	DeleteFile(p_le);
	ASSERT_FALSE(p_le.IsExist());
	DeleteFile(p_be);
	ASSERT_FALSE(p_be.IsExist());
}

TEST(UCS2FileCore, SeekTest)
{
	Path p_b(SGE_STR("./TestData/TestCommon/TestFile/test_ucs2_bs.txt"));
	ASSERT_FALSE(p_b.IsExist());
	Path p_nb(SGE_STR("./TestData/TestCommon/TestFile/test_ucs2_nbs.txt"));
	ASSERT_FALSE(p_nb.IsExist());

	UInt8 bom[2] = {0, 0};
	Int8 test_data[4] = {12, 34, 56, 78};
	BinaryFile bf_output(p_b, FileIOMode::Write);
	bom[0] = 0xff;
	bom[1] = 0xfe;
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_data, sizeof(test_data));
	bf_output.Close();
	ASSERT_TRUE(p_b.IsExist());

	bf_output.Open(p_nb, FileIOMode::Write);
	bf_output.Write(test_data, sizeof(test_data));
	bf_output.Close();
	ASSERT_TRUE(p_nb.IsExist());

	Int8 test_data_input;
	FileCore<Char16, UCS2Trait> file(p_b, FileIOMode::Read);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	ASSERT_EQ(file.MoveFilePosition(FilePositionOrigin::Current, 0), 2);
	ASSERT_EQ(file.Read(&test_data_input, sizeof(test_data_input)), sizeof(test_data_input));
	ASSERT_EQ(test_data_input, test_data[0]);
	ASSERT_EQ(file.Seek(FilePositionOrigin::Begin, 2), 2);
	ASSERT_EQ(file.Seek(FilePositionOrigin::Current, 0), 2);
	ASSERT_EQ(file.MoveFilePosition(FilePositionOrigin::Current, 0), 4);
	ASSERT_EQ(file.Read(&test_data_input, sizeof(test_data_input)), sizeof(test_data_input));
	ASSERT_EQ(test_data_input, test_data[2]);
	file.Close();

	file.Open(p_nb, FileIOMode::Read);
	ASSERT_FALSE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetEndian(), GetSystemEndian());
	ASSERT_EQ(file.MoveFilePosition(FilePositionOrigin::Current, 0), 0);
	ASSERT_EQ(file.Read(&test_data_input, sizeof(test_data_input)), sizeof(test_data_input));
	ASSERT_EQ(test_data_input, test_data[0]);
	ASSERT_EQ(file.Seek(FilePositionOrigin::Begin, 2), 2);
	ASSERT_EQ(file.Seek(FilePositionOrigin::Current, 0), 2);
	ASSERT_EQ(file.MoveFilePosition(FilePositionOrigin::Current, 0), 2);
	ASSERT_EQ(file.Read(&test_data_input, sizeof(test_data_input)), sizeof(test_data_input));
	ASSERT_EQ(test_data_input, test_data[2]);
	file.Close();

	DeleteFile(p_b);
	ASSERT_FALSE(p_b.IsExist());
	DeleteFile(p_nb);
	ASSERT_FALSE(p_nb.IsExist());
}

TEST(UTF8FileCore, InstanceTest)
{
	FileCore<char, UTF8Trait> file;
}

TEST(UTF8FileCore, OpenTest)
{
	Path p_b(SGE_STR("./TestData/TestCommon/TestFile/test_utf8_b.txt"));
	ASSERT_FALSE(p_b.IsExist());
	Path p_nb(SGE_STR("./TestData/TestCommon/TestFile/test_utf8_nb.txt"));
	ASSERT_FALSE(p_nb.IsExist());

	UInt8 bom[3] = {0xef, 0xbb, 0xbf};
	BinaryFile bf_output(p_b, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Close();
	ASSERT_TRUE(p_b.IsExist());

	bf_output.Open(p_nb, FileIOMode::Write);
	bf_output.Close();
	ASSERT_TRUE(p_nb.IsExist());

	{
		FileCore<char, UTF8Trait> file(p_b, FileIOMode::Read);
		ASSERT_TRUE(file.IsHasBomHeader());
	}
	{
		FileCore<char, UTF8Trait> file(p_nb, FileIOMode::Read);
		ASSERT_FALSE(file.IsHasBomHeader());
	}
	FileCore<char, UTF8Trait> file;
	file.Open(p_b, FileIOMode::Read);
	ASSERT_TRUE(file.IsHasBomHeader());
	file.Close();
	file.Open(p_nb, FileIOMode::Read);
	ASSERT_FALSE(file.IsHasBomHeader());
	file.Close();

	DeleteFile(p_b);
	ASSERT_FALSE(p_b.IsExist());
	DeleteFile(p_nb);
	ASSERT_FALSE(p_nb.IsExist());
}

TEST(UTF8FileCore, SetHasBomHeaderTest)
{
	Path p_b(SGE_STR("./TestData/TestCommon/TestFile/test_utf8_bs.txt"));
	ASSERT_FALSE(p_b.IsExist());
	Path p_nb(SGE_STR("./TestData/TestCommon/TestFile/test_utf8_nbs.txt"));
	ASSERT_FALSE(p_nb.IsExist());

	UInt8 test_data[4] = {12, 34, 56, 78};
	UInt8 bom[3] = {0xef, 0xbb, 0xbf};
	BinaryFile bf_output(p_b, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_data, sizeof(test_data));
	bf_output.Close();
	ASSERT_TRUE(p_b.IsExist());

	bf_output.Open(p_nb, FileIOMode::Write);
	bf_output.Close();
	ASSERT_TRUE(p_nb.IsExist());

	UInt8 test_data_input[4] = {0, 0, 0, 0};
	FileCore<char, UTF8Trait> file(p_b, FileIOMode::Read | FileIOMode::Write);
	ASSERT_TRUE(file.IsHasBomHeader());
	memset(test_data_input, 0, sizeof(test_data_input));
	ASSERT_EQ(file.Read(test_data_input, sizeof(test_data_input)), sizeof(test_data_input));
	ASSERT_EQ(memcmp(test_data, test_data_input, sizeof(test_data_input)), 0);
	file.SetHasBomHeader(false);
	ASSERT_FALSE(file.IsHasBomHeader());
	file.Close();

	file.Open(p_nb, FileIOMode::Read | FileIOMode::Write);
	ASSERT_FALSE(file.IsHasBomHeader());
	file.SetHasBomHeader(true);
	ASSERT_TRUE(file.IsHasBomHeader());
	file.Close();

	file.Open(p_b, FileIOMode::Read | FileIOMode::Write);
	ASSERT_FALSE(file.IsHasBomHeader());
	memset(test_data_input, 0, sizeof(test_data_input));
	ASSERT_EQ(file.Read(test_data_input, sizeof(test_data_input)), sizeof(test_data_input));
	ASSERT_EQ(memcmp(test_data, test_data_input, sizeof(test_data_input)), 0);
	file.SetHasBomHeader(true);
	ASSERT_TRUE(file.IsHasBomHeader());
	file.Close();

	file.Open(p_nb, FileIOMode::Read | FileIOMode::Write);
	ASSERT_TRUE(file.IsHasBomHeader());
	file.SetHasBomHeader(false);
	ASSERT_FALSE(file.IsHasBomHeader());
	file.Close();

	file.Open(p_b, FileIOMode::Read);
	ASSERT_TRUE(file.IsHasBomHeader());
	memset(test_data_input, 0, sizeof(test_data_input));
	ASSERT_EQ(file.Read(test_data_input, sizeof(test_data_input)), sizeof(test_data_input));
	ASSERT_EQ(memcmp(test_data, test_data_input, sizeof(test_data_input)), 0);
	file.Close();

	file.Open(p_nb, FileIOMode::Read);
	ASSERT_FALSE(file.IsHasBomHeader());
	file.Close();

	DeleteFile(p_b);
	ASSERT_FALSE(p_b.IsExist());
	DeleteFile(p_nb);
	ASSERT_FALSE(p_nb.IsExist());
}

TEST(UTF8FileCore, ReadCharTest)
{
	Path p_b(SGE_STR("./TestData/TestCommon/TestFile/test_utf8_br.txt"));
	ASSERT_FALSE(p_b.IsExist());
	Path p_nb(SGE_STR("./TestData/TestCommon/TestFile/test_utf8_nbr.txt"));
	ASSERT_FALSE(p_nb.IsExist());

	char test_data[] = SGE_U8STR("测");
	UInt8 bom[3] = {0xef, 0xbb, 0xbf};
	BinaryFile bf_output(p_b, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_data, sizeof(test_data));
	bf_output.Close();
	ASSERT_TRUE(p_b.IsExist());

	bf_output.Open(p_nb, FileIOMode::Write);
	bf_output.Write(test_data, sizeof(test_data));
	bf_output.Close();
	ASSERT_TRUE(p_nb.IsExist());

	char test_data_input[sizeof(test_data) / sizeof(char)];
	FileCore<char, UTF8Trait> file(p_b, FileIOMode::Read);
	ASSERT_TRUE(file.IsHasBomHeader());
	memset(test_data_input, 0, sizeof(test_data_input));
	ASSERT_EQ(file.ReadChar(test_data_input), test_data_input + (StringImplement::GetMultipleByteCharSize<char, UTF8Trait>(test_data)));
	ASSERT_EQ(memcmp(test_data, test_data_input, sizeof(test_data_input)), 0);
	file.Close();

	file.Open(p_nb, FileIOMode::Read);
	ASSERT_FALSE(file.IsHasBomHeader());
	memset(test_data_input, 0, sizeof(test_data_input));
	ASSERT_EQ(file.ReadChar(test_data_input), test_data_input + (StringImplement::GetMultipleByteCharSize<char, UTF8Trait>(test_data)));
	ASSERT_EQ(memcmp(test_data, test_data_input, sizeof(test_data_input)), 0);
	file.Close();

	DeleteFile(p_b);
	ASSERT_FALSE(p_b.IsExist());
	DeleteFile(p_nb);
	ASSERT_FALSE(p_nb.IsExist());
}

TEST(UTF8FileCore, WriteCharTest)
{
	Path p_b(SGE_STR("./TestData/TestCommon/TestFile/test_utf8_bw.txt"));
	ASSERT_FALSE(p_b.IsExist());
	Path p_nb(SGE_STR("./TestData/TestCommon/TestFile/test_utf8_nbw.txt"));
	ASSERT_FALSE(p_nb.IsExist());

	char test_data[] = SGE_U8STR("测");
	UInt8 bom[3] = {0xef, 0xbb, 0xbf};
	BinaryFile bf_output(p_b, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Close();
	ASSERT_TRUE(p_b.IsExist());

	bf_output.Open(p_nb, FileIOMode::Write);
	bf_output.Close();
	ASSERT_TRUE(p_nb.IsExist());

	FileCore<char, UTF8Trait> file(p_b, FileIOMode::Read | FileIOMode::Write);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.WriteChar(test_data), test_data + (StringImplement::GetMultipleByteCharSize<char, UTF8Trait>(test_data)));
	file.Close();

	file.Open(p_nb, FileIOMode::Read | FileIOMode::Write);
	ASSERT_FALSE(file.IsHasBomHeader());
	ASSERT_EQ(file.WriteChar(test_data), test_data + (StringImplement::GetMultipleByteCharSize<char, UTF8Trait>(test_data)));
	file.Close();

	char test_data_input[sizeof(test_data) / sizeof(char)];
	BinaryFile bf_input(p_b, FileIOMode::Read);
	bf_input.MoveFilePosition(FilePositionOrigin::Begin, 3);
	memset(test_data_input, 0, sizeof(test_data_input));
	ASSERT_EQ(bf_input.Read(test_data_input, sizeof(test_data_input)), sizeof(test_data) - 1);
	ASSERT_EQ(memcmp(test_data, test_data_input, sizeof(test_data)), 0);
	bf_input.Close();

	bf_input.Open(p_nb, FileIOMode::Read);
	memset(test_data_input, 0, sizeof(test_data_input));
	ASSERT_EQ(bf_input.Read(test_data_input, sizeof(test_data_input)), sizeof(test_data) - 1);
	ASSERT_EQ(memcmp(test_data, test_data_input, sizeof(test_data)), 0);
	bf_input.Close();

	DeleteFile(p_b);
	ASSERT_FALSE(p_b.IsExist());
	DeleteFile(p_nb);
	ASSERT_FALSE(p_nb.IsExist());
}

TEST(UTF8FileCore, SeekTest)
{
	Path p_b(SGE_STR("./TestData/TestCommon/TestFile/test_utf8_bsk.txt"));
	ASSERT_FALSE(p_b.IsExist());
	Path p_nb(SGE_STR("./TestData/TestCommon/TestFile/test_utf8_nbsk.txt"));
	ASSERT_FALSE(p_nb.IsExist());

	char test_data[] = SGE_U8STR("测");
	UInt8 bom[3] = {0xef, 0xbb, 0xbf};
	BinaryFile bf_output(p_b, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_data, sizeof(test_data));
	bf_output.Close();
	ASSERT_TRUE(p_b.IsExist());

	bf_output.Open(p_nb, FileIOMode::Write);
	bf_output.Write(test_data, sizeof(test_data));
	bf_output.Close();
	ASSERT_TRUE(p_nb.IsExist());

	char test_data_input[sizeof(test_data) / sizeof(char)];
	FileCore<char, UTF8Trait> file(p_b, FileIOMode::Read);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.MoveFilePosition(FilePositionOrigin::Current, 0), 3);
	memset(test_data_input, 0, sizeof(test_data_input));
	file.ReadChar(test_data_input);
	ASSERT_EQ(memcmp(test_data, test_data_input, sizeof(test_data_input)), 0);
	ASSERT_EQ(file.Seek(FilePositionOrigin::Begin, 1), 1);
	ASSERT_EQ(file.Seek(FilePositionOrigin::Current, 0), 1);
	ASSERT_EQ(file.MoveFilePosition(FilePositionOrigin::Current, 0), 4);
	memset(test_data_input, 0, sizeof(test_data_input));
	ASSERT_EQ(file.Read(test_data_input, sizeof(test_data) - 1), sizeof(test_data) - 1);
	ASSERT_EQ(memcmp(test_data + 1, test_data_input, sizeof(test_data) - 1), 0);
	file.Close();

	file.Open(p_nb, FileIOMode::Read);
	ASSERT_FALSE(file.IsHasBomHeader());
	ASSERT_EQ(file.MoveFilePosition(FilePositionOrigin::Current, 0), 0);
	memset(test_data_input, 0, sizeof(test_data_input));
	file.ReadChar(test_data_input);
	ASSERT_EQ(memcmp(test_data, test_data_input, sizeof(test_data_input)), 0);
	ASSERT_EQ(file.Seek(FilePositionOrigin::Begin, 1), 1);
	ASSERT_EQ(file.Seek(FilePositionOrigin::Current, 0), 1);
	ASSERT_EQ(file.MoveFilePosition(FilePositionOrigin::Current, 0), 1);
	memset(test_data_input, 0, sizeof(test_data_input));
	ASSERT_EQ(file.Read(test_data_input, sizeof(test_data) - 1), sizeof(test_data) - 1);
	ASSERT_EQ(memcmp(test_data + 1, test_data_input, sizeof(test_data) - 1), 0);
	file.Close();

	DeleteFile(p_b);
	ASSERT_FALSE(p_b.IsExist());
	DeleteFile(p_nb);
	ASSERT_FALSE(p_nb.IsExist());
}

TEST(UCS2File, InstanceTest)
{
	UCS2File file;
}

TEST(UCS2File, OpenTest)
{
	Path p_cr(SGE_STR("./TestData/TestCommon/TestFile/test_usc2_cr.txt"));
	ASSERT_FALSE(p_cr.IsExist());
	Path p_lf(SGE_STR("./TestData/TestCommon/TestFile/test_usc2_lf.txt"));
	ASSERT_FALSE(p_lf.IsExist());
	Path p_crlf(SGE_STR("./TestData/TestCommon/TestFile/test_usc2_crlf.txt"));
	ASSERT_FALSE(p_crlf.IsExist());
	Path p_nbk(SGE_STR("./TestData/TestCommon/TestFile/test_usc2_nbk.txt"));
	ASSERT_FALSE(p_nbk.IsExist());

	BinaryFile bf_output;
	UInt8 bom[2] = {0xff, 0xfe};
	Char16 test_str[] = SGE_WSTR("this is test 测试");
	Char16 test_input[sizeof(test_str) / sizeof(Char16)];
	Char16 cr[] = SGE_WSTR("\r");
	Char16 crlf[] = SGE_WSTR("\r\n");
	Char16 lf[] = SGE_WSTR("\n");

	bf_output.Open(p_cr, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_str, sizeof(test_str) - sizeof(Char16));
	bf_output.Write(cr, sizeof(cr) - sizeof(Char16));
	bf_output.Close();
	ASSERT_TRUE(p_cr.IsExist());

	bf_output.Open(p_lf, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_str, sizeof(test_str) - sizeof(Char16));
	bf_output.Write(lf, sizeof(lf) - sizeof(Char16));
	bf_output.Close();
	ASSERT_TRUE(p_lf.IsExist());

	bf_output.Open(p_crlf, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_str, sizeof(test_str) - sizeof(Char16));
	bf_output.Write(crlf, sizeof(crlf) - sizeof(Char16));
	bf_output.Close();
	ASSERT_TRUE(p_crlf.IsExist());

	bf_output.Open(p_nbk, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_str, sizeof(test_str) - sizeof(Char16));
	bf_output.Close();
	ASSERT_TRUE(p_nbk.IsExist());

	{
		UCS2File file(p_cr, FileIOMode::Read);
		ASSERT_EQ(file.GetEndian(), Endian::Little);
		ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CR);
		memset(test_input, 0, sizeof(test_input));
		ASSERT_EQ(file.Read(test_input, sizeof(test_input) - sizeof(Char16)), sizeof(test_input) - sizeof(Char16));
		ASSERT_EQ(memcmp(test_input, test_str, sizeof(test_input) - sizeof(Char16)), 0);
	}
	UCS2File file(p_lf, FileIOMode::Read);
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::LF);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(test_input) - sizeof(Char16)), sizeof(test_input) - sizeof(Char16));
	ASSERT_EQ(memcmp(test_input, test_str, sizeof(test_input) - sizeof(Char16)), 0);
	file.Close();

	file.Open(p_crlf, FileIOMode::Read);
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CRLF);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(test_input) - sizeof(Char16)), sizeof(test_input) - sizeof(Char16));
	ASSERT_EQ(memcmp(test_input, test_str, sizeof(test_input) - sizeof(Char16)), 0);
	file.Close();

	file.Open(p_nbk, FileIOMode::Read);
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::Unknown);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(test_input) - sizeof(Char16)), sizeof(test_input) - sizeof(Char16));
	ASSERT_EQ(memcmp(test_input, test_str, sizeof(test_input) - sizeof(Char16)), 0);
	file.Close();

	DeleteFile(p_cr);
	ASSERT_FALSE(p_cr.IsExist());
	DeleteFile(p_lf);
	ASSERT_FALSE(p_lf.IsExist());
	DeleteFile(p_crlf);
	ASSERT_FALSE(p_crlf.IsExist());
	DeleteFile(p_nbk);
	ASSERT_FALSE(p_nbk.IsExist());
}

TEST(UCS2File, SetFileLineBreakTest)
{
	Path p_cr(SGE_STR("./TestData/TestCommon/TestFile/test_usc2_crs.txt"));
	ASSERT_FALSE(p_cr.IsExist());
	Path p_lf(SGE_STR("./TestData/TestCommon/TestFile/test_usc2_lfs.txt"));
	ASSERT_FALSE(p_lf.IsExist());
	Path p_crlf(SGE_STR("./TestData/TestCommon/TestFile/test_usc2_crlfs.txt"));
	ASSERT_FALSE(p_crlf.IsExist());

	BinaryFile bf_output;
	UInt8 bom[2] = {0xff, 0xfe};
	Char16 test_str[] = SGE_WSTR("this is test 测试");
	Char16 test_str2[] = SGE_WSTR("file end?");
	Char16 test_input[sizeof(test_str) / sizeof(Char16)];
	Char16 cr[] = SGE_WSTR("\r");
	Char16 crlf[] = SGE_WSTR("\r\n");
	Char16 lf[] = SGE_WSTR("\n");

	bf_output.Open(p_cr, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_str, sizeof(test_str) - sizeof(Char16));
	bf_output.Write(cr, sizeof(cr) - sizeof(Char16));
	bf_output.Write(test_str2, sizeof(test_str2) - sizeof(Char16));
	bf_output.Close();
	ASSERT_TRUE(p_cr.IsExist());

	bf_output.Open(p_lf, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_str, sizeof(test_str) - sizeof(Char16));
	bf_output.Write(lf, sizeof(lf) - sizeof(Char16));
	bf_output.Write(test_str2, sizeof(test_str2) - sizeof(Char16));
	bf_output.Close();
	ASSERT_TRUE(p_lf.IsExist());

	bf_output.Open(p_crlf, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_str, sizeof(test_str) - sizeof(Char16));
	bf_output.Write(crlf, sizeof(crlf) - sizeof(Char16));
	bf_output.Write(test_str2, sizeof(test_str2) - sizeof(Char16));
	bf_output.Close();
	ASSERT_TRUE(p_crlf.IsExist());

	UCS2File file(p_cr, FileIOMode::Read | FileIOMode::Write);
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CR);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(test_input) - sizeof(Char16)), sizeof(test_input) - sizeof(Char16));
	ASSERT_EQ(memcmp(test_input, test_str, sizeof(test_input) - sizeof(Char16)), 0);
	file.SetFileLineBreak(FileLineBreak::LF);
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::LF);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(lf) - sizeof(Char16)), sizeof(lf) - sizeof(Char16));
	ASSERT_EQ(memcmp(test_input, lf, sizeof(lf) - sizeof(Char16)), 0);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(test_str2) - sizeof(Char16)), sizeof(test_str2) - sizeof(Char16));
	ASSERT_EQ(memcmp(test_input, test_str2, sizeof(test_str2) - sizeof(Char16)), 0);
	file.Close();

	file.Open(p_lf, FileIOMode::Read | FileIOMode::Write);
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::LF);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(test_input) - sizeof(Char16)), sizeof(test_input) - sizeof(Char16));
	ASSERT_EQ(memcmp(test_input, test_str, sizeof(test_input) - sizeof(Char16)), 0);
	file.SetFileLineBreak(FileLineBreak::CRLF);
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CRLF);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(crlf) - sizeof(Char16)), sizeof(crlf) - sizeof(Char16));
	ASSERT_EQ(memcmp(test_input, crlf, sizeof(crlf) - sizeof(Char16)), 0);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(test_str2) - sizeof(Char16)), sizeof(test_str2) - sizeof(Char16));
	ASSERT_EQ(memcmp(test_input, test_str2, sizeof(test_str2) - sizeof(Char16)), 0);
	file.Close();

	file.Open(p_crlf, FileIOMode::Read | FileIOMode::Write);
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CRLF);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(test_input) - sizeof(Char16)), sizeof(test_input) - sizeof(Char16));
	ASSERT_EQ(memcmp(test_input, test_str, sizeof(test_input) - sizeof(Char16)), 0);
	file.SetFileLineBreak(FileLineBreak::CR);
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CR);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(cr) - sizeof(Char16)), sizeof(cr) - sizeof(Char16));
	ASSERT_EQ(memcmp(test_input, cr, sizeof(cr) - sizeof(Char16)), 0);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(test_str2) - sizeof(Char16)), sizeof(test_str2) - sizeof(Char16));
	ASSERT_EQ(memcmp(test_input, test_str2, sizeof(test_str2) - sizeof(Char16)), 0);
	file.Close();

	file.Open(p_cr, FileIOMode::Read);
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::LF);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(test_input) - sizeof(Char16)), sizeof(test_input) - sizeof(Char16));
	ASSERT_EQ(memcmp(test_input, test_str, sizeof(test_input) - sizeof(Char16)), 0);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(lf) - sizeof(Char16)), sizeof(lf) - sizeof(Char16));
	ASSERT_EQ(memcmp(test_input, lf, sizeof(lf) - sizeof(Char16)), 0);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(test_str2) - sizeof(Char16)), sizeof(test_str2) - sizeof(Char16));
	ASSERT_EQ(memcmp(test_input, test_str2, sizeof(test_str2) - sizeof(Char16)), 0);
	file.Close();

	file.Open(p_lf, FileIOMode::Read);
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CRLF);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(test_input) - sizeof(Char16)), sizeof(test_input) - sizeof(Char16));
	ASSERT_EQ(memcmp(test_input, test_str, sizeof(test_input) - sizeof(Char16)), 0);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(crlf) - sizeof(Char16)), sizeof(crlf) - sizeof(Char16));
	ASSERT_EQ(memcmp(test_input, crlf, sizeof(crlf) - sizeof(Char16)), 0);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(test_str2) - sizeof(Char16)), sizeof(test_str2) - sizeof(Char16));
	ASSERT_EQ(memcmp(test_input, test_str2, sizeof(test_str2) - sizeof(Char16)), 0);
	file.Close();

	file.Open(p_crlf, FileIOMode::Read);
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CR);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(test_input) - sizeof(Char16)), sizeof(test_input) - sizeof(Char16));
	ASSERT_EQ(memcmp(test_input, test_str, sizeof(test_input) - sizeof(Char16)), 0);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(cr) - sizeof(Char16)), sizeof(cr) - sizeof(Char16));
	ASSERT_EQ(memcmp(test_input, cr, sizeof(cr) - sizeof(Char16)), 0);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(test_str2) - sizeof(Char16)), sizeof(test_str2) - sizeof(Char16));
	ASSERT_EQ(memcmp(test_input, test_str2, sizeof(test_str2) - sizeof(Char16)), 0);
	file.Close();

	DeleteFile(p_cr);
	ASSERT_FALSE(p_cr.IsExist());
	DeleteFile(p_lf);
	ASSERT_FALSE(p_lf.IsExist());
	DeleteFile(p_crlf);
	ASSERT_FALSE(p_crlf.IsExist());
}

TEST(UCS2File, ReadLineTest)
{
	Path p_cr(SGE_STR("./TestData/TestCommon/TestFile/test_usc2_crr.txt"));
	ASSERT_FALSE(p_cr.IsExist());
	Path p_lf(SGE_STR("./TestData/TestCommon/TestFile/test_usc2_lfr.txt"));
	ASSERT_FALSE(p_lf.IsExist());
	Path p_crlf(SGE_STR("./TestData/TestCommon/TestFile/test_usc2_crlfr.txt"));
	ASSERT_FALSE(p_crlf.IsExist());

	BinaryFile bf_output;
	UInt8 bom[2] = {0xff, 0xfe};
	Char16 test_str[] = SGE_WSTR("this is test 测试");
	Char16 test_str2[] = SGE_WSTR("file end?");
	Char16 cr[] = SGE_WSTR("\r");
	Char16 crlf[] = SGE_WSTR("\r\n");
	Char16 lf[] = SGE_WSTR("\n");

	bf_output.Open(p_cr, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_str, sizeof(test_str) - sizeof(Char16));
	bf_output.Write(cr, sizeof(cr) - sizeof(Char16));
	bf_output.Write(test_str2, sizeof(test_str2) - sizeof(Char16));
	bf_output.Close();
	ASSERT_TRUE(p_cr.IsExist());

	bf_output.Open(p_lf, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_str, sizeof(test_str) - sizeof(Char16));
	bf_output.Write(lf, sizeof(lf) - sizeof(Char16));
	bf_output.Write(test_str2, sizeof(test_str2) - sizeof(Char16));
	bf_output.Close();
	ASSERT_TRUE(p_lf.IsExist());

	bf_output.Open(p_crlf, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_str, sizeof(test_str) - sizeof(Char16));
	bf_output.Write(crlf, sizeof(crlf) - sizeof(Char16));
	bf_output.Write(test_str2, sizeof(test_str2) - sizeof(Char16));
	bf_output.Close();
	ASSERT_TRUE(p_crlf.IsExist());

	UCS2File file(p_cr, FileIOMode::Read);
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CR);
	ASSERT_EQ(file.ReadLine(), test_str);
	ASSERT_EQ(file.ReadLine(), test_str2);
	file.Close();

	file.Open(p_lf, FileIOMode::Read);
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::LF);
	ASSERT_EQ(file.ReadLine(), test_str);
	ASSERT_EQ(file.ReadLine(), test_str2);
	file.Close();

	file.Open(p_crlf, FileIOMode::Read);
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CRLF);
	ASSERT_EQ(file.ReadLine(), test_str);
	ASSERT_EQ(file.ReadLine(), test_str2);
	file.Close();

	DeleteFile(p_cr);
	ASSERT_FALSE(p_cr.IsExist());
	DeleteFile(p_lf);
	ASSERT_FALSE(p_lf.IsExist());
	DeleteFile(p_crlf);
	ASSERT_FALSE(p_crlf.IsExist());
}

TEST(UCS2File, ReadWordTest)
{
	Path p_cr(SGE_STR("./TestData/TestCommon/TestFile/test_usc2_crw.txt"));
	ASSERT_FALSE(p_cr.IsExist());
	Path p_lf(SGE_STR("./TestData/TestCommon/TestFile/test_usc2_lfw.txt"));
	ASSERT_FALSE(p_lf.IsExist());
	Path p_crlf(SGE_STR("./TestData/TestCommon/TestFile/test_usc2_crlfw.txt"));
	ASSERT_FALSE(p_crlf.IsExist());

	BinaryFile bf_output;
	UInt8 bom[2] = {0xff, 0xfe};
	Char16 test_str[] = SGE_WSTR("this is test 测试");
	Char16 test_str2[] = SGE_WSTR("file end?");
	Char16 cr[] = SGE_WSTR("\r");
	Char16 crlf[] = SGE_WSTR("\r\n");
	Char16 lf[] = SGE_WSTR("\n");

	bf_output.Open(p_cr, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_str, sizeof(test_str) - sizeof(Char16));
	bf_output.Write(cr, sizeof(cr) - sizeof(Char16));
	bf_output.Write(test_str2, sizeof(test_str2) - sizeof(Char16));
	bf_output.Close();
	ASSERT_TRUE(p_cr.IsExist());

	bf_output.Open(p_lf, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_str, sizeof(test_str) - sizeof(Char16));
	bf_output.Write(lf, sizeof(lf) - sizeof(Char16));
	bf_output.Write(test_str2, sizeof(test_str2) - sizeof(Char16));
	bf_output.Close();
	ASSERT_TRUE(p_lf.IsExist());

	bf_output.Open(p_crlf, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_str, sizeof(test_str) - sizeof(Char16));
	bf_output.Write(crlf, sizeof(crlf) - sizeof(Char16));
	bf_output.Write(test_str2, sizeof(test_str2) - sizeof(Char16));
	bf_output.Close();
	ASSERT_TRUE(p_crlf.IsExist());

	UCS2File file(p_cr, FileIOMode::Read);
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CR);
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("this"));
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("is"));
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("test"));
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("测试"));
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("file"));
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("end?"));
	file.Close();

	file.Open(p_lf, FileIOMode::Read);
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::LF);
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("this"));
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("is"));
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("test"));
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("测试"));
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("file"));
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("end?"));
	file.Close();

	file.Open(p_crlf, FileIOMode::Read);
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CRLF);
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("this"));
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("is"));
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("test"));
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("测试"));
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("file"));
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("end?"));
	file.Close();

	DeleteFile(p_cr);
	ASSERT_FALSE(p_cr.IsExist());
	DeleteFile(p_lf);
	ASSERT_FALSE(p_lf.IsExist());
	DeleteFile(p_crlf);
	ASSERT_FALSE(p_crlf.IsExist());
}

TEST(UCS2File, ReadOperatorTest)
{
	Path p_cr(SGE_STR("./TestData/TestCommon/TestFile/test_usc2_cro.txt"));
	ASSERT_FALSE(p_cr.IsExist());
	Path p_lf(SGE_STR("./TestData/TestCommon/TestFile/test_usc2_lfo.txt"));
	ASSERT_FALSE(p_lf.IsExist());
	Path p_crlf(SGE_STR("./TestData/TestCommon/TestFile/test_usc2_crlfo.txt"));
	ASSERT_FALSE(p_crlf.IsExist());

	BinaryFile bf_output;
	UInt8 bom[2] = {0xff, 0xfe};
	Char16 test_str[] = SGE_WSTR("1 1.5 2.5");
	Char16 test_str2[] = SGE_WSTR("str");
	Char16 cr[] = SGE_WSTR("\r");
	Char16 crlf[] = SGE_WSTR("\r\n");
	Char16 lf[] = SGE_WSTR("\n");

	bf_output.Open(p_cr, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_str, sizeof(test_str) - sizeof(Char16));
	bf_output.Write(cr, sizeof(cr) - sizeof(Char16));
	bf_output.Write(test_str2, sizeof(test_str2) - sizeof(Char16));
	bf_output.Write(cr, sizeof(cr) - sizeof(Char16));
	bf_output.Close();
	ASSERT_TRUE(p_cr.IsExist());

	bf_output.Open(p_lf, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_str, sizeof(test_str) - sizeof(Char16));
	bf_output.Write(lf, sizeof(lf) - sizeof(Char16));
	bf_output.Write(test_str2, sizeof(test_str2) - sizeof(Char16));
	bf_output.Write(lf, sizeof(lf) - sizeof(Char16));
	bf_output.Close();
	ASSERT_TRUE(p_lf.IsExist());

	bf_output.Open(p_crlf, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_str, sizeof(test_str) - sizeof(Char16));
	bf_output.Write(crlf, sizeof(crlf) - sizeof(Char16));
	bf_output.Write(test_str2, sizeof(test_str2) - sizeof(Char16));
	bf_output.Write(crlf, sizeof(crlf) - sizeof(Char16));
	bf_output.Close();
	ASSERT_TRUE(p_crlf.IsExist());

	int ival = 0;
	float fval = 0.0f;
	double dval = 0.0f;
	UCS2String sval;
	int uval = 5;

	UCS2File file(p_cr, FileIOMode::Read);
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CR);
	ASSERT_TRUE(file >> ival >> fval >> dval >> sval);
	ASSERT_FALSE(file >> uval);
	ASSERT_EQ(ival, 1);
	ASSERT_EQ(fval, 1.5f);
	ASSERT_EQ(dval, 2.5);
	ASSERT_EQ(sval, SGE_WSTR("str"));
	ASSERT_EQ(uval, 5);
	file.Close();

	file.Open(p_lf, FileIOMode::Read);
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::LF);
	ival = 0;
	fval = 0;
	dval = 0;
	sval.Clear();
	ASSERT_TRUE(file >> ival >> fval >> dval >> sval);
	ASSERT_FALSE(file >> uval);
	ASSERT_EQ(ival, 1);
	ASSERT_EQ(fval, 1.5f);
	ASSERT_EQ(dval, 2.5);
	ASSERT_EQ(sval, SGE_WSTR("str"));
	ASSERT_EQ(uval, 5);
	file.Close();

	file.Open(p_crlf, FileIOMode::Read);
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CRLF);
	ival = 0;
	fval = 0;
	dval = 0;
	sval.Clear();
	ASSERT_TRUE(file >> ival >> fval >> dval >> sval);
	ASSERT_FALSE(file >> uval);
	ASSERT_EQ(ival, 1);
	ASSERT_EQ(fval, 1.5f);
	ASSERT_EQ(dval, 2.5);
	ASSERT_EQ(sval, SGE_WSTR("str"));
	ASSERT_EQ(uval, 5);
	file.Close();

	DeleteFile(p_cr);
	ASSERT_FALSE(p_cr.IsExist());
	DeleteFile(p_lf);
	ASSERT_FALSE(p_lf.IsExist());
	DeleteFile(p_crlf);
	ASSERT_FALSE(p_crlf.IsExist());
}

TEST(UCS2File, WriteStringTest)
{
	Path p_cr(SGE_STR("./TestData/TestCommon/TestFile/test_usc2_crww.txt"));
	ASSERT_FALSE(p_cr.IsExist());
	Path p_lf(SGE_STR("./TestData/TestCommon/TestFile/test_usc2_lfww.txt"));
	ASSERT_FALSE(p_lf.IsExist());
	Path p_crlf(SGE_STR("./TestData/TestCommon/TestFile/test_usc2_crlfww.txt"));
	ASSERT_FALSE(p_crlf.IsExist());

	Char16 test_str[] = SGE_WSTR("this is test 测试");
	Char16 test_str2[] = SGE_WSTR("file end?");
	Char16 cr[] = SGE_WSTR("\r");
	Char16 crlf[] = SGE_WSTR("\r\n");
	Char16 lf[] = SGE_WSTR("\n");

	UCS2File output_file;
	output_file.Open(p_cr, FileIOMode::Write);
	output_file.SetEndian(Endian::Little);
	output_file.WriteString(test_str);
	output_file.WriteString(cr);
	output_file.WriteString(test_str2);
	output_file.Close();
	ASSERT_TRUE(p_cr.IsExist());

	output_file.Open(p_lf, FileIOMode::Write);
	output_file.SetEndian(Endian::Little);
	output_file.WriteString(test_str);
	output_file.WriteString(lf);
	output_file.WriteString(test_str2);
	output_file.Close();
	ASSERT_TRUE(p_lf.IsExist());

	output_file.Open(p_crlf, FileIOMode::Write);
	output_file.SetEndian(Endian::Little);
	output_file.WriteString(test_str);
	output_file.WriteString(crlf);
	output_file.WriteString(test_str2);
	output_file.Close();
	ASSERT_TRUE(p_crlf.IsExist());

	UCS2File file(p_cr, FileIOMode::Read);
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CR);
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("this"));
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("is"));
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("test"));
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("测试"));
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("file"));
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("end?"));
	file.Close();

	file.Open(p_lf, FileIOMode::Read);
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::LF);
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("this"));
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("is"));
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("test"));
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("测试"));
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("file"));
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("end?"));
	file.Close();

	file.Open(p_crlf, FileIOMode::Read);
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CRLF);
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("this"));
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("is"));
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("test"));
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("测试"));
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("file"));
	ASSERT_EQ(file.ReadWord(), SGE_WSTR("end?"));
	file.Close();

	DeleteFile(p_cr);
	ASSERT_FALSE(p_cr.IsExist());
	DeleteFile(p_lf);
	ASSERT_FALSE(p_lf.IsExist());
	DeleteFile(p_crlf);
	ASSERT_FALSE(p_crlf.IsExist());
}

TEST(UCS2File, WriteOperatorTest)
{
	Path p_cr(SGE_STR("./TestData/TestCommon/TestFile/test_usc2_crwo.txt"));
	ASSERT_FALSE(p_cr.IsExist());
	Path p_lf(SGE_STR("./TestData/TestCommon/TestFile/test_usc2_lfwo.txt"));
	ASSERT_FALSE(p_lf.IsExist());
	Path p_crlf(SGE_STR("./TestData/TestCommon/TestFile/test_usc2_crlfwo.txt"));
	ASSERT_FALSE(p_crlf.IsExist());

	UCS2File output_file;
	output_file.Open(p_cr, FileIOMode::Read | FileIOMode::Write);
	output_file.SetEndian(Endian::Little);
	output_file.SetFileLineBreak(FileLineBreak::CR);
	output_file << 1 << SGE_WSTR(" ") << 1.5f << SGE_WSTR(" ") << 2.5f << EndLine;
	output_file << SGE_WSTR("str") << EndLine;
	output_file.Close();
	ASSERT_TRUE(p_cr.IsExist());

	output_file.Open(p_lf, FileIOMode::Read | FileIOMode::Write);
	output_file.SetEndian(Endian::Little);
	output_file.SetFileLineBreak(FileLineBreak::LF);
	output_file << 1 << SGE_WSTR(" ") << 1.5f << SGE_WSTR(" ") << 2.5f << EndLine;
	output_file << SGE_WSTR("str") << EndLine;
	output_file.Close();
	ASSERT_TRUE(p_lf.IsExist());

	output_file.Open(p_crlf, FileIOMode::Read | FileIOMode::Write);
	output_file.SetEndian(Endian::Little);
	output_file.SetFileLineBreak(FileLineBreak::CRLF);
	output_file << 1 << SGE_WSTR(" ") << 1.5f << SGE_WSTR(" ") << 2.5f << EndLine;
	output_file << SGE_WSTR("str") << EndLine;
	output_file.Close();
	ASSERT_TRUE(p_crlf.IsExist());

	int ival = 0;
	float fval = 0.0f;
	double dval = 0.0f;
	UCS2String sval;
	int uval = 5;

	UCS2File file(p_cr, FileIOMode::Read);
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CR);
	ASSERT_TRUE(file >> ival >> fval >> dval >> sval);
	ASSERT_FALSE(file >> uval);
	ASSERT_EQ(ival, 1);
	ASSERT_EQ(fval, 1.5f);
	ASSERT_EQ(dval, 2.5);
	ASSERT_EQ(sval, SGE_WSTR("str"));
	ASSERT_EQ(uval, 5);
	file.Close();

	file.Open(p_lf, FileIOMode::Read);
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::LF);
	ival = 0;
	fval = 0;
	dval = 0;
	sval.Clear();
	ASSERT_TRUE(file >> ival >> fval >> dval >> sval);
	ASSERT_FALSE(file >> uval);
	ASSERT_EQ(ival, 1);
	ASSERT_EQ(fval, 1.5f);
	ASSERT_EQ(dval, 2.5);
	ASSERT_EQ(sval, SGE_WSTR("str"));
	ASSERT_EQ(uval, 5);
	file.Close();

	file.Open(p_crlf, FileIOMode::Read);
	ASSERT_EQ(file.GetEndian(), Endian::Little);
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CRLF);
	ival = 0;
	fval = 0;
	dval = 0;
	sval.Clear();
	ASSERT_TRUE(file >> ival >> fval >> dval >> sval);
	ASSERT_FALSE(file >> uval);
	ASSERT_EQ(ival, 1);
	ASSERT_EQ(fval, 1.5f);
	ASSERT_EQ(dval, 2.5);
	ASSERT_EQ(sval, SGE_WSTR("str"));
	ASSERT_EQ(uval, 5);
	file.Close();

	DeleteFile(p_cr);
	ASSERT_FALSE(p_cr.IsExist());
	DeleteFile(p_lf);
	ASSERT_FALSE(p_lf.IsExist());
	DeleteFile(p_crlf);
	ASSERT_FALSE(p_crlf.IsExist());
}

TEST(UTF8File, InstanceTest)
{
	UTF8File file;
}

TEST(UTF8File, OpenTest)
{
	Path p_cr(SGE_STR("./TestData/TestCommon/TestFile/test_utf8_cr.txt"));
	ASSERT_FALSE(p_cr.IsExist());
	Path p_lf(SGE_STR("./TestData/TestCommon/TestFile/test_utf8_lf.txt"));
	ASSERT_FALSE(p_lf.IsExist());
	Path p_crlf(SGE_STR("./TestData/TestCommon/TestFile/test_utf8_crlf.txt"));
	ASSERT_FALSE(p_crlf.IsExist());
	Path p_nbk(SGE_STR("./TestData/TestCommon/TestFile/test_utf8_nbk.txt"));
	ASSERT_FALSE(p_nbk.IsExist());

	BinaryFile bf_output;
	UInt8 bom[3] = {0xef, 0xbb, 0xbf};
	char test_str[] = SGE_U8STR("this is test 测试");
	char test_input[sizeof(test_str) / sizeof(char)];
	char cr[] = SGE_U8STR("\r");
	char crlf[] = SGE_U8STR("\r\n");
	char lf[] = SGE_U8STR("\n");

	bf_output.Open(p_cr, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_str, sizeof(test_str) - sizeof(char));
	bf_output.Write(cr, sizeof(cr) - sizeof(char));
	bf_output.Close();
	ASSERT_TRUE(p_cr.IsExist());

	bf_output.Open(p_lf, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_str, sizeof(test_str) - sizeof(char));
	bf_output.Write(lf, sizeof(lf) - sizeof(char));
	bf_output.Close();
	ASSERT_TRUE(p_lf.IsExist());

	bf_output.Open(p_crlf, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_str, sizeof(test_str) - sizeof(char));
	bf_output.Write(crlf, sizeof(crlf) - sizeof(char));
	bf_output.Close();
	ASSERT_TRUE(p_crlf.IsExist());

	bf_output.Open(p_nbk, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_str, sizeof(test_str) - sizeof(char));
	bf_output.Close();
	ASSERT_TRUE(p_nbk.IsExist());

	{
		UTF8File file(p_cr, FileIOMode::Read);
		ASSERT_TRUE(file.IsHasBomHeader());
		ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CR);
		memset(test_input, 0, sizeof(test_input));
		ASSERT_EQ(file.Read(test_input, sizeof(test_input) - sizeof(char)), sizeof(test_input) - sizeof(char));
		ASSERT_EQ(memcmp(test_input, test_str, sizeof(test_input) - sizeof(char)), 0);
	}
	UTF8File file(p_lf, FileIOMode::Read);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::LF);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(test_input) - sizeof(char)), sizeof(test_input) - sizeof(char));
	ASSERT_EQ(memcmp(test_input, test_str, sizeof(test_input) - sizeof(char)), 0);
	file.Close();

	file.Open(p_crlf, FileIOMode::Read);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CRLF);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(test_input) - sizeof(char)), sizeof(test_input) - sizeof(char));
	ASSERT_EQ(memcmp(test_input, test_str, sizeof(test_input) - sizeof(char)), 0);
	file.Close();

	file.Open(p_nbk, FileIOMode::Read);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::Unknown);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(test_input) - sizeof(char)), sizeof(test_input) - sizeof(char));
	ASSERT_EQ(memcmp(test_input, test_str, sizeof(test_input) - sizeof(char)), 0);
	file.Close();

	DeleteFile(p_cr);
	ASSERT_FALSE(p_cr.IsExist());
	DeleteFile(p_lf);
	ASSERT_FALSE(p_lf.IsExist());
	DeleteFile(p_crlf);
	ASSERT_FALSE(p_crlf.IsExist());
	DeleteFile(p_nbk);
	ASSERT_FALSE(p_nbk.IsExist());
}

TEST(UTF8File, SetFileLineBreakTest)
{
	Path p_cr(SGE_STR("./TestData/TestCommon/TestFile/test_utf8_crs.txt"));
	ASSERT_FALSE(p_cr.IsExist());
	Path p_lf(SGE_STR("./TestData/TestCommon/TestFile/test_utf8_lfs.txt"));
	ASSERT_FALSE(p_lf.IsExist());
	Path p_crlf(SGE_STR("./TestData/TestCommon/TestFile/test_utf8_crlfs.txt"));
	ASSERT_FALSE(p_crlf.IsExist());

	BinaryFile bf_output;
	UInt8 bom[3] = {0xef, 0xbb, 0xbf};
	char test_str[] = SGE_U8STR("this is test 测试");
	char test_str2[] = SGE_U8STR("file end?");
	char test_input[sizeof(test_str) / sizeof(char)];
	char cr[] = SGE_U8STR("\r");
	char crlf[] = SGE_U8STR("\r\n");
	char lf[] = SGE_U8STR("\n");

	bf_output.Open(p_cr, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_str, sizeof(test_str) - sizeof(char));
	bf_output.Write(cr, sizeof(cr) - sizeof(char));
	bf_output.Write(test_str2, sizeof(test_str2) - sizeof(char));
	bf_output.Close();
	ASSERT_TRUE(p_cr.IsExist());

	bf_output.Open(p_lf, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_str, sizeof(test_str) - sizeof(char));
	bf_output.Write(lf, sizeof(lf) - sizeof(char));
	bf_output.Write(test_str2, sizeof(test_str2) - sizeof(char));
	bf_output.Close();
	ASSERT_TRUE(p_lf.IsExist());

	bf_output.Open(p_crlf, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_str, sizeof(test_str) - sizeof(char));
	bf_output.Write(crlf, sizeof(crlf) - sizeof(char));
	bf_output.Write(test_str2, sizeof(test_str2) - sizeof(char));
	bf_output.Close();
	ASSERT_TRUE(p_crlf.IsExist());

	UTF8File file(p_cr, FileIOMode::Read | FileIOMode::Write);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CR);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(test_input) - sizeof(char)), sizeof(test_input) - sizeof(char));
	ASSERT_EQ(memcmp(test_input, test_str, sizeof(test_input) - sizeof(char)), 0);
	file.SetFileLineBreak(FileLineBreak::LF);
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::LF);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(lf) - sizeof(char)), sizeof(lf) - sizeof(char));
	ASSERT_EQ(memcmp(test_input, lf, sizeof(lf) - sizeof(char)), 0);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(test_str2) - sizeof(char)), sizeof(test_str2) - sizeof(char));
	ASSERT_EQ(memcmp(test_input, test_str2, sizeof(test_str2) - sizeof(char)), 0);
	file.Close();

	file.Open(p_lf, FileIOMode::Read | FileIOMode::Write);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::LF);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(test_input) - sizeof(char)), sizeof(test_input) - sizeof(char));
	ASSERT_EQ(memcmp(test_input, test_str, sizeof(test_input) - sizeof(char)), 0);
	file.SetFileLineBreak(FileLineBreak::CRLF);
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CRLF);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(crlf) - sizeof(char)), sizeof(crlf) - sizeof(char));
	ASSERT_EQ(memcmp(test_input, crlf, sizeof(crlf) - sizeof(char)), 0);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(test_str2) - sizeof(char)), sizeof(test_str2) - sizeof(char));
	ASSERT_EQ(memcmp(test_input, test_str2, sizeof(test_str2) - sizeof(char)), 0);
	file.Close();

	file.Open(p_crlf, FileIOMode::Read | FileIOMode::Write);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CRLF);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(test_input) - sizeof(char)), sizeof(test_input) - sizeof(char));
	ASSERT_EQ(memcmp(test_input, test_str, sizeof(test_input) - sizeof(char)), 0);
	file.SetFileLineBreak(FileLineBreak::CR);
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CR);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(cr) - sizeof(char)), sizeof(cr) - sizeof(char));
	ASSERT_EQ(memcmp(test_input, cr, sizeof(cr) - sizeof(char)), 0);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(test_str2) - sizeof(char)), sizeof(test_str2) - sizeof(char));
	ASSERT_EQ(memcmp(test_input, test_str2, sizeof(test_str2) - sizeof(char)), 0);
	file.Close();

	file.Open(p_cr, FileIOMode::Read);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::LF);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(test_input) - sizeof(char)), sizeof(test_input) - sizeof(char));
	ASSERT_EQ(memcmp(test_input, test_str, sizeof(test_input) - sizeof(char)), 0);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(lf) - sizeof(char)), sizeof(lf) - sizeof(char));
	ASSERT_EQ(memcmp(test_input, lf, sizeof(lf) - sizeof(char)), 0);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(test_str2) - sizeof(char)), sizeof(test_str2) - sizeof(char));
	ASSERT_EQ(memcmp(test_input, test_str2, sizeof(test_str2) - sizeof(char)), 0);
	file.Close();

	file.Open(p_lf, FileIOMode::Read);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CRLF);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(test_input) - sizeof(char)), sizeof(test_input) - sizeof(char));
	ASSERT_EQ(memcmp(test_input, test_str, sizeof(test_input) - sizeof(char)), 0);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(crlf) - sizeof(char)), sizeof(crlf) - sizeof(char));
	ASSERT_EQ(memcmp(test_input, crlf, sizeof(crlf) - sizeof(char)), 0);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(test_str2) - sizeof(char)), sizeof(test_str2) - sizeof(char));
	ASSERT_EQ(memcmp(test_input, test_str2, sizeof(test_str2) - sizeof(char)), 0);
	file.Close();

	file.Open(p_crlf, FileIOMode::Read);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CR);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(test_input) - sizeof(char)), sizeof(test_input) - sizeof(char));
	ASSERT_EQ(memcmp(test_input, test_str, sizeof(test_input) - sizeof(char)), 0);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(cr) - sizeof(char)), sizeof(cr) - sizeof(char));
	ASSERT_EQ(memcmp(test_input, cr, sizeof(cr) - sizeof(char)), 0);
	memset(test_input, 0, sizeof(test_input));
	ASSERT_EQ(file.Read(test_input, sizeof(test_str2) - sizeof(char)), sizeof(test_str2) - sizeof(char));
	ASSERT_EQ(memcmp(test_input, test_str2, sizeof(test_str2) - sizeof(char)), 0);
	file.Close();

	DeleteFile(p_cr);
	ASSERT_FALSE(p_cr.IsExist());
	DeleteFile(p_lf);
	ASSERT_FALSE(p_lf.IsExist());
	DeleteFile(p_crlf);
	ASSERT_FALSE(p_crlf.IsExist());
}

TEST(UTF8File, ReadLineTest)
{
	Path p_cr(SGE_STR("./TestData/TestCommon/TestFile/test_utf8_crr.txt"));
	ASSERT_FALSE(p_cr.IsExist());
	Path p_lf(SGE_STR("./TestData/TestCommon/TestFile/test_utf8_lfr.txt"));
	ASSERT_FALSE(p_lf.IsExist());
	Path p_crlf(SGE_STR("./TestData/TestCommon/TestFile/test_utf8_crlfr.txt"));
	ASSERT_FALSE(p_crlf.IsExist());

	BinaryFile bf_output;
	UInt8 bom[3] = {0xef, 0xbb, 0xbf};
	char test_str[] = SGE_U8STR("this is test 测试");
	char test_str2[] = SGE_U8STR("file end?");
	char cr[] = SGE_U8STR("\r");
	char crlf[] = SGE_U8STR("\r\n");
	char lf[] = SGE_U8STR("\n");

	bf_output.Open(p_cr, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_str, sizeof(test_str) - sizeof(char));
	bf_output.Write(cr, sizeof(cr) - sizeof(char));
	bf_output.Write(test_str2, sizeof(test_str2) - sizeof(char));
	bf_output.Close();
	ASSERT_TRUE(p_cr.IsExist());

	bf_output.Open(p_lf, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_str, sizeof(test_str) - sizeof(char));
	bf_output.Write(lf, sizeof(lf) - sizeof(char));
	bf_output.Write(test_str2, sizeof(test_str2) - sizeof(char));
	bf_output.Close();
	ASSERT_TRUE(p_lf.IsExist());

	bf_output.Open(p_crlf, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_str, sizeof(test_str) - sizeof(char));
	bf_output.Write(crlf, sizeof(crlf) - sizeof(char));
	bf_output.Write(test_str2, sizeof(test_str2) - sizeof(char));
	bf_output.Close();
	ASSERT_TRUE(p_crlf.IsExist());

	UTF8File file(p_cr, FileIOMode::Read);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CR);
	ASSERT_EQ(file.ReadLine(), test_str);
	ASSERT_EQ(file.ReadLine(), test_str2);
	file.Close();

	file.Open(p_lf, FileIOMode::Read);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::LF);
	ASSERT_EQ(file.ReadLine(), test_str);
	ASSERT_EQ(file.ReadLine(), test_str2);
	file.Close();

	file.Open(p_crlf, FileIOMode::Read);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CRLF);
	ASSERT_EQ(file.ReadLine(), test_str);
	ASSERT_EQ(file.ReadLine(), test_str2);
	file.Close();

	DeleteFile(p_cr);
	ASSERT_FALSE(p_cr.IsExist());
	DeleteFile(p_lf);
	ASSERT_FALSE(p_lf.IsExist());
	DeleteFile(p_crlf);
	ASSERT_FALSE(p_crlf.IsExist());
}

TEST(UTF8File, ReadWordTest)
{
	Path p_cr(SGE_STR("./TestData/TestCommon/TestFile/test_utf8_crw.txt"));
	ASSERT_FALSE(p_cr.IsExist());
	Path p_lf(SGE_STR("./TestData/TestCommon/TestFile/test_utf8_lfw.txt"));
	ASSERT_FALSE(p_lf.IsExist());
	Path p_crlf(SGE_STR("./TestData/TestCommon/TestFile/test_utf8_crlfw.txt"));
	ASSERT_FALSE(p_crlf.IsExist());

	BinaryFile bf_output;
	UInt8 bom[3] = {0xef, 0xbb, 0xbf};
	char test_str[] = SGE_U8STR("this is test 测试");
	char test_str2[] = SGE_U8STR("file end?");
	char cr[] = SGE_U8STR("\r");
	char crlf[] = SGE_U8STR("\r\n");
	char lf[] = SGE_U8STR("\n");

	bf_output.Open(p_cr, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_str, sizeof(test_str) - sizeof(char));
	bf_output.Write(cr, sizeof(cr) - sizeof(char));
	bf_output.Write(test_str2, sizeof(test_str2) - sizeof(char));
	bf_output.Close();
	ASSERT_TRUE(p_cr.IsExist());

	bf_output.Open(p_lf, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_str, sizeof(test_str) - sizeof(char));
	bf_output.Write(lf, sizeof(lf) - sizeof(char));
	bf_output.Write(test_str2, sizeof(test_str2) - sizeof(char));
	bf_output.Close();
	ASSERT_TRUE(p_lf.IsExist());

	bf_output.Open(p_crlf, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_str, sizeof(test_str) - sizeof(char));
	bf_output.Write(crlf, sizeof(crlf) - sizeof(char));
	bf_output.Write(test_str2, sizeof(test_str2) - sizeof(char));
	bf_output.Close();
	ASSERT_TRUE(p_crlf.IsExist());

	UTF8File file(p_cr, FileIOMode::Read);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CR);
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("this"));
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("is"));
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("test"));
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("测试"));
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("file"));
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("end?"));
	file.Close();

	file.Open(p_lf, FileIOMode::Read);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::LF);
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("this"));
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("is"));
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("test"));
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("测试"));
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("file"));
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("end?"));
	file.Close();

	file.Open(p_crlf, FileIOMode::Read);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CRLF);
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("this"));
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("is"));
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("test"));
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("测试"));
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("file"));
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("end?"));
	file.Close();

	DeleteFile(p_cr);
	ASSERT_FALSE(p_cr.IsExist());
	DeleteFile(p_lf);
	ASSERT_FALSE(p_lf.IsExist());
	DeleteFile(p_crlf);
	ASSERT_FALSE(p_crlf.IsExist());
}

TEST(UTF8File, ReadOperatorTest)
{
	Path p_cr(SGE_STR("./TestData/TestCommon/TestFile/test_utf8_cro.txt"));
	ASSERT_FALSE(p_cr.IsExist());
	Path p_lf(SGE_STR("./TestData/TestCommon/TestFile/test_utf8_lfo.txt"));
	ASSERT_FALSE(p_lf.IsExist());
	Path p_crlf(SGE_STR("./TestData/TestCommon/TestFile/test_utf8_crlfo.txt"));
	ASSERT_FALSE(p_crlf.IsExist());

	BinaryFile bf_output;
	UInt8 bom[3] = {0xef, 0xbb, 0xbf};
	char test_str[] = SGE_U8STR("1 1.5 2.5");
	char test_str2[] = SGE_U8STR("str");
	char cr[] = SGE_U8STR("\r");
	char crlf[] = SGE_U8STR("\r\n");
	char lf[] = SGE_U8STR("\n");

	bf_output.Open(p_cr, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_str, sizeof(test_str) - sizeof(char));
	bf_output.Write(cr, sizeof(cr) - sizeof(char));
	bf_output.Write(test_str2, sizeof(test_str2) - sizeof(char));
	bf_output.Write(cr, sizeof(cr) - sizeof(char));
	bf_output.Close();
	ASSERT_TRUE(p_cr.IsExist());

	bf_output.Open(p_lf, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_str, sizeof(test_str) - sizeof(char));
	bf_output.Write(lf, sizeof(lf) - sizeof(char));
	bf_output.Write(test_str2, sizeof(test_str2) - sizeof(char));
	bf_output.Write(lf, sizeof(lf) - sizeof(char));
	bf_output.Close();
	ASSERT_TRUE(p_lf.IsExist());

	bf_output.Open(p_crlf, FileIOMode::Write);
	bf_output.Write(bom, sizeof(bom));
	bf_output.Write(test_str, sizeof(test_str) - sizeof(char));
	bf_output.Write(crlf, sizeof(crlf) - sizeof(char));
	bf_output.Write(test_str2, sizeof(test_str2) - sizeof(char));
	bf_output.Write(crlf, sizeof(crlf) - sizeof(char));
	bf_output.Close();
	ASSERT_TRUE(p_crlf.IsExist());

	int ival = 0;
	float fval = 0.0f;
	double dval = 0.0f;
	UTF8String sval;
	int uval = 5;

	UTF8File file(p_cr, FileIOMode::Read);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CR);
	ASSERT_TRUE(file >> ival >> fval >> dval >> sval);
	ASSERT_FALSE(file >> uval);
	ASSERT_EQ(ival, 1);
	ASSERT_EQ(fval, 1.5f);
	ASSERT_EQ(dval, 2.5);
	ASSERT_EQ(sval, SGE_U8STR("str"));
	ASSERT_EQ(uval, 5);
	file.Close();

	file.Open(p_lf, FileIOMode::Read);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::LF);
	ival = 0;
	fval = 0;
	dval = 0;
	sval.Clear();
	ASSERT_TRUE(file >> ival >> fval >> dval >> sval);
	ASSERT_FALSE(file >> uval);
	ASSERT_EQ(ival, 1);
	ASSERT_EQ(fval, 1.5f);
	ASSERT_EQ(dval, 2.5);
	ASSERT_EQ(sval, SGE_U8STR("str"));
	ASSERT_EQ(uval, 5);
	file.Close();

	file.Open(p_crlf, FileIOMode::Read);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CRLF);
	ival = 0;
	fval = 0;
	dval = 0;
	sval.Clear();
	ASSERT_TRUE(file >> ival >> fval >> dval >> sval);
	ASSERT_FALSE(file >> uval);
	ASSERT_EQ(ival, 1);
	ASSERT_EQ(fval, 1.5f);
	ASSERT_EQ(dval, 2.5);
	ASSERT_EQ(sval, SGE_U8STR("str"));
	ASSERT_EQ(uval, 5);
	file.Close();

	DeleteFile(p_cr);
	ASSERT_FALSE(p_cr.IsExist());
	DeleteFile(p_lf);
	ASSERT_FALSE(p_lf.IsExist());
	DeleteFile(p_crlf);
	ASSERT_FALSE(p_crlf.IsExist());
}

TEST(UTF8File, WriteStringTest)
{
	Path p_cr(SGE_STR("./TestData/TestCommon/TestFile/test_utf8_crww.txt"));
	ASSERT_FALSE(p_cr.IsExist());
	Path p_lf(SGE_STR("./TestData/TestCommon/TestFile/test_utf8_lfww.txt"));
	ASSERT_FALSE(p_lf.IsExist());
	Path p_crlf(SGE_STR("./TestData/TestCommon/TestFile/test_utf8_crlfww.txt"));
	ASSERT_FALSE(p_crlf.IsExist());

	UTF8File output_file;
	char test_str[] = SGE_U8STR("this is test 测试");
	char test_str2[] = SGE_U8STR("file end?");
	char cr[] = SGE_U8STR("\r");
	char crlf[] = SGE_U8STR("\r\n");
	char lf[] = SGE_U8STR("\n");

	output_file.Open(p_cr, FileIOMode::Read | FileIOMode::Write);
	output_file.SetHasBomHeader(true);
	output_file.WriteString(test_str);
	output_file.WriteString(cr);
	output_file.WriteString(test_str2);
	output_file.Close();
	ASSERT_TRUE(p_cr.IsExist());

	output_file.Open(p_lf, FileIOMode::Read | FileIOMode::Write);
	output_file.SetHasBomHeader(true);
	output_file.WriteString(test_str);
	output_file.WriteString(lf);
	output_file.WriteString(test_str2);
	output_file.Close();
	ASSERT_TRUE(p_lf.IsExist());

	output_file.Open(p_crlf, FileIOMode::Read | FileIOMode::Write);
	output_file.SetHasBomHeader(true);
	output_file.WriteString(test_str);
	output_file.WriteString(crlf);
	output_file.WriteString(test_str2);
	output_file.Close();
	ASSERT_TRUE(p_crlf.IsExist());

	UTF8File file(p_cr, FileIOMode::Read);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CR);
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("this"));
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("is"));
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("test"));
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("测试"));
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("file"));
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("end?"));
	file.Close();

	file.Open(p_lf, FileIOMode::Read);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::LF);
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("this"));
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("is"));
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("test"));
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("测试"));
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("file"));
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("end?"));
	file.Close();

	file.Open(p_crlf, FileIOMode::Read);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CRLF);
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("this"));
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("is"));
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("test"));
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("测试"));
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("file"));
	ASSERT_EQ(file.ReadWord(), SGE_U8STR("end?"));
	file.Close();

	DeleteFile(p_cr);
	ASSERT_FALSE(p_cr.IsExist());
	DeleteFile(p_lf);
	ASSERT_FALSE(p_lf.IsExist());
	DeleteFile(p_crlf);
	ASSERT_FALSE(p_crlf.IsExist());
}

TEST(UTF8File, WriteOperatorTest)
{
	Path p_cr(SGE_STR("./TestData/TestCommon/TestFile/test_utf8_crwo.txt"));
	ASSERT_FALSE(p_cr.IsExist());
	Path p_lf(SGE_STR("./TestData/TestCommon/TestFile/test_utf8_lfwo.txt"));
	ASSERT_FALSE(p_lf.IsExist());
	Path p_crlf(SGE_STR("./TestData/TestCommon/TestFile/test_utf8_crlfwo.txt"));
	ASSERT_FALSE(p_crlf.IsExist());

	UTF8File output_file;

	output_file.Open(p_cr, FileIOMode::Read | FileIOMode::Write);
	output_file.SetHasBomHeader(true);
	output_file.SetFileLineBreak(FileLineBreak::CR);
	output_file << 1 << SGE_U8STR(" ") << 1.5f << SGE_U8STR(" ") << 2.5f << EndLine;
	output_file << SGE_U8STR("str") << EndLine;
	output_file.Close();
	ASSERT_TRUE(p_cr.IsExist());

	output_file.Open(p_lf, FileIOMode::Read | FileIOMode::Write);
	output_file.SetHasBomHeader(true);
	output_file.SetFileLineBreak(FileLineBreak::LF);
	output_file << 1 << SGE_U8STR(" ") << 1.5f << SGE_U8STR(" ") << 2.5f << EndLine;
	output_file << SGE_U8STR("str") << EndLine;
	output_file.Close();
	ASSERT_TRUE(p_lf.IsExist());

	output_file.Open(p_crlf, FileIOMode::Read | FileIOMode::Write);
	output_file.SetHasBomHeader(true);
	output_file.SetFileLineBreak(FileLineBreak::CRLF);
	output_file << 1 << SGE_U8STR(" ") << 1.5f << SGE_U8STR(" ") << 2.5f << EndLine;
	output_file << SGE_U8STR("str") << EndLine;
	output_file.Close();
	ASSERT_TRUE(p_crlf.IsExist());

	int ival = 0;
	float fval = 0.0f;
	double dval = 0.0f;
	UTF8String sval;
	int uval = 5;

	UTF8File file(p_cr, FileIOMode::Read);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CR);
	ASSERT_TRUE(file >> ival >> fval >> dval >> sval);
	ASSERT_FALSE(file >> uval);
	ASSERT_EQ(ival, 1);
	ASSERT_EQ(fval, 1.5f);
	ASSERT_EQ(dval, 2.5);
	ASSERT_EQ(sval, SGE_U8STR("str"));
	ASSERT_EQ(uval, 5);
	file.Close();

	file.Open(p_lf, FileIOMode::Read);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::LF);
	ival = 0;
	fval = 0;
	dval = 0;
	sval.Clear();
	ASSERT_TRUE(file >> ival >> fval >> dval >> sval);
	ASSERT_FALSE(file >> uval);
	ASSERT_EQ(ival, 1);
	ASSERT_EQ(fval, 1.5f);
	ASSERT_EQ(dval, 2.5);
	ASSERT_EQ(sval, SGE_U8STR("str"));
	ASSERT_EQ(uval, 5);
	file.Close();

	file.Open(p_crlf, FileIOMode::Read);
	ASSERT_TRUE(file.IsHasBomHeader());
	ASSERT_EQ(file.GetFileLineBreak(), FileLineBreak::CRLF);
	ival = 0;
	fval = 0;
	dval = 0;
	sval.Clear();
	ASSERT_TRUE(file >> ival >> fval >> dval >> sval);
	ASSERT_FALSE(file >> uval);
	ASSERT_EQ(ival, 1);
	ASSERT_EQ(fval, 1.5f);
	ASSERT_EQ(dval, 2.5);
	ASSERT_EQ(sval, SGE_U8STR("str"));
	ASSERT_EQ(uval, 5);
	file.Close();

	DeleteFile(p_cr);
	ASSERT_FALSE(p_cr.IsExist());
	DeleteFile(p_lf);
	ASSERT_FALSE(p_lf.IsExist());
	DeleteFile(p_crlf);
	ASSERT_FALSE(p_crlf.IsExist());
}