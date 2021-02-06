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
	ASSERT_EQ(wcscmp(pstr1, pcstr), 0);
	ASSERT_EQ(StringImplement<Char>::StorageRef::GetCount(pstr1), 1);
	StringImplement<Char>::StorageRef::CountIncrease(pstr1);
	ASSERT_EQ(StringImplement<Char>::StorageRef::TryRelease(pstr1, 5), false);
	ASSERT_EQ(wcscmp(pstr1, pcstr), 0);
	ASSERT_EQ(StringImplement<Char>::StorageRef::GetCount(pstr1), 2);
	StringImplement<Char>::StorageRef::CountDecrease(pstr1);
	ASSERT_EQ(wcscmp(pstr1, pcstr), 0);
	ASSERT_EQ(StringImplement<Char>::StorageRef::GetCount(pstr1), 1);
	ASSERT_EQ(StringImplement<Char>::StorageRef::TryRelease(pstr1, 5), true);
}