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
#include "Container/HashMap.hpp"

TEST(Hash, HashTest)
{
	HashType std_hash_re = std::hash<int>()(12345);
	HashType sge_hash_re = SpaceGameEngine::Hash<int>::GetHash(12345);
	ASSERT_EQ(std_hash_re, sge_hash_re);
}

TEST(HashMap, InstanceTest)
{
	HashMap<int, int> hm1;
}

TEST(HashMapIterator, GetBeginTest)
{
	HashMap<int, int> hm1;
	auto iter1 = hm1.GetBegin();
	ASSERT_TRUE((std::is_same_v<decltype(iter1), HashMap<int, int>::Iterator>));
	//todo : change a test instance
}

TEST(HashMapIterator, GetEndTest)
{
	HashMap<int, int> hm1;
	auto iter1 = hm1.GetEnd();
	ASSERT_TRUE((std::is_same_v<decltype(iter1), HashMap<int, int>::Iterator>));
	//todo : change a test instance
	ASSERT_EQ(hm1.GetBegin(), iter1);
}

TEST(HashMapIterator, GetConstBeginTest)
{
	HashMap<int, int> hm1;
	auto iter1 = hm1.GetConstBegin();
	ASSERT_TRUE((std::is_same_v<decltype(iter1), HashMap<int, int>::ConstIterator>));
	//todo : change a test instance
}

TEST(HashMapIterator, GetConstEndTest)
{
	HashMap<int, int> hm1;
	auto iter1 = hm1.GetConstEnd();
	ASSERT_TRUE((std::is_same_v<decltype(iter1), HashMap<int, int>::ConstIterator>));
	//todo : change a test instance
	ASSERT_EQ(hm1.GetConstBegin(), iter1);
}