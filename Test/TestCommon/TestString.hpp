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
	ASSERT_EQ(memcmp(pstr2, pcstr, 5 * sizeof(Char)), 0);
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
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(s1.GetRealSize()), StringImplement<Char>::StringCategory::Small);

	const Char* pcstr1 = SGE_STR("测试\0");
	StringImplement<Char>::Storage s2(SGE_STR("测试\0"), 3);
	ASSERT_EQ(s2.GetSize(), 3);
	ASSERT_EQ(memcmp(s2.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(s2.GetRealSize()), StringImplement<Char>::StringCategory::Small);

	const Char* pcstr2 = SGE_STR("测试一下中等长度的字符串，看看这样够不够字数，能不能过16/2\0");
	StringImplement<Char>::Storage s3(pcstr2, 31);
	ASSERT_EQ(s3.GetRealSize(), 31);
	ASSERT_EQ(s3.GetSize(), 31);
	ASSERT_EQ(memcmp(s3.GetData(), pcstr2, sizeof(Char) * 31), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(s3.GetRealSize()), StringImplement<Char>::StringCategory::Medium);

	const Char* pcstr3 = SGE_STR("一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890\0");
	StringImplement<Char>::Storage s4(pcstr3, 401);
	ASSERT_EQ(s4.GetRealSize(), 401);
	ASSERT_EQ(s4.GetSize(), 401);
	ASSERT_EQ(memcmp(s4.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(s4.GetRealSize()), StringImplement<Char>::StringCategory::Large);
}

TEST(Storage, CopyConstructionTest)
{
	StringImplement<Char>::Storage s1;
	ASSERT_EQ(s1.GetSize(), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(s1.GetRealSize()), StringImplement<Char>::StringCategory::Small);
	StringImplement<Char>::Storage cs1(s1);
	ASSERT_EQ(cs1.GetSize(), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(cs1.GetRealSize()), StringImplement<Char>::StringCategory::Small);
	StringImplement<Char>::Storage rs1(std::move(s1));
	ASSERT_EQ(rs1.GetSize(), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(rs1.GetRealSize()), StringImplement<Char>::StringCategory::Small);
	StringImplement<Char, CharTrait<Char>, StdAllocator>::Storage acs1(cs1);
	ASSERT_EQ(acs1.GetSize(), 0);
	ASSERT_EQ((StringImplement<Char, CharTrait<Char>, StdAllocator>::GetStringCategoryByRealSize(acs1.GetRealSize())), (StringImplement<Char, CharTrait<Char>, StdAllocator>::StringCategory::Small));
	StringImplement<Char, CharTrait<Char>, StdAllocator>::Storage ars1(std::move(rs1));
	ASSERT_EQ(ars1.GetSize(), 0);
	ASSERT_EQ((StringImplement<Char, CharTrait<Char>, StdAllocator>::GetStringCategoryByRealSize(ars1.GetRealSize())), (StringImplement<Char, CharTrait<Char>, StdAllocator>::StringCategory::Small));

	const Char* pcstr1 = SGE_STR("测试\0");
	StringImplement<Char>::Storage s2(SGE_STR("测试\0"), 3);
	ASSERT_EQ(s2.GetSize(), 3);
	ASSERT_EQ(memcmp(s2.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(s2.GetRealSize()), StringImplement<Char>::StringCategory::Small);
	StringImplement<Char>::Storage cs2(s2);
	ASSERT_EQ(cs2.GetSize(), 3);
	ASSERT_EQ(memcmp(cs2.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(cs2.GetRealSize()), StringImplement<Char>::StringCategory::Small);
	StringImplement<Char>::Storage rs2(std::move(s2));
	ASSERT_EQ(rs2.GetSize(), 3);
	ASSERT_EQ(memcmp(rs2.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(rs2.GetRealSize()), StringImplement<Char>::StringCategory::Small);
	StringImplement<Char, CharTrait<Char>, StdAllocator>::Storage acs2(cs2);
	ASSERT_EQ(acs2.GetSize(), 3);
	ASSERT_EQ(memcmp(acs2.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ((StringImplement<Char, CharTrait<Char>, StdAllocator>::GetStringCategoryByRealSize(acs2.GetRealSize())), (StringImplement<Char, CharTrait<Char>, StdAllocator>::StringCategory::Small));
	StringImplement<Char, CharTrait<Char>, StdAllocator>::Storage ars2(std::move(rs2));
	ASSERT_EQ(ars2.GetSize(), 3);
	ASSERT_EQ(memcmp(ars2.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ((StringImplement<Char, CharTrait<Char>, StdAllocator>::GetStringCategoryByRealSize(ars2.GetRealSize())), (StringImplement<Char, CharTrait<Char>, StdAllocator>::StringCategory::Small));

	const Char* pcstr2 = SGE_STR("测试一下中等长度的字符串，看看这样够不够字数，能不能过16/2\0");
	StringImplement<Char>::Storage s3(pcstr2, 31);
	ASSERT_EQ(s3.GetRealSize(), 31);
	ASSERT_EQ(s3.GetSize(), 31);
	ASSERT_EQ(memcmp(s3.GetData(), pcstr2, sizeof(Char) * 31), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(s3.GetRealSize()), StringImplement<Char>::StringCategory::Medium);
	StringImplement<Char>::Storage cs3(s3);
	ASSERT_EQ(cs3.GetRealSize(), 31);
	ASSERT_EQ(cs3.GetSize(), 31);
	ASSERT_EQ(memcmp(cs3.GetData(), pcstr2, sizeof(Char) * 31), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(cs3.GetRealSize()), StringImplement<Char>::StringCategory::Medium);
	StringImplement<Char>::Storage rs3(std::move(s3));
	ASSERT_EQ(rs3.GetRealSize(), 31);
	ASSERT_EQ(rs3.GetSize(), 31);
	ASSERT_EQ(memcmp(rs3.GetData(), pcstr2, sizeof(Char) * 31), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(rs3.GetRealSize()), StringImplement<Char>::StringCategory::Medium);
	StringImplement<Char, CharTrait<Char>, StdAllocator>::Storage acs3(cs3);
	ASSERT_EQ(acs3.GetRealSize(), 31);
	ASSERT_EQ(acs3.GetSize(), 31);
	ASSERT_EQ(memcmp(acs3.GetData(), pcstr2, sizeof(Char) * 31), 0);
	ASSERT_EQ((StringImplement<Char, CharTrait<Char>, StdAllocator>::GetStringCategoryByRealSize(acs3.GetRealSize())), (StringImplement<Char, CharTrait<Char>, StdAllocator>::StringCategory::Medium));
	StringImplement<Char, CharTrait<Char>, StdAllocator>::Storage ars3(std::move(rs3));
	ASSERT_EQ(ars3.GetRealSize(), 31);
	ASSERT_EQ(ars3.GetSize(), 31);
	ASSERT_EQ(memcmp(ars3.GetData(), pcstr2, sizeof(Char) * 31), 0);
	ASSERT_EQ((StringImplement<Char, CharTrait<Char>, StdAllocator>::GetStringCategoryByRealSize(ars3.GetRealSize())), (StringImplement<Char, CharTrait<Char>, StdAllocator>::StringCategory::Medium));

	const Char* pcstr3 = SGE_STR("一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890\0");
	StringImplement<Char>::Storage s4(pcstr3, 401);
	ASSERT_EQ(s4.GetRealSize(), 401);
	ASSERT_EQ(s4.GetSize(), 401);
	ASSERT_EQ(memcmp(s4.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(s4.GetRealSize()), StringImplement<Char>::StringCategory::Large);
	StringImplement<Char>::Storage cs4(s4);
	ASSERT_EQ(cs4.GetRealSize(), 401);
	ASSERT_EQ(cs4.GetSize(), 401);
	ASSERT_EQ(memcmp(cs4.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(cs4.GetRealSize()), StringImplement<Char>::StringCategory::Large);
	StringImplement<Char>::Storage rs4(std::move(s4));
	ASSERT_EQ(rs4.GetRealSize(), 401);
	ASSERT_EQ(rs4.GetSize(), 401);
	ASSERT_EQ(memcmp(rs4.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(rs4.GetRealSize()), StringImplement<Char>::StringCategory::Large);
	StringImplement<Char, CharTrait<Char>, StdAllocator>::Storage acs4(cs4);
	ASSERT_EQ(acs4.GetRealSize(), 401);
	ASSERT_EQ(acs4.GetSize(), 401);
	ASSERT_EQ(memcmp(acs4.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ((StringImplement<Char, CharTrait<Char>, StdAllocator>::GetStringCategoryByRealSize(acs4.GetRealSize())), (StringImplement<Char, CharTrait<Char>, StdAllocator>::StringCategory::Large));
	StringImplement<Char, CharTrait<Char>, StdAllocator>::Storage ars4(std::move(rs4));
	ASSERT_EQ(ars4.GetRealSize(), 401);
	ASSERT_EQ(ars4.GetSize(), 401);
	ASSERT_EQ(memcmp(ars4.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ((StringImplement<Char, CharTrait<Char>, StdAllocator>::GetStringCategoryByRealSize(ars4.GetRealSize())), (StringImplement<Char, CharTrait<Char>, StdAllocator>::StringCategory::Large));
}
TEST(Storage, CopyAssignmentTest)
{
	StringImplement<Char>::Storage s1;
	ASSERT_EQ(s1.GetSize(), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(s1.GetRealSize()), StringImplement<Char>::StringCategory::Small);

	const Char* pcstr1 = SGE_STR("测试\0");
	StringImplement<Char>::Storage s2(SGE_STR("测试\0"), 3);
	ASSERT_EQ(s2.GetSize(), 3);
	ASSERT_EQ(memcmp(s2.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(s2.GetRealSize()), StringImplement<Char>::StringCategory::Small);

	const Char* pcstr2 = SGE_STR("测试一下中等长度的字符串，看看这样够不够字数，能不能过16/2\0");
	StringImplement<Char>::Storage s3(pcstr2, 31);
	ASSERT_EQ(s3.GetRealSize(), 31);
	ASSERT_EQ(s3.GetSize(), 31);
	ASSERT_EQ(memcmp(s3.GetData(), pcstr2, sizeof(Char) * 31), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(s3.GetRealSize()), StringImplement<Char>::StringCategory::Medium);

	const Char* pcstr3 = SGE_STR("一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890\0");
	StringImplement<Char>::Storage s4(pcstr3, 401);
	ASSERT_EQ(s4.GetRealSize(), 401);
	ASSERT_EQ(s4.GetSize(), 401);
	ASSERT_EQ(memcmp(s4.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(s4.GetRealSize()), StringImplement<Char>::StringCategory::Large);

	const Char* pcstr_s = SGE_STR("S\0");
	//63
	const Char* pcstr_m = SGE_STR("测试一下中等长度的字符串的拷贝，to see whether it can work correctly, 能不能过16/2\0");
	//403
	const Char* pcstr_l = SGE_STR("测试一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890\0");

	StringImplement<Char>::Storage ss_s(pcstr_s, 2);
	ASSERT_EQ(ss_s.GetRealSize(), 2);
	ASSERT_EQ(ss_s.GetSize(), 2);
	ASSERT_EQ(memcmp(ss_s.GetData(), pcstr_s, 2 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(ss_s.GetRealSize()), StringImplement<Char>::StringCategory::Small);
	ss_s = s2;
	ASSERT_EQ(ss_s.GetSize(), 3);
	ASSERT_EQ(memcmp(ss_s.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(ss_s.GetRealSize()), StringImplement<Char>::StringCategory::Small);

	StringImplement<Char>::Storage sm_s(pcstr_s, 2);
	ASSERT_EQ(sm_s.GetRealSize(), 2);
	ASSERT_EQ(sm_s.GetSize(), 2);
	ASSERT_EQ(memcmp(sm_s.GetData(), pcstr_s, 2 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(sm_s.GetRealSize()), StringImplement<Char>::StringCategory::Small);
	sm_s = s3;
	ASSERT_EQ(sm_s.GetRealSize(), 31);
	ASSERT_EQ(sm_s.GetSize(), 31);
	ASSERT_EQ(memcmp(sm_s.GetData(), pcstr2, sizeof(Char) * 31), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(sm_s.GetRealSize()), StringImplement<Char>::StringCategory::Medium);

	StringImplement<Char>::Storage sl_s(pcstr_s, 2);
	ASSERT_EQ(sl_s.GetRealSize(), 2);
	ASSERT_EQ(sl_s.GetSize(), 2);
	ASSERT_EQ(memcmp(sl_s.GetData(), pcstr_s, 2 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(sl_s.GetRealSize()), StringImplement<Char>::StringCategory::Small);
	sl_s = s4;
	ASSERT_EQ(sl_s.GetRealSize(), 401);
	ASSERT_EQ(sl_s.GetSize(), 401);
	ASSERT_EQ(memcmp(sl_s.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(sl_s.GetRealSize()), StringImplement<Char>::StringCategory::Large);

	StringImplement<Char>::Storage ms_s(pcstr_m, 63);
	ASSERT_EQ(ms_s.GetRealSize(), 63);
	ASSERT_EQ(ms_s.GetSize(), 63);
	ASSERT_EQ(memcmp(ms_s.GetData(), pcstr_m, 63 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(ms_s.GetRealSize()), StringImplement<Char>::StringCategory::Medium);
	ms_s = s2;
	ASSERT_EQ(ms_s.GetSize(), 3);
	ASSERT_EQ(memcmp(ms_s.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(ms_s.GetRealSize()), StringImplement<Char>::StringCategory::Small);

	StringImplement<Char>::Storage mm_s(pcstr_m, 63);
	ASSERT_EQ(mm_s.GetRealSize(), 63);
	ASSERT_EQ(mm_s.GetSize(), 63);
	ASSERT_EQ(memcmp(mm_s.GetData(), pcstr_m, 63 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(mm_s.GetRealSize()), StringImplement<Char>::StringCategory::Medium);
	mm_s = s3;
	ASSERT_EQ(mm_s.GetSize(), 31);
	ASSERT_EQ(memcmp(mm_s.GetData(), pcstr2, sizeof(Char) * 31), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(mm_s.GetRealSize()), StringImplement<Char>::StringCategory::Medium);

	StringImplement<Char>::Storage ml_s(pcstr_m, 63);
	ASSERT_EQ(ml_s.GetRealSize(), 63);
	ASSERT_EQ(ml_s.GetSize(), 63);
	ASSERT_EQ(memcmp(ml_s.GetData(), pcstr_m, 63 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(ml_s.GetRealSize()), StringImplement<Char>::StringCategory::Medium);
	ml_s = s4;
	ASSERT_EQ(ml_s.GetRealSize(), 401);
	ASSERT_EQ(ml_s.GetSize(), 401);
	ASSERT_EQ(memcmp(ml_s.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(ml_s.GetRealSize()), StringImplement<Char>::StringCategory::Large);

	StringImplement<Char>::Storage ls_s(pcstr_l, 403);
	ASSERT_EQ(ls_s.GetRealSize(), 403);
	ASSERT_EQ(ls_s.GetSize(), 403);
	ASSERT_EQ(memcmp(ls_s.GetData(), pcstr_l, 403 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(ls_s.GetRealSize()), StringImplement<Char>::StringCategory::Large);
	ls_s = s2;
	ASSERT_EQ(ls_s.GetSize(), 3);
	ASSERT_EQ(memcmp(ls_s.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(ls_s.GetRealSize()), StringImplement<Char>::StringCategory::Small);

	StringImplement<Char>::Storage lm_s(pcstr_l, 403);
	ASSERT_EQ(lm_s.GetRealSize(), 403);
	ASSERT_EQ(lm_s.GetSize(), 403);
	ASSERT_EQ(memcmp(lm_s.GetData(), pcstr_l, 403 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(lm_s.GetRealSize()), StringImplement<Char>::StringCategory::Large);
	lm_s = s3;
	ASSERT_EQ(lm_s.GetRealSize(), 31);
	ASSERT_EQ(lm_s.GetSize(), 31);
	ASSERT_EQ(memcmp(lm_s.GetData(), pcstr2, sizeof(Char) * 31), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(lm_s.GetRealSize()), StringImplement<Char>::StringCategory::Medium);

	StringImplement<Char>::Storage ll_s(pcstr_l, 403);
	ASSERT_EQ(ll_s.GetRealSize(), 403);
	ASSERT_EQ(ll_s.GetSize(), 403);
	ASSERT_EQ(memcmp(ll_s.GetData(), pcstr_l, 403 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(ll_s.GetRealSize()), StringImplement<Char>::StringCategory::Large);
	ll_s = s4;
	ASSERT_EQ(ll_s.GetRealSize(), 401);
	ASSERT_EQ(ll_s.GetSize(), 401);
	ASSERT_EQ(memcmp(ll_s.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ(StringImplement<Char>::GetStringCategoryByRealSize(ll_s.GetRealSize()), StringImplement<Char>::StringCategory::Large);
}