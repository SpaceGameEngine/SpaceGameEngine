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
#include "SGEString.hpp"
#include "gtest/gtest.h"
#include <cstring>

using namespace SpaceGameEngine;

TEST(StorageRef, InstanceTest)
{
	Char* pstr1 = StringImplement<Char>::StorageRef::Create(4);
	ASSERT_EQ(StringImplement<Char>::StorageRef::GetCount(pstr1), 1);
	ASSERT_EQ(StringImplement<Char>::StorageRef::TryRelease(pstr1, 4), true);

	const Char* pcstr = SGE_STR("这是测试");
	Char* pstr2 = StringImplement<Char>::StorageRef::Create(pcstr, 5);
	ASSERT_EQ(wcscmp(pstr2, pcstr), 0);
	ASSERT_EQ(StringImplement<Char>::StorageRef::GetCount(pstr2), 1);
	ASSERT_EQ(StringImplement<Char>::StorageRef::TryRelease(pstr2, 5), true);
}

TEST(StorageRef, TryReleaseTest)
{
	const Char* pcstr = SGE_STR("这是测试");
	Char* pstr1 = StringImplement<Char>::StorageRef::Create(pcstr, 5);
	ASSERT_EQ(memcmp(pstr1, pcstr, sizeof(Char) * 5), 0);
	ASSERT_EQ(StringImplement<Char>::StorageRef::GetCount(pstr1), 1);
	StringImplement<Char>::StorageRef::CountIncrease(pstr1);
	ASSERT_EQ(StringImplement<Char>::StorageRef::TryRelease(pstr1, 5), false);
	ASSERT_EQ(memcmp(pstr1, pcstr, sizeof(Char) * 5), 0);
	ASSERT_EQ(StringImplement<Char>::StorageRef::GetCount(pstr1), 2);
	StringImplement<Char>::StorageRef::CountDecrease(pstr1);
	ASSERT_EQ(memcmp(pstr1, pcstr, sizeof(Char) * 5), 0);
	ASSERT_EQ(StringImplement<Char>::StorageRef::GetCount(pstr1), 1);
	ASSERT_EQ(StringImplement<Char>::StorageRef::TryRelease(pstr1, 5), true);
}

TEST(Storage, InstanceTest)
{
	StringImplement<Char>::Storage s1;
	ASSERT_EQ(s1.GetSize(), 0);

	const Char* pcstr1 = SGE_STR("测试\0");
	StringImplement<Char>::Storage s2(SGE_STR("测试\0"), 3);
	ASSERT_EQ(s2.GetSize(), 3);
	ASSERT_EQ(memcmp(s2.GetData(), pcstr1, sizeof(Char) * 3), 0);

	const Char* pcstr2 = SGE_STR("测试一下中等长度的字符串，看看这样够不够字数，能不能过16/2\0");
	StringImplement<Char>::Storage s3(pcstr2, 31);
	ASSERT_EQ(s3.GetRealSize(), 31);
	ASSERT_EQ(s3.GetSize(), 31);
	ASSERT_EQ(memcmp(s3.GetData(), pcstr2, sizeof(Char) * 31), 0);

	const Char* pcstr3 = SGE_STR("一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890\0");
	StringImplement<Char>::Storage s4(pcstr3, 401);
	ASSERT_EQ(s4.GetRealSize(), 401);
	ASSERT_EQ(s4.GetSize(), 401);
	ASSERT_EQ(memcmp(s4.GetData(), pcstr3, sizeof(Char) * 401), 0);
}

TEST(Storage, CopyConstructionTest)
{
	StringImplement<Char>::Storage s1;
	ASSERT_EQ(s1.GetSize(), 0);
	StringImplement<Char>::Storage cs1(s1);
	ASSERT_EQ(cs1.GetSize(), 0);
	StringImplement<Char>::Storage rs1(std::move(s1));
	ASSERT_EQ(rs1.GetSize(), 0);
	StringImplement<Char, CharTrait<Char>, StdAllocator>::Storage acs1(cs1);
	ASSERT_EQ(acs1.GetSize(), 0);
	StringImplement<Char, CharTrait<Char>, StdAllocator>::Storage ars1(std::move(rs1));
	ASSERT_EQ(ars1.GetSize(), 0);

	const Char* pcstr1 = SGE_STR("测试\0");
	StringImplement<Char>::Storage s2(SGE_STR("测试\0"), 3);
	ASSERT_EQ(s2.GetSize(), 3);
	ASSERT_EQ(memcmp(s2.GetData(), pcstr1, sizeof(Char) * 3), 0);
	StringImplement<Char>::Storage cs2(s2);
	ASSERT_EQ(cs2.GetSize(), 3);
	ASSERT_EQ(memcmp(cs2.GetData(), pcstr1, sizeof(Char) * 3), 0);
	StringImplement<Char>::Storage rs2(std::move(s2));
	ASSERT_EQ(rs2.GetSize(), 3);
	ASSERT_EQ(memcmp(rs2.GetData(), pcstr1, sizeof(Char) * 3), 0);
	StringImplement<Char, CharTrait<Char>, StdAllocator>::Storage acs2(cs2);
	ASSERT_EQ(acs2.GetSize(), 3);
	ASSERT_EQ(memcmp(acs2.GetData(), pcstr1, sizeof(Char) * 3), 0);
	StringImplement<Char, CharTrait<Char>, StdAllocator>::Storage ars2(std::move(rs2));
	ASSERT_EQ(ars2.GetSize(), 3);
	ASSERT_EQ(memcmp(ars2.GetData(), pcstr1, sizeof(Char) * 3), 0);

	const Char* pcstr2 = SGE_STR("测试一下中等长度的字符串，看看这样够不够字数，能不能过16/2\0");
	StringImplement<Char>::Storage s3(pcstr2, 31);
	ASSERT_EQ(s3.GetRealSize(), 31);
	ASSERT_EQ(s3.GetSize(), 31);
	ASSERT_EQ(memcmp(s3.GetData(), pcstr2, sizeof(Char) * 31), 0);
	StringImplement<Char>::Storage cs3(s3);
	ASSERT_EQ(cs3.GetRealSize(), 31);
	ASSERT_EQ(cs3.GetSize(), 31);
	ASSERT_EQ(memcmp(cs3.GetData(), pcstr2, sizeof(Char) * 31), 0);
	StringImplement<Char>::Storage rs3(std::move(s3));
	ASSERT_EQ(rs3.GetRealSize(), 31);
	ASSERT_EQ(rs3.GetSize(), 31);
	ASSERT_EQ(memcmp(rs3.GetData(), pcstr2, sizeof(Char) * 31), 0);
	StringImplement<Char, CharTrait<Char>, StdAllocator>::Storage acs3(cs3);
	ASSERT_EQ(acs3.GetRealSize(), 31);
	ASSERT_EQ(acs3.GetSize(), 31);
	ASSERT_EQ(memcmp(acs3.GetData(), pcstr2, sizeof(Char) * 31), 0);
	StringImplement<Char, CharTrait<Char>, StdAllocator>::Storage ars3(std::move(rs3));
	ASSERT_EQ(ars3.GetRealSize(), 31);
	ASSERT_EQ(ars3.GetSize(), 31);
	ASSERT_EQ(memcmp(ars3.GetData(), pcstr2, sizeof(Char) * 31), 0);

	const Char* pcstr3 = SGE_STR("一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890\0");
	StringImplement<Char>::Storage s4(pcstr3, 401);
	ASSERT_EQ(s4.GetRealSize(), 401);
	ASSERT_EQ(s4.GetSize(), 401);
	ASSERT_EQ(memcmp(s4.GetData(), pcstr3, sizeof(Char) * 401), 0);
	StringImplement<Char>::Storage cs4(s4);
	ASSERT_EQ(cs4.GetRealSize(), 401);
	ASSERT_EQ(cs4.GetSize(), 401);
	ASSERT_EQ(memcmp(cs4.GetData(), pcstr3, sizeof(Char) * 401), 0);
	StringImplement<Char>::Storage rs4(std::move(s4));
	ASSERT_EQ(rs4.GetRealSize(), 401);
	ASSERT_EQ(rs4.GetSize(), 401);
	ASSERT_EQ(memcmp(rs4.GetData(), pcstr3, sizeof(Char) * 401), 0);
	StringImplement<Char, CharTrait<Char>, StdAllocator>::Storage acs4(cs4);
	ASSERT_EQ(acs4.GetRealSize(), 401);
	ASSERT_EQ(acs4.GetSize(), 401);
	ASSERT_EQ(memcmp(acs4.GetData(), pcstr3, sizeof(Char) * 401), 0);
	StringImplement<Char, CharTrait<Char>, StdAllocator>::Storage ars4(std::move(rs4));
	ASSERT_EQ(ars4.GetRealSize(), 401);
	ASSERT_EQ(ars4.GetSize(), 401);
	ASSERT_EQ(memcmp(ars4.GetData(), pcstr3, sizeof(Char) * 401), 0);
}