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
#include "Container/Vector.hpp"
#include "gtest/gtest.h"
#include <cstring>

using namespace SpaceGameEngine;

inline bool IsUTF8CharSame(const char* p1, const char* p2)
{
	auto size1 = StringImplement::GetMultipleByteCharSize<char, UTF8Trait>(p1);
	auto size2 = StringImplement::GetMultipleByteCharSize<char, UTF8Trait>(p2);
	if (size1 != size2)
		return false;
	else
		return memcmp(p1, p2, sizeof(char) * size1) == 0;
}

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

	const Char* pcstr1 = SGE_STR("测试");
	StringImplement::Storage<Char> s2(SGE_STR("测试"), 3);
	ASSERT_EQ(s2.GetSize(), 3);
	ASSERT_EQ(memcmp(s2.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s2.GetRealSize()), StringImplement::StringCategory::Small);

	const Char* pcstr2 = SGE_STR("测试一下中等长度的字符串，看看这样够不够字数，能不能过16/2");
	StringImplement::Storage<Char> s3(pcstr2, 32);
	ASSERT_EQ(s3.GetRealSize(), 32);
	ASSERT_EQ(s3.GetSize(), 32);
	ASSERT_EQ(memcmp(s3.GetData(), pcstr2, sizeof(Char) * 32), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s3.GetRealSize()), StringImplement::StringCategory::Medium);

	const Char* pcstr3 = SGE_STR("一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890");
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

	const Char* pcstr1 = SGE_STR("测试");
	StringImplement::Storage<Char> s2(SGE_STR("测试"), 3);
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

	const Char* pcstr2 = SGE_STR("测试一下中等长度的字符串，看看这样够不够字数，能不能过16/2");
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

	const Char* pcstr3 = SGE_STR("一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890");
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

	const Char* pcstr1 = SGE_STR("测试");
	StringImplement::Storage<Char> s2(SGE_STR("测试"), 3);
	ASSERT_EQ(s2.GetSize(), 3);
	ASSERT_EQ(memcmp(s2.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s2.GetRealSize()), StringImplement::StringCategory::Small);

	const Char* pcstr2 = SGE_STR("测试一下中等长度的字符串，看看这样够不够字数，能不能过16/2");
	StringImplement::Storage<Char> s3(pcstr2, 32);
	ASSERT_EQ(s3.GetRealSize(), 32);
	ASSERT_EQ(s3.GetSize(), 32);
	ASSERT_EQ(memcmp(s3.GetData(), pcstr2, sizeof(Char) * 32), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s3.GetRealSize()), StringImplement::StringCategory::Medium);

	const Char* pcstr3 = SGE_STR("一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890");
	StringImplement::Storage<Char> s4(pcstr3, 401);
	ASSERT_EQ(s4.GetRealSize(), 401);
	ASSERT_EQ(s4.GetSize(), 401);
	ASSERT_EQ(memcmp(s4.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s4.GetRealSize()), StringImplement::StringCategory::Large);

	const Char* pcstr_s = SGE_STR("S");
	//63
	const Char* pcstr_m = SGE_STR("测试一下中等长度的字符串的拷贝，to see whether it can work correctly, 能不能过16/2");
	//403
	const Char* pcstr_l = SGE_STR("测试一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890");

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

	const Char* pcstr1 = SGE_STR("测试");
	StringImplement::Storage<Char> s2(SGE_STR("测试"), 3);
	ASSERT_EQ(s2.GetSize(), 3);
	ASSERT_EQ(memcmp(s2.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s2.GetRealSize()), StringImplement::StringCategory::Small);

	const Char* pcstr2 = SGE_STR("测试一下中等长度的字符串，看看这样够不够字数，能不能过16/2");
	StringImplement::Storage<Char> s3(pcstr2, 32);
	ASSERT_EQ(s3.GetRealSize(), 32);
	ASSERT_EQ(s3.GetSize(), 32);
	ASSERT_EQ(memcmp(s3.GetData(), pcstr2, sizeof(Char) * 32), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s3.GetRealSize()), StringImplement::StringCategory::Medium);

	const Char* pcstr3 = SGE_STR("一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890");
	StringImplement::Storage<Char> s4(pcstr3, 401);
	ASSERT_EQ(s4.GetRealSize(), 401);
	ASSERT_EQ(s4.GetSize(), 401);
	ASSERT_EQ(memcmp(s4.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s4.GetRealSize()), StringImplement::StringCategory::Large);

	const Char* pcstr_s = SGE_STR("S");
	//63
	const Char* pcstr_m = SGE_STR("测试一下中等长度的字符串的拷贝，to see whether it can work correctly, 能不能过16/2");
	//403
	const Char* pcstr_l = SGE_STR("测试一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890");

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

	const Char* pcstr1 = SGE_STR("测试");
	StringImplement::Storage<Char, StdAllocator> s2(SGE_STR("测试"), 3);
	ASSERT_EQ(s2.GetSize(), 3);
	ASSERT_EQ(memcmp(s2.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ((StringImplement::GetStringCategoryByRealSize<Char>(s2.GetRealSize())), (StringImplement::StringCategory::Small));

	const Char* pcstr2 = SGE_STR("测试一下中等长度的字符串，看看这样够不够字数，能不能过16/2");
	StringImplement::Storage<Char, StdAllocator> s3(pcstr2, 32);
	ASSERT_EQ(s3.GetRealSize(), 32);
	ASSERT_EQ(s3.GetSize(), 32);
	ASSERT_EQ(memcmp(s3.GetData(), pcstr2, sizeof(Char) * 32), 0);
	ASSERT_EQ((StringImplement::GetStringCategoryByRealSize<Char>(s3.GetRealSize())), (StringImplement::StringCategory::Medium));

	const Char* pcstr3 = SGE_STR("一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890");
	StringImplement::Storage<Char, StdAllocator> s4(pcstr3, 401);
	ASSERT_EQ(s4.GetRealSize(), 401);
	ASSERT_EQ(s4.GetSize(), 401);
	ASSERT_EQ(memcmp(s4.GetData(), pcstr3, sizeof(Char) * 401), 0);
	ASSERT_EQ((StringImplement::GetStringCategoryByRealSize<Char>(s4.GetRealSize())), (StringImplement::StringCategory::Large));

	const Char* pcstr_s = SGE_STR("S");
	//63
	const Char* pcstr_m = SGE_STR("测试一下中等长度的字符串的拷贝，to see whether it can work correctly, 能不能过16/2");
	//403
	const Char* pcstr_l = SGE_STR("测试一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890");

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

	const Char* pcstr1 = SGE_STR("测试");
	StringImplement::Storage<Char, StdAllocator> s2(SGE_STR("测试"), 3);
	ASSERT_EQ(s2.GetSize(), 3);
	ASSERT_EQ(memcmp(s2.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ((StringImplement::GetStringCategoryByRealSize<Char>(s2.GetRealSize())), (StringImplement::StringCategory::Small));

	StringImplement::Storage<Char, StdAllocator> s2_2(SGE_STR("测试"), 3);
	ASSERT_EQ(s2_2.GetSize(), 3);
	ASSERT_EQ(memcmp(s2_2.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ((StringImplement::GetStringCategoryByRealSize<Char>(s2_2.GetRealSize())), (StringImplement::StringCategory::Small));

	StringImplement::Storage<Char, StdAllocator> s2_3(SGE_STR("测试"), 3);
	ASSERT_EQ(s2_3.GetSize(), 3);
	ASSERT_EQ(memcmp(s2_3.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ((StringImplement::GetStringCategoryByRealSize<Char>(s2_3.GetRealSize())), (StringImplement::StringCategory::Small));

	const Char* pcstr2 = SGE_STR("测试一下中等长度的字符串，看看这样够不够字数，能不能过16/2");
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

	const Char* pcstr3 = SGE_STR("一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890");
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

	const Char* pcstr_s = SGE_STR("S");
	//63
	const Char* pcstr_m = SGE_STR("测试一下中等长度的字符串的拷贝，to see whether it can work correctly, 能不能过16/2");
	//403
	const Char* pcstr_l = SGE_STR("测试一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890");

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
	const Char* pcstr1 = SGE_STR("测试");
	const Char* pcstr2 = SGE_STR("测试一下中等长度的字符串，看看这样够不够字数，能不能过16/2");
	const Char* pcstr3 = SGE_STR("一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890");

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

	const Char* pcstr1 = SGE_STR("测试");
	StringImplement::Storage<Char> s2(SGE_STR("测试"), 3);
	ASSERT_EQ(s2.GetSize(), 3);
	ASSERT_EQ(memcmp(s2.GetData(), pcstr1, sizeof(Char) * 3), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s2.GetRealSize()), StringImplement::StringCategory::Small);
	s2.Clear();
	ASSERT_EQ(s2.GetRealSize(), 0);

	const Char* pcstr2 = SGE_STR("测试一下中等长度的字符串，看看这样够不够字数，能不能过16/2");
	StringImplement::Storage<Char> s3(pcstr2, 32);
	ASSERT_EQ(s3.GetRealSize(), 32);
	ASSERT_EQ(s3.GetSize(), 32);
	ASSERT_EQ(memcmp(s3.GetData(), pcstr2, sizeof(Char) * 32), 0);
	ASSERT_EQ(StringImplement::GetStringCategoryByRealSize<Char>(s3.GetRealSize()), StringImplement::StringCategory::Medium);
	s3.Clear();
	ASSERT_EQ(s3.GetRealSize(), 0);
	ASSERT_EQ(s3.GetSize(), 0);

	const Char* pcstr3 = SGE_STR("一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890");
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
	const Char* pcstr = SGE_STR("一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890");

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

TEST(StringImplement, GetNextMultipleByteCharTest)
{
	//"这是12345abcde";
	/*
	8FD9 1000111111011001 => 11101000 10111111 10011001 => 232 191 153
	662F 110011000101111  => 11100110 10011000 10101111 => 230 152 175
	*/
	const char* pcstr = u8"这是12345abcde";
	auto p = pcstr;
	std::size_t i = 0;
	while (*p != '\0')
	{
		i += 1;
		p = StringImplement::GetNextMultipleByteChar<char, UTF8Trait>(p);
	}
	ASSERT_EQ(i, 12);
}

TEST(StringImplement, GetPreviousMultipleByteCharTest)
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

TEST(StringImplement, GetMultipleByteCharSizeTest)
{
	ASSERT_EQ((StringImplement::GetMultipleByteCharSize<char, UTF8Trait>(u8"这")), 3);
	ASSERT_EQ((StringImplement::GetMultipleByteCharSize<char, UTF8Trait>(u8"是")), 3);
	ASSERT_EQ((StringImplement::GetMultipleByteCharSize<char, UTF8Trait>(u8"a")), 1);
	ASSERT_EQ((StringImplement::GetMultipleByteCharSize<char, UTF8Trait>(u8"1")), 1);
}

TEST(StringImplement, UTF8CharToUCS2CharTest)
{
	ASSERT_EQ(StringImplement::UTF8CharToUCS2Char(u8"严"), SGE_STR('严'));
	ASSERT_EQ(StringImplement::UTF8CharToUCS2Char(u8"и"), SGE_STR('и'));
}

TEST(StringImplement, GetUCS2CharToUTF8CharSizeTest)
{
	ASSERT_EQ(StringImplement::GetUCS2CharToUTF8CharSize(SGE_STR('严')), 3);
	ASSERT_EQ(StringImplement::GetUCS2CharToUTF8CharSize(SGE_STR('и')), 2);
	ASSERT_EQ(StringImplement::GetUCS2CharToUTF8CharSize(SGE_STR('!')), 1);
}

TEST(StringImplement, UCS2CharToUTF8CharTest)
{
	char pbuf[4];
	memset(pbuf, 0, sizeof(pbuf));
	char* pre = nullptr;
	pre = StringImplement::UCS2CharToUTF8Char(SGE_STR('这'), pbuf);
	ASSERT_TRUE(IsUTF8CharSame(pbuf, u8"这"));
	ASSERT_EQ(pre, pbuf + 3);
	pre = StringImplement::UCS2CharToUTF8Char(SGE_STR('и'), pbuf);
	ASSERT_TRUE(IsUTF8CharSame(pbuf, u8"и"));
	ASSERT_EQ(pre, pbuf + 2);
	pre = StringImplement::UCS2CharToUTF8Char(SGE_STR('1'), pbuf);
	ASSERT_TRUE(IsUTF8CharSame(pbuf, u8"1"));
	ASSERT_EQ(pre, pbuf + 1);
}

TEST(StringImplement, CompareMultipleByteCharTest)
{
	ASSERT_EQ((StringImplement::CompareMultipleByteChar<char, UTF8Trait>(u8"a", u8"b")), -1);
	ASSERT_EQ((StringImplement::CompareMultipleByteChar<char, UTF8Trait>(u8"a", u8"这")), -1);
	ASSERT_EQ((StringImplement::CompareMultipleByteChar<char, UTF8Trait>(u8"这", u8"a")), 1);
	ASSERT_EQ((StringImplement::CompareMultipleByteChar<char, UTF8Trait>(u8"这", u8"这")), 0);
	ASSERT_EQ((StringImplement::CompareMultipleByteChar<char, UTF8Trait>(u8"a", u8"a")), 0);
}

TEST(StringCore, GetCStringSize)
{
	const char* pcstr = u8"这是12345abcde";
	ASSERT_EQ((StringCore<char, UTF8Trait>::GetCStringSize(pcstr)), 12);
}

TEST(StringCore, GetCStringNormalSize)
{
	const char* pcstr = u8"这是12345abcde";
	ASSERT_EQ((StringCore<char, UTF8Trait>::GetCStringNormalSize(pcstr)), 16);
}

TEST(StringCore, InstanceTest)
{
	StringCore<Char> s1;
	ASSERT_EQ(s1.GetSize(), 0);
	StringCore<Char, CharTrait<Char>, StdAllocator> s2;
	ASSERT_EQ(s2.GetSize(), 0);
	StringCore<Char> s3(SGE_STR("这是12345abcde"));
	ASSERT_EQ(s3.GetSize(), 12);
	ASSERT_EQ(memcmp(s3.GetData(), SGE_STR("这是12345abcde"), s3.GetSize() * sizeof(Char)), 0);
	StringCore<char, UTF8Trait, StdAllocator> s4(u8"这是12345abcde");
	ASSERT_EQ(s4.GetSize(), 12);
	ASSERT_EQ(memcmp(s4.GetData(), u8"这是12345abcde", s4.GetSize() * sizeof(char)), 0);
}

TEST(StringCore, CopyConstructionTest)
{
	StringCore<Char> s1(SGE_STR("这是12345abcde"));
	ASSERT_EQ(s1.GetSize(), 12);
	ASSERT_EQ(memcmp(s1.GetData(), SGE_STR("这是12345abcde"), s1.GetSize() * sizeof(Char)), 0);

	StringCore<Char, CharTrait<Char>, StdAllocator> s2(SGE_STR("这是12345abcde"));
	ASSERT_EQ(s2.GetSize(), 12);
	ASSERT_EQ(memcmp(s2.GetData(), SGE_STR("这是12345abcde"), s2.GetSize() * sizeof(Char)), 0);

	StringCore<Char> s3(s1);
	ASSERT_EQ(s3.GetSize(), 12);
	ASSERT_EQ(memcmp(s3.GetData(), SGE_STR("这是12345abcde"), s3.GetSize() * sizeof(Char)), 0);

	StringCore<Char> s4(std::move(s3));
	ASSERT_EQ(s4.GetSize(), 12);
	ASSERT_EQ(memcmp(s4.GetData(), SGE_STR("这是12345abcde"), s4.GetSize() * sizeof(Char)), 0);

	StringCore<Char> s5(s2);
	ASSERT_EQ(s5.GetSize(), 12);
	ASSERT_EQ(memcmp(s5.GetData(), SGE_STR("这是12345abcde"), s5.GetSize() * sizeof(Char)), 0);

	StringCore<Char, CharTrait<Char>, StdAllocator> s6(std::move(s5));
	ASSERT_EQ(s6.GetSize(), 12);
	ASSERT_EQ(memcmp(s6.GetData(), SGE_STR("这是12345abcde"), s6.GetSize() * sizeof(Char)), 0);
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

	s6 = SGE_STR("测试");

	ASSERT_EQ(s6.GetSize(), 2);
	ASSERT_EQ(memcmp(s6.GetData(), SGE_STR("测试"), sizeof(Char) * s6.GetSize()), 0);
}

TEST(StringCore, EqualOperatorTest)
{
	StringCore<Char> s1(SGE_STR("这是个测试"));
	StringCore<Char, CharTrait<Char>, StdAllocator> s2(SGE_STR("这是个测试"));
	StringCore<Char> s3(SGE_STR("test"));

	ASSERT_EQ(s1 == s2, true);
	ASSERT_EQ(s1 != s2, false);
	ASSERT_EQ(s1 == s3, false);
	ASSERT_EQ(s2 == s3, false);
	ASSERT_EQ(s1 == SGE_STR("这是个测试"), true);
	ASSERT_EQ(s2 == SGE_STR("test"), false);
	ASSERT_EQ(s3 == SGE_STR("test"), true);
}

TEST(StringCore, ClearTest)
{
	StringCore<Char> s(SGE_STR("test"));
	ASSERT_EQ(s, SGE_STR("test"));
	ASSERT_EQ(s.GetSize(), 4);
	s.Clear();
	ASSERT_EQ(s != SGE_STR("test"), true);
	ASSERT_EQ(s.GetSize(), 0);
}

TEST(StringCore, GetNormalSizeTest)
{
	StringCore<Char> s1(SGE_STR("这是"));
	ASSERT_EQ(s1, SGE_STR("这是"));
	ASSERT_EQ(s1.GetSize(), 2);
	ASSERT_EQ(s1.GetNormalSize(), 2);

	StringCore<char, UTF8Trait> s2(u8"这是");
	ASSERT_EQ(s2, u8"这是");
	ASSERT_EQ(s2.GetSize(), 2);
	ASSERT_EQ(s2.GetNormalSize(), 6);
}

TEST(StringCore, GetRealSizeTest)
{
	StringCore<Char> s1(SGE_STR("这是"));
	ASSERT_EQ(s1, SGE_STR("这是"));
	ASSERT_EQ(s1.GetSize(), 2);
	ASSERT_EQ(s1.GetRealSize(), 2);

	StringCore<char, UTF8Trait> s2(u8"这是");
	ASSERT_EQ(s2, u8"这是");
	ASSERT_EQ(s2.GetSize(), 2);
	ASSERT_EQ(s2.GetRealSize(), 6);
}

TEST(StringCore, SetRealSizeTest)
{
	StringCore<Char> s1(SGE_STR("这是"));
	ASSERT_EQ(s1, SGE_STR("这是"));
	ASSERT_EQ(s1.GetSize(), 2);
	ASSERT_EQ(s1.GetRealSize(), 2);
	s1.SetRealSize(100);
	ASSERT_EQ(s1, SGE_STR("这是"));
	ASSERT_EQ(s1.GetSize(), 2);
	ASSERT_EQ(s1.GetRealSize(), 100);
	ASSERT_EQ(s1.GetNormalSize(), 2);

	StringCore<char, UTF8Trait> s2(u8"这是");
	ASSERT_EQ(s2, u8"这是");
	ASSERT_EQ(s2.GetSize(), 2);
	ASSERT_EQ(s2.GetRealSize(), 6);
	s2.SetRealSize(300);
	ASSERT_EQ(s2, u8"这是");
	ASSERT_EQ(s2.GetSize(), 2);
	ASSERT_EQ(s2.GetRealSize(), 300);
	ASSERT_EQ(s2.GetNormalSize(), 6);
}

TEST(StringCore, SelfPlusTest)
{
	UCS2String s1(SGE_STR("这是"));
	ASSERT_EQ(s1.GetSize(), 2);
	ASSERT_EQ(s1, SGE_STR("这是"));
	UCS2String s1_2(SGE_STR("测试"));
	ASSERT_EQ(s1_2.GetSize(), 2);
	ASSERT_EQ(s1_2, SGE_STR("测试"));
	UCS2String s1_3(SGE_STR("1234567890abcdefghij"));
	ASSERT_EQ(s1_3.GetSize(), 20);
	ASSERT_EQ(s1_3, SGE_STR("1234567890abcdefghij"));
	const Char* pc_long_str = SGE_STR("一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890");
	StringCore<Char16, UCS2Trait, StdAllocator> s1_4(pc_long_str);
	ASSERT_EQ(s1_4.GetSize(), 400);
	ASSERT_EQ(s1_4, pc_long_str);
	s1 += s1_2;
	ASSERT_EQ(s1.GetSize(), 4);
	ASSERT_EQ(s1, SGE_STR("这是测试"));
	s1 += s1_3;
	ASSERT_EQ(s1.GetSize(), 24);
	ASSERT_EQ(s1, SGE_STR("这是测试1234567890abcdefghij"));
	s1 += s1_4;
	ASSERT_EQ(s1.GetSize(), 424);
	ASSERT_EQ(s1, SGE_STR("这是测试1234567890abcdefghij一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890"));
	s1 += SGE_STR('这');
	ASSERT_EQ(s1.GetSize(), 425);
	ASSERT_EQ(s1, SGE_STR("这是测试1234567890abcdefghij一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890这"));
	s1 += SGE_STR("是测试");
	ASSERT_EQ(s1.GetSize(), 428);
	ASSERT_EQ(s1, SGE_STR("这是测试1234567890abcdefghij一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890这是测试"));

	UTF8String s2(u8"这是");
	ASSERT_EQ(s2.GetSize(), 2);
	ASSERT_EQ(s2, u8"这是");
	UTF8String s2_2(u8"测试");
	ASSERT_EQ(s2_2.GetSize(), 2);
	ASSERT_EQ(s2_2, u8"测试");
	UTF8String s2_3(u8"1234567890abcdefghij");
	ASSERT_EQ(s2_3.GetSize(), 20);
	ASSERT_EQ(s2_3, u8"1234567890abcdefghij");
	const char* pc_long_str_2 = u8"一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890";
	StringCore<char, UTF8Trait, StdAllocator> s2_4(pc_long_str_2);
	ASSERT_EQ(s2_4.GetSize(), 400);
	ASSERT_EQ(s2_4, pc_long_str_2);
	s2 += s2_2;
	ASSERT_EQ(s2.GetSize(), 4);
	ASSERT_EQ(s2, u8"这是测试");
	s2 += s2_3;
	ASSERT_EQ(s2.GetSize(), 24);
	ASSERT_EQ(s2, u8"这是测试1234567890abcdefghij");
	s2 += s2_4;
	ASSERT_EQ(s2.GetSize(), 424);
	ASSERT_EQ(s2, u8"这是测试1234567890abcdefghij一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890");
	s2 += u8'a';
	ASSERT_EQ(s2.GetSize(), 425);
	ASSERT_EQ(s2, u8"这是测试1234567890abcdefghij一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890a");
	s2 += u8"这是测试";
	ASSERT_EQ(s2.GetSize(), 429);
	ASSERT_EQ(s2, u8"这是测试1234567890abcdefghij一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890a这是测试");
}

TEST(StringCore, OperatorPlusTest)
{
	UCS2String s1(SGE_STR("这是"));
	ASSERT_EQ(s1.GetSize(), 2);
	ASSERT_EQ(s1, SGE_STR("这是"));
	UCS2String s1_2(SGE_STR("测试"));
	ASSERT_EQ(s1_2.GetSize(), 2);
	ASSERT_EQ(s1_2, SGE_STR("测试"));
	UCS2String s1_3(SGE_STR("1234567890abcdefghij"));
	ASSERT_EQ(s1_3.GetSize(), 20);
	ASSERT_EQ(s1_3, SGE_STR("1234567890abcdefghij"));
	const Char* pc_long_str = SGE_STR("一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890");
	StringCore<Char16, UCS2Trait, StdAllocator> s1_4(pc_long_str);
	ASSERT_EQ(s1_4.GetSize(), 400);
	ASSERT_EQ(s1_4, pc_long_str);

	ASSERT_EQ(s1 + s1_2, SGE_STR("这是测试"));
	ASSERT_EQ(s1 + s1_3, SGE_STR("这是1234567890abcdefghij"));
	ASSERT_EQ(s1 + s1_4, SGE_STR("这是一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890"));
	ASSERT_EQ(s1 + SGE_STR('!'), SGE_STR("这是!"));
	ASSERT_EQ(s1 + SGE_STR("测试"), SGE_STR("这是测试"));

	ASSERT_EQ(s1.GetSize(), 2);
	ASSERT_EQ(s1, SGE_STR("这是"));
	ASSERT_EQ(s1_2.GetSize(), 2);
	ASSERT_EQ(s1_2, SGE_STR("测试"));
	ASSERT_EQ(s1_3.GetSize(), 20);
	ASSERT_EQ(s1_3, SGE_STR("1234567890abcdefghij"));
	ASSERT_EQ(s1_4.GetSize(), 400);
	ASSERT_EQ(s1_4, pc_long_str);

	UTF8String s2(u8"这是");
	ASSERT_EQ(s2.GetSize(), 2);
	ASSERT_EQ(s2, u8"这是");
	UTF8String s2_2(u8"测试");
	ASSERT_EQ(s2_2.GetSize(), 2);
	ASSERT_EQ(s2_2, u8"测试");
	UTF8String s2_3(u8"1234567890abcdefghij");
	ASSERT_EQ(s2_3.GetSize(), 20);
	ASSERT_EQ(s2_3, u8"1234567890abcdefghij");
	const char* pc_long_str_2 = u8"一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890";
	StringCore<char, UTF8Trait, StdAllocator> s2_4(pc_long_str_2);
	ASSERT_EQ(s2_4.GetSize(), 400);
	ASSERT_EQ(s2_4, pc_long_str_2);

	ASSERT_EQ(s2 + s2_2, u8"这是测试");
	ASSERT_EQ(s2 + s2_3, u8"这是1234567890abcdefghij");
	ASSERT_EQ(s2 + s2_4, u8"这是一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890一二三四五六七八九十1234567890");
	ASSERT_EQ(s2 + u8'!', u8"这是!");
	ASSERT_EQ(s2 + u8"测试", u8"这是测试");

	ASSERT_EQ(s2.GetSize(), 2);
	ASSERT_EQ(s2, u8"这是");
	ASSERT_EQ(s2_2.GetSize(), 2);
	ASSERT_EQ(s2_2, u8"测试");
	ASSERT_EQ(s2_3.GetSize(), 20);
	ASSERT_EQ(s2_3, u8"1234567890abcdefghij");
	ASSERT_EQ(s2_4.GetSize(), 400);
	ASSERT_EQ(s2_4, pc_long_str_2);
}

TEST(StringCore, SizeValueConstructionTest)
{
	UCS2String s1(10, SGE_STR('这'));
	ASSERT_EQ(s1.GetSize(), 10);
	ASSERT_EQ(s1.GetNormalSize(), 10);
	ASSERT_EQ(s1, SGE_STR("这这这这这这这这这这"));
	ASSERT_EQ(*(s1.GetData() + 10), 0);

	UTF8String s2(10, u8"这");
	ASSERT_EQ(s2.GetSize(), 10);
	ASSERT_EQ(s2.GetNormalSize(), 30);
	ASSERT_EQ(s2, u8"这这这这这这这这这这");
	ASSERT_EQ(*(s2.GetData() + 30), 0);
}

TEST(StringCore, IteratorPairConstructionTest)
{
	Vector<Char16> v1 = {SGE_STR('这'), SGE_STR('是'), SGE_STR('测'), SGE_STR('试')};

	UCS2String s1(v1.GetBegin(), v1.GetEnd());
	ASSERT_EQ(s1.GetSize(), 4);
	ASSERT_EQ(s1.GetNormalSize(), 4);
	ASSERT_EQ(s1, SGE_STR("这是测试"));
	ASSERT_EQ(*(s1.GetData() + 4), 0);

	const UTF8String s2_0(u8"12345这是个测试abcde");
	ASSERT_EQ(s2_0.GetSize(), 15);
	ASSERT_EQ(s2_0.GetNormalSize(), 25);
	ASSERT_EQ(s2_0, u8"12345这是个测试abcde");
	ASSERT_EQ(*(s2_0.GetData() + 25), 0);

	UTF8String s2(s2_0.GetConstBegin(), s2_0.GetConstEnd());
	ASSERT_EQ(s2.GetSize(), 15);
	ASSERT_EQ(s2.GetNormalSize(), 25);
	ASSERT_EQ(s2, u8"12345这是个测试abcde");
	ASSERT_EQ(*(s2.GetData() + 25), 0);
}

TEST(StringCore, CompareLessTest)
{
	UCS2String s1_1(SGE_STR("abcde"));
	UCS2String s1_2(SGE_STR("ab"));
	UCS2String s1_3(SGE_STR("这是"));
	StringCore<Char16, UCS2Trait, StdAllocator> s1_4(SGE_STR("严格"));

	ASSERT_TRUE(s1_2 < s1_1);
	ASSERT_TRUE(s1_4 < s1_3);
	ASSERT_TRUE(s1_1 < s1_3);
	ASSERT_TRUE(s1_2 < s1_3);
	ASSERT_TRUE(s1_2 < s1_4);
	ASSERT_TRUE(s1_1 < s1_4);
	ASSERT_TRUE(s1_1 < SGE_STR("b"));
	ASSERT_TRUE(s1_3 < SGE_STR("这是测试"));

	UTF8String s2_1(u8"abcde");
	UTF8String s2_2(u8"ab");
	UTF8String s2_3(u8"这是");
	StringCore<char, UTF8Trait, StdAllocator> s2_4(u8"严格");

	ASSERT_TRUE(s2_2 < s2_1);
	ASSERT_TRUE(s2_4 < s2_3);
	ASSERT_TRUE(s2_1 < s2_3);
	ASSERT_TRUE(s2_2 < s2_3);
	ASSERT_TRUE(s2_2 < s2_4);
	ASSERT_TRUE(s2_1 < s2_4);
	ASSERT_TRUE(s2_1 < u8"b");
	ASSERT_TRUE(s2_3 < u8"这是测试");
}

TEST(StringCore, CompareLargeTest)
{
	UCS2String s1_1(SGE_STR("abcde"));
	UCS2String s1_2(SGE_STR("ab"));
	UCS2String s1_3(SGE_STR("这是"));
	StringCore<Char16, UCS2Trait, StdAllocator> s1_4(SGE_STR("严格"));

	ASSERT_TRUE(s1_1 > s1_2);
	ASSERT_TRUE(s1_3 > s1_4);
	ASSERT_TRUE(s1_3 > s1_1);
	ASSERT_TRUE(s1_3 > s1_2);
	ASSERT_TRUE(s1_4 > s1_2);
	ASSERT_TRUE(s1_4 > s1_1);
	ASSERT_TRUE(s1_1 > SGE_STR("a"));
	ASSERT_TRUE(s1_3 > SGE_STR("严1"));

	UTF8String s2_1(u8"abcde");
	UTF8String s2_2(u8"ab");
	UTF8String s2_3(u8"这是");
	StringCore<char, UTF8Trait, StdAllocator> s2_4(u8"严格");

	ASSERT_TRUE(s2_1 > s2_2);
	ASSERT_TRUE(s2_3 > s2_4);
	ASSERT_TRUE(s2_3 > s2_1);
	ASSERT_TRUE(s2_3 > s2_2);
	ASSERT_TRUE(s2_4 > s2_2);
	ASSERT_TRUE(s2_4 > s2_1);
	ASSERT_TRUE(s2_1 > u8"a");
	ASSERT_TRUE(s2_3 > u8"严1");
}

TEST(StringCore, CompareLessEqualTest)
{
	UCS2String s1_1(SGE_STR("abc"));
	UCS2String s1_2(SGE_STR("a"));
	StringCore<Char16, UCS2Trait, StdAllocator> s1_3(SGE_STR("abc"));
	ASSERT_TRUE(s1_2 <= s1_1);
	ASSERT_TRUE(s1_1 <= s1_3);
	ASSERT_TRUE(s1_3 <= s1_1);
	ASSERT_FALSE(s1_1 <= s1_2);
	ASSERT_FALSE(s1_1 <= SGE_STR("aaaaa测试"));
	ASSERT_TRUE(s1_1 <= SGE_STR("测试"));

	UTF8String s2_1(u8"abc");
	UTF8String s2_2(u8"a");
	StringCore<char, UTF8Trait, StdAllocator> s2_3(u8"abc");
	ASSERT_TRUE(s2_2 <= s2_1);
	ASSERT_TRUE(s2_1 <= s2_3);
	ASSERT_TRUE(s2_3 <= s2_1);
	ASSERT_FALSE(s2_1 <= s2_2);
	ASSERT_FALSE(s2_1 <= u8"aaaaa测试");
	ASSERT_TRUE(s2_1 <= u8"测试");
}

TEST(StringCore, CompareLargeEqualTest)
{
	UCS2String s1_1(SGE_STR("abc"));
	UCS2String s1_2(SGE_STR("a"));
	StringCore<Char16, UCS2Trait, StdAllocator> s1_3(SGE_STR("abc"));
	ASSERT_TRUE(s1_1 >= s1_2);
	ASSERT_TRUE(s1_3 >= s1_1);
	ASSERT_TRUE(s1_1 >= s1_3);
	ASSERT_FALSE(s1_2 >= s1_1);
	ASSERT_TRUE(s1_1 >= SGE_STR("aaaaa测试"));
	ASSERT_FALSE(s1_1 >= SGE_STR("测试"));

	UTF8String s2_1(u8"abc");
	UTF8String s2_2(u8"a");
	StringCore<char, UTF8Trait, StdAllocator> s2_3(u8"abc");
	ASSERT_TRUE(s2_1 >= s2_2);
	ASSERT_TRUE(s2_3 >= s2_1);
	ASSERT_TRUE(s2_1 >= s2_3);
	ASSERT_FALSE(s2_2 >= s2_1);
	ASSERT_TRUE(s2_1 >= u8"aaaaa测试");
	ASSERT_FALSE(s2_1 >= u8"测试");
}

TEST(StringCore, IsTotallyOrderedTest)
{
	ASSERT_TRUE(IsTotallyOrdered<UCS2String>::Value);
	ASSERT_TRUE(IsTotallyOrdered<UTF8String>::Value);
}

TEST(StringCore, InsertTest)
{
	UCS2String s1_1(SGE_STR("12345"));
	ASSERT_EQ(s1_1.GetSize(), 5);
	const UCS2String s1_2(SGE_STR("这是个测试"));
	ASSERT_EQ(s1_2.GetSize(), 5);
	UCS2String s1_3(s1_1);
	ASSERT_EQ(s1_3.GetSize(), 5);
	StringCore<Char16, UCS2Trait, StdAllocator> s1_4(SGE_STR("Это тест"));
	ASSERT_EQ(s1_4.GetSize(), 8);
	Vector<Char16> v1 = {SGE_STR('字'),
						 SGE_STR('符')};
	ASSERT_EQ(v1.GetSize(), 2);

	auto iter1_1 = s1_1.Insert(s1_1.GetConstEnd(), s1_2);
	ASSERT_EQ(s1_1.GetSize(), 10);
	ASSERT_EQ(s1_1, SGE_STR("12345这是个测试"));
	ASSERT_EQ(*iter1_1, SGE_STR('这'));

	auto iter1_2 = s1_3.Insert(s1_3.GetBegin() + 1, s1_2);
	ASSERT_EQ(s1_3.GetSize(), 10);
	ASSERT_EQ(s1_3, SGE_STR("1这是个测试2345"));
	ASSERT_EQ(*iter1_2, SGE_STR('这'));

	auto iter1_3 = s1_1.Insert(s1_1.GetConstBegin() + 1, s1_4);
	ASSERT_EQ(s1_1.GetSize(), 18);
	ASSERT_EQ(s1_1, SGE_STR("1Это тест2345这是个测试"));
	ASSERT_EQ(*iter1_3, SGE_STR('Э'));

	auto iter1_4 = s1_1.Insert(s1_1.GetConstEnd() - 1, SGE_STR("δοκιμή"));
	ASSERT_EQ(s1_1.GetSize(), 24);
	ASSERT_EQ(s1_1, SGE_STR("1Это тест2345这是个测δοκιμή试"));
	ASSERT_EQ(*iter1_4, SGE_STR('δ'));

	auto iter1_5 = s1_1.Insert(s1_1.GetConstBegin() + 2, v1.GetConstBegin(), v1.GetConstEnd());
	ASSERT_EQ(s1_1.GetSize(), 26);
	ASSERT_EQ(s1_1, SGE_STR("1Э字符то тест2345这是个测δοκιμή试"));
	ASSERT_EQ(*iter1_5, SGE_STR('字'));

	UTF8String s2_1(u8"12345");
	ASSERT_EQ(s2_1.GetSize(), 5);
	const UTF8String s2_2(u8"这是个测试");
	ASSERT_EQ(s2_2.GetSize(), 5);
	UTF8String s2_3(s2_1);
	ASSERT_EQ(s2_3.GetSize(), 5);
	StringCore<char, UTF8Trait, StdAllocator> s2_4(SGE_U8STR("Это тест"));
	ASSERT_EQ(s2_4.GetSize(), 8);
	UTF8String s2_5(SGE_U8STR("字符"));
	ASSERT_EQ(s2_5.GetSize(), 2);

	auto iter2_1 = s2_1.Insert(s2_1.GetConstEnd(), s2_2);
	ASSERT_EQ(s2_1.GetSize(), 10);
	ASSERT_EQ(s2_1, SGE_U8STR("12345这是个测试"));
	ASSERT_TRUE(IsUTF8CharSame(*iter2_1, SGE_U8STR("这")));

	auto iter2_2 = s2_3.Insert(s2_3.GetBegin() + 1, s2_2);
	ASSERT_EQ(s2_3.GetSize(), 10);
	ASSERT_EQ(s2_3, SGE_U8STR("1这是个测试2345"));
	ASSERT_TRUE(IsUTF8CharSame(*iter2_2, SGE_U8STR("这")));

	auto iter2_3 = s2_1.Insert(s2_1.GetConstBegin() + 1, s2_4);
	ASSERT_EQ(s2_1.GetSize(), 18);
	ASSERT_EQ(s2_1, SGE_U8STR("1Это тест2345这是个测试"));
	ASSERT_TRUE(IsUTF8CharSame(*iter2_3, SGE_U8STR("Э")));

	auto iter2_4 = s2_1.Insert(s2_1.GetConstEnd() - 1, SGE_U8STR("δοκιμή"));
	ASSERT_EQ(s2_1.GetSize(), 24);
	ASSERT_EQ(s2_1, SGE_U8STR("1Это тест2345这是个测δοκιμή试"));
	ASSERT_TRUE(IsUTF8CharSame(*iter2_4, SGE_U8STR("δ")));

	auto iter2_5 = s2_1.Insert(s2_1.GetConstBegin() + 2, s2_5.GetConstBegin(), s2_5.GetConstEnd());
	ASSERT_EQ(s2_1.GetSize(), 26);
	ASSERT_EQ(s2_1, SGE_U8STR("1Э字符то тест2345这是个测δοκιμή试"));
	ASSERT_TRUE(IsUTF8CharSame(*iter2_5, SGE_U8STR("字")));
}

TEST(StringCore, InsertReverseTest)
{
	UCS2String s1_1(SGE_STR("12345"));
	ASSERT_EQ(s1_1.GetSize(), 5);
	const UCS2String s1_2(SGE_STR("这是个测试"));
	ASSERT_EQ(s1_2.GetSize(), 5);
	UCS2String s1_3(s1_1);
	ASSERT_EQ(s1_3.GetSize(), 5);
	UCS2String s1_4(s1_1);
	ASSERT_EQ(s1_4.GetSize(), 5);
	StringCore<Char16, UCS2Trait, StdAllocator> s1_5(SGE_STR("Это тест"));
	ASSERT_EQ(s1_5.GetSize(), 8);
	Vector<Char16> v1 = {SGE_STR('字'),
						 SGE_STR('符')};
	ASSERT_EQ(v1.GetSize(), 2);

	auto iter1_1 = s1_1.Insert(s1_1.GetConstReverseBegin(), s1_2);
	ASSERT_EQ(s1_1.GetSize(), 10);
	ASSERT_EQ(s1_1, SGE_STR("12345试测个是这"));
	ASSERT_EQ(*iter1_1, SGE_STR('这'));

	auto iter1_2 = s1_3.Insert(s1_3.GetConstReverseEnd(), s1_2);
	ASSERT_EQ(s1_3.GetSize(), 10);
	ASSERT_EQ(s1_3, SGE_STR("试测个是这12345"));
	ASSERT_EQ(*iter1_2, SGE_STR('这'));

	auto iter1_3 = s1_4.Insert(s1_4.GetConstReverseEnd() - 1, s1_2);
	ASSERT_EQ(s1_4.GetSize(), 10);
	ASSERT_EQ(s1_4, SGE_STR("1试测个是这2345"));
	ASSERT_EQ(*iter1_3, SGE_STR('这'));

	auto iter1_4 = s1_1.Insert(s1_1.GetReverseBegin() + 1, s1_5);
	ASSERT_EQ(s1_1.GetSize(), 18);
	ASSERT_EQ(s1_1, SGE_STR("12345试测个是тсет отЭ这"));
	ASSERT_EQ(*iter1_4, SGE_STR('Э'));

	auto iter1_5 = s1_1.Insert(s1_1.GetConstReverseEnd() - 1, SGE_STR("δοκιμή"));
	ASSERT_EQ(s1_1.GetSize(), 24);
	ASSERT_EQ(s1_1, SGE_STR("1ήμικοδ2345试测个是тсет отЭ这"));
	ASSERT_EQ(*iter1_5, SGE_STR('δ'));

	auto iter1_6 = s1_1.Insert(s1_1.GetConstReverseBegin() + 2, v1.GetConstReverseBegin(), v1.GetConstReverseEnd());
	ASSERT_EQ(s1_1.GetSize(), 26);
	ASSERT_EQ(s1_1, SGE_STR("1ήμικοδ2345试测个是тсет от字符Э这"));
	ASSERT_EQ(*iter1_6, SGE_STR('符'));

	UTF8String s2_1(SGE_U8STR("12345"));
	ASSERT_EQ(s2_1.GetSize(), 5);
	const UTF8String s2_2(SGE_U8STR("这是个测试"));
	ASSERT_EQ(s2_2.GetSize(), 5);
	UTF8String s2_3(s2_1);
	ASSERT_EQ(s2_3.GetSize(), 5);
	UTF8String s2_4(s2_1);
	ASSERT_EQ(s2_4.GetSize(), 5);
	StringCore<char, UTF8Trait, StdAllocator> s2_5(SGE_U8STR("Это тест"));
	ASSERT_EQ(s2_5.GetSize(), 8);
	UTF8String s2_6(SGE_U8STR("字符"));
	ASSERT_EQ(s2_6.GetSize(), 2);

	auto iter2_1 = s2_1.Insert(s2_1.GetConstReverseBegin(), s2_2);
	ASSERT_EQ(s2_1.GetSize(), 10);
	ASSERT_EQ(s2_1, SGE_U8STR("12345试测个是这"));
	ASSERT_TRUE(IsUTF8CharSame(*iter2_1, SGE_U8STR("这")));

	auto iter2_2 = s2_3.Insert(s2_3.GetConstReverseEnd(), s2_2);
	ASSERT_EQ(s2_3.GetSize(), 10);
	ASSERT_EQ(s2_3, SGE_U8STR("试测个是这12345"));
	ASSERT_TRUE(IsUTF8CharSame(*iter2_2, SGE_U8STR("这")));

	auto iter2_3 = s2_4.Insert(s2_4.GetConstReverseEnd() - 1, s2_2);
	ASSERT_EQ(s2_4.GetSize(), 10);
	ASSERT_EQ(s2_4, SGE_U8STR("1试测个是这2345"));
	ASSERT_TRUE(IsUTF8CharSame(*iter2_3, SGE_U8STR("这")));

	auto iter2_4 = s2_1.Insert(s2_1.GetReverseBegin() + 1, s2_5);
	ASSERT_EQ(s2_1.GetSize(), 18);
	ASSERT_EQ(s2_1, SGE_U8STR("12345试测个是тсет отЭ这"));
	ASSERT_TRUE(IsUTF8CharSame(*iter2_4, SGE_U8STR("Э")));

	auto iter2_5 = s2_1.Insert(s2_1.GetConstReverseEnd() - 1, SGE_U8STR("δοκιμή"));
	ASSERT_EQ(s2_1.GetSize(), 24);
	ASSERT_EQ(s2_1, SGE_U8STR("1ήμικοδ2345试测个是тсет отЭ这"));
	ASSERT_TRUE(IsUTF8CharSame(*iter2_5, SGE_U8STR("δ")));

	auto iter2_6 = s2_1.Insert(s2_1.GetConstReverseBegin() + 2, s2_6.GetConstReverseBegin(), s2_6.GetConstReverseEnd());
	ASSERT_EQ(s2_1.GetSize(), 26);
	ASSERT_EQ(s2_1, SGE_U8STR("1ήμικοδ2345试测个是тсет от字符Э这"));
	ASSERT_TRUE(IsUTF8CharSame(*iter2_6, SGE_U8STR("符")));
}

TEST(StringCore, RemoveTest)
{
	UCS2String s1_1(SGE_STR("这是测试"));
	ASSERT_EQ(s1_1.GetSize(), 4);

	auto iter1_1 = s1_1.Remove(s1_1.GetConstBegin() + 1);
	ASSERT_EQ(s1_1.GetSize(), 3);
	ASSERT_EQ(s1_1, SGE_STR("这测试"));
	ASSERT_EQ(*iter1_1, SGE_STR('测'));

	auto iter1_2 = s1_1.Remove(s1_1.GetReverseEnd() - 1);
	ASSERT_EQ(s1_1.GetSize(), 2);
	ASSERT_EQ(s1_1, SGE_STR("测试"));
	ASSERT_EQ(*iter1_2, SGE_STR('\0'));

	UTF8String s2_1(SGE_U8STR("这是测试"));
	ASSERT_EQ(s2_1.GetSize(), 4);

	auto iter2_1 = s2_1.Remove(s2_1.GetConstBegin() + 1);
	ASSERT_EQ(s2_1.GetSize(), 3);
	ASSERT_EQ(s2_1, SGE_U8STR("这测试"));
	ASSERT_TRUE(IsUTF8CharSame(*iter2_1, SGE_U8STR("测")));

	auto iter2_2 = s2_1.Remove(s2_1.GetReverseEnd() - 1);
	ASSERT_EQ(s2_1.GetSize(), 2);
	ASSERT_EQ(s2_1, SGE_U8STR("测试"));
	ASSERT_TRUE(IsUTF8CharSame(*iter2_2, SGE_U8STR("\0")));
}

TEST(StringCore, RemoveRangeTest)
{
	UCS2String s1_1(SGE_STR("这是一个测试"));
	ASSERT_EQ(s1_1.GetSize(), 6);

	auto iter1_1 = s1_1.Remove(s1_1.GetConstBegin(), s1_1.GetConstBegin() + 2);
	ASSERT_EQ(s1_1.GetSize(), 4);
	ASSERT_EQ(s1_1, SGE_STR("一个测试"));
	ASSERT_EQ(*iter1_1, SGE_STR('一'));

	auto iter1_2 = s1_1.Remove(s1_1.GetReverseBegin(), s1_1.GetReverseEnd() - 1);
	ASSERT_EQ(s1_1.GetSize(), 1);
	ASSERT_EQ(s1_1, SGE_STR("一"));
	ASSERT_EQ(*iter1_2, SGE_STR('一'));

	UTF8String s2_1(SGE_U8STR("这是一个测试"));
	ASSERT_EQ(s2_1.GetSize(), 6);

	auto iter2_1 = s2_1.Remove(s2_1.GetConstBegin(), s2_1.GetConstBegin() + 2);
	ASSERT_EQ(s2_1.GetSize(), 4);
	ASSERT_EQ(s2_1, SGE_U8STR("一个测试"));
	ASSERT_TRUE(IsUTF8CharSame(*iter2_1, SGE_U8STR("一")));

	auto iter2_2 = s2_1.Remove(s2_1.GetReverseBegin(), s2_1.GetReverseEnd() - 1);
	ASSERT_EQ(s2_1.GetSize(), 1);
	ASSERT_EQ(s2_1, SGE_U8STR("一"));
	ASSERT_TRUE(IsUTF8CharSame(*iter2_2, SGE_U8STR("一")));
}

TEST(StringCore, OperatorAtTest)
{
	UCS2String s1_1(SGE_STR("这是测试"));
	const UCS2String s1_2(SGE_STR("这是测试"));
	ASSERT_EQ(s1_1.GetSize(), 4);
	ASSERT_EQ(s1_2.GetSize(), 4);
	for (SizeType i = 0; i < 4; i++)
		ASSERT_EQ(s1_1[i], s1_2[i]);

	s1_1[1] = SGE_STR('i');

	ASSERT_EQ(s1_1.GetSize(), 4);
	ASSERT_EQ(s1_1, SGE_STR("这i测试"));

	UTF8String s2_1(SGE_U8STR("这是测试"));
	const UTF8String s2_2(SGE_U8STR("这是测试"));
	ASSERT_EQ(s2_1.GetSize(), 4);
	ASSERT_EQ(s2_2.GetSize(), 4);
	for (SizeType i = 0; i < 4; i++)
		ASSERT_TRUE(IsUTF8CharSame(s2_1[i], s2_2[i]));

	memcpy(s2_1[1], SGE_U8STR("亦"), sizeof(char) * 3);

	ASSERT_EQ(s2_1.GetSize(), 4);
	ASSERT_EQ(s2_1, SGE_U8STR("这亦测试"));
}

TEST(StringCoreIterator, GetBeginTest)
{
	UCS2String s1(SGE_STR("这是测试"));
	auto iter1 = s1.GetBegin();
	ASSERT_EQ(*iter1, SGE_STR('这'));

	UTF8String s2(u8"这是测试");
	auto iter2 = s2.GetBegin();
	ASSERT_EQ(memcmp(*iter2, u8"这", sizeof(char) * StringImplement::GetMultipleByteCharSize<char, UTF8Trait>(u8"这")), 0);
}

TEST(StringCoreIterator, GetEndTest)
{
	UCS2String s1(SGE_STR("这是测试"));
	auto iter1 = s1.GetEnd();
	ASSERT_EQ(*iter1, SGE_STR('\0'));

	UTF8String s2(u8"这是测试");
	auto iter2 = s2.GetEnd();
	ASSERT_EQ(memcmp(*iter2, u8"\0", sizeof(char) * StringImplement::GetMultipleByteCharSize<char, UTF8Trait>(u8"\0")), 0);
}

TEST(StringCoreIterator, GetConstBeginTest)
{
	const UCS2String s1(SGE_STR("这是测试"));
	auto iter1 = s1.GetConstBegin();
	ASSERT_EQ(*iter1, SGE_STR('这'));

	const UTF8String s2(u8"这是测试");
	auto iter2 = s2.GetConstBegin();
	ASSERT_EQ(memcmp(*iter2, u8"这", sizeof(char) * StringImplement::GetMultipleByteCharSize<char, UTF8Trait>(u8"这")), 0);
}

TEST(StringCoreIterator, GetConstEndTest)
{
	const UCS2String s1(SGE_STR("这是测试"));
	auto iter1 = s1.GetConstEnd();
	ASSERT_EQ(*iter1, SGE_STR('\0'));

	const UTF8String s2(u8"这是测试");
	auto iter2 = s2.GetConstEnd();
	ASSERT_EQ(memcmp(*iter2, u8"\0", sizeof(char) * StringImplement::GetMultipleByteCharSize<char, UTF8Trait>(u8"\0")), 0);
}

TEST(StringCoreIterator, GetReverseBeginTest)
{
	UCS2String s1(SGE_STR("这是测试"));
	auto iter1 = s1.GetReverseBegin();
	ASSERT_EQ(*iter1, SGE_STR('试'));

	UTF8String s2(u8"这是测试");
	auto iter2 = s2.GetReverseBegin();
	ASSERT_TRUE(IsUTF8CharSame(*iter2, u8"试"));
}

TEST(StringCoreIterator, GetReverseEndTest)
{
	UCS2String s1(SGE_STR("这是测试"));
	auto iter1 = s1.GetReverseEnd();
	ASSERT_EQ(*(iter1 - 1), SGE_STR('这'));

	UTF8String s2(u8"这是测试");
	auto iter2 = s2.GetReverseEnd();
	ASSERT_TRUE(IsUTF8CharSame(*(iter2 - 1), u8"这"));
}

TEST(StringCoreIterator, GetConstReverseBeginTest)
{
	const UCS2String s1(SGE_STR("这是测试"));
	auto iter1 = s1.GetConstReverseBegin();
	ASSERT_EQ(*iter1, SGE_STR('试'));

	const UTF8String s2(u8"这是测试");
	auto iter2 = s2.GetConstReverseBegin();
	ASSERT_TRUE(IsUTF8CharSame(*iter2, u8"试"));
}

TEST(StringCoreIterator, GetConstReverseEndTest)
{
	const UCS2String s1(SGE_STR("这是测试"));
	auto iter1 = s1.GetConstReverseEnd();
	ASSERT_EQ(*(iter1 - 1), SGE_STR('这'));

	const UTF8String s2(u8"这是测试");
	auto iter2 = s2.GetConstReverseEnd();
	ASSERT_TRUE(IsUTF8CharSame(*(iter2 - 1), u8"这"));
}

TEST(StringCoreIterator, CopyTest)
{
	UCS2String s1(SGE_STR("这是测试"));
	auto iter1 = s1.GetBegin();
	ASSERT_EQ(*iter1, SGE_STR('这'));
	auto iter2 = iter1;
	ASSERT_EQ(*iter1, SGE_STR('这'));
	ASSERT_EQ(*iter2, SGE_STR('这'));
	iter2 = s1.GetEnd();
	ASSERT_EQ(*iter2, SGE_STR('\0'));

	UTF8String s2(u8"这是测试");
	auto iter3 = s2.GetBegin();
	ASSERT_TRUE(IsUTF8CharSame(*iter3, u8"这"));
	auto iter4 = iter3;
	ASSERT_TRUE(IsUTF8CharSame(*iter3, u8"这"));
	ASSERT_TRUE(IsUTF8CharSame(*iter4, u8"这"));
	iter4 = s2.GetEnd();
	ASSERT_TRUE(IsUTF8CharSame(*iter4, u8"\0"));

	UCS2String s3(SGE_STR("这是测试"));
	auto iter5 = s3.GetReverseBegin();
	ASSERT_EQ(*iter5, SGE_STR('试'));
	auto iter6 = iter5;
	ASSERT_EQ(*iter5, SGE_STR('试'));
	ASSERT_EQ(*iter6, SGE_STR('试'));
	iter6 = s3.GetReverseEnd() - 1;
	ASSERT_EQ(*iter6, SGE_STR('这'));

	const UTF8String s4(u8"这是测试");
	auto iter7 = s4.GetConstReverseBegin();
	ASSERT_TRUE(IsUTF8CharSame(*iter7, u8"试"));
	auto iter8 = iter7;
	ASSERT_TRUE(IsUTF8CharSame(*iter7, u8"试"));
	ASSERT_TRUE(IsUTF8CharSame(*iter8, u8"试"));
	iter8 = s4.GetConstReverseEnd() - 1;
	ASSERT_TRUE(IsUTF8CharSame(*iter8, u8"这"));
}

TEST(StringCoreIterator, CalculationOperatorTest)
{
	UCS2String s1(SGE_STR("这是测试"));
	auto iter1 = s1.GetBegin();
	ASSERT_EQ(*iter1, SGE_STR('这'));
	iter1++;
	ASSERT_EQ(*iter1, SGE_STR('是'));
	++iter1;
	ASSERT_EQ(*iter1, SGE_STR('测'));
	iter1 += 1;
	ASSERT_EQ(*iter1, SGE_STR('试'));
	iter1 = iter1 + 1;
	ASSERT_EQ(*iter1, SGE_STR('\0'));
	iter1--;
	ASSERT_EQ(*iter1, SGE_STR('试'));
	--iter1;
	ASSERT_EQ(*iter1, SGE_STR('测'));
	iter1 -= 1;
	ASSERT_EQ(*iter1, SGE_STR('是'));
	iter1 = iter1 - 1;
	ASSERT_EQ(*iter1, SGE_STR('这'));

	UTF8String s2(u8"这是测试");
	auto iter2 = s2.GetBegin();
	ASSERT_TRUE(IsUTF8CharSame(*iter2, u8"这"));
	iter2++;
	ASSERT_TRUE(IsUTF8CharSame(*iter2, u8"是"));
	++iter2;
	ASSERT_TRUE(IsUTF8CharSame(*iter2, u8"测"));
	iter2 += 1;
	ASSERT_TRUE(IsUTF8CharSame(*iter2, u8"试"));
	iter2 = iter2 + 1;
	ASSERT_TRUE(IsUTF8CharSame(*iter2, u8"\0"));
	iter2--;
	ASSERT_TRUE(IsUTF8CharSame(*iter2, u8"试"));
	--iter2;
	ASSERT_TRUE(IsUTF8CharSame(*iter2, u8"测"));
	iter2 -= 1;
	ASSERT_TRUE(IsUTF8CharSame(*iter2, u8"是"));
	iter2 = iter2 - 1;
	ASSERT_TRUE(IsUTF8CharSame(*iter2, u8"这"));

	const UCS2String s3(SGE_STR("这是测试"));
	auto iter3 = s3.GetConstReverseBegin() - 1;
	ASSERT_EQ(*iter3, SGE_STR('\0'));
	iter3++;
	ASSERT_EQ(*iter3, SGE_STR('试'));
	++iter3;
	ASSERT_EQ(*iter3, SGE_STR('测'));
	iter3 += 1;
	ASSERT_EQ(*iter3, SGE_STR('是'));
	iter3 = iter3 + 1;
	ASSERT_EQ(*iter3, SGE_STR('这'));
	iter3--;
	ASSERT_EQ(*iter3, SGE_STR('是'));
	--iter3;
	ASSERT_EQ(*iter3, SGE_STR('测'));
	iter3 -= 1;
	ASSERT_EQ(*iter3, SGE_STR('试'));
	iter3 = iter3 - 1;
	ASSERT_EQ(*iter3, SGE_STR('\0'));

	UTF8String s4(u8"这是测试");
	auto iter4 = s4.GetReverseBegin() - 1;
	ASSERT_TRUE(IsUTF8CharSame(*iter4, u8"\0"));
	iter4++;
	ASSERT_TRUE(IsUTF8CharSame(*iter4, u8"试"));
	++iter4;
	ASSERT_TRUE(IsUTF8CharSame(*iter4, u8"测"));
	iter4 += 1;
	ASSERT_TRUE(IsUTF8CharSame(*iter4, u8"是"));
	iter4 = iter4 + 1;
	ASSERT_TRUE(IsUTF8CharSame(*iter4, u8"这"));
	iter4--;
	ASSERT_TRUE(IsUTF8CharSame(*iter4, u8"是"));
	--iter4;
	ASSERT_TRUE(IsUTF8CharSame(*iter4, u8"测"));
	iter4 -= 1;
	ASSERT_TRUE(IsUTF8CharSame(*iter4, u8"试"));
	iter4 = iter4 - 1;
	ASSERT_TRUE(IsUTF8CharSame(*iter4, u8"\0"));
}

TEST(StringCoreIterator, DistanceTest)
{
	UCS2String s1(SGE_STR("这是测试"));
	ASSERT_EQ(s1.GetEnd() - s1.GetBegin(), 4);
	ASSERT_EQ(s1.GetReverseEnd() - s1.GetReverseBegin(), 4);

	UTF8String s2(u8"这是测试");
	ASSERT_EQ(s2.GetConstEnd() - s2.GetConstBegin(), 4);
	ASSERT_EQ(s2.GetConstReverseEnd() - s2.GetConstReverseBegin(), 4);
}

TEST(StringCoreIterator, OutOfRangeTest)
{
	UCS2String s1(SGE_STR("这是测试"));
	ASSERT_FALSE(UCS2String::Iterator::OutOfRangeError::Judge(s1.GetBegin(), s1.GetData(), s1.GetData() + s1.GetNormalSize()));
	ASSERT_FALSE(UCS2String::Iterator::OutOfRangeError::Judge(s1.GetEnd(), s1.GetData(), s1.GetData() + s1.GetNormalSize()));
	ASSERT_TRUE(UCS2String::Iterator::OutOfRangeError::Judge(s1.GetEnd() + 1, s1.GetData(), s1.GetData() + s1.GetNormalSize()));
	ASSERT_TRUE(UCS2String::Iterator::OutOfRangeError::Judge(s1.GetBegin() - 1, s1.GetData(), s1.GetData() + s1.GetNormalSize()));

	ASSERT_FALSE(UCS2String::ReverseIterator::OutOfRangeError::Judge(s1.GetReverseBegin(), s1.GetData() - 1, s1.GetData() + s1.GetNormalSize() - 1));
	ASSERT_FALSE(UCS2String::ReverseIterator::OutOfRangeError::Judge(s1.GetReverseEnd(), s1.GetData() - 1, s1.GetData() + s1.GetNormalSize() - 1));
	ASSERT_TRUE(UCS2String::ReverseIterator::OutOfRangeError::Judge(s1.GetReverseEnd() + 1, s1.GetData() - 1, s1.GetData() + s1.GetNormalSize() - 1));
	ASSERT_TRUE(UCS2String::ReverseIterator::OutOfRangeError::Judge(s1.GetReverseBegin() - 1, s1.GetData() - 1, s1.GetData() + s1.GetNormalSize() - 1));

	UTF8String s2(u8"这是测试");
	ASSERT_FALSE(UTF8String::Iterator::OutOfRangeError::Judge(s2.GetBegin(), s2.GetData(), s2.GetData() + s2.GetNormalSize()));
	ASSERT_FALSE(UTF8String::Iterator::OutOfRangeError::Judge(s2.GetEnd(), s2.GetData(), s2.GetData() + s2.GetNormalSize()));
	ASSERT_TRUE(UTF8String::Iterator::OutOfRangeError::Judge(s2.GetBegin() - 1, s2.GetData(), s2.GetData() + s2.GetNormalSize()));
	ASSERT_TRUE(UTF8String::Iterator::OutOfRangeError::Judge(s2.GetEnd() + 1, s2.GetData(), s2.GetData() + s2.GetNormalSize()));

	ASSERT_FALSE(UTF8String::ConstReverseIterator::OutOfRangeError::Judge(s2.GetConstReverseBegin(), StringImplement::GetPreviousMultipleByteChar<char, UTF8Trait>(s2.GetData()), StringImplement::GetPreviousMultipleByteChar<char, UTF8Trait>(s2.GetData() + s2.GetNormalSize())));
	ASSERT_FALSE(UTF8String::ConstReverseIterator::OutOfRangeError::Judge(s2.GetConstReverseEnd(), StringImplement::GetPreviousMultipleByteChar<char, UTF8Trait>(s2.GetData()), StringImplement::GetPreviousMultipleByteChar<char, UTF8Trait>(s2.GetData() + s2.GetNormalSize())));
	ASSERT_TRUE(UTF8String::ConstReverseIterator::OutOfRangeError::Judge(s2.GetConstReverseEnd() + 1, StringImplement::GetPreviousMultipleByteChar<char, UTF8Trait>(s2.GetData()), StringImplement::GetPreviousMultipleByteChar<char, UTF8Trait>(s2.GetData() + s2.GetNormalSize())));
	ASSERT_TRUE(UTF8String::ConstReverseIterator::OutOfRangeError::Judge(s2.GetConstReverseBegin() - 1, StringImplement::GetPreviousMultipleByteChar<char, UTF8Trait>(s2.GetData()), StringImplement::GetPreviousMultipleByteChar<char, UTF8Trait>(s2.GetData() + s2.GetNormalSize())));
}

TEST(StringCoreIterator, IsStringCoreIteratorTest)
{
	ASSERT_TRUE((UCS2String::IsStringCoreIterator<UCS2String::Iterator>::Value));
	ASSERT_TRUE((UCS2String::IsStringCoreIterator<UCS2String::ConstIterator>::Value));
	ASSERT_TRUE((UCS2String::IsStringCoreIterator<UCS2String::ReverseIterator>::Value));
	ASSERT_TRUE((UCS2String::IsStringCoreIterator<UCS2String::ConstReverseIterator>::Value));

	ASSERT_FALSE((UCS2String::IsStringCoreIterator<int>::Value));
	ASSERT_FALSE((UCS2String::IsStringCoreIterator<UTF8String::ReverseIterator>::Value));

	ASSERT_TRUE((UTF8String::IsStringCoreIterator<UTF8String::Iterator>::Value));
	ASSERT_TRUE((UTF8String::IsStringCoreIterator<UTF8String::ConstIterator>::Value));
	ASSERT_TRUE((UTF8String::IsStringCoreIterator<UTF8String::ReverseIterator>::Value));
	ASSERT_TRUE((UTF8String::IsStringCoreIterator<UTF8String::ConstReverseIterator>::Value));

	ASSERT_FALSE((UTF8String::IsStringCoreIterator<int>::Value));
	ASSERT_FALSE((UTF8String::IsStringCoreIterator<UCS2String::ReverseIterator>::Value));
}