/*
Copyright 2022 creatorlxd

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

TEST(HashMap, InstanceTest)
{
	HashMap<int, int> hm1;
	ASSERT_EQ(hm1.GetSize(), 0);
	ASSERT_EQ(hm1.GetBucketQuantity(), hm1.sm_DefaultBucketQuantity);
}

TEST(HashMap, ListConstructionTest)
{
	const int test_size = 10;
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

	HashMap<test_hashmap_object, test_hashmap_object>* phm = new HashMap<test_hashmap_object, test_hashmap_object>(
		{Pair<const test_hashmap_object, test_hashmap_object>(test_hashmap_object(0, key_rel_func), test_hashmap_object(0, val_rel_func)),
		 Pair<const test_hashmap_object, test_hashmap_object>(test_hashmap_object(1, key_rel_func), test_hashmap_object(1, val_rel_func)),
		 Pair<const test_hashmap_object, test_hashmap_object>(test_hashmap_object(2, key_rel_func), test_hashmap_object(2, val_rel_func)),
		 Pair<const test_hashmap_object, test_hashmap_object>(test_hashmap_object(3, key_rel_func), test_hashmap_object(3, val_rel_func)),
		 Pair<const test_hashmap_object, test_hashmap_object>(test_hashmap_object(4, key_rel_func), test_hashmap_object(4, val_rel_func)),
		 Pair<const test_hashmap_object, test_hashmap_object>(test_hashmap_object(5, key_rel_func), test_hashmap_object(5, val_rel_func)),
		 Pair<const test_hashmap_object, test_hashmap_object>(test_hashmap_object(6, key_rel_func), test_hashmap_object(6, val_rel_func)),
		 Pair<const test_hashmap_object, test_hashmap_object>(test_hashmap_object(7, key_rel_func), test_hashmap_object(7, val_rel_func)),
		 Pair<const test_hashmap_object, test_hashmap_object>(test_hashmap_object(8, key_rel_func), test_hashmap_object(8, val_rel_func)),
		 Pair<const test_hashmap_object, test_hashmap_object>(test_hashmap_object(9, key_rel_func), test_hashmap_object(9, val_rel_func))});

	ASSERT_EQ(phm->GetSize(), test_size);
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}
	delete phm;
	for (int i = 0; i < test_size; i++)
	{
		//initializer_list can only return const variable, so move is useless
		ASSERT_EQ(key_pool[i], 2);
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(HashMap, GetLoadFactorTest)
{
	HashMap<int, int> hm1;
	ASSERT_EQ(hm1.GetLoadFactor(), hm1.sm_DefaultLoadFactor);
}

TEST(HashMap, SetLoadFactorTest)
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
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));

	phm->SetLoadFactor(0.75);

	ASSERT_EQ(phm->GetSize(), test_size);
	ASSERT_EQ(phm->GetLoadFactor(), 0.75);
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}
	delete phm;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 1);
		ASSERT_EQ(val_pool[i], 1);
	}
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
	HashMap<int, int> hm1;
	ASSERT_EQ(hm1.GetSize(), 0);
	hm1.Insert(1, 1);
	ASSERT_EQ(hm1.GetSize(), 1);
	hm1.Insert(1, 2);
	ASSERT_EQ(hm1.GetSize(), 1);
	hm1.Insert(2, 2);
	hm1.Insert(3, 3);
	ASSERT_EQ(hm1.GetSize(), 3);

	const HashMap<int, int> chm1({Pair<const int, int>(1, 1),
								  Pair<const int, int>(2, 2),
								  Pair<const int, int>(3, 3),
								  Pair<const int, int>(4, 4)});
	ASSERT_EQ(chm1.GetSize(), 4);
}

TEST(HashMap, GetBucketQuantityTest)
{
	ASSERT_EQ((SpaceGameEngine::HashMap<int, int>::sm_DefaultLoadFactor), 1.0f);
	ASSERT_EQ((SpaceGameEngine::HashMap<int, int>::sm_DefaultBucketQuantity), 16);

	HashMap<int, int> hm1;
	ASSERT_EQ(hm1.GetBucketQuantity(), (SpaceGameEngine::HashMap<int, int>::sm_DefaultBucketQuantity));
	hm1.Insert(1, 1);
	ASSERT_EQ(hm1.GetBucketQuantity(), (SpaceGameEngine::HashMap<int, int>::sm_DefaultBucketQuantity));
	hm1.Insert(1, 2);
	ASSERT_EQ(hm1.GetBucketQuantity(), (SpaceGameEngine::HashMap<int, int>::sm_DefaultBucketQuantity));
	hm1.Insert(2, 2);
	hm1.Insert(3, 3);
	ASSERT_EQ(hm1.GetBucketQuantity(), (SpaceGameEngine::HashMap<int, int>::sm_DefaultBucketQuantity));

	const HashMap<int, int> chm1({Pair<const int, int>(1, 1),
								  Pair<const int, int>(2, 2),
								  Pair<const int, int>(3, 3),
								  Pair<const int, int>(4, 4)});
	ASSERT_EQ(chm1.GetBucketQuantity(), 4);
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
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}
	delete phm;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 1);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashMap, InsertListTest)
{
	HashMap<test_hashmap_object, test_hashmap_object>* phm = new HashMap<test_hashmap_object, test_hashmap_object>();
	const int test_size = 10;
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

	//test repeat insert
	phm->Insert(test_hashmap_object(0, key_rel_func), test_hashmap_object(-1));
	ASSERT_EQ(phm->GetSize(), 1);
	ASSERT_EQ((*phm)[test_hashmap_object(0)].val, -1);

	//have 11 elements, test repeat insert
	phm->Insert({Pair<const test_hashmap_object, test_hashmap_object>(test_hashmap_object(0, key_rel_func), test_hashmap_object(0, val_rel_func)),
				 Pair<const test_hashmap_object, test_hashmap_object>(test_hashmap_object(1, key_rel_func), test_hashmap_object(0)),
				 Pair<const test_hashmap_object, test_hashmap_object>(test_hashmap_object(1), test_hashmap_object(1, val_rel_func)),
				 Pair<const test_hashmap_object, test_hashmap_object>(test_hashmap_object(2, key_rel_func), test_hashmap_object(2, val_rel_func)),
				 Pair<const test_hashmap_object, test_hashmap_object>(test_hashmap_object(3, key_rel_func), test_hashmap_object(3, val_rel_func)),
				 Pair<const test_hashmap_object, test_hashmap_object>(test_hashmap_object(4, key_rel_func), test_hashmap_object(4, val_rel_func)),
				 Pair<const test_hashmap_object, test_hashmap_object>(test_hashmap_object(5, key_rel_func), test_hashmap_object(5, val_rel_func)),
				 Pair<const test_hashmap_object, test_hashmap_object>(test_hashmap_object(6, key_rel_func), test_hashmap_object(6, val_rel_func)),
				 Pair<const test_hashmap_object, test_hashmap_object>(test_hashmap_object(7, key_rel_func), test_hashmap_object(7, val_rel_func)),
				 Pair<const test_hashmap_object, test_hashmap_object>(test_hashmap_object(8, key_rel_func), test_hashmap_object(8, val_rel_func)),
				 Pair<const test_hashmap_object, test_hashmap_object>(test_hashmap_object(9, key_rel_func), test_hashmap_object(9, val_rel_func))});

	ASSERT_EQ(phm->GetSize(), test_size);
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}
	delete phm;
	for (int i = 0; i < test_size; i++)
	{
		//initializer_list can only return const variable, so move is useless
		ASSERT_EQ(key_pool[i], 2);
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(HashMap, RemoveTest)
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
	SizeType now_bucket_quantity = phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize());
	ASSERT_EQ(phm->GetBucketQuantity(), now_bucket_quantity);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}

	int rm_cnt = 0;
	auto iter = phm->GetBegin();
	while (iter != phm->GetEnd())
	{
		iter = phm->Remove(iter);
		rm_cnt += 1;
	}
	ASSERT_EQ(rm_cnt, test_size);
	ASSERT_EQ(phm->GetSize(), 0);
	ASSERT_EQ(phm->GetBucketQuantity(), now_bucket_quantity);
	delete phm;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 1);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashMap, RemoveByKeyTest)
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
	SizeType now_bucket_quantity = phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize());
	ASSERT_EQ(phm->GetBucketQuantity(), now_bucket_quantity);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_TRUE(phm->RemoveByKey(test_hashmap_object(i)));
	}

	ASSERT_EQ(phm->GetSize(), 0);
	ASSERT_EQ(phm->GetBucketQuantity(), now_bucket_quantity);
	delete phm;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 1);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashMap, ShrinkToFitTest)
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
	SizeType now_bucket_quantity = phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize());
	ASSERT_EQ(phm->GetBucketQuantity(), now_bucket_quantity);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_TRUE(phm->RemoveByKey(test_hashmap_object(i)));
	}

	ASSERT_EQ(phm->GetSize(), 0);
	ASSERT_EQ(phm->GetBucketQuantity(), now_bucket_quantity);

	phm->ShrinkToFit();

	ASSERT_EQ(phm->GetSize(), 0);
	ASSERT_TRUE(phm->GetBucketQuantity() != now_bucket_quantity);
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));

	delete phm;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 1);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashMap, FindTest)
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
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));

	for (int i = test_size - 1; i >= 0; i--)
	{
		auto iter = phm->Find(test_hashmap_object(i));
		ASSERT_EQ(iter->m_First.val, i);
		ASSERT_EQ(iter->m_Second.val, i);
	}

	auto niter = phm->Find(test_hashmap_object(test_size));
	ASSERT_EQ(niter, phm->GetEnd());

	ASSERT_EQ(phm->GetSize(), test_size);
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));

	const HashMap<test_hashmap_object, test_hashmap_object>* pchm = phm;

	for (int i = test_size - 1; i >= 0; i--)
	{
		auto iter = pchm->Find(test_hashmap_object(i));
		ASSERT_EQ(iter->m_First.val, i);
		ASSERT_EQ(iter->m_Second.val, i);
	}

	auto cniter = pchm->Find(test_hashmap_object(test_size));
	ASSERT_EQ(cniter, pchm->GetConstEnd());

	ASSERT_EQ(pchm->GetSize(), test_size);
	ASSERT_EQ(pchm->GetBucketQuantity(), pchm->GetCorrectBucketQuantity(pchm->GetLoadFactor(), pchm->GetSize()));

	delete phm;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 1);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashMap, GetTest)
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
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(phm->Get(test_hashmap_object(i)).val, i);
	}

	ASSERT_EQ(phm->GetSize(), test_size);
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));

	const HashMap<test_hashmap_object, test_hashmap_object>* pchm = phm;

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pchm->Get(test_hashmap_object(i)).val, i);
	}

	ASSERT_EQ(pchm->GetSize(), test_size);
	ASSERT_EQ(pchm->GetBucketQuantity(), pchm->GetCorrectBucketQuantity(pchm->GetLoadFactor(), pchm->GetSize()));

	delete phm;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 1);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashMap, OperatorTest)
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
		(*phm)[test_hashmap_object(i, key_rel_func)] = test_hashmap_object(i, val_rel_func);
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
		(*phm)[test_hashmap_object(i)].val += 1;
	}
	ASSERT_EQ(phm->GetSize(), test_size);
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i + 1);
		(*phm)[test_hashmap_object(i)].val -= 1;
	}
	ASSERT_EQ(phm->GetSize(), test_size);
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));
	delete phm;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 1);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashMap, RehashTest)
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
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));
	phm->Rehash(1);
	ASSERT_EQ(phm->GetSize(), test_size);
	ASSERT_EQ(phm->GetBucketQuantity(), 1);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}
	delete phm;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 1);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashMap, CopyConstructionTest)
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
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}

	HashMap<test_hashmap_object, test_hashmap_object>* phm2 = new HashMap<test_hashmap_object, test_hashmap_object>(*phm);

	ASSERT_EQ(phm->GetSize(), test_size);
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}

	ASSERT_EQ(phm2->GetSize(), test_size);
	ASSERT_EQ(phm2->GetBucketQuantity(), phm2->GetCorrectBucketQuantity(phm2->GetLoadFactor(), phm2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm2)[test_hashmap_object(i)].val, i);
	}

	delete phm;
	delete phm2;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 2);
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(HashMap, MoveConstructionTest)
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
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}

	HashMap<test_hashmap_object, test_hashmap_object>* phm2 = new HashMap<test_hashmap_object, test_hashmap_object>(std::move(*phm));

	ASSERT_EQ(phm2->GetSize(), test_size);
	ASSERT_EQ(phm2->GetBucketQuantity(), phm2->GetCorrectBucketQuantity(phm2->GetLoadFactor(), phm2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm2)[test_hashmap_object(i)].val, i);
	}

	delete phm;
	delete phm2;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 1);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashMap, CopyAssignmentTest)
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
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}

	HashMap<test_hashmap_object, test_hashmap_object>* phm2 = new HashMap<test_hashmap_object, test_hashmap_object>();

	ASSERT_EQ(phm2->GetSize(), 0);

	*phm2 = *phm;

	ASSERT_EQ(phm->GetSize(), test_size);
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}

	ASSERT_EQ(phm2->GetSize(), test_size);
	ASSERT_EQ(phm2->GetBucketQuantity(), phm2->GetCorrectBucketQuantity(phm2->GetLoadFactor(), phm2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm2)[test_hashmap_object(i)].val, i);
	}

	delete phm;
	delete phm2;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 2);
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(HashMap, MoveAssignmentTest)
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
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}

	HashMap<test_hashmap_object, test_hashmap_object>* phm2 = new HashMap<test_hashmap_object, test_hashmap_object>();

	ASSERT_EQ(phm2->GetSize(), 0);

	*phm2 = std::move(*phm);

	ASSERT_EQ(phm2->GetSize(), test_size);
	ASSERT_EQ(phm2->GetBucketQuantity(), phm2->GetCorrectBucketQuantity(phm2->GetLoadFactor(), phm2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm2)[test_hashmap_object(i)].val, i);
	}

	delete phm;
	delete phm2;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 1);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashMap, AnotherAllocatorCopyConstructionTest)
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
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}

	HashMap<test_hashmap_object, test_hashmap_object, Hash<test_hashmap_object>, StdAllocator>* phm2 = new HashMap<test_hashmap_object, test_hashmap_object, Hash<test_hashmap_object>, StdAllocator>(*phm);

	ASSERT_EQ(phm->GetSize(), test_size);
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}

	ASSERT_EQ(phm2->GetSize(), test_size);
	ASSERT_EQ(phm2->GetBucketQuantity(), phm2->GetCorrectBucketQuantity(phm2->GetLoadFactor(), phm2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm2)[test_hashmap_object(i)].val, i);
	}

	HashMap<test_hashmap_object, test_hashmap_object>* phm3 = new HashMap<test_hashmap_object, test_hashmap_object>(*phm2);

	ASSERT_EQ(phm->GetSize(), test_size);
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}

	ASSERT_EQ(phm2->GetSize(), test_size);
	ASSERT_EQ(phm2->GetBucketQuantity(), phm2->GetCorrectBucketQuantity(phm2->GetLoadFactor(), phm2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm2)[test_hashmap_object(i)].val, i);
	}

	ASSERT_EQ(phm3->GetSize(), test_size);
	ASSERT_EQ(phm3->GetBucketQuantity(), phm3->GetCorrectBucketQuantity(phm3->GetLoadFactor(), phm3->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm3)[test_hashmap_object(i)].val, i);
	}

	delete phm;
	delete phm2;
	delete phm3;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 3);
		ASSERT_EQ(val_pool[i], 3);
	}
}

TEST(HashMap, AnotherAllocatorMoveConstructionTest)
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
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}

	HashMap<test_hashmap_object, test_hashmap_object, Hash<test_hashmap_object>, StdAllocator>* phm2 = new HashMap<test_hashmap_object, test_hashmap_object, Hash<test_hashmap_object>, StdAllocator>(std::move(*phm));

	ASSERT_EQ(phm2->GetSize(), test_size);
	ASSERT_EQ(phm2->GetBucketQuantity(), phm2->GetCorrectBucketQuantity(phm2->GetLoadFactor(), phm2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm2)[test_hashmap_object(i)].val, i);
	}

	HashMap<test_hashmap_object, test_hashmap_object>* phm3 = new HashMap<test_hashmap_object, test_hashmap_object>(std::move(*phm2));

	ASSERT_EQ(phm3->GetSize(), test_size);
	ASSERT_EQ(phm3->GetBucketQuantity(), phm3->GetCorrectBucketQuantity(phm3->GetLoadFactor(), phm3->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm3)[test_hashmap_object(i)].val, i);
	}

	delete phm;
	delete phm2;
	delete phm3;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 3);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashMap, AnotherAllocatorCopyAssignmentTest)
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
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}

	HashMap<test_hashmap_object, test_hashmap_object, Hash<test_hashmap_object>, StdAllocator>* phm2 = new HashMap<test_hashmap_object, test_hashmap_object, Hash<test_hashmap_object>, StdAllocator>();

	ASSERT_EQ(phm2->GetSize(), 0);

	*phm2 = *phm;

	ASSERT_EQ(phm->GetSize(), test_size);
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}

	ASSERT_EQ(phm2->GetSize(), test_size);
	ASSERT_EQ(phm2->GetBucketQuantity(), phm2->GetCorrectBucketQuantity(phm2->GetLoadFactor(), phm2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm2)[test_hashmap_object(i)].val, i);
	}

	HashMap<test_hashmap_object, test_hashmap_object>* phm3 = new HashMap<test_hashmap_object, test_hashmap_object>();

	ASSERT_EQ(phm3->GetSize(), 0);

	*phm3 = *phm2;

	ASSERT_EQ(phm->GetSize(), test_size);
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}

	ASSERT_EQ(phm2->GetSize(), test_size);
	ASSERT_EQ(phm2->GetBucketQuantity(), phm2->GetCorrectBucketQuantity(phm2->GetLoadFactor(), phm2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm2)[test_hashmap_object(i)].val, i);
	}

	ASSERT_EQ(phm3->GetSize(), test_size);
	ASSERT_EQ(phm3->GetBucketQuantity(), phm3->GetCorrectBucketQuantity(phm3->GetLoadFactor(), phm3->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm3)[test_hashmap_object(i)].val, i);
	}

	delete phm;
	delete phm2;
	delete phm3;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 3);
		ASSERT_EQ(val_pool[i], 3);
	}
}

TEST(HashMap, AnotherAllocatorMoveAssignmentTest)
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
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}

	HashMap<test_hashmap_object, test_hashmap_object, Hash<test_hashmap_object>, StdAllocator>* phm2 = new HashMap<test_hashmap_object, test_hashmap_object, Hash<test_hashmap_object>, StdAllocator>();

	ASSERT_EQ(phm2->GetSize(), 0);

	*phm2 = std::move(*phm);

	ASSERT_EQ(phm2->GetSize(), test_size);
	ASSERT_EQ(phm2->GetBucketQuantity(), phm2->GetCorrectBucketQuantity(phm2->GetLoadFactor(), phm2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm2)[test_hashmap_object(i)].val, i);
	}

	HashMap<test_hashmap_object, test_hashmap_object>* phm3 = new HashMap<test_hashmap_object, test_hashmap_object>();

	ASSERT_EQ(phm3->GetSize(), 0);

	*phm3 = std::move(*phm2);

	ASSERT_EQ(phm3->GetSize(), test_size);
	ASSERT_EQ(phm3->GetBucketQuantity(), phm3->GetCorrectBucketQuantity(phm3->GetLoadFactor(), phm3->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm3)[test_hashmap_object(i)].val, i);
	}

	delete phm;
	delete phm2;
	delete phm3;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 3);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashMap, EqualTest)
{
	const HashMap<int, int, Hash<int>, MemoryManagerAllocator> hm1({{1, 10},
																	{2, 20},
																	{3, 30}});
	const HashMap<int, int, Hash<int>, MemoryManagerAllocator> hm2({{1, 10},
																	{2, 20}});
	const HashMap<int, int, Hash<int>, StdAllocator> hm2_({{1, 10},
														   {2, 20}});
	const HashMap<int, int, Hash<int>, MemoryManagerAllocator> hm3({{1, 10},
																	{2, 21},
																	{3, 30}});
	const HashMap<int, int, Hash<int>, StdAllocator> hm3_({{1, 10},
														   {2, 21},
														   {3, 30}});
	const HashMap<int, int, Hash<int>, MemoryManagerAllocator> hm4({{1, 10},
																	{2, 20},
																	{3, 30}});
	const HashMap<int, int, Hash<int>, StdAllocator> hm4_({{1, 10},
														   {2, 20},
														   {3, 30}});

	ASSERT_FALSE(hm1 == hm2);
	ASSERT_FALSE(hm1 == hm2_);
	ASSERT_FALSE(hm1 == hm3);
	ASSERT_FALSE(hm1 == hm3_);
	ASSERT_TRUE(hm1 == hm4);
	ASSERT_TRUE(hm1 == hm4_);
}

TEST(HashMap, NotEqualTest)
{
	const HashMap<int, int, Hash<int>, MemoryManagerAllocator> hm1({{1, 10},
																	{2, 20},
																	{3, 30}});
	const HashMap<int, int, Hash<int>, MemoryManagerAllocator> hm2({{1, 10},
																	{2, 20}});
	const HashMap<int, int, Hash<int>, StdAllocator> hm2_({{1, 10},
														   {2, 20}});
	const HashMap<int, int, Hash<int>, MemoryManagerAllocator> hm3({{1, 10},
																	{2, 21},
																	{3, 30}});
	const HashMap<int, int, Hash<int>, StdAllocator> hm3_({{1, 10},
														   {2, 21},
														   {3, 30}});
	const HashMap<int, int, Hash<int>, MemoryManagerAllocator> hm4({{1, 10},
																	{2, 20},
																	{3, 30}});
	const HashMap<int, int, Hash<int>, StdAllocator> hm4_({{1, 10},
														   {2, 20},
														   {3, 30}});

	ASSERT_TRUE(hm1 != hm2);
	ASSERT_TRUE(hm1 != hm2_);
	ASSERT_TRUE(hm1 != hm3);
	ASSERT_TRUE(hm1 != hm3_);
	ASSERT_FALSE(hm1 != hm4);
	ASSERT_FALSE(hm1 != hm4_);
}

TEST(HashMap, SwapTest)
{
	HashMap<test_hashmap_object, test_hashmap_object>* phm = new HashMap<test_hashmap_object, test_hashmap_object>();
	const int test_size = 1000;
	int key_pool[2 * test_size];
	int val_pool[2 * test_size];
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
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}

	HashMap<test_hashmap_object, test_hashmap_object>* phm2 = new HashMap<test_hashmap_object, test_hashmap_object>();

	for (int i = test_size; i < 2 * test_size; ++i)
	{
		auto iter = phm2->Insert(test_hashmap_object(i, key_rel_func), test_hashmap_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->m_First.val, i);
		ASSERT_EQ(iter.m_First->m_Second.val, i);
		ASSERT_TRUE(iter.m_Second);
	}

	ASSERT_EQ(phm2->GetSize(), test_size);
	ASSERT_EQ(phm2->GetBucketQuantity(), phm2->GetCorrectBucketQuantity(phm2->GetLoadFactor(), phm2->GetSize()));
	for (int i = 2 * test_size - 1; i >= test_size; i--)
	{
		ASSERT_EQ((*phm2)[test_hashmap_object(i)].val, i);
	}

	HashMap<test_hashmap_object, test_hashmap_object>* phm3 = new HashMap<test_hashmap_object, test_hashmap_object>(std::move(*phm));

	ASSERT_EQ(phm3->GetSize(), test_size);
	ASSERT_EQ(phm3->GetBucketQuantity(), phm3->GetCorrectBucketQuantity(phm3->GetLoadFactor(), phm3->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm3)[test_hashmap_object(i)].val, i);
	}

	*phm = std::move(*phm2);

	ASSERT_EQ(phm->GetSize(), test_size);
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));
	for (int i = 2 * test_size - 1; i >= test_size; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}

	*phm2 = std::move(*phm3);

	ASSERT_EQ(phm2->GetSize(), test_size);
	ASSERT_EQ(phm2->GetBucketQuantity(), phm2->GetCorrectBucketQuantity(phm2->GetLoadFactor(), phm2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm2)[test_hashmap_object(i)].val, i);
	}

	delete phm;
	delete phm2;
	delete phm3;
	for (int i = 0; i < 2 * test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 1);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashMap, AnotherAllocatorSwapTest)
{
	HashMap<test_hashmap_object, test_hashmap_object>* phm = new HashMap<test_hashmap_object, test_hashmap_object>();
	const int test_size = 1000;
	int key_pool[2 * test_size];
	int val_pool[2 * test_size];
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
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}

	HashMap<test_hashmap_object, test_hashmap_object, Hash<test_hashmap_object>, StdAllocator>* phm2 = new HashMap<test_hashmap_object, test_hashmap_object, Hash<test_hashmap_object>, StdAllocator>();

	for (int i = test_size; i < 2 * test_size; ++i)
	{
		auto iter = phm2->Insert(test_hashmap_object(i, key_rel_func), test_hashmap_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->m_First.val, i);
		ASSERT_EQ(iter.m_First->m_Second.val, i);
		ASSERT_TRUE(iter.m_Second);
	}

	ASSERT_EQ(phm2->GetSize(), test_size);
	ASSERT_EQ(phm2->GetBucketQuantity(), phm2->GetCorrectBucketQuantity(phm2->GetLoadFactor(), phm2->GetSize()));
	for (int i = 2 * test_size - 1; i >= test_size; i--)
	{
		ASSERT_EQ((*phm2)[test_hashmap_object(i)].val, i);
	}

	HashMap<test_hashmap_object, test_hashmap_object>* phm3 = new HashMap<test_hashmap_object, test_hashmap_object>(std::move(*phm));

	ASSERT_EQ(phm3->GetSize(), test_size);
	ASSERT_EQ(phm3->GetBucketQuantity(), phm3->GetCorrectBucketQuantity(phm3->GetLoadFactor(), phm3->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm3)[test_hashmap_object(i)].val, i);
	}

	*phm = std::move(*phm2);

	ASSERT_EQ(phm->GetSize(), test_size);
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));
	for (int i = 2 * test_size - 1; i >= test_size; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}

	*phm2 = std::move(*phm3);

	ASSERT_EQ(phm2->GetSize(), test_size);
	ASSERT_EQ(phm2->GetBucketQuantity(), phm2->GetCorrectBucketQuantity(phm2->GetLoadFactor(), phm2->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm2)[test_hashmap_object(i)].val, i);
	}

	delete phm;
	delete phm2;
	delete phm3;
	for (int i = 0; i < 2 * test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 2);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashMapIterator, OutOfRangeErrorTest)
{
	HashMap<int, int> hm1({{1, 1}, {2, 2}});
	ASSERT_TRUE((HashMap<int, int>::Iterator::OutOfRangeError::Judge(hm1.GetEnd())));
	ASSERT_TRUE((HashMap<int, int>::ConstIterator::OutOfRangeError::Judge(hm1.GetConstEnd())));

	ASSERT_FALSE((HashMap<int, int>::Iterator::OutOfRangeError::Judge(hm1.GetBegin())));
	ASSERT_FALSE((HashMap<int, int>::ConstIterator::OutOfRangeError::Judge(hm1.GetConstBegin())));

	ASSERT_TRUE((HashMap<int, int>::Iterator::OutOfRangeError::Judge(hm1.GetBegin() + 2)));
	ASSERT_TRUE((HashMap<int, int>::ConstIterator::OutOfRangeError::Judge(hm1.GetConstBegin() + 2)));
}

TEST(HashMapIterator, GetTest)
{
	HashMap<test_hashmap_object, test_hashmap_object>* phm = new HashMap<test_hashmap_object, test_hashmap_object>();
	const int test_size = 1000;
	int key_pool[test_size];
	int val_pool[test_size];
	int key_check_pool[test_size];
	int val_check_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	memset(key_check_pool, 0, sizeof(key_check_pool));
	memset(val_check_pool, 0, sizeof(val_check_pool));
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
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}

	for (HashMap<test_hashmap_object, test_hashmap_object>::Iterator i = phm->GetBegin(); i != phm->GetEnd(); ++i)
	{
		key_check_pool[i->m_First.val] += 1;
		val_check_pool[i->m_Second.val] += 1;
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_check_pool[i], 1);
		ASSERT_EQ(val_check_pool[i], 1);
	}

	delete phm;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 1);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashMapIterator, GetConstTest)
{
	HashMap<test_hashmap_object, test_hashmap_object>* phm = new HashMap<test_hashmap_object, test_hashmap_object>();
	const int test_size = 1000;
	int key_pool[test_size];
	int val_pool[test_size];
	int key_check_pool[test_size];
	int val_check_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	memset(key_check_pool, 0, sizeof(key_check_pool));
	memset(val_check_pool, 0, sizeof(val_check_pool));
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
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}

	const HashMap<test_hashmap_object, test_hashmap_object>* pchm = phm;

	for (HashMap<test_hashmap_object, test_hashmap_object>::ConstIterator i = pchm->GetConstBegin(); i != pchm->GetConstEnd(); ++i)
	{
		key_check_pool[i->m_First.val] += 1;
		val_check_pool[i->m_Second.val] += 1;
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_check_pool[i], 1);
		ASSERT_EQ(val_check_pool[i], 1);
	}

	delete phm;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 1);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashMapIterator, CopyConstructionTest)
{
	HashMap<int, int> hm1(
		{{0, 0},
		 {1, 1},
		 {2, 2},
		 {3, 3},
		 {4, 4},
		 {5, 5},
		 {6, 6},
		 {7, 7},
		 {8, 8},
		 {9, 9}});

	ASSERT_EQ(hm1.GetSize(), 10);

	HashMap<int, int>::Iterator iter1 = hm1.GetBegin();
	ASSERT_EQ(iter1->m_First, iter1->m_Second);
	HashMap<int, int>::Iterator iter2(iter1);
	ASSERT_EQ(iter1->m_First, iter2->m_First);
	ASSERT_EQ(iter1->m_Second, iter2->m_Second);

	HashMap<int, int>::ConstIterator citer1 = hm1.GetConstBegin();
	ASSERT_EQ(citer1->m_First, citer1->m_Second);
	HashMap<int, int>::ConstIterator citer2(citer1);
	ASSERT_EQ(citer1->m_First, citer2->m_First);
	ASSERT_EQ(citer1->m_Second, citer2->m_Second);

	HashMap<int, int>::ConstIterator citer3(iter1);
	ASSERT_EQ(iter1->m_First, citer3->m_First);
	ASSERT_EQ(iter1->m_Second, citer3->m_Second);
}

TEST(HashMapIterator, CopyAssignmentTest)
{
	HashMap<int, int> hm1(
		{{0, 0},
		 {1, 1},
		 {2, 2},
		 {3, 3},
		 {4, 4},
		 {5, 5},
		 {6, 6},
		 {7, 7},
		 {8, 8},
		 {9, 9}});

	ASSERT_EQ(hm1.GetSize(), 10);

	HashMap<int, int>::Iterator iter1 = hm1.GetBegin();
	ASSERT_EQ(iter1->m_First, iter1->m_Second);
	HashMap<int, int>::Iterator iter2 = hm1.GetEnd();
	ASSERT_EQ(iter2, hm1.GetEnd());
	iter2 = iter1;
	ASSERT_EQ(iter1->m_First, iter2->m_First);
	ASSERT_EQ(iter1->m_Second, iter2->m_Second);

	HashMap<int, int>::ConstIterator citer1 = hm1.GetConstBegin();
	ASSERT_EQ(citer1->m_First, citer1->m_Second);
	HashMap<int, int>::ConstIterator citer2 = hm1.GetConstEnd();
	ASSERT_EQ(citer2, hm1.GetConstEnd());
	citer2 = citer1;
	ASSERT_EQ(citer1->m_First, citer2->m_First);
	ASSERT_EQ(citer1->m_Second, citer2->m_Second);

	HashMap<int, int>::ConstIterator citer3 = hm1.GetConstEnd();
	ASSERT_EQ(citer3, hm1.GetConstEnd());
	citer3 = iter1;
	ASSERT_EQ(iter1->m_First, citer3->m_First);
	ASSERT_EQ(iter1->m_Second, citer3->m_Second);
}

TEST(HashMapIterator, SelfIncrementTest)
{
	HashMap<test_hashmap_object, test_hashmap_object>* phm = new HashMap<test_hashmap_object, test_hashmap_object>();
	const int test_size = 1000;
	int key_pool[test_size];
	int val_pool[test_size];
	int key_check_pool[test_size];
	int val_check_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	memset(key_check_pool, 0, sizeof(key_check_pool));
	memset(val_check_pool, 0, sizeof(val_check_pool));
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
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}

	for (HashMap<test_hashmap_object, test_hashmap_object>::Iterator i = phm->GetBegin(); i != phm->GetEnd(); ++i)
	{
		key_check_pool[i->m_First.val] += 1;
		val_check_pool[i->m_Second.val] += 1;
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_check_pool[i], 1);
		ASSERT_EQ(val_check_pool[i], 1);
	}

	for (HashMap<test_hashmap_object, test_hashmap_object>::Iterator i = phm->GetBegin(); i != phm->GetEnd(); i++)
	{
		key_check_pool[i->m_First.val] += 1;
		val_check_pool[i->m_Second.val] += 1;
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_check_pool[i], 2);
		ASSERT_EQ(val_check_pool[i], 2);
	}

	const HashMap<test_hashmap_object, test_hashmap_object>* pchm = phm;

	for (HashMap<test_hashmap_object, test_hashmap_object>::ConstIterator i = pchm->GetConstBegin(); i != pchm->GetConstEnd(); ++i)
	{
		key_check_pool[i->m_First.val] += 1;
		val_check_pool[i->m_Second.val] += 1;
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_check_pool[i], 3);
		ASSERT_EQ(val_check_pool[i], 3);
	}

	for (HashMap<test_hashmap_object, test_hashmap_object>::ConstIterator i = pchm->GetConstBegin(); i != pchm->GetConstEnd(); i++)
	{
		key_check_pool[i->m_First.val] += 1;
		val_check_pool[i->m_Second.val] += 1;
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_check_pool[i], 4);
		ASSERT_EQ(val_check_pool[i], 4);
	}

	delete phm;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 1);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashMapIterator, PlusTest)
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
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}

	int cnt = 0;
	for (HashMap<test_hashmap_object, test_hashmap_object>::Iterator i = phm->GetBegin(); i != phm->GetEnd(); i = i + 2)
	{
		ASSERT_EQ(i->m_First, i->m_Second);
		cnt += 1;
	}
	ASSERT_EQ(cnt, test_size / 2);

	cnt = 0;
	for (HashMap<test_hashmap_object, test_hashmap_object>::Iterator i = phm->GetBegin(); i != phm->GetEnd(); i += 2)
	{
		ASSERT_EQ(i->m_First, i->m_Second);
		cnt += 1;
	}
	ASSERT_EQ(cnt, test_size / 2);

	const HashMap<test_hashmap_object, test_hashmap_object>* pchm = phm;

	cnt = 0;
	for (HashMap<test_hashmap_object, test_hashmap_object>::ConstIterator i = pchm->GetConstBegin(); i != pchm->GetConstEnd(); i = i + 2)
	{
		ASSERT_EQ(i->m_First, i->m_Second);
		cnt += 1;
	}
	ASSERT_EQ(cnt, test_size / 2);

	cnt = 0;
	for (HashMap<test_hashmap_object, test_hashmap_object>::ConstIterator i = pchm->GetConstBegin(); i != pchm->GetConstEnd(); i += 2)
	{
		ASSERT_EQ(i->m_First, i->m_Second);
		cnt += 1;
	}
	ASSERT_EQ(cnt, test_size / 2);

	delete phm;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 1);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashMapIterator, DistanceTest)
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
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}

	ASSERT_EQ(phm->GetEnd() - phm->GetBegin(), test_size);

	const HashMap<test_hashmap_object, test_hashmap_object>* pchm = phm;

	ASSERT_EQ(pchm->GetConstEnd() - pchm->GetConstBegin(), test_size);

	delete phm;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 1);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashMapIterator, OperatorPointerTest)
{
	HashMap<test_hashmap_object, test_hashmap_object>* phm = new HashMap<test_hashmap_object, test_hashmap_object>();
	const int test_size = 1000;
	int key_pool[test_size];
	int val_pool[test_size];
	int key_check_pool[test_size];
	int val_check_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	memset(key_check_pool, 0, sizeof(key_check_pool));
	memset(val_check_pool, 0, sizeof(val_check_pool));
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
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}

	for (HashMap<test_hashmap_object, test_hashmap_object>::Iterator i = phm->GetBegin(); i != phm->GetEnd(); ++i)
	{
		key_check_pool[i->m_First.val] += 1;
		val_check_pool[i->m_Second.val] += 1;
		ASSERT_EQ(i->m_First.val, i->m_Second.val);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_check_pool[i], 1);
		ASSERT_EQ(val_check_pool[i], 1);
	}

	const HashMap<test_hashmap_object, test_hashmap_object>* pchm = phm;

	for (HashMap<test_hashmap_object, test_hashmap_object>::ConstIterator i = pchm->GetConstBegin(); i != pchm->GetConstEnd(); ++i)
	{
		key_check_pool[i->m_First.val] += 1;
		val_check_pool[i->m_Second.val] += 1;
		ASSERT_EQ(i->m_First.val, i->m_Second.val);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_check_pool[i], 2);
		ASSERT_EQ(val_check_pool[i], 2);
	}

	delete phm;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 1);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashMapIterator, OperatorReferenceTest)
{
	HashMap<test_hashmap_object, test_hashmap_object>* phm = new HashMap<test_hashmap_object, test_hashmap_object>();
	const int test_size = 1000;
	int key_pool[test_size];
	int val_pool[test_size];
	int key_check_pool[test_size];
	int val_check_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	memset(key_check_pool, 0, sizeof(key_check_pool));
	memset(val_check_pool, 0, sizeof(val_check_pool));
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
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}

	for (HashMap<test_hashmap_object, test_hashmap_object>::Iterator i = phm->GetBegin(); i != phm->GetEnd(); ++i)
	{
		key_check_pool[(*i).m_First.val] += 1;
		val_check_pool[(*i).m_Second.val] += 1;
		ASSERT_EQ((*i).m_First.val, (*i).m_Second.val);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_check_pool[i], 1);
		ASSERT_EQ(val_check_pool[i], 1);
	}

	const HashMap<test_hashmap_object, test_hashmap_object>* pchm = phm;

	for (HashMap<test_hashmap_object, test_hashmap_object>::ConstIterator i = pchm->GetConstBegin(); i != pchm->GetConstEnd(); ++i)
	{
		key_check_pool[(*i).m_First.val] += 1;
		val_check_pool[(*i).m_Second.val] += 1;
		ASSERT_EQ((*i).m_First.val, (*i).m_Second.val);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_check_pool[i], 2);
		ASSERT_EQ(val_check_pool[i], 2);
	}

	delete phm;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 1);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(HashMapIterator, EqualityTest)
{
	HashMap<int, int> hm1(
		{{0, 0},
		 {1, 1},
		 {2, 2},
		 {3, 3},
		 {4, 4},
		 {5, 5},
		 {6, 6},
		 {7, 7},
		 {8, 8},
		 {9, 9}});

	ASSERT_EQ(hm1.GetSize(), 10);

	HashMap<int, int>::Iterator iter1 = hm1.GetBegin();
	iter1 += 1;
	ASSERT_EQ(iter1->m_First, iter1->m_Second);
	HashMap<int, int>::Iterator iter2 = hm1.GetBegin() + 1;
	ASSERT_EQ(iter2->m_First, iter2->m_Second);

	ASSERT_EQ(iter1, iter2);
	ASSERT_TRUE(iter1 != hm1.GetBegin());
	ASSERT_TRUE(iter2 != hm1.GetEnd());

	HashMap<int, int>::ConstIterator citer1 = hm1.GetConstBegin();
	citer1 += 1;
	ASSERT_EQ(citer1->m_First, citer1->m_Second);
	HashMap<int, int>::ConstIterator citer2 = hm1.GetConstBegin() + 1;
	ASSERT_EQ(citer2->m_First, citer2->m_Second);

	ASSERT_EQ(citer1, citer2);
	ASSERT_TRUE(citer1 != hm1.GetConstBegin());
	ASSERT_TRUE(citer2 != hm1.GetConstEnd());
}

TEST(HashMapIterator, GetDataTest)
{
	HashMap<test_hashmap_object, test_hashmap_object>* phm = new HashMap<test_hashmap_object, test_hashmap_object>();
	const int test_size = 1000;
	int key_pool[test_size];
	int val_pool[test_size];
	int key_check_pool[test_size];
	int val_check_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	memset(key_check_pool, 0, sizeof(key_check_pool));
	memset(val_check_pool, 0, sizeof(val_check_pool));
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
	ASSERT_EQ(phm->GetBucketQuantity(), phm->GetCorrectBucketQuantity(phm->GetLoadFactor(), phm->GetSize()));
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*phm)[test_hashmap_object(i)].val, i);
	}

	for (HashMap<test_hashmap_object, test_hashmap_object>::Iterator i = phm->GetBegin(); i != phm->GetEnd(); ++i)
	{
		key_check_pool[i.GetData()->m_First.val] += 1;
		val_check_pool[i.GetData()->m_Second.val] += 1;
		ASSERT_EQ(i.GetData()->m_First.val, i.GetData()->m_Second.val);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_check_pool[i], 1);
		ASSERT_EQ(val_check_pool[i], 1);
	}

	const HashMap<test_hashmap_object, test_hashmap_object>* pchm = phm;

	for (HashMap<test_hashmap_object, test_hashmap_object>::ConstIterator i = pchm->GetConstBegin(); i != pchm->GetConstEnd(); ++i)
	{
		key_check_pool[i.GetData()->m_First.val] += 1;
		val_check_pool[i.GetData()->m_Second.val] += 1;
		ASSERT_EQ(i.GetData()->m_First.val, i.GetData()->m_Second.val);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_check_pool[i], 2);
		ASSERT_EQ(val_check_pool[i], 2);
	}

	delete phm;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 1);
		ASSERT_EQ(val_pool[i], 1);
	}
}