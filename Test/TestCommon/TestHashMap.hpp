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

using namespace SpaceGameEngine;

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

TEST(HashMap, GetLoadFactorTest)
{
	HashMap<int, int> hm1;
	ASSERT_EQ(hm1.GetLoadFactor(), hm1.sm_DefaultLoadFactor);
}

TEST(HashMap, GetCorrectBucketQuantityTest)
{
	using hm = HashMap<int, int>;
	ASSERT_EQ(hm::GetCorrectBucketQuantity(1.0, 0), 1);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(1.0, 1), 1);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(1.0, 2), 2);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(1.0, 3), 4);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(1.0, 4), 4);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(1.0, 5), 8);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(1.0, 6), 8);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(1.0, 7), 8);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(1.0, 8), 8);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(1.0, 9), 16);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(1.0, 10), 16);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(1.0, 11), 16);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(1.0, 12), 16);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(1.0, 13), 16);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(1.0, 14), 16);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(1.0, 15), 16);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(1.0, 16), 16);

	ASSERT_EQ(hm::GetCorrectBucketQuantity(0.5, 0), 1);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(0.5, 1), 2);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(0.5, 2), 4);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(0.5, 3), 8);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(0.5, 4), 8);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(0.5, 5), 16);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(0.5, 6), 16);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(0.5, 7), 16);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(0.5, 8), 16);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(0.5, 9), 32);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(0.5, 10), 32);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(0.5, 11), 32);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(0.5, 12), 32);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(0.5, 13), 32);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(0.5, 14), 32);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(0.5, 15), 32);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(0.5, 16), 32);

	ASSERT_EQ(hm::GetCorrectBucketQuantity(2.0, 0), 1);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(2.0, 1), 1);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(2.0, 2), 1);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(2.0, 3), 2);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(2.0, 4), 2);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(2.0, 5), 4);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(2.0, 6), 4);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(2.0, 7), 4);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(2.0, 8), 4);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(2.0, 9), 8);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(2.0, 10), 8);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(2.0, 11), 8);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(2.0, 12), 8);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(2.0, 13), 8);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(2.0, 14), 8);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(2.0, 15), 8);
	ASSERT_EQ(hm::GetCorrectBucketQuantity(2.0, 16), 8);
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