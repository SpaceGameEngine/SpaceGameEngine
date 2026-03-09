/*
Copyright 2026 creatorlxd

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
#include "Utility/Hash.hpp"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;

TEST(Hash, HashTest)
{
	HashType std_hash_re = std::hash<int>()(12345);
	HashType sge_hash_re = SpaceGameEngine::Hash<int>::GetHash(12345);
	ASSERT_EQ(std_hash_re, sge_hash_re);
}

TEST(Hash, StringHashTest)
{
	ASSERT_EQ(Hash<UCS2String>::GetHash(UCS2String(SGE_STR(""))), 0);
	ASSERT_EQ(Hash<UTF8String>::GetHash(UTF8String(SGE_U8STR(""))), 0);

	ASSERT_TRUE(Hash<UCS2String>::GetHash(UCS2String(SGE_STR("this is a test"))) != 0);
	ASSERT_TRUE(Hash<UTF8String>::GetHash(UTF8String(SGE_U8STR("this is a test"))) != 0);

	ASSERT_TRUE(Hash<UCS2String>::GetHash(UCS2String(SGE_STR("这是测试"))) != 0);
	ASSERT_TRUE(Hash<UTF8String>::GetHash(UTF8String(SGE_U8STR("这是测试"))) != 0);

	ASSERT_EQ(Hash<UCS2String>::GetHash(UCS2String(SGE_STR("a"))), Hash<UCS2String>::GetHash(UCS2String(SGE_STR("a"))));
	ASSERT_TRUE(Hash<UCS2String>::GetHash(UCS2String(SGE_STR("a"))) != Hash<UCS2String>::GetHash(UCS2String(SGE_STR("b"))));

	ASSERT_EQ(Hash<UTF8String>::GetHash(UTF8String(SGE_U8STR("a"))), Hash<UTF8String>::GetHash(UTF8String(SGE_U8STR("a"))));
	ASSERT_TRUE(Hash<UTF8String>::GetHash(UTF8String(SGE_U8STR("a"))) != Hash<UTF8String>::GetHash(UTF8String(SGE_U8STR("b"))));
}