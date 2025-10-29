/*
Copyright 2025 creatorlxd

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
#include "Container/HashSet.hpp"
#include <functional>
#include <cstring>
#include <vector>

using namespace SpaceGameEngine;

struct test_hashset_object
{
	std::function<void(test_hashset_object&)> rel_func;
	test_hashset_object()
		: key(0), rel_func([](test_hashset_object&) {})
	{
	}
	test_hashset_object(int k, int v = 0)
		: key(k), value(v), rel_func([](test_hashset_object&) {})
	{
	}
	test_hashset_object(int k, const std::function<void(test_hashset_object&)>& func)
		: key(k), value(0), rel_func(func)
	{
	}
	test_hashset_object(int k, int v, const std::function<void(test_hashset_object&)>& func)
		: key(k), value(v), rel_func(func)
	{
	}
	test_hashset_object(const test_hashset_object& o) noexcept
		: key(o.key), value(o.value), rel_func(o.rel_func)
	{
	}
	test_hashset_object(test_hashset_object&& o) noexcept
		: key(o.key), value(o.value), rel_func(std::move(o.rel_func))
	{
		o.rel_func = [](test_hashset_object&) {};
	}
	~test_hashset_object()
	{
		rel_func(*this);
	}
	int key;
	int value;

	test_hashset_object& operator=(const test_hashset_object& o)
	{
		key = o.key;
		value = o.value;
		rel_func = o.rel_func;
		return *this;
	}

	test_hashset_object& operator=(test_hashset_object&& o)
	{
		key = o.key;
		value = o.value;
		rel_func = std::move(o.rel_func);
		o.rel_func = [](test_hashset_object&) {};
		return *this;
	}

	bool operator<(const test_hashset_object& o) const
	{
		return key < o.key;
	}

	// only compare key here
	bool operator==(const test_hashset_object& o) const
	{
		return key == o.key && value == o.value;
	}

	// only compare key here
	bool operator!=(const test_hashset_object& o) const
	{
		return key != o.key || value != o.value;
	}
};

template<>
struct SpaceGameEngine::Equal<test_hashset_object>
{
	inline static constexpr bool Compare(const test_hashset_object& lhs, const test_hashset_object& rhs)
	{
		return lhs.key == rhs.key;
	}
};

template<>
struct SpaceGameEngine::Hash<test_hashset_object>
{
	inline static HashType GetHash(const test_hashset_object& t)
	{
		return std::hash<int>()(t.key);
	}
};

TEST(HashSet, InstanceTest)
{
	HashSet<int> hs1;
	ASSERT_EQ(hs1.GetSize(), 0);
	ASSERT_EQ(hs1.GetBucketQuantity(), hs1.DefaultBucketQuantity);
}

TEST(HashSet, ListConstructionTest)
{
	const int test_size = 10;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashset_object& o) {
		val_pool[o.key] += 1;
	};

	HashSet<test_hashset_object>* phs = new HashSet<test_hashset_object>(
		{test_hashset_object(0, val_rel_func),
		 test_hashset_object(1, val_rel_func),
		 test_hashset_object(2, val_rel_func),
		 test_hashset_object(3, val_rel_func),
		 test_hashset_object(4, val_rel_func),
		 test_hashset_object(5, val_rel_func),
		 test_hashset_object(6, val_rel_func),
		 test_hashset_object(7, val_rel_func),
		 test_hashset_object(8, val_rel_func),
		 test_hashset_object(9, val_rel_func)});

	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs)[test_hashset_object(i)].key, i);
	}
	delete phs;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(HashSet, ClearTest)
{
	HashSet<test_hashset_object>* phs = new HashSet<test_hashset_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashset_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = phs->Insert(test_hashset_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));

	phs->Clear();
	ASSERT_EQ(phs->GetSize(), 0);

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
	delete phs;
}

TEST(HashSet, GetLoadFactorTest)
{
	HashSet<int> hs1;
	ASSERT_EQ(hs1.GetLoadFactor(), hs1.DefaultLoadFactor);
}

TEST(HashSet, SetLoadFactorTest)
{
	HashSet<test_hashset_object>* phs = new HashSet<test_hashset_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashset_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = phs->Insert(test_hashset_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));

	phs->SetLoadFactor(0.75);

	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetLoadFactor(), 0.75);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs)[test_hashset_object(i)].key, i);
	}
	delete phs;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashSet, GetCorrectBucketQuantityTest)
{
	using hs = HashSet<int>;
	ASSERT_EQ(hs::GetCorrectBucketQuantity(1.0, 0), 1);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(1.0, 1), 1);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(1.0, 2), 2);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(1.0, 3), 4);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(1.0, 4), 4);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(1.0, 5), 8);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(1.0, 6), 8);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(1.0, 7), 8);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(1.0, 8), 8);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(1.0, 9), 16);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(1.0, 10), 16);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(1.0, 11), 16);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(1.0, 12), 16);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(1.0, 13), 16);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(1.0, 14), 16);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(1.0, 15), 16);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(1.0, 16), 16);

	ASSERT_EQ(hs::GetCorrectBucketQuantity(0.5, 0), 1);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(0.5, 1), 2);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(0.5, 2), 4);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(0.5, 3), 8);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(0.5, 4), 8);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(0.5, 5), 16);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(0.5, 6), 16);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(0.5, 7), 16);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(0.5, 8), 16);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(0.5, 9), 32);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(0.5, 10), 32);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(0.5, 11), 32);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(0.5, 12), 32);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(0.5, 13), 32);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(0.5, 14), 32);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(0.5, 15), 32);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(0.5, 16), 32);

	ASSERT_EQ(hs::GetCorrectBucketQuantity(2.0, 0), 1);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(2.0, 1), 1);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(2.0, 2), 1);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(2.0, 3), 2);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(2.0, 4), 2);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(2.0, 5), 4);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(2.0, 6), 4);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(2.0, 7), 4);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(2.0, 8), 4);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(2.0, 9), 8);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(2.0, 10), 8);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(2.0, 11), 8);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(2.0, 12), 8);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(2.0, 13), 8);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(2.0, 14), 8);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(2.0, 15), 8);
	ASSERT_EQ(hs::GetCorrectBucketQuantity(2.0, 16), 8);
}

TEST(HashSet, GetSizeTest)
{
	HashSet<int> hs1;
	ASSERT_EQ(hs1.GetSize(), 0);
	hs1.Insert(1);
	ASSERT_EQ(hs1.GetSize(), 1);
	hs1.Insert(1);
	ASSERT_EQ(hs1.GetSize(), 1);
	hs1.Insert(2);
	hs1.Insert(3);
	ASSERT_EQ(hs1.GetSize(), 3);

	const HashSet<int> chs1({1, 2, 3, 4});
	ASSERT_EQ(chs1.GetSize(), 4);
}

TEST(HashSet, GetBucketQuantityTest)
{
	ASSERT_EQ((SpaceGameEngine::HashSet<int>::DefaultLoadFactor), 1.0f);
	ASSERT_EQ((SpaceGameEngine::HashSet<int>::DefaultBucketQuantity), 16);

	HashSet<int> hs1;
	ASSERT_EQ(hs1.GetBucketQuantity(), (SpaceGameEngine::HashSet<int>::DefaultBucketQuantity));
	hs1.Insert(1);
	ASSERT_EQ(hs1.GetBucketQuantity(), (SpaceGameEngine::HashSet<int>::DefaultBucketQuantity));
	hs1.Insert(1);
	ASSERT_EQ(hs1.GetBucketQuantity(), (SpaceGameEngine::HashSet<int>::DefaultBucketQuantity));
	hs1.Insert(2);
	hs1.Insert(3);
	ASSERT_EQ(hs1.GetBucketQuantity(), (SpaceGameEngine::HashSet<int>::DefaultBucketQuantity));

	const HashSet<int> chs1({1, 2, 3, 4});
	ASSERT_EQ(chs1.GetBucketQuantity(), 4);
}

TEST(HashSet, InsertTest)
{
	HashSet<test_hashset_object>* phs = new HashSet<test_hashset_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashset_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = phs->Insert(test_hashset_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs)[test_hashset_object(i)].key, i);
		auto iter = phs->Insert(test_hashset_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_FALSE(iter.m_Second);
	}
	delete phs;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(HashSet, UpsertTest)
{
	HashSet<test_hashset_object>* phs = new HashSet<test_hashset_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashset_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = phs->Upsert(test_hashset_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs)[test_hashset_object(i)].key, i);
		auto iter = phs->Upsert(test_hashset_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_FALSE(iter.m_Second);
	}
	delete phs;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashSet, InsertListTest)
{
	HashSet<test_hashset_object>* phs = new HashSet<test_hashset_object>();
	const int test_size = 10;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashset_object& o) {
		val_pool[o.key] += 1;
	};

	// test repeat insert
	phs->Insert(test_hashset_object(0, -1));
	ASSERT_EQ(phs->GetSize(), 1);
	ASSERT_EQ(phs->GetBegin()->value, -1);

	// have 11 elements, test repeat insert
	phs->Insert({test_hashset_object(0, 0, val_rel_func),
				 test_hashset_object(1, -1),
				 test_hashset_object(1, 1, val_rel_func),
				 test_hashset_object(2, 2, val_rel_func),
				 test_hashset_object(3, 3, val_rel_func),
				 test_hashset_object(4, 4, val_rel_func),
				 test_hashset_object(5, 5, val_rel_func),
				 test_hashset_object(6, 6, val_rel_func),
				 test_hashset_object(7, 7, val_rel_func),
				 test_hashset_object(8, 8, val_rel_func),
				 test_hashset_object(9, 9, val_rel_func)});

	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));
	for (int i = test_size - 1; i >= 2; i--)
	{
		ASSERT_EQ((*phs)[test_hashset_object(i)].key, i);
	}
	ASSERT_EQ((*phs)[test_hashset_object(0)].value, -1);
	ASSERT_EQ((*phs)[test_hashset_object(1)].value, -1);
	delete phs;
	for (int i = 0; i < test_size; i++)
	{
		// initializer_list can only return const variable, so move is useless
		ASSERT_EQ(val_pool[i], i > 1 ? 2 : 1);
	}
}

TEST(HashSet, UpsertListTest)
{
	HashSet<test_hashset_object>* phs = new HashSet<test_hashset_object>();
	const int test_size = 10;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashset_object& o) {
		val_pool[o.key] += 1;
	};

	// test repeat insert
	phs->Upsert(test_hashset_object(0, -1));
	ASSERT_EQ(phs->GetSize(), 1);
	ASSERT_EQ(phs->GetBegin()->value, -1);

	// have 11 elements, test repeat insert
	phs->Upsert({test_hashset_object(0, 0, val_rel_func),
				 test_hashset_object(1, -1),
				 test_hashset_object(1, 1, val_rel_func),
				 test_hashset_object(2, 2, val_rel_func),
				 test_hashset_object(3, 3, val_rel_func),
				 test_hashset_object(4, 4, val_rel_func),
				 test_hashset_object(5, 5, val_rel_func),
				 test_hashset_object(6, 6, val_rel_func),
				 test_hashset_object(7, 7, val_rel_func),
				 test_hashset_object(8, 8, val_rel_func),
				 test_hashset_object(9, 9, val_rel_func)});

	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs)[test_hashset_object(i)].key, i);
	}
	delete phs;
	for (int i = 0; i < test_size; i++)
	{
		// initializer_list can only return const variable, so move is useless
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(HashSet, RemoveTest)
{
	HashSet<test_hashset_object>* phs = new HashSet<test_hashset_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashset_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = phs->Insert(test_hashset_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(phs->GetSize(), test_size);
	SizeType now_bucket_quantity = phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize());
	ASSERT_EQ(phs->GetBucketQuantity(), now_bucket_quantity);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs)[test_hashset_object(i)].key, i);
	}

	int rm_cnt = 0;
	auto iter = phs->GetBegin();
	while (iter != phs->GetEnd())
	{
		iter = phs->Remove(iter);
		rm_cnt += 1;
	}
	ASSERT_EQ(rm_cnt, test_size);
	ASSERT_EQ(phs->GetSize(), 0);
	ASSERT_EQ(phs->GetBucketQuantity(), now_bucket_quantity);
	delete phs;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashSet, RemoveByValueTest)
{
	HashSet<test_hashset_object>* phs = new HashSet<test_hashset_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashset_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = phs->Insert(test_hashset_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(phs->GetSize(), test_size);
	SizeType now_bucket_quantity = phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize());
	ASSERT_EQ(phs->GetBucketQuantity(), now_bucket_quantity);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs)[test_hashset_object(i)].key, i);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_TRUE(phs->RemoveByValue(test_hashset_object(i)));
	}

	ASSERT_EQ(phs->GetSize(), 0);
	ASSERT_EQ(phs->GetBucketQuantity(), now_bucket_quantity);
	delete phs;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashSet, ShrinkToFitTest)
{
	HashSet<test_hashset_object>* phs = new HashSet<test_hashset_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashset_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = phs->Insert(test_hashset_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(phs->GetSize(), test_size);
	SizeType now_bucket_quantity = phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize());
	ASSERT_EQ(phs->GetBucketQuantity(), now_bucket_quantity);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs)[test_hashset_object(i)].key, i);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_TRUE(phs->RemoveByValue(test_hashset_object(i)));
	}

	ASSERT_EQ(phs->GetSize(), 0);
	ASSERT_EQ(phs->GetBucketQuantity(), now_bucket_quantity);

	phs->ShrinkToFit();

	ASSERT_EQ(phs->GetSize(), 0);
	ASSERT_TRUE(phs->GetBucketQuantity() != now_bucket_quantity);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));

	delete phs;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashSet, ContainsTest)
{
	const HashSet<int> hs({1, 2, 3});
	ASSERT_FALSE(hs.Contains(0));
	ASSERT_TRUE(hs.Contains(1));
	ASSERT_TRUE(hs.Contains(2));
	ASSERT_TRUE(hs.Contains(3));
	ASSERT_FALSE(hs.Contains(4));
}

TEST(HashSet, FindTest)
{
	HashSet<test_hashset_object>* phs = new HashSet<test_hashset_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashset_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = phs->Insert(test_hashset_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));

	for (int i = test_size - 1; i >= 0; i--)
	{
		auto iter = phs->Find(test_hashset_object(i));
		ASSERT_EQ(iter->key, i);
	}

	auto niter = phs->Find(test_hashset_object(test_size));
	ASSERT_EQ(niter, phs->GetEnd());

	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));

	const HashSet<test_hashset_object>* pchs = phs;

	for (int i = test_size - 1; i >= 0; i--)
	{
		auto iter = pchs->Find(test_hashset_object(i));
		ASSERT_EQ(iter->key, i);
	}

	auto cniter = pchs->Find(test_hashset_object(test_size));
	ASSERT_EQ(cniter, pchs->GetConstEnd());

	ASSERT_EQ(pchs->GetSize(), test_size);
	ASSERT_EQ(pchs->GetBucketQuantity(), pchs->GetCorrectBucketQuantity(pchs->GetLoadFactor(), pchs->GetSize()));

	delete phs;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashSet, GetTest)
{
	HashSet<test_hashset_object>* phs = new HashSet<test_hashset_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashset_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = phs->Insert(test_hashset_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(phs->Get(test_hashset_object(i)).key, i);
	}

	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));

	const HashSet<test_hashset_object>* pchs = phs;

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pchs->Get(test_hashset_object(i)).key, i);
	}

	ASSERT_EQ(pchs->GetSize(), test_size);
	ASSERT_EQ(pchs->GetBucketQuantity(), pchs->GetCorrectBucketQuantity(pchs->GetLoadFactor(), pchs->GetSize()));

	delete phs;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashSet, OperatorTest)
{
	HashSet<test_hashset_object>* phs = new HashSet<test_hashset_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashset_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		(*phs)[test_hashset_object(i)] = test_hashset_object(i, i, val_rel_func);
		ASSERT_EQ((*phs)[test_hashset_object(i)].key, i);
		ASSERT_EQ((*phs)[test_hashset_object(i)].value, i);
		(*phs)[test_hashset_object(i)].value += 1;
	}
	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs)[test_hashset_object(i)].key, i);
		ASSERT_EQ((*phs)[test_hashset_object(i)].value, i + 1);
		(*phs)[test_hashset_object(i)].value -= 1;
	}
	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));
	delete phs;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashSet, RehashTest)
{
	HashSet<test_hashset_object>* phs = new HashSet<test_hashset_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashset_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = phs->Insert(test_hashset_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));
	phs->Rehash(1);
	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), 1);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs)[test_hashset_object(i)].key, i);
	}
	delete phs;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashSet, CopyConstructionTest)
{
	HashSet<test_hashset_object>* phs = new HashSet<test_hashset_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashset_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = phs->Insert(test_hashset_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs)[test_hashset_object(i)].key, i);
	}

	HashSet<test_hashset_object>* phs2 = new HashSet<test_hashset_object>(*phs);

	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs)[test_hashset_object(i)].key, i);
	}

	ASSERT_EQ(phs2->GetSize(), test_size);
	ASSERT_EQ(phs2->GetBucketQuantity(), phs2->GetCorrectBucketQuantity(phs2->GetLoadFactor(), phs2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs2)[test_hashset_object(i)].key, i);
	}

	delete phs;
	delete phs2;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(HashSet, MoveConstructionTest)
{
	HashSet<test_hashset_object>* phs = new HashSet<test_hashset_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashset_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = phs->Insert(test_hashset_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs)[test_hashset_object(i)].key, i);
	}

	HashSet<test_hashset_object>* phs2 = new HashSet<test_hashset_object>(std::move(*phs));

	ASSERT_EQ(phs2->GetSize(), test_size);
	ASSERT_EQ(phs2->GetBucketQuantity(), phs2->GetCorrectBucketQuantity(phs2->GetLoadFactor(), phs2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs2)[test_hashset_object(i)].key, i);
	}

	delete phs;
	delete phs2;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashSet, CopyAssignmentTest)
{
	HashSet<test_hashset_object>* phs = new HashSet<test_hashset_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashset_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = phs->Insert(test_hashset_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs)[test_hashset_object(i)].key, i);
	}

	HashSet<test_hashset_object>* phs2 = new HashSet<test_hashset_object>();

	ASSERT_EQ(phs2->GetSize(), 0);

	*phs2 = *phs;

	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs)[test_hashset_object(i)].key, i);
	}

	ASSERT_EQ(phs2->GetSize(), test_size);
	ASSERT_EQ(phs2->GetBucketQuantity(), phs2->GetCorrectBucketQuantity(phs2->GetLoadFactor(), phs2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs2)[test_hashset_object(i)].key, i);
	}

	delete phs;
	delete phs2;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(HashSet, MoveAssignmentTest)
{
	HashSet<test_hashset_object>* phs = new HashSet<test_hashset_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashset_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = phs->Insert(test_hashset_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs)[test_hashset_object(i)].key, i);
	}

	HashSet<test_hashset_object>* phs2 = new HashSet<test_hashset_object>();

	ASSERT_EQ(phs2->GetSize(), 0);

	*phs2 = std::move(*phs);

	ASSERT_EQ(phs2->GetSize(), test_size);
	ASSERT_EQ(phs2->GetBucketQuantity(), phs2->GetCorrectBucketQuantity(phs2->GetLoadFactor(), phs2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs2)[test_hashset_object(i)].key, i);
	}

	delete phs;
	delete phs2;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashSet, AnotherAllocatorCopyConstructionTest)
{
	HashSet<test_hashset_object>* phs = new HashSet<test_hashset_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashset_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = phs->Insert(test_hashset_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs)[test_hashset_object(i)].key, i);
	}

	HashSet<test_hashset_object, Hash<test_hashset_object>, StdAllocator>* phs2 = new HashSet<test_hashset_object, Hash<test_hashset_object>, StdAllocator>(*phs);

	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs)[test_hashset_object(i)].key, i);
	}

	ASSERT_EQ(phs2->GetSize(), test_size);
	ASSERT_EQ(phs2->GetBucketQuantity(), phs2->GetCorrectBucketQuantity(phs2->GetLoadFactor(), phs2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs2)[test_hashset_object(i)].key, i);
	}

	HashSet<test_hashset_object>* phs3 = new HashSet<test_hashset_object>(*phs2);

	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs)[test_hashset_object(i)].key, i);
	}

	ASSERT_EQ(phs2->GetSize(), test_size);
	ASSERT_EQ(phs2->GetBucketQuantity(), phs2->GetCorrectBucketQuantity(phs2->GetLoadFactor(), phs2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs2)[test_hashset_object(i)].key, i);
	}

	ASSERT_EQ(phs3->GetSize(), test_size);
	ASSERT_EQ(phs3->GetBucketQuantity(), phs3->GetCorrectBucketQuantity(phs3->GetLoadFactor(), phs3->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs3)[test_hashset_object(i)].key, i);
	}

	delete phs;
	delete phs2;
	delete phs3;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 3);
	}
}

TEST(HashSet, AnotherAllocatorMoveConstructionTest)
{
	HashSet<test_hashset_object>* phs = new HashSet<test_hashset_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashset_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = phs->Insert(test_hashset_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs)[test_hashset_object(i)].key, i);
	}

	HashSet<test_hashset_object, Hash<test_hashset_object>, StdAllocator>* phs2 = new HashSet<test_hashset_object, Hash<test_hashset_object>, StdAllocator>(std::move(*phs));

	ASSERT_EQ(phs2->GetSize(), test_size);
	ASSERT_EQ(phs2->GetBucketQuantity(), phs2->GetCorrectBucketQuantity(phs2->GetLoadFactor(), phs2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs2)[test_hashset_object(i)].key, i);
	}

	HashSet<test_hashset_object>* phs3 = new HashSet<test_hashset_object>(std::move(*phs2));

	ASSERT_EQ(phs3->GetSize(), test_size);
	ASSERT_EQ(phs3->GetBucketQuantity(), phs3->GetCorrectBucketQuantity(phs3->GetLoadFactor(), phs3->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs3)[test_hashset_object(i)].key, i);
	}

	delete phs;
	delete phs2;
	delete phs3;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashSet, AnotherAllocatorCopyAssignmentTest)
{
	HashSet<test_hashset_object>* phs = new HashSet<test_hashset_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashset_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = phs->Insert(test_hashset_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs)[test_hashset_object(i)].key, i);
	}

	HashSet<test_hashset_object, Hash<test_hashset_object>, StdAllocator>* phs2 = new HashSet<test_hashset_object, Hash<test_hashset_object>, StdAllocator>();

	ASSERT_EQ(phs2->GetSize(), 0);

	*phs2 = *phs;

	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs)[test_hashset_object(i)].key, i);
	}

	ASSERT_EQ(phs2->GetSize(), test_size);
	ASSERT_EQ(phs2->GetBucketQuantity(), phs2->GetCorrectBucketQuantity(phs2->GetLoadFactor(), phs2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs2)[test_hashset_object(i)].key, i);
	}

	HashSet<test_hashset_object>* phs3 = new HashSet<test_hashset_object>();

	ASSERT_EQ(phs3->GetSize(), 0);

	*phs3 = *phs2;

	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs)[test_hashset_object(i)].key, i);
	}

	ASSERT_EQ(phs2->GetSize(), test_size);
	ASSERT_EQ(phs2->GetBucketQuantity(), phs2->GetCorrectBucketQuantity(phs2->GetLoadFactor(), phs2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs2)[test_hashset_object(i)].key, i);
	}

	ASSERT_EQ(phs3->GetSize(), test_size);
	ASSERT_EQ(phs3->GetBucketQuantity(), phs3->GetCorrectBucketQuantity(phs3->GetLoadFactor(), phs3->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs3)[test_hashset_object(i)].key, i);
	}

	delete phs;
	delete phs2;
	delete phs3;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 3);
	}
}

TEST(HashSet, AnotherAllocatorMoveAssignmentTest)
{
	HashSet<test_hashset_object>* phs = new HashSet<test_hashset_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashset_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = phs->Insert(test_hashset_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs)[test_hashset_object(i)].key, i);
	}

	HashSet<test_hashset_object, Hash<test_hashset_object>, StdAllocator>* phs2 = new HashSet<test_hashset_object, Hash<test_hashset_object>, StdAllocator>();

	ASSERT_EQ(phs2->GetSize(), 0);

	*phs2 = std::move(*phs);

	ASSERT_EQ(phs2->GetSize(), test_size);
	ASSERT_EQ(phs2->GetBucketQuantity(), phs2->GetCorrectBucketQuantity(phs2->GetLoadFactor(), phs2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs2)[test_hashset_object(i)].key, i);
	}

	HashSet<test_hashset_object>* phs3 = new HashSet<test_hashset_object>();

	ASSERT_EQ(phs3->GetSize(), 0);

	*phs3 = std::move(*phs2);

	ASSERT_EQ(phs3->GetSize(), test_size);
	ASSERT_EQ(phs3->GetBucketQuantity(), phs3->GetCorrectBucketQuantity(phs3->GetLoadFactor(), phs3->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs3)[test_hashset_object(i)].key, i);
	}

	delete phs;
	delete phs2;
	delete phs3;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashSet, EqualTest)
{
	const HashSet<test_hashset_object, Hash<test_hashset_object>, MemoryManagerAllocator> hs1({test_hashset_object(1, 10),
																							   test_hashset_object(2, 20),
																							   test_hashset_object(3, 30)});
	const HashSet<test_hashset_object, Hash<test_hashset_object>, MemoryManagerAllocator> hs2({test_hashset_object(1, 10),
																							   test_hashset_object(2, 20)});
	const HashSet<test_hashset_object, Hash<test_hashset_object>, StdAllocator> hs2_({test_hashset_object(1, 10),
																					  test_hashset_object(2, 20)});
	const HashSet<test_hashset_object, Hash<test_hashset_object>, MemoryManagerAllocator> hs3({test_hashset_object(1, 10),
																							   test_hashset_object(2, 21),
																							   test_hashset_object(3, 30)});
	const HashSet<test_hashset_object, Hash<test_hashset_object>, StdAllocator> hs3_({test_hashset_object(1, 10),
																					  test_hashset_object(2, 21),
																					  test_hashset_object(3, 30)});
	const HashSet<test_hashset_object, Hash<test_hashset_object>, MemoryManagerAllocator> hs4({test_hashset_object(1, 10),
																							   test_hashset_object(2, 20),
																							   test_hashset_object(3, 30)});
	const HashSet<test_hashset_object, Hash<test_hashset_object>, StdAllocator> hs4_({test_hashset_object(1, 10),
																					  test_hashset_object(2, 20),
																					  test_hashset_object(3, 30)});

	ASSERT_FALSE(hs1 == hs2);
	ASSERT_FALSE(hs1 == hs2_);
	ASSERT_FALSE(hs1 == hs3);
	ASSERT_FALSE(hs1 == hs3_);
	ASSERT_TRUE(hs1 == hs4);
	ASSERT_TRUE(hs1 == hs4_);
}

TEST(HashSet, NotEqualTest)
{
	const HashSet<test_hashset_object, Hash<test_hashset_object>, MemoryManagerAllocator> hs1({test_hashset_object(1, 10),
																							   test_hashset_object(2, 20),
																							   test_hashset_object(3, 30)});
	const HashSet<test_hashset_object, Hash<test_hashset_object>, MemoryManagerAllocator> hs2({test_hashset_object(1, 10),
																							   test_hashset_object(2, 20)});
	const HashSet<test_hashset_object, Hash<test_hashset_object>, StdAllocator> hs2_({test_hashset_object(1, 10),
																					  test_hashset_object(2, 20)});
	const HashSet<test_hashset_object, Hash<test_hashset_object>, MemoryManagerAllocator> hs3({test_hashset_object(1, 10),
																							   test_hashset_object(2, 21),
																							   test_hashset_object(3, 30)});
	const HashSet<test_hashset_object, Hash<test_hashset_object>, StdAllocator> hs3_({test_hashset_object(1, 10),
																					  test_hashset_object(2, 21),
																					  test_hashset_object(3, 30)});
	const HashSet<test_hashset_object, Hash<test_hashset_object>, MemoryManagerAllocator> hs4({test_hashset_object(1, 10),
																							   test_hashset_object(2, 20),
																							   test_hashset_object(3, 30)});
	const HashSet<test_hashset_object, Hash<test_hashset_object>, StdAllocator> hs4_({test_hashset_object(1, 10),
																					  test_hashset_object(2, 20),
																					  test_hashset_object(3, 30)});

	ASSERT_TRUE(hs1 != hs2);
	ASSERT_TRUE(hs1 != hs2_);
	ASSERT_TRUE(hs1 != hs3);
	ASSERT_TRUE(hs1 != hs3_);
	ASSERT_FALSE(hs1 != hs4);
	ASSERT_FALSE(hs1 != hs4_);
}

TEST(HashSet, SwapTest)
{
	HashSet<test_hashset_object>* phs = new HashSet<test_hashset_object>();
	const int test_size = 1000;
	int val_pool[2 * test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashset_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = phs->Insert(test_hashset_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs)[test_hashset_object(i)].key, i);
	}

	HashSet<test_hashset_object>* phs2 = new HashSet<test_hashset_object>();

	for (int i = test_size; i < 2 * test_size; ++i)
	{
		auto iter = phs2->Insert(test_hashset_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}

	ASSERT_EQ(phs2->GetSize(), test_size);
	ASSERT_EQ(phs2->GetBucketQuantity(), phs2->GetCorrectBucketQuantity(phs2->GetLoadFactor(), phs2->GetSize()));
	for (int i = 2 * test_size - 1; i >= test_size; i--)
	{
		ASSERT_EQ((*phs2)[test_hashset_object(i)].key, i);
	}

	HashSet<test_hashset_object>* phs3 = new HashSet<test_hashset_object>(std::move(*phs));

	ASSERT_EQ(phs3->GetSize(), test_size);
	ASSERT_EQ(phs3->GetBucketQuantity(), phs3->GetCorrectBucketQuantity(phs3->GetLoadFactor(), phs3->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs3)[test_hashset_object(i)].key, i);
	}

	*phs = std::move(*phs2);

	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));
	for (int i = 2 * test_size - 1; i >= test_size; i--)
	{
		ASSERT_EQ((*phs)[test_hashset_object(i)].key, i);
	}

	*phs2 = std::move(*phs3);

	ASSERT_EQ(phs2->GetSize(), test_size);
	ASSERT_EQ(phs2->GetBucketQuantity(), phs2->GetCorrectBucketQuantity(phs2->GetLoadFactor(), phs2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs2)[test_hashset_object(i)].key, i);
	}

	delete phs;
	delete phs2;
	delete phs3;
	for (int i = 0; i < 2 * test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashSet, AnotherAllocatorSwapTest)
{
	HashSet<test_hashset_object>* phs = new HashSet<test_hashset_object>();
	const int test_size = 1000;
	int val_pool[2 * test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashset_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = phs->Insert(test_hashset_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs)[test_hashset_object(i)].key, i);
	}

	HashSet<test_hashset_object, Hash<test_hashset_object>, StdAllocator>* phs2 = new HashSet<test_hashset_object, Hash<test_hashset_object>, StdAllocator>();

	for (int i = test_size; i < 2 * test_size; ++i)
	{
		auto iter = phs2->Insert(test_hashset_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}

	ASSERT_EQ(phs2->GetSize(), test_size);
	ASSERT_EQ(phs2->GetBucketQuantity(), phs2->GetCorrectBucketQuantity(phs2->GetLoadFactor(), phs2->GetSize()));
	for (int i = 2 * test_size - 1; i >= test_size; i--)
	{
		ASSERT_EQ((*phs2)[test_hashset_object(i)].key, i);
	}

	HashSet<test_hashset_object>* phs3 = new HashSet<test_hashset_object>(std::move(*phs));

	ASSERT_EQ(phs3->GetSize(), test_size);
	ASSERT_EQ(phs3->GetBucketQuantity(), phs3->GetCorrectBucketQuantity(phs3->GetLoadFactor(), phs3->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs3)[test_hashset_object(i)].key, i);
	}

	*phs = std::move(*phs2);

	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));
	for (int i = 2 * test_size - 1; i >= test_size; i--)
	{
		ASSERT_EQ((*phs)[test_hashset_object(i)].key, i);
	}

	*phs2 = std::move(*phs3);

	ASSERT_EQ(phs2->GetSize(), test_size);
	ASSERT_EQ(phs2->GetBucketQuantity(), phs2->GetCorrectBucketQuantity(phs2->GetLoadFactor(), phs2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phs2)[test_hashset_object(i)].key, i);
	}

	delete phs;
	delete phs2;
	delete phs3;
	for (int i = 0; i < 2 * test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashSet, ConstructByEmptyListTest)
{
	HashSet<int> hs(std::initializer_list<int>{});
	ASSERT_EQ(hs.GetSize(), 0);

	hs.Insert(2);
	ASSERT_EQ(hs.GetSize(), 1);
	ASSERT_EQ(*hs.GetBegin(), 2);
}

TEST(HashSet, InsertEmptyListTest)
{
	HashSet<int> hs({2});
	ASSERT_EQ(hs.GetSize(), 1);
	ASSERT_EQ(*hs.GetBegin(), 2);

	hs.Insert(std::initializer_list<int>{});
	ASSERT_EQ(hs.GetSize(), 1);
	ASSERT_EQ(*hs.GetBegin(), 2);
}

TEST(HashSetIterator, OutOfRangeErrorTest)
{
	HashSet<int> hs1({1, 2});
	ASSERT_TRUE((HashSet<int>::Iterator::OutOfRangeError::Judge(hs1.GetEnd())));
	ASSERT_TRUE((HashSet<int>::ConstIterator::OutOfRangeError::Judge(hs1.GetConstEnd())));

	ASSERT_FALSE((HashSet<int>::Iterator::OutOfRangeError::Judge(hs1.GetBegin())));
	ASSERT_FALSE((HashSet<int>::ConstIterator::OutOfRangeError::Judge(hs1.GetConstBegin())));

	ASSERT_TRUE((HashSet<int>::Iterator::OutOfRangeError::Judge(hs1.GetBegin() + 2)));
	ASSERT_TRUE((HashSet<int>::ConstIterator::OutOfRangeError::Judge(hs1.GetConstBegin() + 2)));
}

TEST(HashSetIterator, GetTest)
{
	HashSet<test_hashset_object>* phs = new HashSet<test_hashset_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	int val_check_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	memset(val_check_pool, 0, sizeof(val_check_pool));
	auto val_rel_func = [&](test_hashset_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = phs->Insert(test_hashset_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));

	for (HashSet<test_hashset_object>::Iterator i = phs->GetBegin(); i != phs->GetEnd(); ++i)
	{
		val_check_pool[i->key] += 1;
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_check_pool[i], 1);
	}

	delete phs;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashSetIterator, GetConstTest)
{
	HashSet<test_hashset_object>* phs = new HashSet<test_hashset_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	int val_check_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	memset(val_check_pool, 0, sizeof(val_check_pool));
	auto val_rel_func = [&](test_hashset_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = phs->Insert(test_hashset_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));

	const HashSet<test_hashset_object>* pchs = phs;

	for (HashSet<test_hashset_object>::ConstIterator i = pchs->GetConstBegin(); i != pchs->GetConstEnd(); ++i)
	{
		val_check_pool[i->key] += 1;
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_check_pool[i], 1);
	}

	delete phs;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashSetIterator, CopyConstructionTest)
{
	HashSet<int> hs1({0, 1, 2, 3, 4, 5, 6, 7, 8, 9});

	ASSERT_EQ(hs1.GetSize(), 10);

	HashSet<int>::Iterator iter1 = hs1.GetBegin();
	ASSERT_GE(*iter1, 0);
	ASSERT_LE(*iter1, 9);
	HashSet<int>::Iterator iter2(iter1);
	ASSERT_EQ(*iter1, *iter2);

	HashSet<int>::ConstIterator citer1 = hs1.GetConstBegin();
	ASSERT_GE(*citer1, 0);
	ASSERT_LE(*citer1, 9);
	HashSet<int>::ConstIterator citer2(citer1);
	ASSERT_EQ(*citer1, *citer2);

	HashSet<int>::ConstIterator citer3(iter1);
	ASSERT_EQ(*iter1, *citer3);
}

TEST(HashSetIterator, CopyAssignmentTest)
{
	HashSet<int> hs1({0, 1, 2, 3, 4, 5, 6, 7, 8, 9});

	ASSERT_EQ(hs1.GetSize(), 10);

	HashSet<int>::Iterator iter1 = hs1.GetBegin();
	ASSERT_GE(*iter1, 0);
	ASSERT_LE(*iter1, 9);
	HashSet<int>::Iterator iter2 = hs1.GetEnd();
	ASSERT_EQ(iter2, hs1.GetEnd());
	iter2 = iter1;
	ASSERT_EQ(*iter1, *iter2);

	HashSet<int>::ConstIterator citer1 = hs1.GetConstBegin();
	ASSERT_GE(*citer1, 0);
	ASSERT_LE(*citer1, 9);
	HashSet<int>::ConstIterator citer2 = hs1.GetConstEnd();
	ASSERT_EQ(citer2, hs1.GetConstEnd());
	citer2 = citer1;
	ASSERT_EQ(*citer1, *citer2);

	HashSet<int>::ConstIterator citer3 = hs1.GetConstEnd();
	ASSERT_EQ(citer3, hs1.GetConstEnd());
	citer3 = iter1;
	ASSERT_EQ(*iter1, *citer3);
}

TEST(HashSetIterator, SelfIncrementTest)
{
	HashSet<test_hashset_object>* phs = new HashSet<test_hashset_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	int val_check_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	memset(val_check_pool, 0, sizeof(val_check_pool));
	auto val_rel_func = [&](test_hashset_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = phs->Insert(test_hashset_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));

	for (HashSet<test_hashset_object>::Iterator i = phs->GetBegin(); i != phs->GetEnd(); ++i)
	{
		val_check_pool[i->key] += 1;
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_check_pool[i], 1);
	}

	for (HashSet<test_hashset_object>::Iterator i = phs->GetBegin(); i != phs->GetEnd(); i++)
	{
		val_check_pool[i->key] += 1;
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_check_pool[i], 2);
	}

	const HashSet<test_hashset_object>* pchs = phs;

	for (HashSet<test_hashset_object>::ConstIterator i = pchs->GetConstBegin(); i != pchs->GetConstEnd(); ++i)
	{
		val_check_pool[i->key] += 1;
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_check_pool[i], 3);
	}

	for (HashSet<test_hashset_object>::ConstIterator i = pchs->GetConstBegin(); i != pchs->GetConstEnd(); i++)
	{
		val_check_pool[i->key] += 1;
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_check_pool[i], 4);
	}

	delete phs;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashSetIterator, PlusTest)
{
	HashSet<test_hashset_object>* phs = new HashSet<test_hashset_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashset_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = phs->Insert(test_hashset_object(i, i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));

	int cnt = 0;
	for (HashSet<test_hashset_object>::Iterator i = phs->GetBegin(); i != phs->GetEnd(); i = i + 2)
	{
		ASSERT_EQ(i->key, i->value);
		cnt += 1;
	}
	ASSERT_EQ(cnt, test_size / 2);

	cnt = 0;
	for (HashSet<test_hashset_object>::Iterator i = phs->GetBegin(); i != phs->GetEnd(); i += 2)
	{
		ASSERT_EQ(i->key, i->value);
		cnt += 1;
	}
	ASSERT_EQ(cnt, test_size / 2);

	const HashSet<test_hashset_object>* pchs = phs;

	cnt = 0;
	for (HashSet<test_hashset_object>::ConstIterator i = pchs->GetConstBegin(); i != pchs->GetConstEnd(); i = i + 2)
	{
		ASSERT_EQ(i->key, i->value);
		cnt += 1;
	}
	ASSERT_EQ(cnt, test_size / 2);

	cnt = 0;
	for (HashSet<test_hashset_object>::ConstIterator i = pchs->GetConstBegin(); i != pchs->GetConstEnd(); i += 2)
	{
		ASSERT_EQ(i->key, i->value);
		cnt += 1;
	}
	ASSERT_EQ(cnt, test_size / 2);

	delete phs;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashSetIterator, DistanceTest)
{
	HashSet<test_hashset_object>* phs = new HashSet<test_hashset_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashset_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = phs->Insert(test_hashset_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));

	ASSERT_EQ(phs->GetEnd() - phs->GetBegin(), test_size);

	const HashSet<test_hashset_object>* pchs = phs;

	ASSERT_EQ(pchs->GetConstEnd() - pchs->GetConstBegin(), test_size);

	delete phs;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashSetIterator, OperatorPointerTest)
{
	HashSet<test_hashset_object>* phs = new HashSet<test_hashset_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	int val_check_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	memset(val_check_pool, 0, sizeof(val_check_pool));
	auto val_rel_func = [&](test_hashset_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = phs->Insert(test_hashset_object(i, i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));

	for (HashSet<test_hashset_object>::Iterator i = phs->GetBegin(); i != phs->GetEnd(); ++i)
	{
		val_check_pool[i->key] += 1;
		ASSERT_EQ(i->key, i->value);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_check_pool[i], 1);
	}

	const HashSet<test_hashset_object>* pchs = phs;

	for (HashSet<test_hashset_object>::ConstIterator i = pchs->GetConstBegin(); i != pchs->GetConstEnd(); ++i)
	{
		val_check_pool[i->key] += 1;
		ASSERT_EQ(i->key, i->value);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_check_pool[i], 2);
	}

	delete phs;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashSetIterator, OperatorReferenceTest)
{
	HashSet<test_hashset_object>* phs = new HashSet<test_hashset_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	int val_check_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	memset(val_check_pool, 0, sizeof(val_check_pool));
	auto val_rel_func = [&](test_hashset_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = phs->Insert(test_hashset_object(i, i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));

	for (HashSet<test_hashset_object>::Iterator i = phs->GetBegin(); i != phs->GetEnd(); ++i)
	{
		val_check_pool[(*i).key] += 1;
		ASSERT_EQ((*i).key, (*i).value);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_check_pool[i], 1);
	}

	const HashSet<test_hashset_object>* pchs = phs;

	for (HashSet<test_hashset_object>::ConstIterator i = pchs->GetConstBegin(); i != pchs->GetConstEnd(); ++i)
	{
		val_check_pool[(*i).key] += 1;
		ASSERT_EQ((*i).key, (*i).value);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_check_pool[i], 2);
	}

	delete phs;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashSetIterator, EqualityTest)
{
	HashSet<int> hs1({0, 1, 2, 3, 4, 5, 6, 7, 8, 9});

	ASSERT_EQ(hs1.GetSize(), 10);

	HashSet<int>::Iterator iter1 = hs1.GetBegin();
	iter1 += 1;
	ASSERT_GE(*iter1, 0);
	ASSERT_LE(*iter1, 9);
	HashSet<int>::Iterator iter2 = hs1.GetBegin() + 1;
	ASSERT_GE(*iter2, 0);
	ASSERT_LE(*iter2, 9);

	ASSERT_EQ(iter1, iter2);
	ASSERT_TRUE(iter1 != hs1.GetBegin());
	ASSERT_TRUE(iter2 != hs1.GetEnd());

	HashSet<int>::ConstIterator citer1 = hs1.GetConstBegin();
	citer1 += 1;
	ASSERT_GE(*citer1, 0);
	ASSERT_LE(*citer1, 9);
	HashSet<int>::ConstIterator citer2 = hs1.GetConstBegin() + 1;
	ASSERT_GE(*citer2, 0);
	ASSERT_LE(*citer2, 9);

	ASSERT_EQ(citer1, citer2);
	ASSERT_TRUE(citer1 != hs1.GetConstBegin());
	ASSERT_TRUE(citer2 != hs1.GetConstEnd());
}

TEST(HashSetIterator, GetDataTest)
{
	HashSet<test_hashset_object>* phs = new HashSet<test_hashset_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	int val_check_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	memset(val_check_pool, 0, sizeof(val_check_pool));
	auto val_rel_func = [&](test_hashset_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = phs->Insert(test_hashset_object(i, i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(phs->GetSize(), test_size);
	ASSERT_EQ(phs->GetBucketQuantity(), phs->GetCorrectBucketQuantity(phs->GetLoadFactor(), phs->GetSize()));

	for (HashSet<test_hashset_object>::Iterator i = phs->GetBegin(); i != phs->GetEnd(); ++i)
	{
		val_check_pool[i.GetData()->key] += 1;
		ASSERT_EQ(i.GetData()->key, i.GetData()->value);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_check_pool[i], 1);
	}

	const HashSet<test_hashset_object>* pchs = phs;

	for (HashSet<test_hashset_object>::ConstIterator i = pchs->GetConstBegin(); i != pchs->GetConstEnd(); ++i)
	{
		val_check_pool[i.GetData()->key] += 1;
		ASSERT_EQ(i.GetData()->key, i.GetData()->value);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_check_pool[i], 2);
	}

	delete phs;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}
