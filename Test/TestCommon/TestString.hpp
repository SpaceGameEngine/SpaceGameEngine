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
	Char* pstr1 = StringImplement::StorageRef<Char>::Create(4);
	ASSERT_EQ(StringImplement::StorageRef<Char>::GetCount(pstr1), 1);
	ASSERT_EQ(StringImplement::StorageRef<Char>::TryRelease(pstr1, 4), true);

	const Char* pcstr = SGE_STR("这是测试");
	Char* pstr2 = StringImplement::StorageRef<Char>::Create(pcstr, 5);
	ASSERT_EQ(memcmp(pstr2, pcstr, 5 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::StorageRef<Char>::GetCount(pstr2), 1);
	ASSERT_EQ(StringImplement::StorageRef<Char>::TryRelease(pstr2, 5), true);
}

TEST(StorageRef, TryReleaseTest)
{
	const Char* pcstr = SGE_STR("这是测试");
	Char* pstr1 = StringImplement::StorageRef<Char>::Create(pcstr, 5);
	ASSERT_EQ(memcmp(pstr1, pcstr, sizeof(Char) * 5), 0);
	ASSERT_EQ(StringImplement::StorageRef<Char>::GetCount(pstr1), 1);
	StringImplement::StorageRef<Char>::CountIncrease(pstr1);
	ASSERT_EQ(StringImplement::StorageRef<Char>::TryRelease(pstr1, 5), false);
	ASSERT_EQ(memcmp(pstr1, pcstr, sizeof(Char) * 5), 0);
	ASSERT_EQ(StringImplement::StorageRef<Char>::GetCount(pstr1), 2);
	StringImplement::StorageRef<Char>::CountDecrease(pstr1);
	ASSERT_EQ(memcmp(pstr1, pcstr, sizeof(Char) * 5), 0);
	ASSERT_EQ(StringImplement::StorageRef<Char>::GetCount(pstr1), 1);
	ASSERT_EQ(StringImplement::StorageRef<Char>::TryRelease(pstr1, 5), true);
}

TEST(Storage, InstanceTest)
{
	StringImplement::Storage<Char> s1;
	ASSERT_EQ(s1.GetSize(), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s1.GetRealSize()), StringImplement::StringCategory::Small);

	const Char* pcstr1 = SGE_STR("测试\0");
	StringImplement::Storage<Char> s2(SGE_STR("测试\0"), 3);
	ASSERT_EQ(s2.GetSize(), 3);
	ASSERT_EQ(memcmp(s2.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s2.GetRealSize()), StringImplement::StringCategory::Small);

	const Char* pcstr2 = SGE_STR("测试一下中等长度的字符串，看看这样够不够字数，能不能过16/2\0");
	StringImplement::Storage<Char> s3(pcstr2, 32);
	ASSERT_EQ(s3.GetRealSize(), 32);
	ASSERT_EQ(s3.GetSize(), 32);
	ASSERT_EQ(memcmp(s3.GetData(), pcstr2, sizeof(Char) * 32), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s3.GetRealSize()), StringImplement::StringCategory::Medium);

	const Char* pcstr3 = SGE_STR("一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890\0");
	StringImplement::Storage<Char> s4(pcstr3, 401);
	ASSERT_EQ(s4.GetRealSize(), 401);
	ASSERT_EQ(s4.GetSize(), 401);
	ASSERT_EQ(memcmp(s4.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s4.GetRealSize()), StringImplement::StringCategory::Large);
}

TEST(Storage, CopyConstructionTest)
{
	StringImplement::Storage<Char> s1;
	ASSERT_EQ(s1.GetSize(), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s1.GetRealSize()), StringImplement::StringCategory::Small);
	StringImplement::Storage<Char> cs1(s1);
	ASSERT_EQ(cs1.GetSize(), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(cs1.GetRealSize()), StringImplement::StringCategory::Small);
	StringImplement::Storage<Char> rs1(std::move(s1));
	ASSERT_EQ(rs1.GetSize(), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(rs1.GetRealSize()), StringImplement::StringCategory::Small);
	StringImplement::Storage<Char, StdAllocator> acs1(cs1);
	ASSERT_EQ(acs1.GetSize(), 0);
	ASSERT_EQ((StringImplement::GetStringCategoryByRealSize<Char>(acs1.GetRealSize())), (StringImplement::StringCategory::Small));
	StringImplement::Storage<Char, StdAllocator> ars1(std::move(rs1));
	ASSERT_EQ(ars1.GetSize(), 0);
	ASSERT_EQ((StringImplement::GetStringCategoryByRealSize<Char>(ars1.GetRealSize())), (StringImplement::StringCategory::Small));

	const Char* pcstr1 = SGE_STR("测试\0");
	StringImplement::Storage<Char> s2(SGE_STR("测试\0"), 3);
	ASSERT_EQ(s2.GetSize(), 3);
	ASSERT_EQ(memcmp(s2.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s2.GetRealSize()), StringImplement::StringCategory::Small);
	StringImplement::Storage<Char> cs2(s2);
	ASSERT_EQ(cs2.GetSize(), 3);
	ASSERT_EQ(memcmp(cs2.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(cs2.GetRealSize()), StringImplement::StringCategory::Small);
	StringImplement::Storage<Char> rs2(std::move(s2));
	ASSERT_EQ(rs2.GetSize(), 3);
	ASSERT_EQ(memcmp(rs2.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(rs2.GetRealSize()), StringImplement::StringCategory::Small);
	StringImplement::Storage<Char, StdAllocator> acs2(cs2);
	ASSERT_EQ(acs2.GetSize(), 3);
	ASSERT_EQ(memcmp(acs2.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ((StringImplement::GetStringCategoryByRealSize<Char>(acs2.GetRealSize())), (StringImplement::StringCategory::Small));
	StringImplement::Storage<Char, StdAllocator> ars2(std::move(rs2));
	ASSERT_EQ(ars2.GetSize(), 3);
	ASSERT_EQ(memcmp(ars2.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ((StringImplement::GetStringCategoryByRealSize<Char>(ars2.GetRealSize())), (StringImplement::StringCategory::Small));

	const Char* pcstr2 = SGE_STR("测试一下中等长度的字符串，看看这样够不够字数，能不能过16/2\0");
	StringImplement::Storage<Char> s3(pcstr2, 32);
	ASSERT_EQ(s3.GetRealSize(), 32);
	ASSERT_EQ(s3.GetSize(), 32);
	ASSERT_EQ(memcmp(s3.GetData(), pcstr2, sizeof(Char) * 32), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s3.GetRealSize()), StringImplement::StringCategory::Medium);
	StringImplement::Storage<Char> cs3(s3);
	ASSERT_EQ(cs3.GetRealSize(), 32);
	ASSERT_EQ(cs3.GetSize(), 32);
	ASSERT_EQ(memcmp(cs3.GetData(), pcstr2, sizeof(Char) * 32), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(cs3.GetRealSize()), StringImplement::StringCategory::Medium);
	StringImplement::Storage<Char> rs3(std::move(s3));
	ASSERT_EQ(rs3.GetRealSize(), 32);
	ASSERT_EQ(rs3.GetSize(), 32);
	ASSERT_EQ(memcmp(rs3.GetData(), pcstr2, sizeof(Char) * 32), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(rs3.GetRealSize()), StringImplement::StringCategory::Medium);
	StringImplement::Storage<Char, StdAllocator> acs3(cs3);
	ASSERT_EQ(acs3.GetRealSize(), 32);
	ASSERT_EQ(acs3.GetSize(), 32);
	ASSERT_EQ(memcmp(acs3.GetData(), pcstr2, sizeof(Char) * 32), 0);
	ASSERT_EQ((StringImplement::GetStringCategoryByRealSize<Char>(acs3.GetRealSize())), (StringImplement::StringCategory::Medium));
	StringImplement::Storage<Char, StdAllocator> ars3(std::move(rs3));
	ASSERT_EQ(ars3.GetRealSize(), 32);
	ASSERT_EQ(ars3.GetSize(), 32);
	ASSERT_EQ(memcmp(ars3.GetData(), pcstr2, sizeof(Char) * 32), 0);
	ASSERT_EQ((StringImplement::GetStringCategoryByRealSize<Char>(ars3.GetRealSize())), (StringImplement::StringCategory::Medium));

	const Char* pcstr3 = SGE_STR("一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890\0");
	StringImplement::Storage<Char> s4(pcstr3, 401);
	ASSERT_EQ(s4.GetRealSize(), 401);
	ASSERT_EQ(s4.GetSize(), 401);
	ASSERT_EQ(memcmp(s4.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s4.GetRealSize()), StringImplement::StringCategory::Large);
	StringImplement::Storage<Char> cs4(s4);
	ASSERT_EQ(cs4.GetRealSize(), 401);
	ASSERT_EQ(cs4.GetSize(), 401);
	ASSERT_EQ(memcmp(cs4.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(cs4.GetRealSize()), StringImplement::StringCategory::Large);
	StringImplement::Storage<Char> rs4(std::move(s4));
	ASSERT_EQ(rs4.GetRealSize(), 401);
	ASSERT_EQ(rs4.GetSize(), 401);
	ASSERT_EQ(memcmp(rs4.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(rs4.GetRealSize()), StringImplement::StringCategory::Large);
	StringImplement::Storage<Char, StdAllocator> acs4(cs4);
	ASSERT_EQ(acs4.GetRealSize(), 401);
	ASSERT_EQ(acs4.GetSize(), 401);
	ASSERT_EQ(memcmp(acs4.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ((StringImplement::GetStringCategoryByRealSize<Char>(acs4.GetRealSize())), (StringImplement::StringCategory::Large));
	StringImplement::Storage<Char, StdAllocator> ars4(std::move(rs4));
	ASSERT_EQ(ars4.GetRealSize(), 401);
	ASSERT_EQ(ars4.GetSize(), 401);
	ASSERT_EQ(memcmp(ars4.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ((StringImplement::GetStringCategoryByRealSize<Char>(ars4.GetRealSize())), (StringImplement::StringCategory::Large));
}

TEST(Storage, CopyAssignmentTest)
{
	StringImplement::Storage<Char> s1;
	ASSERT_EQ(s1.GetSize(), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s1.GetRealSize()), StringImplement::StringCategory::Small);

	const Char* pcstr1 = SGE_STR("测试\0");
	StringImplement::Storage<Char> s2(SGE_STR("测试\0"), 3);
	ASSERT_EQ(s2.GetSize(), 3);
	ASSERT_EQ(memcmp(s2.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s2.GetRealSize()), StringImplement::StringCategory::Small);

	const Char* pcstr2 = SGE_STR("测试一下中等长度的字符串，看看这样够不够字数，能不能过16/2\0");
	StringImplement::Storage<Char> s3(pcstr2, 32);
	ASSERT_EQ(s3.GetRealSize(), 32);
	ASSERT_EQ(s3.GetSize(), 32);
	ASSERT_EQ(memcmp(s3.GetData(), pcstr2, sizeof(Char) * 32), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s3.GetRealSize()), StringImplement::StringCategory::Medium);

	const Char* pcstr3 = SGE_STR("一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890\0");
	StringImplement::Storage<Char> s4(pcstr3, 401);
	ASSERT_EQ(s4.GetRealSize(), 401);
	ASSERT_EQ(s4.GetSize(), 401);
	ASSERT_EQ(memcmp(s4.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s4.GetRealSize()), StringImplement::StringCategory::Large);

	const Char* pcstr_s = SGE_STR("S\0");
	//63
	const Char* pcstr_m = SGE_STR("测试一下中等长度的字符串的拷贝，to see whether it can work correctly, 能不能过16/2\0");
	//403
	const Char* pcstr_l = SGE_STR("测试一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890\0");

	StringImplement::Storage<Char> ss_s(pcstr_s, 2);
	ASSERT_EQ(ss_s.GetRealSize(), 2);
	ASSERT_EQ(ss_s.GetSize(), 2);
	ASSERT_EQ(memcmp(ss_s.GetData(), pcstr_s, 2 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ss_s.GetRealSize()), StringImplement::StringCategory::Small);
	ss_s = s2;
	ASSERT_EQ(ss_s.GetSize(), 3);
	ASSERT_EQ(memcmp(ss_s.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ss_s.GetRealSize()), StringImplement::StringCategory::Small);

	StringImplement::Storage<Char> sm_s(pcstr_s, 2);
	ASSERT_EQ(sm_s.GetRealSize(), 2);
	ASSERT_EQ(sm_s.GetSize(), 2);
	ASSERT_EQ(memcmp(sm_s.GetData(), pcstr_s, 2 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(sm_s.GetRealSize()), StringImplement::StringCategory::Small);
	sm_s = s3;
	ASSERT_EQ(sm_s.GetRealSize(), 32);
	ASSERT_EQ(sm_s.GetSize(), 32);
	ASSERT_EQ(memcmp(sm_s.GetData(), pcstr2, sizeof(Char) * 32), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(sm_s.GetRealSize()), StringImplement::StringCategory::Medium);

	StringImplement::Storage<Char> sl_s(pcstr_s, 2);
	ASSERT_EQ(sl_s.GetRealSize(), 2);
	ASSERT_EQ(sl_s.GetSize(), 2);
	ASSERT_EQ(memcmp(sl_s.GetData(), pcstr_s, 2 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(sl_s.GetRealSize()), StringImplement::StringCategory::Small);
	sl_s = s4;
	ASSERT_EQ(sl_s.GetRealSize(), 401);
	ASSERT_EQ(sl_s.GetSize(), 401);
	ASSERT_EQ(memcmp(sl_s.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(sl_s.GetRealSize()), StringImplement::StringCategory::Large);

	StringImplement::Storage<Char> ms_s(pcstr_m, 63);
	ASSERT_EQ(ms_s.GetRealSize(), 63);
	ASSERT_EQ(ms_s.GetSize(), 63);
	ASSERT_EQ(memcmp(ms_s.GetData(), pcstr_m, 63 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ms_s.GetRealSize()), StringImplement::StringCategory::Medium);
	ms_s = s2;
	ASSERT_EQ(ms_s.GetSize(), 3);
	ASSERT_EQ(memcmp(ms_s.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ms_s.GetRealSize()), StringImplement::StringCategory::Small);

	StringImplement::Storage<Char> mm_s(pcstr_m, 63);
	ASSERT_EQ(mm_s.GetRealSize(), 63);
	ASSERT_EQ(mm_s.GetSize(), 63);
	ASSERT_EQ(memcmp(mm_s.GetData(), pcstr_m, 63 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(mm_s.GetRealSize()), StringImplement::StringCategory::Medium);
	mm_s = s3;
	ASSERT_EQ(mm_s.GetSize(), 32);
	ASSERT_EQ(memcmp(mm_s.GetData(), pcstr2, sizeof(Char) * 32), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(mm_s.GetRealSize()), StringImplement::StringCategory::Medium);

	StringImplement::Storage<Char> ml_s(pcstr_m, 63);
	ASSERT_EQ(ml_s.GetRealSize(), 63);
	ASSERT_EQ(ml_s.GetSize(), 63);
	ASSERT_EQ(memcmp(ml_s.GetData(), pcstr_m, 63 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ml_s.GetRealSize()), StringImplement::StringCategory::Medium);
	ml_s = s4;
	ASSERT_EQ(ml_s.GetRealSize(), 401);
	ASSERT_EQ(ml_s.GetSize(), 401);
	ASSERT_EQ(memcmp(ml_s.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ml_s.GetRealSize()), StringImplement::StringCategory::Large);

	StringImplement::Storage<Char> ls_s(pcstr_l, 403);
	ASSERT_EQ(ls_s.GetRealSize(), 403);
	ASSERT_EQ(ls_s.GetSize(), 403);
	ASSERT_EQ(memcmp(ls_s.GetData(), pcstr_l, 403 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ls_s.GetRealSize()), StringImplement::StringCategory::Large);
	ls_s = s2;
	ASSERT_EQ(ls_s.GetSize(), 3);
	ASSERT_EQ(memcmp(ls_s.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ls_s.GetRealSize()), StringImplement::StringCategory::Small);

	StringImplement::Storage<Char> lm_s(pcstr_l, 403);
	ASSERT_EQ(lm_s.GetRealSize(), 403);
	ASSERT_EQ(lm_s.GetSize(), 403);
	ASSERT_EQ(memcmp(lm_s.GetData(), pcstr_l, 403 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(lm_s.GetRealSize()), StringImplement::StringCategory::Large);
	lm_s = s3;
	ASSERT_EQ(lm_s.GetRealSize(), 32);
	ASSERT_EQ(lm_s.GetSize(), 32);
	ASSERT_EQ(memcmp(lm_s.GetData(), pcstr2, sizeof(Char) * 32), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(lm_s.GetRealSize()), StringImplement::StringCategory::Medium);

	StringImplement::Storage<Char> ll_s(pcstr_l, 403);
	ASSERT_EQ(ll_s.GetRealSize(), 403);
	ASSERT_EQ(ll_s.GetSize(), 403);
	ASSERT_EQ(memcmp(ll_s.GetData(), pcstr_l, 403 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ll_s.GetRealSize()), StringImplement::StringCategory::Large);
	ll_s = s4;
	ASSERT_EQ(ll_s.GetRealSize(), 401);
	ASSERT_EQ(ll_s.GetSize(), 401);
	ASSERT_EQ(memcmp(ll_s.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ll_s.GetRealSize()), StringImplement::StringCategory::Large);
}

TEST(Storage, MoveAssignmentTest)
{
	StringImplement::Storage<Char> s1;
	ASSERT_EQ(s1.GetSize(), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s1.GetRealSize()), StringImplement::StringCategory::Small);

	const Char* pcstr1 = SGE_STR("测试\0");
	StringImplement::Storage<Char> s2(SGE_STR("测试\0"), 3);
	ASSERT_EQ(s2.GetSize(), 3);
	ASSERT_EQ(memcmp(s2.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s2.GetRealSize()), StringImplement::StringCategory::Small);

	const Char* pcstr2 = SGE_STR("测试一下中等长度的字符串，看看这样够不够字数，能不能过16/2\0");
	StringImplement::Storage<Char> s3(pcstr2, 32);
	ASSERT_EQ(s3.GetRealSize(), 32);
	ASSERT_EQ(s3.GetSize(), 32);
	ASSERT_EQ(memcmp(s3.GetData(), pcstr2, sizeof(Char) * 32), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s3.GetRealSize()), StringImplement::StringCategory::Medium);

	const Char* pcstr3 = SGE_STR("一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890\0");
	StringImplement::Storage<Char> s4(pcstr3, 401);
	ASSERT_EQ(s4.GetRealSize(), 401);
	ASSERT_EQ(s4.GetSize(), 401);
	ASSERT_EQ(memcmp(s4.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s4.GetRealSize()), StringImplement::StringCategory::Large);

	const Char* pcstr_s = SGE_STR("S\0");
	//63
	const Char* pcstr_m = SGE_STR("测试一下中等长度的字符串的拷贝，to see whether it can work correctly, 能不能过16/2\0");
	//403
	const Char* pcstr_l = SGE_STR("测试一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890\0");

	StringImplement::Storage<Char> ss_s(pcstr_s, 2);
	ASSERT_EQ(ss_s.GetRealSize(), 2);
	ASSERT_EQ(ss_s.GetSize(), 2);
	ASSERT_EQ(memcmp(ss_s.GetData(), pcstr_s, 2 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ss_s.GetRealSize()), StringImplement::StringCategory::Small);
	ss_s = std::move(s2);
	ASSERT_EQ(ss_s.GetSize(), 3);
	ASSERT_EQ(memcmp(ss_s.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ss_s.GetRealSize()), StringImplement::StringCategory::Small);

	StringImplement::Storage<Char> sm_s(pcstr_s, 2);
	ASSERT_EQ(sm_s.GetRealSize(), 2);
	ASSERT_EQ(sm_s.GetSize(), 2);
	ASSERT_EQ(memcmp(sm_s.GetData(), pcstr_s, 2 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(sm_s.GetRealSize()), StringImplement::StringCategory::Small);
	sm_s = std::move(s3);
	ASSERT_EQ(sm_s.GetRealSize(), 32);
	ASSERT_EQ(sm_s.GetSize(), 32);
	ASSERT_EQ(memcmp(sm_s.GetData(), pcstr2, sizeof(Char) * 32), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(sm_s.GetRealSize()), StringImplement::StringCategory::Medium);
	ASSERT_EQ(s3.GetSize(), 0);
	ASSERT_EQ(s3.GetRealSize(), 0);

	StringImplement::Storage<Char> sl_s(pcstr_s, 2);
	ASSERT_EQ(sl_s.GetRealSize(), 2);
	ASSERT_EQ(sl_s.GetSize(), 2);
	ASSERT_EQ(memcmp(sl_s.GetData(), pcstr_s, 2 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(sl_s.GetRealSize()), StringImplement::StringCategory::Small);
	sl_s = std::move(s4);
	ASSERT_EQ(sl_s.GetRealSize(), 401);
	ASSERT_EQ(sl_s.GetSize(), 401);
	ASSERT_EQ(memcmp(sl_s.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(sl_s.GetRealSize()), StringImplement::StringCategory::Large);
	ASSERT_EQ(s4.GetSize(), 0);
	ASSERT_EQ(s4.GetRealSize(), 0);

	StringImplement::Storage<Char> ms_s(pcstr_m, 63);
	ASSERT_EQ(ms_s.GetRealSize(), 63);
	ASSERT_EQ(ms_s.GetSize(), 63);
	ASSERT_EQ(memcmp(ms_s.GetData(), pcstr_m, 63 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ms_s.GetRealSize()), StringImplement::StringCategory::Medium);
	ms_s = std::move(ss_s);
	ASSERT_EQ(ms_s.GetSize(), 3);
	ASSERT_EQ(memcmp(ms_s.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ms_s.GetRealSize()), StringImplement::StringCategory::Small);

	StringImplement::Storage<Char> mm_s(pcstr_m, 63);
	ASSERT_EQ(mm_s.GetRealSize(), 63);
	ASSERT_EQ(mm_s.GetSize(), 63);
	ASSERT_EQ(memcmp(mm_s.GetData(), pcstr_m, 63 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(mm_s.GetRealSize()), StringImplement::StringCategory::Medium);
	mm_s = std::move(sm_s);
	ASSERT_EQ(mm_s.GetRealSize(), 32);
	ASSERT_EQ(mm_s.GetSize(), 32);
	ASSERT_EQ(memcmp(mm_s.GetData(), pcstr2, sizeof(Char) * 32), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(mm_s.GetRealSize()), StringImplement::StringCategory::Medium);
	ASSERT_EQ(sm_s.GetSize(), 0);
	ASSERT_EQ(sm_s.GetRealSize(), 0);

	StringImplement::Storage<Char> ml_s(pcstr_m, 63);
	ASSERT_EQ(ml_s.GetRealSize(), 63);
	ASSERT_EQ(ml_s.GetSize(), 63);
	ASSERT_EQ(memcmp(ml_s.GetData(), pcstr_m, 63 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ml_s.GetRealSize()), StringImplement::StringCategory::Medium);
	ml_s = std::move(sl_s);
	ASSERT_EQ(ml_s.GetRealSize(), 401);
	ASSERT_EQ(ml_s.GetSize(), 401);
	ASSERT_EQ(memcmp(ml_s.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ml_s.GetRealSize()), StringImplement::StringCategory::Large);
	ASSERT_EQ(sl_s.GetSize(), 0);
	ASSERT_EQ(sl_s.GetRealSize(), 0);

	StringImplement::Storage<Char> ls_s(pcstr_l, 403);
	ASSERT_EQ(ls_s.GetRealSize(), 403);
	ASSERT_EQ(ls_s.GetSize(), 403);
	ASSERT_EQ(memcmp(ls_s.GetData(), pcstr_l, 403 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ls_s.GetRealSize()), StringImplement::StringCategory::Large);
	ls_s = std::move(ms_s);
	ASSERT_EQ(ls_s.GetSize(), 3);
	ASSERT_EQ(memcmp(ls_s.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ls_s.GetRealSize()), StringImplement::StringCategory::Small);

	StringImplement::Storage<Char> lm_s(pcstr_l, 403);
	ASSERT_EQ(lm_s.GetRealSize(), 403);
	ASSERT_EQ(lm_s.GetSize(), 403);
	ASSERT_EQ(memcmp(lm_s.GetData(), pcstr_l, 403 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(lm_s.GetRealSize()), StringImplement::StringCategory::Large);
	lm_s = std::move(mm_s);
	ASSERT_EQ(lm_s.GetRealSize(), 32);
	ASSERT_EQ(lm_s.GetSize(), 32);
	ASSERT_EQ(memcmp(lm_s.GetData(), pcstr2, sizeof(Char) * 32), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(lm_s.GetRealSize()), StringImplement::StringCategory::Medium);
	ASSERT_EQ(mm_s.GetSize(), 0);
	ASSERT_EQ(mm_s.GetRealSize(), 0);

	StringImplement::Storage<Char> ll_s(pcstr_l, 403);
	ASSERT_EQ(ll_s.GetRealSize(), 403);
	ASSERT_EQ(ll_s.GetSize(), 403);
	ASSERT_EQ(memcmp(ll_s.GetData(), pcstr_l, 403 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ll_s.GetRealSize()), StringImplement::StringCategory::Large);
	ll_s = std::move(ml_s);
	ASSERT_EQ(ll_s.GetRealSize(), 401);
	ASSERT_EQ(ll_s.GetSize(), 401);
	ASSERT_EQ(memcmp(ll_s.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ll_s.GetRealSize()), StringImplement::StringCategory::Large);
	ASSERT_EQ(ml_s.GetSize(), 0);
	ASSERT_EQ(ml_s.GetRealSize(), 0);
}

TEST(Storage, CopyAssignmentForAnotherAllocatorTest)
{
	StringImplement::Storage<Char, StdAllocator> s1;
	ASSERT_EQ(s1.GetSize(), 0);
	ASSERT_EQ((StringImplement::GetStringCategoryByRealSize<Char>(s1.GetRealSize())), (StringImplement::StringCategory::Small));

	const Char* pcstr1 = SGE_STR("测试\0");
	StringImplement::Storage<Char, StdAllocator> s2(SGE_STR("测试\0"), 3);
	ASSERT_EQ(s2.GetSize(), 3);
	ASSERT_EQ(memcmp(s2.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ((StringImplement::GetStringCategoryByRealSize<Char>(s2.GetRealSize())), (StringImplement::StringCategory::Small));

	const Char* pcstr2 = SGE_STR("测试一下中等长度的字符串，看看这样够不够字数，能不能过16/2\0");
	StringImplement::Storage<Char, StdAllocator> s3(pcstr2, 32);
	ASSERT_EQ(s3.GetRealSize(), 32);
	ASSERT_EQ(s3.GetSize(), 32);
	ASSERT_EQ(memcmp(s3.GetData(), pcstr2, sizeof(Char) * 32), 0);
	ASSERT_EQ((StringImplement::GetStringCategoryByRealSize<Char>(s3.GetRealSize())), (StringImplement::StringCategory::Medium));

	const Char* pcstr3 = SGE_STR("一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890\0");
	StringImplement::Storage<Char, StdAllocator> s4(pcstr3, 401);
	ASSERT_EQ(s4.GetRealSize(), 401);
	ASSERT_EQ(s4.GetSize(), 401);
	ASSERT_EQ(memcmp(s4.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ((StringImplement::GetStringCategoryByRealSize<Char>(s4.GetRealSize())), (StringImplement::StringCategory::Large));

	const Char* pcstr_s = SGE_STR("S\0");
	//63
	const Char* pcstr_m = SGE_STR("测试一下中等长度的字符串的拷贝，to see whether it can work correctly, 能不能过16/2\0");
	//403
	const Char* pcstr_l = SGE_STR("测试一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890\0");

	StringImplement::Storage<Char> ss_s(pcstr_s, 2);
	ASSERT_EQ(ss_s.GetRealSize(), 2);
	ASSERT_EQ(ss_s.GetSize(), 2);
	ASSERT_EQ(memcmp(ss_s.GetData(), pcstr_s, 2 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ss_s.GetRealSize()), StringImplement::StringCategory::Small);
	ss_s = s2;
	ASSERT_EQ(ss_s.GetSize(), 3);
	ASSERT_EQ(memcmp(ss_s.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ss_s.GetRealSize()), StringImplement::StringCategory::Small);

	StringImplement::Storage<Char> sm_s(pcstr_s, 2);
	ASSERT_EQ(sm_s.GetRealSize(), 2);
	ASSERT_EQ(sm_s.GetSize(), 2);
	ASSERT_EQ(memcmp(sm_s.GetData(), pcstr_s, 2 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(sm_s.GetRealSize()), StringImplement::StringCategory::Small);
	sm_s = s3;
	ASSERT_EQ(sm_s.GetRealSize(), 32);
	ASSERT_EQ(sm_s.GetSize(), 32);
	ASSERT_EQ(memcmp(sm_s.GetData(), pcstr2, sizeof(Char) * 32), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(sm_s.GetRealSize()), StringImplement::StringCategory::Medium);

	StringImplement::Storage<Char> sl_s(pcstr_s, 2);
	ASSERT_EQ(sl_s.GetRealSize(), 2);
	ASSERT_EQ(sl_s.GetSize(), 2);
	ASSERT_EQ(memcmp(sl_s.GetData(), pcstr_s, 2 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(sl_s.GetRealSize()), StringImplement::StringCategory::Small);
	sl_s = s4;
	ASSERT_EQ(sl_s.GetRealSize(), 401);
	ASSERT_EQ(sl_s.GetSize(), 401);
	ASSERT_EQ(memcmp(sl_s.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(sl_s.GetRealSize()), StringImplement::StringCategory::Large);

	StringImplement::Storage<Char> ms_s(pcstr_m, 63);
	ASSERT_EQ(ms_s.GetRealSize(), 63);
	ASSERT_EQ(ms_s.GetSize(), 63);
	ASSERT_EQ(memcmp(ms_s.GetData(), pcstr_m, 63 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ms_s.GetRealSize()), StringImplement::StringCategory::Medium);
	ms_s = s2;
	ASSERT_EQ(ms_s.GetSize(), 3);
	ASSERT_EQ(memcmp(ms_s.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ms_s.GetRealSize()), StringImplement::StringCategory::Small);

	StringImplement::Storage<Char> mm_s(pcstr_m, 63);
	ASSERT_EQ(mm_s.GetRealSize(), 63);
	ASSERT_EQ(mm_s.GetSize(), 63);
	ASSERT_EQ(memcmp(mm_s.GetData(), pcstr_m, 63 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(mm_s.GetRealSize()), StringImplement::StringCategory::Medium);
	mm_s = s3;
	ASSERT_EQ(mm_s.GetSize(), 32);
	ASSERT_EQ(memcmp(mm_s.GetData(), pcstr2, sizeof(Char) * 32), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(mm_s.GetRealSize()), StringImplement::StringCategory::Medium);

	StringImplement::Storage<Char> ml_s(pcstr_m, 63);
	ASSERT_EQ(ml_s.GetRealSize(), 63);
	ASSERT_EQ(ml_s.GetSize(), 63);
	ASSERT_EQ(memcmp(ml_s.GetData(), pcstr_m, 63 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ml_s.GetRealSize()), StringImplement::StringCategory::Medium);
	ml_s = s4;
	ASSERT_EQ(ml_s.GetRealSize(), 401);
	ASSERT_EQ(ml_s.GetSize(), 401);
	ASSERT_EQ(memcmp(ml_s.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ml_s.GetRealSize()), StringImplement::StringCategory::Large);

	StringImplement::Storage<Char> ls_s(pcstr_l, 403);
	ASSERT_EQ(ls_s.GetRealSize(), 403);
	ASSERT_EQ(ls_s.GetSize(), 403);
	ASSERT_EQ(memcmp(ls_s.GetData(), pcstr_l, 403 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ls_s.GetRealSize()), StringImplement::StringCategory::Large);
	ls_s = s2;
	ASSERT_EQ(ls_s.GetSize(), 3);
	ASSERT_EQ(memcmp(ls_s.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ls_s.GetRealSize()), StringImplement::StringCategory::Small);

	StringImplement::Storage<Char> lm_s(pcstr_l, 403);
	ASSERT_EQ(lm_s.GetRealSize(), 403);
	ASSERT_EQ(lm_s.GetSize(), 403);
	ASSERT_EQ(memcmp(lm_s.GetData(), pcstr_l, 403 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(lm_s.GetRealSize()), StringImplement::StringCategory::Large);
	lm_s = s3;
	ASSERT_EQ(lm_s.GetRealSize(), 32);
	ASSERT_EQ(lm_s.GetSize(), 32);
	ASSERT_EQ(memcmp(lm_s.GetData(), pcstr2, sizeof(Char) * 32), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(lm_s.GetRealSize()), StringImplement::StringCategory::Medium);

	StringImplement::Storage<Char> ll_s(pcstr_l, 403);
	ASSERT_EQ(ll_s.GetRealSize(), 403);
	ASSERT_EQ(ll_s.GetSize(), 403);
	ASSERT_EQ(memcmp(ll_s.GetData(), pcstr_l, 403 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ll_s.GetRealSize()), StringImplement::StringCategory::Large);
	ll_s = s4;
	ASSERT_EQ(ll_s.GetRealSize(), 401);
	ASSERT_EQ(ll_s.GetSize(), 401);
	ASSERT_EQ(memcmp(ll_s.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ll_s.GetRealSize()), StringImplement::StringCategory::Large);
}

TEST(Storage, MoveAssignmentForAnotherAllocatorTest)
{
	StringImplement::Storage<Char, StdAllocator> s1;
	ASSERT_EQ(s1.GetSize(), 0);
	ASSERT_EQ((StringImplement::GetStringCategoryByRealSize<Char>(s1.GetRealSize())), (StringImplement::StringCategory::Small));

	const Char* pcstr1 = SGE_STR("测试\0");
	StringImplement::Storage<Char, StdAllocator> s2(SGE_STR("测试\0"), 3);
	ASSERT_EQ(s2.GetSize(), 3);
	ASSERT_EQ(memcmp(s2.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ((StringImplement::GetStringCategoryByRealSize<Char>(s2.GetRealSize())), (StringImplement::StringCategory::Small));

	StringImplement::Storage<Char, StdAllocator> s2_2(SGE_STR("测试\0"), 3);
	ASSERT_EQ(s2_2.GetSize(), 3);
	ASSERT_EQ(memcmp(s2_2.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ((StringImplement::GetStringCategoryByRealSize<Char>(s2_2.GetRealSize())), (StringImplement::StringCategory::Small));

	StringImplement::Storage<Char, StdAllocator> s2_3(SGE_STR("测试\0"), 3);
	ASSERT_EQ(s2_3.GetSize(), 3);
	ASSERT_EQ(memcmp(s2_3.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ((StringImplement::GetStringCategoryByRealSize<Char>(s2_3.GetRealSize())), (StringImplement::StringCategory::Small));

	const Char* pcstr2 = SGE_STR("测试一下中等长度的字符串，看看这样够不够字数，能不能过16/2\0");
	StringImplement::Storage<Char, StdAllocator> s3(pcstr2, 32);
	ASSERT_EQ(s3.GetRealSize(), 32);
	ASSERT_EQ(s3.GetSize(), 32);
	ASSERT_EQ(memcmp(s3.GetData(), pcstr2, sizeof(Char) * 32), 0);
	ASSERT_EQ((StringImplement::GetStringCategoryByRealSize<Char>(s3.GetRealSize())), (StringImplement::StringCategory::Medium));

	StringImplement::Storage<Char, StdAllocator> s3_2(pcstr2, 32);
	ASSERT_EQ(s3_2.GetRealSize(), 32);
	ASSERT_EQ(s3_2.GetSize(), 32);
	ASSERT_EQ(memcmp(s3_2.GetData(), pcstr2, sizeof(Char) * 32), 0);
	ASSERT_EQ((StringImplement::GetStringCategoryByRealSize<Char>(s3_2.GetRealSize())), (StringImplement::StringCategory::Medium));

	StringImplement::Storage<Char, StdAllocator> s3_3(pcstr2, 32);
	ASSERT_EQ(s3_3.GetRealSize(), 32);
	ASSERT_EQ(s3_3.GetSize(), 32);
	ASSERT_EQ(memcmp(s3_3.GetData(), pcstr2, sizeof(Char) * 32), 0);
	ASSERT_EQ((StringImplement::GetStringCategoryByRealSize<Char>(s3_3.GetRealSize())), (StringImplement::StringCategory::Medium));

	const Char* pcstr3 = SGE_STR("一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890\0");
	StringImplement::Storage<Char, StdAllocator> s4(pcstr3, 401);
	ASSERT_EQ(s4.GetRealSize(), 401);
	ASSERT_EQ(s4.GetSize(), 401);
	ASSERT_EQ(memcmp(s4.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ((StringImplement::GetStringCategoryByRealSize<Char>(s4.GetRealSize())), (StringImplement::StringCategory::Large));

	StringImplement::Storage<Char, StdAllocator> s4_2(pcstr3, 401);
	ASSERT_EQ(s4_2.GetRealSize(), 401);
	ASSERT_EQ(s4_2.GetSize(), 401);
	ASSERT_EQ(memcmp(s4_2.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ((StringImplement::GetStringCategoryByRealSize<Char>(s4_2.GetRealSize())), (StringImplement::StringCategory::Large));

	StringImplement::Storage<Char, StdAllocator> s4_3(pcstr3, 401);
	ASSERT_EQ(s4_3.GetRealSize(), 401);
	ASSERT_EQ(s4_3.GetSize(), 401);
	ASSERT_EQ(memcmp(s4_3.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ((StringImplement::GetStringCategoryByRealSize<Char>(s4_3.GetRealSize())), (StringImplement::StringCategory::Large));

	const Char* pcstr_s = SGE_STR("S\0");
	//63
	const Char* pcstr_m = SGE_STR("测试一下中等长度的字符串的拷贝，to see whether it can work correctly, 能不能过16/2\0");
	//403
	const Char* pcstr_l = SGE_STR("测试一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890\0");

	StringImplement::Storage<Char> ss_s(pcstr_s, 2);
	ASSERT_EQ(ss_s.GetRealSize(), 2);
	ASSERT_EQ(ss_s.GetSize(), 2);
	ASSERT_EQ(memcmp(ss_s.GetData(), pcstr_s, 2 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ss_s.GetRealSize()), StringImplement::StringCategory::Small);
	ss_s = std::move(s2);
	ASSERT_EQ(ss_s.GetSize(), 3);
	ASSERT_EQ(memcmp(ss_s.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ss_s.GetRealSize()), StringImplement::StringCategory::Small);

	StringImplement::Storage<Char> sm_s(pcstr_s, 2);
	ASSERT_EQ(sm_s.GetRealSize(), 2);
	ASSERT_EQ(sm_s.GetSize(), 2);
	ASSERT_EQ(memcmp(sm_s.GetData(), pcstr_s, 2 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(sm_s.GetRealSize()), StringImplement::StringCategory::Small);
	sm_s = std::move(s3);
	ASSERT_EQ(sm_s.GetRealSize(), 32);
	ASSERT_EQ(sm_s.GetSize(), 32);
	ASSERT_EQ(memcmp(sm_s.GetData(), pcstr2, sizeof(Char) * 32), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(sm_s.GetRealSize()), StringImplement::StringCategory::Medium);
	ASSERT_EQ(s3.GetSize(), 32);
	ASSERT_EQ(s3.GetRealSize(), 32);

	StringImplement::Storage<Char> sl_s(pcstr_s, 2);
	ASSERT_EQ(sl_s.GetRealSize(), 2);
	ASSERT_EQ(sl_s.GetSize(), 2);
	ASSERT_EQ(memcmp(sl_s.GetData(), pcstr_s, 2 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(sl_s.GetRealSize()), StringImplement::StringCategory::Small);
	sl_s = std::move(s4);
	ASSERT_EQ(sl_s.GetRealSize(), 401);
	ASSERT_EQ(sl_s.GetSize(), 401);
	ASSERT_EQ(memcmp(sl_s.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(sl_s.GetRealSize()), StringImplement::StringCategory::Large);
	ASSERT_EQ(s4.GetSize(), 401);
	ASSERT_EQ(s4.GetRealSize(), 401);

	StringImplement::Storage<Char> ms_s(pcstr_m, 63);
	ASSERT_EQ(ms_s.GetRealSize(), 63);
	ASSERT_EQ(ms_s.GetSize(), 63);
	ASSERT_EQ(memcmp(ms_s.GetData(), pcstr_m, 63 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ms_s.GetRealSize()), StringImplement::StringCategory::Medium);
	ms_s = std::move(s2_2);
	ASSERT_EQ(ms_s.GetSize(), 3);
	ASSERT_EQ(memcmp(ms_s.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ms_s.GetRealSize()), StringImplement::StringCategory::Small);

	StringImplement::Storage<Char> mm_s(pcstr_m, 63);
	ASSERT_EQ(mm_s.GetRealSize(), 63);
	ASSERT_EQ(mm_s.GetSize(), 63);
	ASSERT_EQ(memcmp(mm_s.GetData(), pcstr_m, 63 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(mm_s.GetRealSize()), StringImplement::StringCategory::Medium);
	mm_s = std::move(s3_2);
	ASSERT_EQ(mm_s.GetRealSize(), 63);
	ASSERT_EQ(mm_s.GetSize(), 32);
	ASSERT_EQ(memcmp(mm_s.GetData(), pcstr2, sizeof(Char) * 32), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(mm_s.GetRealSize()), StringImplement::StringCategory::Medium);
	ASSERT_EQ(s3_2.GetSize(), 32);
	ASSERT_EQ(s3_2.GetRealSize(), 32);

	StringImplement::Storage<Char> ml_s(pcstr_m, 63);
	ASSERT_EQ(ml_s.GetRealSize(), 63);
	ASSERT_EQ(ml_s.GetSize(), 63);
	ASSERT_EQ(memcmp(ml_s.GetData(), pcstr_m, 63 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ml_s.GetRealSize()), StringImplement::StringCategory::Medium);
	ml_s = std::move(s4_2);
	ASSERT_EQ(ml_s.GetRealSize(), 401);
	ASSERT_EQ(ml_s.GetSize(), 401);
	ASSERT_EQ(memcmp(ml_s.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ml_s.GetRealSize()), StringImplement::StringCategory::Large);
	ASSERT_EQ(s4_2.GetSize(), 401);
	ASSERT_EQ(s4_2.GetRealSize(), 401);

	StringImplement::Storage<Char> ls_s(pcstr_l, 403);
	ASSERT_EQ(ls_s.GetRealSize(), 403);
	ASSERT_EQ(ls_s.GetSize(), 403);
	ASSERT_EQ(memcmp(ls_s.GetData(), pcstr_l, 403 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ls_s.GetRealSize()), StringImplement::StringCategory::Large);
	ls_s = std::move(s2_3);
	ASSERT_EQ(ls_s.GetSize(), 3);
	ASSERT_EQ(memcmp(ls_s.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ls_s.GetRealSize()), StringImplement::StringCategory::Small);

	StringImplement::Storage<Char> lm_s(pcstr_l, 403);
	ASSERT_EQ(lm_s.GetRealSize(), 403);
	ASSERT_EQ(lm_s.GetSize(), 403);
	ASSERT_EQ(memcmp(lm_s.GetData(), pcstr_l, 403 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(lm_s.GetRealSize()), StringImplement::StringCategory::Large);
	lm_s = std::move(s3_3);
	ASSERT_EQ(lm_s.GetRealSize(), 32);
	ASSERT_EQ(lm_s.GetSize(), 32);
	ASSERT_EQ(memcmp(lm_s.GetData(), pcstr2, sizeof(Char) * 32), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(lm_s.GetRealSize()), StringImplement::StringCategory::Medium);
	ASSERT_EQ(s3_3.GetSize(), 32);
	ASSERT_EQ(s3_3.GetRealSize(), 32);

	StringImplement::Storage<Char> ll_s(pcstr_l, 403);
	ASSERT_EQ(ll_s.GetRealSize(), 403);
	ASSERT_EQ(ll_s.GetSize(), 403);
	ASSERT_EQ(memcmp(ll_s.GetData(), pcstr_l, 403 * sizeof(Char)), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ll_s.GetRealSize()), StringImplement::StringCategory::Large);
	ll_s = std::move(s4_3);
	ASSERT_EQ(ll_s.GetRealSize(), 401);
	ASSERT_EQ(ll_s.GetSize(), 401);
	ASSERT_EQ(memcmp(ll_s.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(ll_s.GetRealSize()), StringImplement::StringCategory::Large);
	ASSERT_EQ(s4_3.GetSize(), 401);
	ASSERT_EQ(s4_3.GetRealSize(), 401);
}

TEST(Storage, SetRealSizeTest)
{
	const Char* pcstr1 = SGE_STR("测试\0");
	const Char* pcstr2 = SGE_STR("测试一下中等长度的字符串，看看这样够不够字数，能不能过16/2\0");
	const Char* pcstr3 = SGE_STR("一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890\0");

	StringImplement::Storage<Char> s_s1(pcstr1, 3);
	ASSERT_EQ(s_s1.GetRealSize(), 3);
	ASSERT_EQ(s_s1.GetSize(), 3);
	ASSERT_EQ(memcmp(s_s1.GetData(), pcstr1, 3 * sizeof(Char)), 0);
	s_s1.SetRealSize(6);
	ASSERT_EQ(s_s1.GetRealSize(), 6);
	ASSERT_EQ(s_s1.GetSize(), 6);
	ASSERT_EQ(memcmp(s_s1.GetData(), pcstr1, 3 * sizeof(Char)), 0);

	StringImplement::Storage s_s2(pcstr1, 3);
	ASSERT_EQ(s_s2.GetRealSize(), 3);
	ASSERT_EQ(s_s2.GetSize(), 3);
	ASSERT_EQ(memcmp(s_s2.GetData(), pcstr1, 3 * sizeof(Char)), 0);
	s_s2.SetRealSize(100);
	ASSERT_EQ(s_s2.GetRealSize(), 100);
	ASSERT_EQ(s_s2.GetSize(), 3);
	ASSERT_EQ(memcmp(s_s2.GetData(), pcstr1, 3 * sizeof(Char)), 0);

	StringImplement::Storage s_s3(pcstr1, 3);
	ASSERT_EQ(s_s3.GetRealSize(), 3);
	ASSERT_EQ(s_s3.GetSize(), 3);
	ASSERT_EQ(memcmp(s_s3.GetData(), pcstr1, 3 * sizeof(Char)), 0);
	s_s3.SetRealSize(500);
	ASSERT_EQ(s_s3.GetRealSize(), 500);
	ASSERT_EQ(s_s3.GetSize(), 3);
	ASSERT_EQ(memcmp(s_s3.GetData(), pcstr1, 3 * sizeof(Char)), 0);

	s_s2.SetRealSize(4);
	ASSERT_EQ(s_s2.GetRealSize(), 3);
	ASSERT_EQ(s_s2.GetSize(), 3);
	ASSERT_EQ(memcmp(s_s2.GetData(), pcstr1, 3 * sizeof(Char)), 0);

	StringImplement::Storage m_s2(pcstr2, 32);
	ASSERT_EQ(m_s2.GetRealSize(), 32);
	ASSERT_EQ(m_s2.GetSize(), 32);
	ASSERT_EQ(memcmp(m_s2.GetData(), pcstr2, 32 * sizeof(Char)), 0);
	m_s2.SetRealSize(200);
	ASSERT_EQ(m_s2.GetRealSize(), 200);
	ASSERT_EQ(m_s2.GetSize(), 32);
	ASSERT_EQ(memcmp(m_s2.GetData(), pcstr2, 32 * sizeof(Char)), 0);

	StringImplement::Storage m_s3(pcstr2, 32);
	ASSERT_EQ(m_s3.GetRealSize(), 32);
	ASSERT_EQ(m_s3.GetSize(), 32);
	ASSERT_EQ(memcmp(m_s3.GetData(), pcstr2, 32 * sizeof(Char)), 0);
	m_s3.SetRealSize(500);
	ASSERT_EQ(m_s3.GetRealSize(), 500);
	ASSERT_EQ(m_s3.GetSize(), 32);
	ASSERT_EQ(memcmp(m_s3.GetData(), pcstr2, 32 * sizeof(Char)), 0);

	s_s3.SetRealSize(5);
	ASSERT_EQ(s_s3.GetRealSize(), 3);
	ASSERT_EQ(s_s3.GetSize(), 3);
	ASSERT_EQ(memcmp(s_s3.GetData(), pcstr1, 3 * sizeof(Char)), 0);

	m_s3.SetRealSize(200);
	ASSERT_EQ(m_s3.GetRealSize(), 200);
	ASSERT_EQ(m_s3.GetSize(), 32);
	ASSERT_EQ(memcmp(m_s3.GetData(), pcstr2, 32 * sizeof(Char)), 0);

	StringImplement::Storage l_s3(pcstr3, 401);
	ASSERT_EQ(l_s3.GetRealSize(), 401);
	ASSERT_EQ(l_s3.GetSize(), 401);
	ASSERT_EQ(memcmp(l_s3.GetData(), pcstr3, 401 * sizeof(Char)), 0);
	l_s3.SetRealSize(500);
	ASSERT_EQ(l_s3.GetRealSize(), 500);
	ASSERT_EQ(l_s3.GetSize(), 401);
	ASSERT_EQ(memcmp(l_s3.GetData(), pcstr3, 401 * sizeof(Char)), 0);
}

TEST(Storage, ClearTest)
{
	StringImplement::Storage<Char> s1;
	ASSERT_EQ(s1.GetSize(), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s1.GetRealSize()), StringImplement::StringCategory::Small);
	s1.Clear();
	ASSERT_EQ(s1.GetRealSize(), 0);

	const Char* pcstr1 = SGE_STR("测试\0");
	StringImplement::Storage<Char> s2(SGE_STR("测试\0"), 3);
	ASSERT_EQ(s2.GetSize(), 3);
	ASSERT_EQ(memcmp(s2.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s2.GetRealSize()), StringImplement::StringCategory::Small);
	s2.Clear();
	ASSERT_EQ(s2.GetRealSize(), 0);

	const Char* pcstr2 = SGE_STR("测试一下中等长度的字符串，看看这样够不够字数，能不能过16/2\0");
	StringImplement::Storage<Char> s3(pcstr2, 32);
	ASSERT_EQ(s3.GetRealSize(), 32);
	ASSERT_EQ(s3.GetSize(), 32);
	ASSERT_EQ(memcmp(s3.GetData(), pcstr2, sizeof(Char) * 32), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s3.GetRealSize()), StringImplement::StringCategory::Medium);
	s3.Clear();
	ASSERT_EQ(s3.GetRealSize(), 0);
	ASSERT_EQ(s3.GetSize(), 0);

	const Char* pcstr3 = SGE_STR("一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890\0");
	StringImplement::Storage<Char> s4(pcstr3, 401);
	ASSERT_EQ(s4.GetRealSize(), 401);
	ASSERT_EQ(s4.GetSize(), 401);
	ASSERT_EQ(memcmp(s4.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s4.GetRealSize()), StringImplement::StringCategory::Large);
	s4.Clear();
	ASSERT_EQ(s4.GetRealSize(), 0);
	ASSERT_EQ(s4.GetSize(), 0);
}

TEST(Storage, CopyOnWriteTest)
{
	const Char* pcstr = SGE_STR("一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890\0");

	StringImplement::Storage<Char> s1(pcstr, 401);
	ASSERT_EQ(StringImplement::StorageRef<Char>::GetCount(s1.GetData()), 1);
	ASSERT_EQ(s1.GetRealSize(), 401);
	ASSERT_EQ(s1.GetSize(), 401);
	ASSERT_EQ(memcmp(s1.GetData(), pcstr, sizeof(Char) * 401), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s1.GetRealSize()), StringImplement::StringCategory::Large);

	StringImplement::Storage<Char> s2(s1);
	ASSERT_EQ(s2.GetRealSize(), 401);
	ASSERT_EQ(s2.GetSize(), 401);
	ASSERT_EQ(memcmp(s2.GetData(), pcstr, sizeof(Char) * 401), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s2.GetRealSize()), StringImplement::StringCategory::Large);

	ASSERT_EQ(StringImplement::StorageRef<Char>::GetCount(s1.GetData()), 2);
	ASSERT_EQ(s1.GetData(), s2.GetData());

	s1.CopyOnWrite();

	ASSERT_EQ(StringImplement::StorageRef<Char>::GetCount(s1.GetData()), 1);
	ASSERT_EQ(s1.GetRealSize(), 401);
	ASSERT_EQ(s1.GetSize(), 401);
	ASSERT_EQ(memcmp(s1.GetData(), pcstr, sizeof(Char) * 401), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s1.GetRealSize()), StringImplement::StringCategory::Large);

	ASSERT_EQ(StringImplement::StorageRef<Char>::GetCount(s2.GetData()), 1);
	ASSERT_EQ(s2.GetRealSize(), 401);
	ASSERT_EQ(s2.GetSize(), 401);
	ASSERT_EQ(memcmp(s2.GetData(), pcstr, sizeof(Char) * 401), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s2.GetRealSize()), StringImplement::StringCategory::Large);
}

TEST(StringImplement, GetNextMultipleByteChar)
{
	//"这是12345abcde\0";
	/*
	8FD9 1000111111011001 => 11101000 10111111 10011001 => 232 191 153
	662F 110011000101111  => 11100110 10011000 10101111 => 230 152 175
	*/
	const char* pcstr = u8"这是12345abcde\0";
	auto p = pcstr;
	std::size_t i = 0;
	while (*p != '\0')
	{
		i += 1;
		p = StringImplement::GetNextMultipleByteChar<char, UTF8Trait>(p);
	}
	ASSERT_EQ(i, 12);
}

TEST(StringImplement, GetPreviousMultipleByteChar)
{
	const char* pcstr = u8"\0这是12345abcde";
	auto p = pcstr + 16;
	std::size_t i = 0;
	while (*p != '\0')
	{
		i += 1;
		p = StringImplement::GetPreviousMultipleByteChar<char, UTF8Trait>(p);
	}
	ASSERT_EQ(i, 12);
}

TEST(StringCore, GetCStringSize)
{
	const char* pcstr = u8"这是12345abcde\0";
	ASSERT_EQ((StringCore<char, UTF8Trait>::GetCStringSize(pcstr)), 12);
}

TEST(StringCore, GetCStringNormalSize)
{
	const char* pcstr = u8"这是12345abcde\0";
	ASSERT_EQ((StringCore<char, UTF8Trait>::GetCStringNormalSize(pcstr)), 16);
}

TEST(StringCore, InstanceTest)
{
	StringCore<Char> s1;
	ASSERT_EQ(s1.GetSize(), 0);
	StringCore<Char, CharTrait<Char>, StdAllocator> s2;
	ASSERT_EQ(s2.GetSize(), 0);
	StringCore<Char> s3(SGE_STR("这是12345abcde\0"));
	ASSERT_EQ(s3.GetSize(), 12);
	ASSERT_EQ(memcmp(s3.GetData(), SGE_STR("这是12345abcde\0"), s3.GetSize() * sizeof(Char)), 0);
	StringCore<char, UTF8Trait, StdAllocator> s4(u8"这是12345abcde\0");
	ASSERT_EQ(s4.GetSize(), 12);
	ASSERT_EQ(memcmp(s4.GetData(), u8"这是12345abcde\0", s4.GetSize() * sizeof(char)), 0);
}

TEST(StringCore, CopyConstructionTest)
{
	StringCore<Char> s1(SGE_STR("这是12345abcde\0"));
	ASSERT_EQ(s1.GetSize(), 12);
	ASSERT_EQ(memcmp(s1.GetData(), SGE_STR("这是12345abcde\0"), s1.GetSize() * sizeof(Char)), 0);

	StringCore<Char, CharTrait<Char>, StdAllocator> s2(SGE_STR("这是12345abcde\0"));
	ASSERT_EQ(s2.GetSize(), 12);
	ASSERT_EQ(memcmp(s2.GetData(), SGE_STR("这是12345abcde\0"), s2.GetSize() * sizeof(Char)), 0);

	StringCore<Char> s3(s1);
	ASSERT_EQ(s3.GetSize(), 12);
	ASSERT_EQ(memcmp(s3.GetData(), SGE_STR("这是12345abcde\0"), s3.GetSize() * sizeof(Char)), 0);

	StringCore<Char> s4(std::move(s3));
	ASSERT_EQ(s4.GetSize(), 12);
	ASSERT_EQ(memcmp(s4.GetData(), SGE_STR("这是12345abcde\0"), s4.GetSize() * sizeof(Char)), 0);

	StringCore<Char> s5(s2);
	ASSERT_EQ(s5.GetSize(), 12);
	ASSERT_EQ(memcmp(s5.GetData(), SGE_STR("这是12345abcde\0"), s5.GetSize() * sizeof(Char)), 0);

	StringCore<Char, CharTrait<Char>, StdAllocator> s6(std::move(s5));
	ASSERT_EQ(s6.GetSize(), 12);
	ASSERT_EQ(memcmp(s6.GetData(), SGE_STR("这是12345abcde\0"), s6.GetSize() * sizeof(Char)), 0);
}

TEST(StringCore, AssignmentTest)
{
	StringCore<Char> s(SGE_STR("test"));
	ASSERT_EQ(s.GetSize(), 4);
	ASSERT_EQ(memcmp(s.GetData(), SGE_STR("test"), sizeof(Char) * s.GetSize()), 0);

	StringCore<Char> s2, s3;
	ASSERT_EQ(s2.GetSize(), 0);

	s2 = s;

	ASSERT_EQ(s.GetSize(), 4);
	ASSERT_EQ(memcmp(s.GetData(), SGE_STR("test"), sizeof(Char) * s.GetSize()), 0);
	ASSERT_EQ(s2.GetSize(), 4);
	ASSERT_EQ(memcmp(s2.GetData(), SGE_STR("test"), sizeof(Char) * s2.GetSize()), 0);

	s3 = std::move(s2);

	ASSERT_EQ(s3.GetSize(), 4);
	ASSERT_EQ(memcmp(s3.GetData(), SGE_STR("test"), sizeof(Char) * s3.GetSize()), 0);

	StringCore<Char, CharTrait<Char>, StdAllocator> s4, s5;
	ASSERT_EQ(s4.GetSize(), 0);
	ASSERT_EQ(s5.GetSize(), 0);

	s4 = s3;

	ASSERT_EQ(s4.GetSize(), 4);
	ASSERT_EQ(memcmp(s4.GetData(), SGE_STR("test"), sizeof(Char) * s4.GetSize()), 0);

	s5 = std::move(s3);

	ASSERT_EQ(s5.GetSize(), 4);
	ASSERT_EQ(memcmp(s5.GetData(), SGE_STR("test"), sizeof(Char) * s5.GetSize()), 0);

	StringCore<Char> s6;
	ASSERT_EQ(s6.GetSize(), 0);

	s6 = SGE_STR("测试\0");

	ASSERT_EQ(s6.GetSize(), 2);
	ASSERT_EQ(memcmp(s6.GetData(), SGE_STR("测试\0"), sizeof(Char) * s6.GetSize()), 0);
}