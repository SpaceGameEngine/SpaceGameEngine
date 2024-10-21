/*
Copyright 2024 creatorlxd

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
#pragma once
#include "gtest/gtest.h"
#include "Container/Detail/HashTable.hpp"
#include <functional>
#include <cstring>
#include <vector>

using namespace SpaceGameEngine;

struct test_hashtable_object
{
	std::function<void(test_hashtable_object&)> rel_func;
	test_hashtable_object()
		: key(0), rel_func([](test_hashtable_object&) {})
	{
	}
	test_hashtable_object(int k, int v = 0)
		: key(k), value(v), rel_func([](test_hashtable_object&) {})
	{
	}
	test_hashtable_object(int k, const std::function<void(test_hashtable_object&)>& func)
		: key(k), value(0), rel_func(func)
	{
	}
	test_hashtable_object(int k, int v, const std::function<void(test_hashtable_object&)>& func)
		: key(k), value(v), rel_func(func)
	{
	}
	test_hashtable_object(const test_hashtable_object& o) noexcept
		: key(o.key), value(o.value), rel_func(o.rel_func)
	{
	}
	test_hashtable_object(test_hashtable_object&& o) noexcept
		: key(o.key), value(o.value), rel_func(std::move(o.rel_func))
	{
		o.rel_func = [](test_hashtable_object&) {};
	}
	~test_hashtable_object()
	{
		rel_func(*this);
	}
	int key;
	int value;

	test_hashtable_object& operator=(const test_hashtable_object& o)
	{
		key = o.key;
		value = o.value;
		rel_func = o.rel_func;
		return *this;
	}

	test_hashtable_object& operator=(test_hashtable_object&& o)
	{
		key = o.key;
		value = o.value;
		rel_func = std::move(o.rel_func);
		o.rel_func = [](test_hashtable_object&) {};
		return *this;
	}

	bool operator<(const test_hashtable_object& o) const
	{
		return key < o.key;
	}

	// only compare key here
	bool operator==(const test_hashtable_object& o) const
	{
		return key == o.key && value == o.value;
	}

	// only compare key here
	bool operator!=(const test_hashtable_object& o) const
	{
		return key != o.key || value != o.value;
	}
};

template<>
struct Equal<test_hashtable_object>
{
	inline static constexpr bool Compare(const test_hashtable_object& lhs, const test_hashtable_object& rhs)
	{
		return lhs.key == rhs.key;
	}
};

template<>
struct SpaceGameEngine::Hash<test_hashtable_object>
{
	inline static HashType GetHash(const test_hashtable_object& t)
	{
		return std::hash<int>()(t.key);
	}
};

TEST(HashTable, InstanceTest)
{
	Detail::HashTable<int> ht1;
	ASSERT_EQ(ht1.GetSize(), 0);
	ASSERT_EQ(ht1.GetBucketQuantity(), ht1.sm_DefaultBucketQuantity);
}

TEST(HashTable, ListConstructionTest)
{
	const int test_size = 10;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashtable_object& o) {
		val_pool[o.key] += 1;
	};

	Detail::HashTable<test_hashtable_object>* pht = new Detail::HashTable<test_hashtable_object>(
		{test_hashtable_object(0, val_rel_func),
		 test_hashtable_object(1, val_rel_func),
		 test_hashtable_object(2, val_rel_func),
		 test_hashtable_object(3, val_rel_func),
		 test_hashtable_object(4, val_rel_func),
		 test_hashtable_object(5, val_rel_func),
		 test_hashtable_object(6, val_rel_func),
		 test_hashtable_object(7, val_rel_func),
		 test_hashtable_object(8, val_rel_func),
		 test_hashtable_object(9, val_rel_func)});

	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}
	delete pht;
	for (int i = 0; i < test_size; i++)
	{
		// initializer_list can only return const variable, so move is useless
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(HashTable, GetLoadFactorTest)
{
	Detail::HashTable<int> ht1;
	ASSERT_EQ(ht1.GetLoadFactor(), ht1.sm_DefaultLoadFactor);
}

TEST(HashTable, SetLoadFactorTest)
{
	Detail::HashTable<test_hashtable_object>* pht = new Detail::HashTable<test_hashtable_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashtable_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pht->Insert(test_hashtable_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));

	pht->SetLoadFactor(0.75);

	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetLoadFactor(), 0.75);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}
	delete pht;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashTable, GetCorrectBucketQuantityTest)
{
	using ht = Detail::HashTable<int>;
	ASSERT_EQ(ht::GetCorrectBucketQuantity(1.0, 0), 1);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(1.0, 1), 1);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(1.0, 2), 2);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(1.0, 3), 4);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(1.0, 4), 4);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(1.0, 5), 8);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(1.0, 6), 8);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(1.0, 7), 8);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(1.0, 8), 8);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(1.0, 9), 16);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(1.0, 10), 16);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(1.0, 11), 16);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(1.0, 12), 16);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(1.0, 13), 16);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(1.0, 14), 16);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(1.0, 15), 16);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(1.0, 16), 16);

	ASSERT_EQ(ht::GetCorrectBucketQuantity(0.5, 0), 1);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(0.5, 1), 2);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(0.5, 2), 4);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(0.5, 3), 8);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(0.5, 4), 8);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(0.5, 5), 16);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(0.5, 6), 16);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(0.5, 7), 16);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(0.5, 8), 16);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(0.5, 9), 32);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(0.5, 10), 32);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(0.5, 11), 32);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(0.5, 12), 32);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(0.5, 13), 32);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(0.5, 14), 32);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(0.5, 15), 32);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(0.5, 16), 32);

	ASSERT_EQ(ht::GetCorrectBucketQuantity(2.0, 0), 1);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(2.0, 1), 1);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(2.0, 2), 1);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(2.0, 3), 2);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(2.0, 4), 2);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(2.0, 5), 4);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(2.0, 6), 4);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(2.0, 7), 4);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(2.0, 8), 4);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(2.0, 9), 8);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(2.0, 10), 8);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(2.0, 11), 8);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(2.0, 12), 8);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(2.0, 13), 8);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(2.0, 14), 8);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(2.0, 15), 8);
	ASSERT_EQ(ht::GetCorrectBucketQuantity(2.0, 16), 8);
}

TEST(HashTable, GetSizeTest)
{
	Detail::HashTable<int> ht1;
	ASSERT_EQ(ht1.GetSize(), 0);
	ht1.Insert(1);
	ASSERT_EQ(ht1.GetSize(), 1);
	ht1.Insert(1);
	ASSERT_EQ(ht1.GetSize(), 1);
	ht1.Insert(2);
	ht1.Insert(3);
	ASSERT_EQ(ht1.GetSize(), 3);

	const Detail::HashTable<int> chm1({1, 2, 3, 4});
	ASSERT_EQ(chm1.GetSize(), 4);
}

TEST(HashTable, GetBucketQuantityTest)
{
	ASSERT_EQ((SpaceGameEngine::Detail::HashTable<int>::sm_DefaultLoadFactor), 1.0f);
	ASSERT_EQ((SpaceGameEngine::Detail::HashTable<int>::sm_DefaultBucketQuantity), 16);

	Detail::HashTable<int> ht1;
	ASSERT_EQ(ht1.GetBucketQuantity(), (SpaceGameEngine::Detail::HashTable<int>::sm_DefaultBucketQuantity));
	ht1.Insert(1);
	ASSERT_EQ(ht1.GetBucketQuantity(), (SpaceGameEngine::Detail::HashTable<int>::sm_DefaultBucketQuantity));
	ht1.Insert(1);
	ASSERT_EQ(ht1.GetBucketQuantity(), (SpaceGameEngine::Detail::HashTable<int>::sm_DefaultBucketQuantity));
	ht1.Insert(2);
	ht1.Insert(3);
	ASSERT_EQ(ht1.GetBucketQuantity(), (SpaceGameEngine::Detail::HashTable<int>::sm_DefaultBucketQuantity));

	const Detail::HashTable<int> chm1({1, 2, 3, 4});
	ASSERT_EQ(chm1.GetBucketQuantity(), 4);
}

TEST(HashTable, InsertTest)
{
	Detail::HashTable<test_hashtable_object>* pht = new Detail::HashTable<test_hashtable_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashtable_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pht->Insert(test_hashtable_object(i, i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_EQ(iter.m_First->value, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
		auto iter = pht->Insert(test_hashtable_object(i, test_size - 1 - i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_EQ(iter.m_First->value, i);
		ASSERT_FALSE(iter.m_Second);
	}
	delete pht;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(HashTable, UpsertTest)
{
	Detail::HashTable<test_hashtable_object>* pht = new Detail::HashTable<test_hashtable_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashtable_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pht->Upsert(test_hashtable_object(i, i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_EQ(iter.m_First->value, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
		auto iter = pht->Upsert(test_hashtable_object(i, test_size - 1 - i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_EQ(iter.m_First->value, test_size - 1 - i);
		ASSERT_FALSE(iter.m_Second);
	}
	delete pht;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashTable, InsertListTest)
{
	Detail::HashTable<test_hashtable_object>* pht = new Detail::HashTable<test_hashtable_object>();
	const int test_size = 10;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashtable_object& o) {
		val_pool[o.key] += 1;
	};

	// test repeat insert
	pht->Insert(test_hashtable_object(0, -1));
	ASSERT_EQ(pht->GetSize(), 1);
	ASSERT_EQ(pht->GetBegin()->value, -1);

	// have 11 elements, test repeat insert
	pht->Insert({test_hashtable_object(0, 0, val_rel_func),
				 test_hashtable_object(1, -1),
				 test_hashtable_object(1, 1, val_rel_func),
				 test_hashtable_object(2, 2, val_rel_func),
				 test_hashtable_object(3, 3, val_rel_func),
				 test_hashtable_object(4, 4, val_rel_func),
				 test_hashtable_object(5, 5, val_rel_func),
				 test_hashtable_object(6, 6, val_rel_func),
				 test_hashtable_object(7, 7, val_rel_func),
				 test_hashtable_object(8, 8, val_rel_func),
				 test_hashtable_object(9, 9, val_rel_func)});

	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	for (int i = test_size - 1; i >= 2; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}
	ASSERT_EQ((*pht)[test_hashtable_object(0)].value, -1);
	ASSERT_EQ((*pht)[test_hashtable_object(1)].value, -1);
	delete pht;
	for (int i = 0; i < test_size; i++)
	{
		// initializer_list can only return const variable, so move is useless
		ASSERT_EQ(val_pool[i], i > 1 ? 2 : 1);
	}
}

TEST(HashTable, UpsertListTest)
{
	Detail::HashTable<test_hashtable_object>* pht = new Detail::HashTable<test_hashtable_object>();
	const int test_size = 10;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashtable_object& o) {
		val_pool[o.key] += 1;
	};

	// test repeat insert
	pht->Upsert(test_hashtable_object(0, -1));
	ASSERT_EQ(pht->GetSize(), 1);
	ASSERT_EQ(pht->GetBegin()->value, -1);

	// have 11 elements, test repeat insert
	pht->Upsert({test_hashtable_object(0, 0, val_rel_func),
				 test_hashtable_object(1, -1),
				 test_hashtable_object(1, 1, val_rel_func),
				 test_hashtable_object(2, 2, val_rel_func),
				 test_hashtable_object(3, 3, val_rel_func),
				 test_hashtable_object(4, 4, val_rel_func),
				 test_hashtable_object(5, 5, val_rel_func),
				 test_hashtable_object(6, 6, val_rel_func),
				 test_hashtable_object(7, 7, val_rel_func),
				 test_hashtable_object(8, 8, val_rel_func),
				 test_hashtable_object(9, 9, val_rel_func)});

	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}
	delete pht;
	for (int i = 0; i < test_size; i++)
	{
		// initializer_list can only return const variable, so move is useless
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(HashTable, RemoveTest)
{
	Detail::HashTable<test_hashtable_object>* pht = new Detail::HashTable<test_hashtable_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashtable_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pht->Insert(test_hashtable_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pht->GetSize(), test_size);
	SizeType now_bucket_quantity = pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize());
	ASSERT_EQ(pht->GetBucketQuantity(), now_bucket_quantity);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}

	int rm_cnt = 0;
	auto iter = pht->GetBegin();
	while (iter != pht->GetEnd())
	{
		iter = pht->Remove(iter);
		rm_cnt += 1;
	}
	ASSERT_EQ(rm_cnt, test_size);
	ASSERT_EQ(pht->GetSize(), 0);
	ASSERT_EQ(pht->GetBucketQuantity(), now_bucket_quantity);
	delete pht;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashTable, RemoveByKeyTest)
{
	Detail::HashTable<test_hashtable_object>* pht = new Detail::HashTable<test_hashtable_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashtable_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pht->Insert(test_hashtable_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pht->GetSize(), test_size);
	SizeType now_bucket_quantity = pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize());
	ASSERT_EQ(pht->GetBucketQuantity(), now_bucket_quantity);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_TRUE(pht->RemoveByKey(test_hashtable_object(i)));
	}

	ASSERT_EQ(pht->GetSize(), 0);
	ASSERT_EQ(pht->GetBucketQuantity(), now_bucket_quantity);
	delete pht;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashTable, ShrinkToFitTest)
{
	Detail::HashTable<test_hashtable_object>* pht = new Detail::HashTable<test_hashtable_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashtable_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pht->Insert(test_hashtable_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pht->GetSize(), test_size);
	SizeType now_bucket_quantity = pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize());
	ASSERT_EQ(pht->GetBucketQuantity(), now_bucket_quantity);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_TRUE(pht->RemoveByKey(test_hashtable_object(i)));
	}

	ASSERT_EQ(pht->GetSize(), 0);
	ASSERT_EQ(pht->GetBucketQuantity(), now_bucket_quantity);

	pht->ShrinkToFit();

	ASSERT_EQ(pht->GetSize(), 0);
	ASSERT_TRUE(pht->GetBucketQuantity() != now_bucket_quantity);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));

	delete pht;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashTable, FindTest)
{
	Detail::HashTable<test_hashtable_object>* pht = new Detail::HashTable<test_hashtable_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashtable_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pht->Insert(test_hashtable_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));

	for (int i = test_size - 1; i >= 0; i--)
	{
		auto iter = pht->Find(test_hashtable_object(i));
		ASSERT_EQ(iter->key, i);
	}

	auto niter = pht->Find(test_hashtable_object(test_size));
	ASSERT_EQ(niter, pht->GetEnd());

	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));

	const Detail::HashTable<test_hashtable_object>* pchm = pht;

	for (int i = test_size - 1; i >= 0; i--)
	{
		auto iter = pchm->Find(test_hashtable_object(i));
		ASSERT_EQ(iter->key, i);
	}

	auto cniter = pchm->Find(test_hashtable_object(test_size));
	ASSERT_EQ(cniter, pchm->GetConstEnd());

	ASSERT_EQ(pchm->GetSize(), test_size);
	ASSERT_EQ(pchm->GetBucketQuantity(), pchm->GetCorrectBucketQuantity(pchm->GetLoadFactor(), pchm->GetSize()));

	delete pht;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashTable, GetTest)
{
	Detail::HashTable<test_hashtable_object>* pht = new Detail::HashTable<test_hashtable_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashtable_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pht->Insert(test_hashtable_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pht->Get(test_hashtable_object(i)).key, i);
	}

	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));

	const Detail::HashTable<test_hashtable_object>* pchm = pht;

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pchm->Get(test_hashtable_object(i)).key, i);
	}

	ASSERT_EQ(pchm->GetSize(), test_size);
	ASSERT_EQ(pchm->GetBucketQuantity(), pchm->GetCorrectBucketQuantity(pchm->GetLoadFactor(), pchm->GetSize()));

	delete pht;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashTable, OperatorTest)
{
	Detail::HashTable<test_hashtable_object>* pht = new Detail::HashTable<test_hashtable_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashtable_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		(*pht)[test_hashtable_object(i)] = test_hashtable_object(i, i, val_rel_func);
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
		ASSERT_EQ((*pht)[test_hashtable_object(i)].value, i);
		(*pht)[test_hashtable_object(i)].value += 1;
	}
	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
		ASSERT_EQ((*pht)[test_hashtable_object(i)].value, i + 1);
		(*pht)[test_hashtable_object(i)].value -= 1;
	}
	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	delete pht;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashTable, RehashTest)
{
	Detail::HashTable<test_hashtable_object>* pht = new Detail::HashTable<test_hashtable_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashtable_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pht->Insert(test_hashtable_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	pht->Rehash(1);
	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), 1);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}
	delete pht;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashTable, CopyConstructionTest)
{
	Detail::HashTable<test_hashtable_object>* pht = new Detail::HashTable<test_hashtable_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashtable_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pht->Insert(test_hashtable_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}

	Detail::HashTable<test_hashtable_object>* phm2 = new Detail::HashTable<test_hashtable_object>(*pht);

	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}

	ASSERT_EQ(phm2->GetSize(), test_size);
	ASSERT_EQ(phm2->GetBucketQuantity(), phm2->GetCorrectBucketQuantity(phm2->GetLoadFactor(), phm2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm2)[test_hashtable_object(i)].key, i);
	}

	delete pht;
	delete phm2;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(HashTable, MoveConstructionTest)
{
	Detail::HashTable<test_hashtable_object>* pht = new Detail::HashTable<test_hashtable_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashtable_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pht->Insert(test_hashtable_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}

	Detail::HashTable<test_hashtable_object>* phm2 = new Detail::HashTable<test_hashtable_object>(std::move(*pht));

	ASSERT_EQ(phm2->GetSize(), test_size);
	ASSERT_EQ(phm2->GetBucketQuantity(), phm2->GetCorrectBucketQuantity(phm2->GetLoadFactor(), phm2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm2)[test_hashtable_object(i)].key, i);
	}

	delete pht;
	delete phm2;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashTable, CopyAssignmentTest)
{
	Detail::HashTable<test_hashtable_object>* pht = new Detail::HashTable<test_hashtable_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashtable_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pht->Insert(test_hashtable_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}

	Detail::HashTable<test_hashtable_object>* phm2 = new Detail::HashTable<test_hashtable_object>();

	ASSERT_EQ(phm2->GetSize(), 0);

	*phm2 = *pht;

	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}

	ASSERT_EQ(phm2->GetSize(), test_size);
	ASSERT_EQ(phm2->GetBucketQuantity(), phm2->GetCorrectBucketQuantity(phm2->GetLoadFactor(), phm2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm2)[test_hashtable_object(i)].key, i);
	}

	delete pht;
	delete phm2;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(HashTable, MoveAssignmentTest)
{
	Detail::HashTable<test_hashtable_object>* pht = new Detail::HashTable<test_hashtable_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashtable_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pht->Insert(test_hashtable_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}

	Detail::HashTable<test_hashtable_object>* phm2 = new Detail::HashTable<test_hashtable_object>();

	ASSERT_EQ(phm2->GetSize(), 0);

	*phm2 = std::move(*pht);

	ASSERT_EQ(phm2->GetSize(), test_size);
	ASSERT_EQ(phm2->GetBucketQuantity(), phm2->GetCorrectBucketQuantity(phm2->GetLoadFactor(), phm2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm2)[test_hashtable_object(i)].key, i);
	}

	delete pht;
	delete phm2;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashTable, AnotherAllocatorCopyConstructionTest)
{
	Detail::HashTable<test_hashtable_object>* pht = new Detail::HashTable<test_hashtable_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashtable_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pht->Insert(test_hashtable_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}

	Detail::HashTable<test_hashtable_object, Hash<test_hashtable_object>, Equal<test_hashtable_object>, StdAllocator>* phm2 = new Detail::HashTable<test_hashtable_object, Hash<test_hashtable_object>, Equal<test_hashtable_object>, StdAllocator>(*pht);

	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}

	ASSERT_EQ(phm2->GetSize(), test_size);
	ASSERT_EQ(phm2->GetBucketQuantity(), phm2->GetCorrectBucketQuantity(phm2->GetLoadFactor(), phm2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm2)[test_hashtable_object(i)].key, i);
	}

	Detail::HashTable<test_hashtable_object>* phm3 = new Detail::HashTable<test_hashtable_object>(*phm2);

	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}

	ASSERT_EQ(phm2->GetSize(), test_size);
	ASSERT_EQ(phm2->GetBucketQuantity(), phm2->GetCorrectBucketQuantity(phm2->GetLoadFactor(), phm2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm2)[test_hashtable_object(i)].key, i);
	}

	ASSERT_EQ(phm3->GetSize(), test_size);
	ASSERT_EQ(phm3->GetBucketQuantity(), phm3->GetCorrectBucketQuantity(phm3->GetLoadFactor(), phm3->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm3)[test_hashtable_object(i)].key, i);
	}

	delete pht;
	delete phm2;
	delete phm3;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 3);
	}
}

TEST(HashTable, AnotherAllocatorMoveConstructionTest)
{
	Detail::HashTable<test_hashtable_object>* pht = new Detail::HashTable<test_hashtable_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashtable_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pht->Insert(test_hashtable_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}

	Detail::HashTable<test_hashtable_object, Hash<test_hashtable_object>, Equal<test_hashtable_object>, StdAllocator>* phm2 = new Detail::HashTable<test_hashtable_object, Hash<test_hashtable_object>, Equal<test_hashtable_object>, StdAllocator>(std::move(*pht));

	ASSERT_EQ(phm2->GetSize(), test_size);
	ASSERT_EQ(phm2->GetBucketQuantity(), phm2->GetCorrectBucketQuantity(phm2->GetLoadFactor(), phm2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm2)[test_hashtable_object(i)].key, i);
	}

	Detail::HashTable<test_hashtable_object>* phm3 = new Detail::HashTable<test_hashtable_object>(std::move(*phm2));

	ASSERT_EQ(phm3->GetSize(), test_size);
	ASSERT_EQ(phm3->GetBucketQuantity(), phm3->GetCorrectBucketQuantity(phm3->GetLoadFactor(), phm3->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm3)[test_hashtable_object(i)].key, i);
	}

	delete pht;
	delete phm2;
	delete phm3;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashTable, AnotherAllocatorCopyAssignmentTest)
{
	Detail::HashTable<test_hashtable_object>* pht = new Detail::HashTable<test_hashtable_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashtable_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pht->Insert(test_hashtable_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}

	Detail::HashTable<test_hashtable_object, Hash<test_hashtable_object>, Equal<test_hashtable_object>, StdAllocator>* phm2 = new Detail::HashTable<test_hashtable_object, Hash<test_hashtable_object>, Equal<test_hashtable_object>, StdAllocator>();

	ASSERT_EQ(phm2->GetSize(), 0);

	*phm2 = *pht;

	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}

	ASSERT_EQ(phm2->GetSize(), test_size);
	ASSERT_EQ(phm2->GetBucketQuantity(), phm2->GetCorrectBucketQuantity(phm2->GetLoadFactor(), phm2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm2)[test_hashtable_object(i)].key, i);
	}

	Detail::HashTable<test_hashtable_object>* phm3 = new Detail::HashTable<test_hashtable_object>();

	ASSERT_EQ(phm3->GetSize(), 0);

	*phm3 = *phm2;

	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}

	ASSERT_EQ(phm2->GetSize(), test_size);
	ASSERT_EQ(phm2->GetBucketQuantity(), phm2->GetCorrectBucketQuantity(phm2->GetLoadFactor(), phm2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm2)[test_hashtable_object(i)].key, i);
	}

	ASSERT_EQ(phm3->GetSize(), test_size);
	ASSERT_EQ(phm3->GetBucketQuantity(), phm3->GetCorrectBucketQuantity(phm3->GetLoadFactor(), phm3->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm3)[test_hashtable_object(i)].key, i);
	}

	delete pht;
	delete phm2;
	delete phm3;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 3);
	}
}

TEST(HashTable, AnotherAllocatorMoveAssignmentTest)
{
	Detail::HashTable<test_hashtable_object>* pht = new Detail::HashTable<test_hashtable_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashtable_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pht->Insert(test_hashtable_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}

	Detail::HashTable<test_hashtable_object, Hash<test_hashtable_object>, Equal<test_hashtable_object>, StdAllocator>* phm2 = new Detail::HashTable<test_hashtable_object, Hash<test_hashtable_object>, Equal<test_hashtable_object>, StdAllocator>();

	ASSERT_EQ(phm2->GetSize(), 0);

	*phm2 = std::move(*pht);

	ASSERT_EQ(phm2->GetSize(), test_size);
	ASSERT_EQ(phm2->GetBucketQuantity(), phm2->GetCorrectBucketQuantity(phm2->GetLoadFactor(), phm2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm2)[test_hashtable_object(i)].key, i);
	}

	Detail::HashTable<test_hashtable_object>* phm3 = new Detail::HashTable<test_hashtable_object>();

	ASSERT_EQ(phm3->GetSize(), 0);

	*phm3 = std::move(*phm2);

	ASSERT_EQ(phm3->GetSize(), test_size);
	ASSERT_EQ(phm3->GetBucketQuantity(), phm3->GetCorrectBucketQuantity(phm3->GetLoadFactor(), phm3->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm3)[test_hashtable_object(i)].key, i);
	}

	delete pht;
	delete phm2;
	delete phm3;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashTable, EqualTest)
{
	const Detail::HashTable<test_hashtable_object, Hash<test_hashtable_object>, Equal<test_hashtable_object>, MemoryManagerAllocator> ht1({test_hashtable_object(1, 10),
																																		   test_hashtable_object(2, 20),
																																		   test_hashtable_object(3, 30)});
	const Detail::HashTable<test_hashtable_object, Hash<test_hashtable_object>, Equal<test_hashtable_object>, MemoryManagerAllocator> ht2({test_hashtable_object(1, 10),
																																		   test_hashtable_object(2, 20)});
	const Detail::HashTable<test_hashtable_object, Hash<test_hashtable_object>, Equal<test_hashtable_object>, StdAllocator> ht2_({test_hashtable_object(1, 10),
																																  test_hashtable_object(2, 20)});
	const Detail::HashTable<test_hashtable_object, Hash<test_hashtable_object>, Equal<test_hashtable_object>, MemoryManagerAllocator> hm3({test_hashtable_object(1, 10),
																																		   test_hashtable_object(2, 21),
																																		   test_hashtable_object(3, 30)});
	const Detail::HashTable<test_hashtable_object, Hash<test_hashtable_object>, Equal<test_hashtable_object>, StdAllocator> ht3_({test_hashtable_object(1, 10),
																																  test_hashtable_object(2, 21),
																																  test_hashtable_object(3, 30)});
	const Detail::HashTable<test_hashtable_object, Hash<test_hashtable_object>, Equal<test_hashtable_object>, MemoryManagerAllocator> ht4({test_hashtable_object(1, 10),
																																		   test_hashtable_object(2, 20),
																																		   test_hashtable_object(3, 30)});
	const Detail::HashTable<test_hashtable_object, Hash<test_hashtable_object>, Equal<test_hashtable_object>, StdAllocator> ht4_({test_hashtable_object(1, 10),
																																  test_hashtable_object(2, 20),
																																  test_hashtable_object(3, 30)});

	ASSERT_FALSE(ht1 == ht2);
	ASSERT_FALSE(ht1 == ht2_);
	ASSERT_FALSE(ht1 == hm3);
	ASSERT_FALSE(ht1 == ht3_);
	ASSERT_TRUE(ht1 == ht4);
	ASSERT_TRUE(ht1 == ht4_);
}

TEST(HashTable, NotEqualTest)
{
	const Detail::HashTable<test_hashtable_object, Hash<test_hashtable_object>, Equal<test_hashtable_object>, MemoryManagerAllocator> ht1({test_hashtable_object(1, 10),
																																		   test_hashtable_object(2, 20),
																																		   test_hashtable_object(3, 30)});
	const Detail::HashTable<test_hashtable_object, Hash<test_hashtable_object>, Equal<test_hashtable_object>, MemoryManagerAllocator> ht2({test_hashtable_object(1, 10),
																																		   test_hashtable_object(2, 20)});
	const Detail::HashTable<test_hashtable_object, Hash<test_hashtable_object>, Equal<test_hashtable_object>, StdAllocator> ht2_({test_hashtable_object(1, 10),
																																  test_hashtable_object(2, 20)});
	const Detail::HashTable<test_hashtable_object, Hash<test_hashtable_object>, Equal<test_hashtable_object>, MemoryManagerAllocator> hm3({test_hashtable_object(1, 10),
																																		   test_hashtable_object(2, 21),
																																		   test_hashtable_object(3, 30)});
	const Detail::HashTable<test_hashtable_object, Hash<test_hashtable_object>, Equal<test_hashtable_object>, StdAllocator> ht3_({test_hashtable_object(1, 10),
																																  test_hashtable_object(2, 21),
																																  test_hashtable_object(3, 30)});
	const Detail::HashTable<test_hashtable_object, Hash<test_hashtable_object>, Equal<test_hashtable_object>, MemoryManagerAllocator> ht4({test_hashtable_object(1, 10),
																																		   test_hashtable_object(2, 20),
																																		   test_hashtable_object(3, 30)});
	const Detail::HashTable<test_hashtable_object, Hash<test_hashtable_object>, Equal<test_hashtable_object>, StdAllocator> ht4_({test_hashtable_object(1, 10),
																																  test_hashtable_object(2, 20),
																																  test_hashtable_object(3, 30)});

	ASSERT_TRUE(ht1 != ht2);
	ASSERT_TRUE(ht1 != ht2_);
	ASSERT_TRUE(ht1 != hm3);
	ASSERT_TRUE(ht1 != ht3_);
	ASSERT_FALSE(ht1 != ht4);
	ASSERT_FALSE(ht1 != ht4_);
}

TEST(HashTable, SwapTest)
{
	Detail::HashTable<test_hashtable_object>* pht = new Detail::HashTable<test_hashtable_object>();
	const int test_size = 1000;
	int val_pool[2 * test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashtable_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pht->Insert(test_hashtable_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}

	Detail::HashTable<test_hashtable_object>* phm2 = new Detail::HashTable<test_hashtable_object>();

	for (int i = test_size; i < 2 * test_size; ++i)
	{
		auto iter = phm2->Insert(test_hashtable_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}

	ASSERT_EQ(phm2->GetSize(), test_size);
	ASSERT_EQ(phm2->GetBucketQuantity(), phm2->GetCorrectBucketQuantity(phm2->GetLoadFactor(), phm2->GetSize()));
	for (int i = 2 * test_size - 1; i >= test_size; i--)
	{
		ASSERT_EQ((*phm2)[test_hashtable_object(i)].key, i);
	}

	Detail::HashTable<test_hashtable_object>* phm3 = new Detail::HashTable<test_hashtable_object>(std::move(*pht));

	ASSERT_EQ(phm3->GetSize(), test_size);
	ASSERT_EQ(phm3->GetBucketQuantity(), phm3->GetCorrectBucketQuantity(phm3->GetLoadFactor(), phm3->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm3)[test_hashtable_object(i)].key, i);
	}

	*pht = std::move(*phm2);

	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	for (int i = 2 * test_size - 1; i >= test_size; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}

	*phm2 = std::move(*phm3);

	ASSERT_EQ(phm2->GetSize(), test_size);
	ASSERT_EQ(phm2->GetBucketQuantity(), phm2->GetCorrectBucketQuantity(phm2->GetLoadFactor(), phm2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm2)[test_hashtable_object(i)].key, i);
	}

	delete pht;
	delete phm2;
	delete phm3;
	for (int i = 0; i < 2 * test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashTable, AnotherAllocatorSwapTest)
{
	Detail::HashTable<test_hashtable_object>* pht = new Detail::HashTable<test_hashtable_object>();
	const int test_size = 1000;
	int val_pool[2 * test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashtable_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pht->Insert(test_hashtable_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}

	Detail::HashTable<test_hashtable_object, Hash<test_hashtable_object>, Equal<test_hashtable_object>, StdAllocator>* phm2 = new Detail::HashTable<test_hashtable_object, Hash<test_hashtable_object>, Equal<test_hashtable_object>, StdAllocator>();

	for (int i = test_size; i < 2 * test_size; ++i)
	{
		auto iter = phm2->Insert(test_hashtable_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}

	ASSERT_EQ(phm2->GetSize(), test_size);
	ASSERT_EQ(phm2->GetBucketQuantity(), phm2->GetCorrectBucketQuantity(phm2->GetLoadFactor(), phm2->GetSize()));
	for (int i = 2 * test_size - 1; i >= test_size; i--)
	{
		ASSERT_EQ((*phm2)[test_hashtable_object(i)].key, i);
	}

	Detail::HashTable<test_hashtable_object>* phm3 = new Detail::HashTable<test_hashtable_object>(std::move(*pht));

	ASSERT_EQ(phm3->GetSize(), test_size);
	ASSERT_EQ(phm3->GetBucketQuantity(), phm3->GetCorrectBucketQuantity(phm3->GetLoadFactor(), phm3->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm3)[test_hashtable_object(i)].key, i);
	}

	*pht = std::move(*phm2);

	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	for (int i = 2 * test_size - 1; i >= test_size; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}

	*phm2 = std::move(*phm3);

	ASSERT_EQ(phm2->GetSize(), test_size);
	ASSERT_EQ(phm2->GetBucketQuantity(), phm2->GetCorrectBucketQuantity(phm2->GetLoadFactor(), phm2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm2)[test_hashtable_object(i)].key, i);
	}

	delete pht;
	delete phm2;
	delete phm3;
	for (int i = 0; i < 2 * test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashTable, ConstructByEmptyListTest)
{
	Detail::HashTable<int> ht({});
	ASSERT_EQ(ht.GetSize(), 0);

	ht.Insert(2);
	ASSERT_EQ(ht.GetSize(), 1);
	ASSERT_EQ(*ht.GetBegin(), 2);
}

TEST(HashTable, InsertEmptyListTest)
{
	Detail::HashTable<int> ht({2});
	ASSERT_EQ(ht.GetSize(), 1);
	ASSERT_EQ(*ht.GetBegin(), 2);

	ht.Insert(std::initializer_list<int>{});
	ASSERT_EQ(ht.GetSize(), 1);
	ASSERT_EQ(*ht.GetBegin(), 2);
}

TEST(HashTableIterator, OutOfRangeErrorTest)
{
	Detail::HashTable<int> ht1({1, 2});
	ASSERT_TRUE((Detail::HashTable<int>::Iterator::OutOfRangeError::Judge(ht1.GetEnd())));
	ASSERT_TRUE((Detail::HashTable<int>::ConstIterator::OutOfRangeError::Judge(ht1.GetConstEnd())));

	ASSERT_FALSE((Detail::HashTable<int>::Iterator::OutOfRangeError::Judge(ht1.GetBegin())));
	ASSERT_FALSE((Detail::HashTable<int>::ConstIterator::OutOfRangeError::Judge(ht1.GetConstBegin())));

	ASSERT_TRUE((Detail::HashTable<int>::Iterator::OutOfRangeError::Judge(ht1.GetBegin() + 2)));
	ASSERT_TRUE((Detail::HashTable<int>::ConstIterator::OutOfRangeError::Judge(ht1.GetConstBegin() + 2)));
}

TEST(HashTableIterator, GetTest)
{
	Detail::HashTable<test_hashtable_object>* pht = new Detail::HashTable<test_hashtable_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	int val_check_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	memset(val_check_pool, 0, sizeof(val_check_pool));
	auto val_rel_func = [&](test_hashtable_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pht->Insert(test_hashtable_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}

	for (Detail::HashTable<test_hashtable_object>::Iterator i = pht->GetBegin(); i != pht->GetEnd(); ++i)
	{
		val_check_pool[i->key] += 1;
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_check_pool[i], 1);
	}

	delete pht;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashTableIterator, GetConstTest)
{
	Detail::HashTable<test_hashtable_object>* pht = new Detail::HashTable<test_hashtable_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	int val_check_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	memset(val_check_pool, 0, sizeof(val_check_pool));
	auto val_rel_func = [&](test_hashtable_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pht->Insert(test_hashtable_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}

	const Detail::HashTable<test_hashtable_object>* pchm = pht;

	for (Detail::HashTable<test_hashtable_object>::ConstIterator i = pchm->GetConstBegin(); i != pchm->GetConstEnd(); ++i)
	{
		val_check_pool[i->key] += 1;
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_check_pool[i], 1);
	}

	delete pht;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashTableIterator, CopyConstructionTest)
{
	Detail::HashTable<int> ht1(
		{0, 1, 2, 3, 4, 5, 6, 7, 8, 9});

	ASSERT_EQ(ht1.GetSize(), 10);

	Detail::HashTable<int>::Iterator iter1 = ht1.GetBegin();
	ASSERT_GE(*iter1, 0);
	ASSERT_LE(*iter1, 9);
	Detail::HashTable<int>::Iterator iter2(iter1);
	ASSERT_EQ(*iter1, *iter2);

	Detail::HashTable<int>::ConstIterator citer1 = ht1.GetConstBegin();
	ASSERT_GE(*citer1, 0);
	ASSERT_LE(*citer1, 9);
	Detail::HashTable<int>::ConstIterator citer2(citer1);
	ASSERT_EQ(*citer1, *citer2);

	Detail::HashTable<int>::ConstIterator citer3(iter1);
	ASSERT_EQ(*iter1, *citer3);
}

TEST(HashTableIterator, CopyAssignmentTest)
{
	Detail::HashTable<int> ht1(
		{0, 1, 2, 3, 4, 5, 6, 7, 8, 9});

	ASSERT_EQ(ht1.GetSize(), 10);

	Detail::HashTable<int>::Iterator iter1 = ht1.GetBegin();
	ASSERT_GE(*iter1, 0);
	ASSERT_LE(*iter1, 9);
	Detail::HashTable<int>::Iterator iter2 = ht1.GetEnd();
	ASSERT_EQ(iter2, ht1.GetEnd());
	iter2 = iter1;
	ASSERT_EQ(*iter1, *iter2);

	Detail::HashTable<int>::ConstIterator citer1 = ht1.GetConstBegin();
	ASSERT_GE(*citer1, 0);
	ASSERT_LE(*citer1, 9);
	Detail::HashTable<int>::ConstIterator citer2 = ht1.GetConstEnd();
	ASSERT_EQ(citer2, ht1.GetConstEnd());
	citer2 = citer1;
	ASSERT_EQ(*citer1, *citer2);

	Detail::HashTable<int>::ConstIterator citer3 = ht1.GetConstEnd();
	ASSERT_EQ(citer3, ht1.GetConstEnd());
	citer3 = iter1;
	ASSERT_EQ(*iter1, *citer3);
}

TEST(HashTableIterator, SelfIncrementTest)
{
	Detail::HashTable<test_hashtable_object>* pht = new Detail::HashTable<test_hashtable_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	int val_check_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	memset(val_check_pool, 0, sizeof(val_check_pool));
	auto val_rel_func = [&](test_hashtable_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pht->Insert(test_hashtable_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}

	for (Detail::HashTable<test_hashtable_object>::Iterator i = pht->GetBegin(); i != pht->GetEnd(); ++i)
	{
		val_check_pool[i->key] += 1;
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_check_pool[i], 1);
	}

	for (Detail::HashTable<test_hashtable_object>::Iterator i = pht->GetBegin(); i != pht->GetEnd(); i++)
	{
		val_check_pool[i->key] += 1;
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_check_pool[i], 2);
	}

	const Detail::HashTable<test_hashtable_object>* pchm = pht;

	for (Detail::HashTable<test_hashtable_object>::ConstIterator i = pchm->GetConstBegin(); i != pchm->GetConstEnd(); ++i)
	{
		val_check_pool[i->key] += 1;
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_check_pool[i], 3);
	}

	for (Detail::HashTable<test_hashtable_object>::ConstIterator i = pchm->GetConstBegin(); i != pchm->GetConstEnd(); i++)
	{
		val_check_pool[i->key] += 1;
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_check_pool[i], 4);
	}

	delete pht;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashTableIterator, PlusTest)
{
	Detail::HashTable<test_hashtable_object>* pht = new Detail::HashTable<test_hashtable_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashtable_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pht->Insert(test_hashtable_object(i, i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}

	int cnt = 0;
	for (Detail::HashTable<test_hashtable_object>::Iterator i = pht->GetBegin(); i != pht->GetEnd(); i = i + 2)
	{
		ASSERT_EQ(i->key, i->value);
		cnt += 1;
	}
	ASSERT_EQ(cnt, test_size / 2);

	cnt = 0;
	for (Detail::HashTable<test_hashtable_object>::Iterator i = pht->GetBegin(); i != pht->GetEnd(); i += 2)
	{
		ASSERT_EQ(i->key, i->value);
		cnt += 1;
	}
	ASSERT_EQ(cnt, test_size / 2);

	const Detail::HashTable<test_hashtable_object>* pchm = pht;

	cnt = 0;
	for (Detail::HashTable<test_hashtable_object>::ConstIterator i = pchm->GetConstBegin(); i != pchm->GetConstEnd(); i = i + 2)
	{
		ASSERT_EQ(i->key, i->value);
		cnt += 1;
	}
	ASSERT_EQ(cnt, test_size / 2);

	cnt = 0;
	for (Detail::HashTable<test_hashtable_object>::ConstIterator i = pchm->GetConstBegin(); i != pchm->GetConstEnd(); i += 2)
	{
		ASSERT_EQ(i->key, i->value);
		cnt += 1;
	}
	ASSERT_EQ(cnt, test_size / 2);

	delete pht;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashTableIterator, DistanceTest)
{
	Detail::HashTable<test_hashtable_object>* pht = new Detail::HashTable<test_hashtable_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_hashtable_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pht->Insert(test_hashtable_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}

	ASSERT_EQ(pht->GetEnd() - pht->GetBegin(), test_size);

	const Detail::HashTable<test_hashtable_object>* pchm = pht;

	ASSERT_EQ(pchm->GetConstEnd() - pchm->GetConstBegin(), test_size);

	delete pht;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashTableIterator, OperatorPointerTest)
{
	Detail::HashTable<test_hashtable_object>* pht = new Detail::HashTable<test_hashtable_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	int val_check_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	memset(val_check_pool, 0, sizeof(val_check_pool));
	auto val_rel_func = [&](test_hashtable_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pht->Insert(test_hashtable_object(i, i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}

	for (Detail::HashTable<test_hashtable_object>::Iterator i = pht->GetBegin(); i != pht->GetEnd(); ++i)
	{
		val_check_pool[i->key] += 1;
		ASSERT_EQ(i->key, i->value);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_check_pool[i], 1);
	}

	const Detail::HashTable<test_hashtable_object>* pchm = pht;

	for (Detail::HashTable<test_hashtable_object>::ConstIterator i = pchm->GetConstBegin(); i != pchm->GetConstEnd(); ++i)
	{
		val_check_pool[i->key] += 1;
		ASSERT_EQ(i->key, i->value);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_check_pool[i], 2);
	}

	delete pht;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashTableIterator, OperatorReferenceTest)
{
	Detail::HashTable<test_hashtable_object>* pht = new Detail::HashTable<test_hashtable_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	int val_check_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	memset(val_check_pool, 0, sizeof(val_check_pool));
	auto val_rel_func = [&](test_hashtable_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pht->Insert(test_hashtable_object(i, i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}

	for (Detail::HashTable<test_hashtable_object>::Iterator i = pht->GetBegin(); i != pht->GetEnd(); ++i)
	{
		val_check_pool[(*i).key] += 1;
		ASSERT_EQ((*i).key, (*i).value);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_check_pool[i], 1);
	}

	const Detail::HashTable<test_hashtable_object>* pchm = pht;

	for (Detail::HashTable<test_hashtable_object>::ConstIterator i = pchm->GetConstBegin(); i != pchm->GetConstEnd(); ++i)
	{
		val_check_pool[(*i).key] += 1;
		ASSERT_EQ((*i).key, (*i).value);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_check_pool[i], 2);
	}

	delete pht;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashTableIterator, EqualityTest)
{
	Detail::HashTable<int> ht1(
		{0, 1, 2, 3, 4, 5, 6, 7, 8, 9});

	ASSERT_EQ(ht1.GetSize(), 10);

	Detail::HashTable<int>::Iterator iter1 = ht1.GetBegin();
	iter1 += 1;
	ASSERT_GE(*iter1, 0);
	ASSERT_LE(*iter1, 9);
	Detail::HashTable<int>::Iterator iter2 = ht1.GetBegin() + 1;
	ASSERT_GE(*iter2, 0);
	ASSERT_LE(*iter2, 9);

	ASSERT_EQ(iter1, iter2);
	ASSERT_TRUE(iter1 != ht1.GetBegin());
	ASSERT_TRUE(iter2 != ht1.GetEnd());

	Detail::HashTable<int>::ConstIterator citer1 = ht1.GetConstBegin();
	citer1 += 1;
	ASSERT_GE(*citer1, 0);
	ASSERT_LE(*citer1, 9);
	Detail::HashTable<int>::ConstIterator citer2 = ht1.GetConstBegin() + 1;
	ASSERT_GE(*citer2, 0);
	ASSERT_LE(*citer2, 9);

	ASSERT_EQ(citer1, citer2);
	ASSERT_TRUE(citer1 != ht1.GetConstBegin());
	ASSERT_TRUE(citer2 != ht1.GetConstEnd());
}

TEST(HashTableIterator, GetDataTest)
{
	Detail::HashTable<test_hashtable_object>* pht = new Detail::HashTable<test_hashtable_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	int val_check_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	memset(val_check_pool, 0, sizeof(val_check_pool));
	auto val_rel_func = [&](test_hashtable_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = pht->Insert(test_hashtable_object(i, i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(pht->GetSize(), test_size);
	ASSERT_EQ(pht->GetBucketQuantity(), pht->GetCorrectBucketQuantity(pht->GetLoadFactor(), pht->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pht)[test_hashtable_object(i)].key, i);
	}

	for (Detail::HashTable<test_hashtable_object>::Iterator i = pht->GetBegin(); i != pht->GetEnd(); ++i)
	{
		val_check_pool[i.GetData()->key] += 1;
		ASSERT_EQ(i.GetData()->key, i.GetData()->value);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_check_pool[i], 1);
	}

	const Detail::HashTable<test_hashtable_object>* pchm = pht;

	for (Detail::HashTable<test_hashtable_object>::ConstIterator i = pchm->GetConstBegin(); i != pchm->GetConstEnd(); ++i)
	{
		val_check_pool[i.GetData()->key] += 1;
		ASSERT_EQ(i.GetData()->key, i.GetData()->value);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_check_pool[i], 2);
	}

	delete pht;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}