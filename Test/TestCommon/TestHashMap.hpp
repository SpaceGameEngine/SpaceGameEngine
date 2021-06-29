﻿/*
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

struct test_hashmap_object
{
	std::function<void(test_hashmap_object&)> rel_func;
	test_hashmap_object()
		: val(0), rel_func([](test_hashmap_object&) {})
	{
	}
	test_hashmap_object(int v)
		: val(v), rel_func([](test_hashmap_object&) {})
	{
	}
	test_hashmap_object(int v, const std::function<void(test_hashmap_object&)>& func)
		: val(v), rel_func(func)
	{
	}
	test_hashmap_object(const test_hashmap_object& o) noexcept
		: val(o.val), rel_func(o.rel_func)
	{
	}
	test_hashmap_object(test_hashmap_object&& o) noexcept
		: val(o.val), rel_func(std::move(o.rel_func))
	{
		o.rel_func = [](test_hashmap_object&) {};
	}
	~test_hashmap_object()
	{
		rel_func(*this);
	}
	int val;

	test_hashmap_object& operator=(const test_hashmap_object& o)
	{
		val = o.val;
		rel_func = o.rel_func;
		return *this;
	}

	test_hashmap_object& operator=(test_hashmap_object&& o)
	{
		val = o.val;
		rel_func = std::move(o.rel_func);
		o.rel_func = [](test_hashmap_object&) {};
		return *this;
	}

	bool operator<(const test_hashmap_object& o) const
	{
		return val < o.val;
	}

	bool operator==(const test_hashmap_object& o) const
	{
		return val == o.val;
	}

	bool operator!=(const test_hashmap_object& o) const
	{
		return val != o.val;
	}
};

template<>
struct SpaceGameEngine::Hash<test_hashmap_object>
{
	inline static HashType GetHash(const test_hashmap_object& t)
	{
		return std::hash<int>()(t.val);
	}
};

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

TEST(HashMap, GetSizeTest)
{
	//todo : change a test instance
	HashMap<int, int> hm1;
	ASSERT_EQ(hm1.GetSize(), 0);

	const HashMap<int, int> chm1;
	ASSERT_EQ(chm1.GetSize(), 0);
}

TEST(HashMap, InsertTest)
{
	HashMap<test_hashmap_object, test_hashmap_object>* phm = new HashMap<test_hashmap_object, test_hashmap_object>();
	const int test_size = 1000;
	int key_pool[test_size];
	int val_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	auto key_rel_func = [&](test_hashmap_object& o) {
		key_pool[o.val] += 1;
	};
	auto val_rel_func = [&](test_hashmap_object& o) {
		val_pool[o.val] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = phm->Insert(test_hashmap_object(i, key_rel_func), test_hashmap_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->m_First.val, i);
		ASSERT_EQ(iter.m_First->m_Second.val, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(phm->GetSize(), test_size);
	delete phm;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 1);
		ASSERT_EQ(val_pool[i], 1);
	}
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