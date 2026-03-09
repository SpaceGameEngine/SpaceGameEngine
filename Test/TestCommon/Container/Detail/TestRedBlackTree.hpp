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
#include "gtest/gtest.h"
#include "Container/Detail/RedBlackTree.hpp"
#include <functional>
#include <cstring>
#include <vector>

using namespace SpaceGameEngine;

struct test_rbtree_object
{
	std::function<void(test_rbtree_object&)> rel_func;
	test_rbtree_object()
		: key(0), rel_func([](test_rbtree_object&) {})
	{
	}
	test_rbtree_object(int k, int v = 0)
		: key(k), value(v), rel_func([](test_rbtree_object&) {})
	{
	}
	test_rbtree_object(int k, const std::function<void(test_rbtree_object&)>& func)
		: key(k), value(0), rel_func(func)
	{
	}
	test_rbtree_object(int k, int v, const std::function<void(test_rbtree_object&)>& func)
		: key(k), value(v), rel_func(func)
	{
	}
	test_rbtree_object(const test_rbtree_object& o) noexcept
		: key(o.key), value(o.value), rel_func(o.rel_func)
	{
	}
	test_rbtree_object(test_rbtree_object&& o) noexcept
		: key(o.key), value(o.value), rel_func(std::move(o.rel_func))
	{
		o.rel_func = [](test_rbtree_object&) {};
	}
	~test_rbtree_object()
	{
		rel_func(*this);
	}
	int key;
	int value;

	test_rbtree_object& operator=(const test_rbtree_object& o)
	{
		key = o.key;
		value = o.value;
		rel_func = o.rel_func;
		return *this;
	}

	test_rbtree_object& operator=(test_rbtree_object&& o)
	{
		key = o.key;
		value = o.value;
		rel_func = std::move(o.rel_func);
		o.rel_func = [](test_rbtree_object&) {};
		return *this;
	}

	bool operator<(const test_rbtree_object& o) const
	{
		return key < o.key;
	}

	// only compare key here
	bool operator==(const test_rbtree_object& o) const
	{
		return key == o.key && value == o.value;
	}

	// only compare key here
	bool operator!=(const test_rbtree_object& o) const
	{
		return key != o.key || value != o.value;
	}
};

template<>
struct SpaceGameEngine::Equal<test_rbtree_object>
{
	inline static constexpr bool Compare(const test_rbtree_object& lhs, const test_rbtree_object& rhs)
	{
		return lhs.key == rhs.key;
	}
};

bool operator==(const test_rbtree_object& o, int val)
{
	return o.value == val;
}

bool operator==(int val, const test_rbtree_object& o)
{
	return o.value == val;
}

TEST(RedBlackTree, InitializerListConstructionTest)
{
	const int test_size = 10;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_rbtree_object& o) {
		val_pool[o.key] += 1;
	};

	Detail::RedBlackTree<test_rbtree_object>* prbt = new Detail::RedBlackTree<test_rbtree_object>(
		{test_rbtree_object(0, val_rel_func),
		 test_rbtree_object(1, val_rel_func),
		 test_rbtree_object(2, val_rel_func),
		 test_rbtree_object(3, val_rel_func),
		 test_rbtree_object(4, val_rel_func),
		 test_rbtree_object(5, val_rel_func),
		 test_rbtree_object(6, val_rel_func),
		 test_rbtree_object(7, val_rel_func),
		 test_rbtree_object(8, val_rel_func),
		 test_rbtree_object(9, val_rel_func)});

	ASSERT_EQ(prbt->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(prbt->Find(i)->key, i);
	}
	delete prbt;
	for (int i = 0; i < test_size; i++)
	{
		// initializer_list can only return const variable, so move is useless
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(RedBlackTree, ClearTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_rbtree_object& o) {
		val_pool[o.key] += 1;
	};
	Detail::RedBlackTree<test_rbtree_object>* prbt = new Detail::RedBlackTree<test_rbtree_object>();
	for (int i = 0; i < test_size; i++)
	{
		auto re = prbt->Insert(test_rbtree_object(i, val_rel_func));
		ASSERT_EQ(re.m_First->key, i);
		ASSERT_TRUE(re.m_Second);
	}
	ASSERT_EQ(prbt->GetSize(), test_size);

	prbt->Clear();

	ASSERT_EQ(prbt->GetSize(), 0);
	ASSERT_EQ(prbt->GetBegin().GetData(), prbt->GetEnd().GetData());
	ASSERT_EQ(prbt->GetConstBegin().GetData(), prbt->GetConstEnd().GetData());
	ASSERT_EQ(prbt->GetReverseBegin().GetData(), prbt->GetReverseEnd().GetData());
	ASSERT_EQ(prbt->GetConstReverseBegin().GetData(), prbt->GetConstReverseEnd().GetData());

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}

	delete prbt;
}

TEST(RedBlackTree, InsertTest)
{
	Detail::RedBlackTree<test_rbtree_object>* prbt = new Detail::RedBlackTree<test_rbtree_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_rbtree_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = prbt->Insert(test_rbtree_object(i, i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_EQ(iter.m_First->value, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(prbt->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt)[test_rbtree_object(i)].key, i);
		auto iter = prbt->Insert(test_rbtree_object(i, test_size - 1 - i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_EQ(iter.m_First->value, i);
		ASSERT_FALSE(iter.m_Second);
	}
	delete prbt;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(RedBlackTree, UpsertTest)
{
	Detail::RedBlackTree<test_rbtree_object>* prbt = new Detail::RedBlackTree<test_rbtree_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_rbtree_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = prbt->Upsert(test_rbtree_object(i, i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_EQ(iter.m_First->value, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(prbt->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt)[test_rbtree_object(i)].key, i);
		auto iter = prbt->Upsert(test_rbtree_object(i, test_size - 1 - i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_EQ(iter.m_First->value, test_size - 1 - i);
		ASSERT_FALSE(iter.m_Second);
	}
	delete prbt;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(RedBlackTree, InsertListTest)
{
	Detail::RedBlackTree<test_rbtree_object>* prbt = new Detail::RedBlackTree<test_rbtree_object>();
	const int test_size = 10;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_rbtree_object& o) {
		val_pool[o.key] += 1;
	};

	// test repeat insert
	prbt->Insert(test_rbtree_object(0, -1));
	ASSERT_EQ(prbt->GetSize(), 1);
	ASSERT_EQ(prbt->GetBegin()->value, -1);

	// have 11 elements, test repeat insert
	prbt->Insert({test_rbtree_object(0, 0, val_rel_func),
				  test_rbtree_object(1, -1),
				  test_rbtree_object(1, 1, val_rel_func),
				  test_rbtree_object(2, 2, val_rel_func),
				  test_rbtree_object(3, 3, val_rel_func),
				  test_rbtree_object(4, 4, val_rel_func),
				  test_rbtree_object(5, 5, val_rel_func),
				  test_rbtree_object(6, 6, val_rel_func),
				  test_rbtree_object(7, 7, val_rel_func),
				  test_rbtree_object(8, 8, val_rel_func),
				  test_rbtree_object(9, 9, val_rel_func)});

	ASSERT_EQ(prbt->GetSize(), test_size);

	for (int i = test_size - 1; i >= 2; i--)
	{
		ASSERT_EQ((*prbt)[test_rbtree_object(i)].key, i);
	}
	ASSERT_EQ((*prbt)[test_rbtree_object(0)].value, -1);
	ASSERT_EQ((*prbt)[test_rbtree_object(1)].value, -1);
	delete prbt;
	for (int i = 0; i < test_size; i++)
	{
		// initializer_list can only return const variable, so move is useless
		ASSERT_EQ(val_pool[i], i > 1 ? 2 : 1);
	}
}

TEST(RedBlackTree, UpsertListTest)
{
	Detail::RedBlackTree<test_rbtree_object>* prbt = new Detail::RedBlackTree<test_rbtree_object>();
	const int test_size = 10;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_rbtree_object& o) {
		val_pool[o.key] += 1;
	};

	// test repeat insert
	prbt->Upsert(test_rbtree_object(0, -1));
	ASSERT_EQ(prbt->GetSize(), 1);
	ASSERT_EQ(prbt->GetBegin()->value, -1);

	// have 11 elements, test repeat insert
	prbt->Upsert({test_rbtree_object(0, 0, val_rel_func),
				  test_rbtree_object(1, -1),
				  test_rbtree_object(1, 1, val_rel_func),
				  test_rbtree_object(2, 2, val_rel_func),
				  test_rbtree_object(3, 3, val_rel_func),
				  test_rbtree_object(4, 4, val_rel_func),
				  test_rbtree_object(5, 5, val_rel_func),
				  test_rbtree_object(6, 6, val_rel_func),
				  test_rbtree_object(7, 7, val_rel_func),
				  test_rbtree_object(8, 8, val_rel_func),
				  test_rbtree_object(9, 9, val_rel_func)});

	ASSERT_EQ(prbt->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt)[test_rbtree_object(i)].key, i);
	}
	delete prbt;
	for (int i = 0; i < test_size; i++)
	{
		// initializer_list can only return const variable, so move is useless
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(RedBlackTree, RemoveTest)
{
	Detail::RedBlackTree<test_rbtree_object>* prbt = new Detail::RedBlackTree<test_rbtree_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_rbtree_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = prbt->Insert(test_rbtree_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(prbt->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt)[test_rbtree_object(i)].key, i);
	}

	int rm_cnt = 0;
	auto iter = prbt->GetBegin();
	while (iter != prbt->GetEnd())
	{
		iter = prbt->Remove(iter);
		rm_cnt += 1;
	}
	ASSERT_EQ(rm_cnt, test_size);
	ASSERT_EQ(prbt->GetSize(), 0);

	delete prbt;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(RedBlackTree, RemoveByValueTest)
{
	Detail::RedBlackTree<test_rbtree_object>* prbt = new Detail::RedBlackTree<test_rbtree_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_rbtree_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = prbt->Insert(test_rbtree_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(prbt->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt)[test_rbtree_object(i)].key, i);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_TRUE(prbt->RemoveByValue(test_rbtree_object(i)));
	}

	ASSERT_EQ(prbt->GetSize(), 0);

	delete prbt;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(RedBlackTree, ContainsTest)
{
	const Detail::RedBlackTree<int> rbtree({1, 2, 3});
	ASSERT_FALSE(rbtree.Contains(0));
	ASSERT_TRUE(rbtree.Contains(1));
	ASSERT_TRUE(rbtree.Contains(2));
	ASSERT_TRUE(rbtree.Contains(3));
	ASSERT_FALSE(rbtree.Contains(4));
}

TEST(RedBlackTree, FindTest)
{
	Detail::RedBlackTree<test_rbtree_object>* prbt = new Detail::RedBlackTree<test_rbtree_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_rbtree_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = prbt->Insert(test_rbtree_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(prbt->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; i--)
	{
		auto iter = prbt->Find(test_rbtree_object(i));
		ASSERT_EQ(iter->key, i);
	}

	auto niter = prbt->Find(test_rbtree_object(test_size));
	ASSERT_EQ(niter, prbt->GetEnd());

	ASSERT_EQ(prbt->GetSize(), test_size);

	const Detail::RedBlackTree<test_rbtree_object>* pcrbt = prbt;

	for (int i = test_size - 1; i >= 0; i--)
	{
		auto iter = pcrbt->Find(test_rbtree_object(i));
		ASSERT_EQ(iter->key, i);
	}

	auto cniter = pcrbt->Find(test_rbtree_object(test_size));
	ASSERT_EQ(cniter, pcrbt->GetConstEnd());

	ASSERT_EQ(pcrbt->GetSize(), test_size);

	delete prbt;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(RedBlackTree, GetTest)
{
	Detail::RedBlackTree<test_rbtree_object>* prbt = new Detail::RedBlackTree<test_rbtree_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_rbtree_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = prbt->Insert(test_rbtree_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(prbt->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(prbt->Get(test_rbtree_object(i)).key, i);
	}

	ASSERT_EQ(prbt->GetSize(), test_size);

	const Detail::RedBlackTree<test_rbtree_object>* pcrbt = prbt;

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pcrbt->Get(test_rbtree_object(i)).key, i);
	}

	ASSERT_EQ(pcrbt->GetSize(), test_size);

	delete prbt;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(RedBlackTree, OperatorTest)
{
	Detail::RedBlackTree<test_rbtree_object>* prbt = new Detail::RedBlackTree<test_rbtree_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_rbtree_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		(*prbt)[test_rbtree_object(i)] = test_rbtree_object(i, i, val_rel_func);
		ASSERT_EQ((*prbt)[test_rbtree_object(i)].key, i);
		ASSERT_EQ((*prbt)[test_rbtree_object(i)].value, i);
		(*prbt)[test_rbtree_object(i)].value += 1;
	}
	ASSERT_EQ(prbt->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt)[test_rbtree_object(i)].key, i);
		ASSERT_EQ((*prbt)[test_rbtree_object(i)].value, i + 1);
		(*prbt)[test_rbtree_object(i)].value -= 1;
	}
	ASSERT_EQ(prbt->GetSize(), test_size);

	delete prbt;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(RedBlackTree, ForEachTest)
{
	const int test_size = 1000;
	double test_cnt[test_size];
	for (int i = 0; i < test_size; i++)
	{
		test_cnt[i] = 0;
	}
	Detail::RedBlackTree<Pair<const int, double>, KeyComparer<Less<int>>, KeyComparer<Equal<int>>> m1;
	ASSERT_EQ(m1.GetSize(), 0);
	for (int i = 0; i < test_size; i++)
	{
		m1.Insert(MakePair(i, 1));
	}
	ASSERT_EQ(m1.GetSize(), test_size);

	int last_key = 0;
	m1.ForEach([&last_key](Pair<const int, double>& d) {
		ASSERT_EQ(d.m_First, last_key);
		d.m_Second = d.m_First;
		++last_key;
	});

	ASSERT_EQ(m1.GetSize(), test_size);
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(m1.Find(i)->m_Second, (double)i);
	}

	const Detail::RedBlackTree<Pair<const int, double>, KeyComparer<Less<int>>, KeyComparer<Equal<int>>> m2(m1);
	ASSERT_EQ(m2.GetSize(), test_size);
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(m2.Find(i)->m_Second, (double)i);
	}

	last_key = 0;
	m2.ForEach([&](const Pair<const int, double>& d) {
		ASSERT_EQ(d.m_First, last_key);
		ASSERT_EQ(d.m_First, d.m_Second);
		test_cnt[d.m_First] += 1;
		++last_key;
	});

	ASSERT_EQ(m2.GetSize(), test_size);
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(m2.Find(i)->m_Second, (double)i);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(test_cnt[i], 1);
	}
}

TEST(RedBlackTree, ReverseForEachTest)
{
	const int test_size = 1000;
	double test_cnt[test_size];
	for (int i = 0; i < test_size; i++)
	{
		test_cnt[i] = 0;
	}
	Detail::RedBlackTree<Pair<const int, double>, KeyComparer<Less<int>>, KeyComparer<Equal<int>>> m1;
	ASSERT_EQ(m1.GetSize(), 0);
	for (int i = 0; i < test_size; i++)
	{
		m1.Insert(MakePair(i, 1));
	}
	ASSERT_EQ(m1.GetSize(), test_size);

	int last_key = test_size - 1;
	m1.ReverseForEach([&last_key](Pair<const int, double>& d) {
		ASSERT_EQ(d.m_First, last_key);
		d.m_Second = d.m_First;
		--last_key;
	});

	ASSERT_EQ(m1.GetSize(), test_size);
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(m1.Find(i)->m_Second, (double)i);
	}

	const Detail::RedBlackTree<Pair<const int, double>, KeyComparer<Less<int>>, KeyComparer<Equal<int>>> m2(m1);
	ASSERT_EQ(m2.GetSize(), test_size);
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(m2.Find(i)->m_Second, (double)i);
	}

	last_key = test_size - 1;
	m2.ReverseForEach([&](const Pair<const int, double>& d) {
		ASSERT_EQ(d.m_First, last_key);
		ASSERT_EQ(d.m_First, d.m_Second);
		test_cnt[d.m_First] += 1;
		--last_key;
	});

	ASSERT_EQ(m2.GetSize(), test_size);
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(m2.Find(i)->m_Second, (double)i);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(test_cnt[i], 1);
	}
}

TEST(RedBlackTree, CopyConstructionTest)
{
	Detail::RedBlackTree<test_rbtree_object>* prbt = new Detail::RedBlackTree<test_rbtree_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_rbtree_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = prbt->Insert(test_rbtree_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(prbt->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt)[test_rbtree_object(i)].key, i);
	}

	Detail::RedBlackTree<test_rbtree_object>* prbt2 = new Detail::RedBlackTree<test_rbtree_object>(*prbt);

	ASSERT_EQ(prbt->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt)[test_rbtree_object(i)].key, i);
	}

	ASSERT_EQ(prbt2->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt2)[test_rbtree_object(i)].key, i);
	}

	delete prbt;
	delete prbt2;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(RedBlackTree, MoveConstructionTest)
{
	Detail::RedBlackTree<test_rbtree_object>* prbt = new Detail::RedBlackTree<test_rbtree_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_rbtree_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = prbt->Insert(test_rbtree_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(prbt->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt)[test_rbtree_object(i)].key, i);
	}

	Detail::RedBlackTree<test_rbtree_object>* prbt2 = new Detail::RedBlackTree<test_rbtree_object>(std::move(*prbt));

	ASSERT_EQ(prbt2->GetSize(), test_size);

	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt2)[test_rbtree_object(i)].key, i);
	}

	delete prbt;
	delete prbt2;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(RedBlackTree, CopyAssignmentTest)
{
	Detail::RedBlackTree<test_rbtree_object>* prbt = new Detail::RedBlackTree<test_rbtree_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_rbtree_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = prbt->Insert(test_rbtree_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(prbt->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt)[test_rbtree_object(i)].key, i);
	}

	Detail::RedBlackTree<test_rbtree_object>* prbt2 = new Detail::RedBlackTree<test_rbtree_object>();

	ASSERT_EQ(prbt2->GetSize(), 0);

	*prbt2 = *prbt;

	ASSERT_EQ(prbt->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt)[test_rbtree_object(i)].key, i);
	}

	ASSERT_EQ(prbt2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt2)[test_rbtree_object(i)].key, i);
	}

	delete prbt;
	delete prbt2;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(RedBlackTree, MoveAssignmentTest)
{
	Detail::RedBlackTree<test_rbtree_object>* prbt = new Detail::RedBlackTree<test_rbtree_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_rbtree_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = prbt->Insert(test_rbtree_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(prbt->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt)[test_rbtree_object(i)].key, i);
	}

	Detail::RedBlackTree<test_rbtree_object>* prbt2 = new Detail::RedBlackTree<test_rbtree_object>();

	ASSERT_EQ(prbt2->GetSize(), 0);

	*prbt2 = std::move(*prbt);

	ASSERT_EQ(prbt2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt2)[test_rbtree_object(i)].key, i);
	}

	delete prbt;
	delete prbt2;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(RedBlackTree, AnotherAllocatorCopyConstructionTest)
{
	Detail::RedBlackTree<test_rbtree_object>* prbt = new Detail::RedBlackTree<test_rbtree_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_rbtree_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = prbt->Insert(test_rbtree_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(prbt->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt)[test_rbtree_object(i)].key, i);
	}

	Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, StdAllocator>* prbt2 = new Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, StdAllocator>(*prbt);

	ASSERT_EQ(prbt->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt)[test_rbtree_object(i)].key, i);
	}

	ASSERT_EQ(prbt2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt2)[test_rbtree_object(i)].key, i);
	}

	Detail::RedBlackTree<test_rbtree_object>* prbt3 = new Detail::RedBlackTree<test_rbtree_object>(*prbt2);

	ASSERT_EQ(prbt->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt)[test_rbtree_object(i)].key, i);
	}

	ASSERT_EQ(prbt2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt2)[test_rbtree_object(i)].key, i);
	}

	ASSERT_EQ(prbt3->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt3)[test_rbtree_object(i)].key, i);
	}

	delete prbt;
	delete prbt2;
	delete prbt3;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 3);
	}
}

TEST(RedBlackTree, AnotherAllocatorMoveConstructionTest)
{
	Detail::RedBlackTree<test_rbtree_object>* prbt = new Detail::RedBlackTree<test_rbtree_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_rbtree_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = prbt->Insert(test_rbtree_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(prbt->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt)[test_rbtree_object(i)].key, i);
	}

	Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, StdAllocator>* prbt2 = new Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, StdAllocator>(std::move(*prbt));

	ASSERT_EQ(prbt2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt2)[test_rbtree_object(i)].key, i);
	}

	Detail::RedBlackTree<test_rbtree_object>* prbt3 = new Detail::RedBlackTree<test_rbtree_object>(std::move(*prbt2));

	ASSERT_EQ(prbt3->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt3)[test_rbtree_object(i)].key, i);
	}

	delete prbt;
	delete prbt2;
	delete prbt3;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(RedBlackTree, AnotherAllocatorCopyAssignmentTest)
{
	Detail::RedBlackTree<test_rbtree_object>* prbt = new Detail::RedBlackTree<test_rbtree_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_rbtree_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = prbt->Insert(test_rbtree_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(prbt->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt)[test_rbtree_object(i)].key, i);
	}

	Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, StdAllocator>* prbt2 = new Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, StdAllocator>();

	ASSERT_EQ(prbt2->GetSize(), 0);

	*prbt2 = *prbt;

	ASSERT_EQ(prbt->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt)[test_rbtree_object(i)].key, i);
	}

	ASSERT_EQ(prbt2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt2)[test_rbtree_object(i)].key, i);
	}

	Detail::RedBlackTree<test_rbtree_object>* prbt3 = new Detail::RedBlackTree<test_rbtree_object>();

	ASSERT_EQ(prbt3->GetSize(), 0);

	*prbt3 = *prbt2;

	ASSERT_EQ(prbt->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt)[test_rbtree_object(i)].key, i);
	}

	ASSERT_EQ(prbt2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt2)[test_rbtree_object(i)].key, i);
	}

	ASSERT_EQ(prbt3->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt3)[test_rbtree_object(i)].key, i);
	}

	delete prbt;
	delete prbt2;
	delete prbt3;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 3);
	}
}

TEST(RedBlackTree, AnotherAllocatorMoveAssignmentTest)
{
	Detail::RedBlackTree<test_rbtree_object>* prbt = new Detail::RedBlackTree<test_rbtree_object>();
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_rbtree_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = prbt->Insert(test_rbtree_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(prbt->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt)[test_rbtree_object(i)].key, i);
	}

	Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, StdAllocator>* prbt2 = new Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, StdAllocator>();

	ASSERT_EQ(prbt2->GetSize(), 0);

	*prbt2 = std::move(*prbt);

	ASSERT_EQ(prbt2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt2)[test_rbtree_object(i)].key, i);
	}

	Detail::RedBlackTree<test_rbtree_object>* prbt3 = new Detail::RedBlackTree<test_rbtree_object>();

	ASSERT_EQ(prbt3->GetSize(), 0);

	*prbt3 = std::move(*prbt2);

	ASSERT_EQ(prbt3->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt3)[test_rbtree_object(i)].key, i);
	}

	delete prbt;
	delete prbt2;
	delete prbt3;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(RedBlackTree, EqualTest)
{
	const Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, MemoryManagerAllocator> rbt1({test_rbtree_object(1, 10),
																																	  test_rbtree_object(2, 20),
																																	  test_rbtree_object(3, 30)});
	const Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, MemoryManagerAllocator> rbt2({test_rbtree_object(1, 10),
																																	  test_rbtree_object(2, 20)});
	const Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, StdAllocator> rbt2_({test_rbtree_object(1, 10),
																															 test_rbtree_object(2, 20)});
	const Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, MemoryManagerAllocator> rbt3({test_rbtree_object(1, 10),
																																	  test_rbtree_object(2, 21),
																																	  test_rbtree_object(3, 30)});
	const Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, StdAllocator> rbt3_({test_rbtree_object(1, 10),
																															 test_rbtree_object(2, 21),
																															 test_rbtree_object(3, 30)});
	const Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, MemoryManagerAllocator> rbt4({test_rbtree_object(1, 10),
																																	  test_rbtree_object(2, 20),
																																	  test_rbtree_object(3, 30)});
	const Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, StdAllocator> rbt4_({test_rbtree_object(1, 10),
																															 test_rbtree_object(2, 20),
																															 test_rbtree_object(3, 30)});

	ASSERT_FALSE(rbt1 == rbt2);
	ASSERT_FALSE(rbt1 == rbt2_);
	ASSERT_FALSE(rbt1 == rbt3);
	ASSERT_FALSE(rbt1 == rbt3_);
	ASSERT_TRUE(rbt1 == rbt4);
	ASSERT_TRUE(rbt1 == rbt4_);
}

TEST(RedBlackTree, NotEqualTest)
{
	const Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, MemoryManagerAllocator> rbt1({test_rbtree_object(1, 10),
																																	  test_rbtree_object(2, 20),
																																	  test_rbtree_object(3, 30)});
	const Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, MemoryManagerAllocator> rbt2({test_rbtree_object(1, 10),
																																	  test_rbtree_object(2, 20)});
	const Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, StdAllocator> rbt2_({test_rbtree_object(1, 10),
																															 test_rbtree_object(2, 20)});
	const Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, MemoryManagerAllocator> rbt3({test_rbtree_object(1, 10),
																																	  test_rbtree_object(2, 21),
																																	  test_rbtree_object(3, 30)});
	const Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, StdAllocator> rbt3_({test_rbtree_object(1, 10),
																															 test_rbtree_object(2, 21),
																															 test_rbtree_object(3, 30)});
	const Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, MemoryManagerAllocator> rbt4({test_rbtree_object(1, 10),
																																	  test_rbtree_object(2, 20),
																																	  test_rbtree_object(3, 30)});
	const Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, StdAllocator> rbt4_({test_rbtree_object(1, 10),
																															 test_rbtree_object(2, 20),
																															 test_rbtree_object(3, 30)});

	ASSERT_TRUE(rbt1 != rbt2);
	ASSERT_TRUE(rbt1 != rbt2_);
	ASSERT_TRUE(rbt1 != rbt3);
	ASSERT_TRUE(rbt1 != rbt3_);
	ASSERT_FALSE(rbt1 != rbt4);
	ASSERT_FALSE(rbt1 != rbt4_);
}

TEST(RedBlackTree, SwapTest)
{
	Detail::RedBlackTree<test_rbtree_object>* prbt = new Detail::RedBlackTree<test_rbtree_object>();
	const int test_size = 1000;
	int val_pool[2 * test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_rbtree_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = prbt->Insert(test_rbtree_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(prbt->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt)[test_rbtree_object(i)].key, i);
	}

	Detail::RedBlackTree<test_rbtree_object>* prbt2 = new Detail::RedBlackTree<test_rbtree_object>();

	for (int i = test_size; i < 2 * test_size; ++i)
	{
		auto iter = prbt2->Insert(test_rbtree_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}

	ASSERT_EQ(prbt2->GetSize(), test_size);
	for (int i = 2 * test_size - 1; i >= test_size; i--)
	{
		ASSERT_EQ((*prbt2)[test_rbtree_object(i)].key, i);
	}

	Detail::RedBlackTree<test_rbtree_object>* prbt3 = new Detail::RedBlackTree<test_rbtree_object>(std::move(*prbt));

	ASSERT_EQ(prbt3->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt3)[test_rbtree_object(i)].key, i);
	}

	*prbt = std::move(*prbt2);

	ASSERT_EQ(prbt->GetSize(), test_size);
	for (int i = 2 * test_size - 1; i >= test_size; i--)
	{
		ASSERT_EQ((*prbt)[test_rbtree_object(i)].key, i);
	}

	*prbt2 = std::move(*prbt3);

	ASSERT_EQ(prbt2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt2)[test_rbtree_object(i)].key, i);
	}

	delete prbt;
	delete prbt2;
	delete prbt3;
	for (int i = 0; i < 2 * test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(RedBlackTree, AnotherAllocatorSwapTest)
{
	Detail::RedBlackTree<test_rbtree_object>* prbt = new Detail::RedBlackTree<test_rbtree_object>();
	const int test_size = 1000;
	int val_pool[2 * test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_rbtree_object& o) {
		val_pool[o.key] += 1;
	};
	for (int i = 0; i < test_size; i++)
	{
		auto iter = prbt->Insert(test_rbtree_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}
	ASSERT_EQ(prbt->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt)[test_rbtree_object(i)].key, i);
	}

	Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, StdAllocator>* prbt2 = new Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, StdAllocator>();

	for (int i = test_size; i < 2 * test_size; ++i)
	{
		auto iter = prbt2->Insert(test_rbtree_object(i, val_rel_func));
		ASSERT_EQ(iter.m_First->key, i);
		ASSERT_TRUE(iter.m_Second);
	}

	ASSERT_EQ(prbt2->GetSize(), test_size);
	for (int i = 2 * test_size - 1; i >= test_size; i--)
	{
		ASSERT_EQ((*prbt2)[test_rbtree_object(i)].key, i);
	}

	Detail::RedBlackTree<test_rbtree_object>* prbt3 = new Detail::RedBlackTree<test_rbtree_object>(std::move(*prbt));

	ASSERT_EQ(prbt3->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt3)[test_rbtree_object(i)].key, i);
	}

	*prbt = std::move(*prbt2);

	ASSERT_EQ(prbt->GetSize(), test_size);
	for (int i = 2 * test_size - 1; i >= test_size; i--)
	{
		ASSERT_EQ((*prbt)[test_rbtree_object(i)].key, i);
	}

	*prbt2 = std::move(*prbt3);

	ASSERT_EQ(prbt2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*prbt2)[test_rbtree_object(i)].key, i);
	}

	delete prbt;
	delete prbt2;
	delete prbt3;
	for (int i = 0; i < 2 * test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(RedBlackTree, ConstructByEmptyListTest)
{
	Detail::RedBlackTree<int> rbt(std::initializer_list<int>{});
	ASSERT_EQ(rbt.GetSize(), 0);

	rbt.Insert(2);
	ASSERT_EQ(rbt.GetSize(), 1);
	ASSERT_EQ(*rbt.GetBegin(), 2);
}

TEST(RedBlackTree, InsertEmptyListTest)
{
	Detail::RedBlackTree<int> rbt({2});
	ASSERT_EQ(rbt.GetSize(), 1);
	ASSERT_EQ(*rbt.GetBegin(), 2);

	rbt.Insert(std::initializer_list<int>{});
	ASSERT_EQ(rbt.GetSize(), 1);
	ASSERT_EQ(*rbt.GetBegin(), 2);
}

TEST(RedBlackTreeIterator, GetBeginTest)
{
	Detail::RedBlackTree<int> rbt({1, 2, 3});
	ASSERT_EQ(rbt.GetSize(), 3);
	auto iter = rbt.GetBegin();
	ASSERT_TRUE((std::is_same_v<decltype(iter), Detail::RedBlackTree<int>::Iterator>));
	ASSERT_EQ(*iter, 1);
}

TEST(RedBlackTreeIterator, GetEndTest)
{
	Detail::RedBlackTree<int> rbt({1, 2, 3});
	ASSERT_EQ(rbt.GetSize(), 3);
	auto iter = rbt.GetEnd();
	ASSERT_TRUE((std::is_same_v<decltype(iter), Detail::RedBlackTree<int>::Iterator>));
	--iter;
	ASSERT_EQ(*iter, 3);
}

TEST(RedBlackTreeIterator, GetConstBeginTest)
{
	Detail::RedBlackTree<int> rbt({1, 2, 3});
	ASSERT_EQ(rbt.GetSize(), 3);
	auto iter = rbt.GetConstBegin();
	ASSERT_TRUE((std::is_same_v<decltype(iter), Detail::RedBlackTree<int>::ConstIterator>));
	ASSERT_EQ(*iter, 1);
}

TEST(RedBlackTreeIterator, GetConstEndTest)
{
	Detail::RedBlackTree<int> rbt({1, 2, 3});
	ASSERT_EQ(rbt.GetSize(), 3);
	auto iter = rbt.GetConstEnd();
	ASSERT_TRUE((std::is_same_v<decltype(iter), Detail::RedBlackTree<int>::ConstIterator>));
	--iter;
	ASSERT_EQ(*iter, 3);
}

TEST(RedBlackTreeIterator, GetReverseBeginTest)
{
	Detail::RedBlackTree<int> rbt({1, 2, 3});
	ASSERT_EQ(rbt.GetSize(), 3);
	auto iter = rbt.GetReverseBegin();
	ASSERT_TRUE((std::is_same_v<decltype(iter), Detail::RedBlackTree<int>::ReverseIterator>));
	ASSERT_EQ(*iter, 3);
}

TEST(RedBlackTreeIterator, GetReverseEndTest)
{
	Detail::RedBlackTree<int> rbt({1, 2, 3});
	ASSERT_EQ(rbt.GetSize(), 3);
	auto iter = rbt.GetReverseEnd();
	ASSERT_TRUE((std::is_same_v<decltype(iter), Detail::RedBlackTree<int>::ReverseIterator>));
	--iter;
	ASSERT_EQ(*iter, 1);
}

TEST(RedBlackTreeIterator, GetConstReverseBeginTest)
{
	Detail::RedBlackTree<int> rbt({1, 2, 3});
	ASSERT_EQ(rbt.GetSize(), 3);
	auto iter = rbt.GetConstReverseBegin();
	ASSERT_TRUE((std::is_same_v<decltype(iter), Detail::RedBlackTree<int>::ConstReverseIterator>));
	ASSERT_EQ(*iter, 3);
}

TEST(RedBlackTreeIterator, GetConstReverseEndTest)
{
	Detail::RedBlackTree<int> rbt({1, 2, 3});
	ASSERT_EQ(rbt.GetSize(), 3);
	auto iter = rbt.GetConstReverseEnd();
	ASSERT_TRUE((std::is_same_v<decltype(iter), Detail::RedBlackTree<int>::ConstReverseIterator>));
	--iter;
	ASSERT_EQ(*iter, 1);
}

TEST(RedBlackTreeIterator, IteratorTest)
{
	Detail::RedBlackTree<Pair<const int, int>, KeyComparer<Less<int>>, KeyComparer<Equal<int>>> rbt({MakePair(1, 1.0),
																									 MakePair(2, 2.0),
																									 MakePair(3, 3.0),
																									 MakePair(4, 4.0),
																									 MakePair(5, 5.0),
																									 MakePair(6, 6.0),
																									 MakePair(7, 7.0),
																									 MakePair(8, 8.0),
																									 MakePair(9, 9.0),
																									 MakePair(10, 10.0)});
	ASSERT_EQ(rbt.GetSize(), 10);

	int cnt = 1;
	for (auto i = rbt.GetBegin(); i != rbt.GetEnd(); ++i)
	{
		ASSERT_EQ(cnt, i->m_First);
		(*i).m_Second = 0;
		cnt++;
	}

	auto iter = rbt.GetBegin();
	for (int i = 1; i <= 10; i++)
	{
		ASSERT_EQ(iter->m_First, i);
		ASSERT_EQ((*iter).m_Second, 0);
		++iter;
	}
	--iter;
	for (int i = 10; i > 0; i--)
	{
		ASSERT_EQ(iter->m_First, i);
		ASSERT_EQ((*iter).m_Second, 0);
		--iter;
	}
	++iter;
	for (int i = 1; i <= 10; i++)
	{
		ASSERT_EQ(iter.GetData()->m_First, i);
		ASSERT_EQ(iter.GetData()->m_Second, 0);
		++iter;
	}
}

TEST(RedBlackTreeIterator, ConstIteratorTest)
{
	const Detail::RedBlackTree<Pair<const int, int>, KeyComparer<Less<int>>, KeyComparer<Equal<int>>> rbt({MakePair(1, 1.0),
																										   MakePair(2, 2.0),
																										   MakePair(3, 3.0),
																										   MakePair(4, 4.0),
																										   MakePair(5, 5.0),
																										   MakePair(6, 6.0),
																										   MakePair(7, 7.0),
																										   MakePair(8, 8.0),
																										   MakePair(9, 9.0),
																										   MakePair(10, 10.0)});
	ASSERT_EQ(rbt.GetSize(), 10);

	int cnt = 1;
	for (auto i = rbt.GetConstBegin(); i != rbt.GetConstEnd(); ++i)
	{
		ASSERT_EQ(cnt, i->m_First);
		cnt++;
	}

	auto iter = rbt.GetConstBegin();
	for (int i = 1; i <= 10; i++)
	{
		ASSERT_EQ(iter->m_First, i);
		ASSERT_EQ((*iter).m_Second, (double)i);
		++iter;
	}
	--iter;
	for (int i = 10; i > 0; i--)
	{
		ASSERT_EQ(iter->m_First, i);
		ASSERT_EQ((*iter).m_Second, (double)i);
		--iter;
	}
	++iter;
	for (int i = 1; i <= 10; i++)
	{
		ASSERT_EQ(iter.GetData()->m_First, i);
		ASSERT_EQ(iter.GetData()->m_Second, (double)i);
		++iter;
	}
}

TEST(RedBlackTreeIterator, ReverseIteratorTest)
{
	Detail::RedBlackTree<Pair<const int, int>, KeyComparer<Less<int>>, KeyComparer<Equal<int>>> rbt({MakePair(1, 1.0),
																									 MakePair(2, 2.0),
																									 MakePair(3, 3.0),
																									 MakePair(4, 4.0),
																									 MakePair(5, 5.0),
																									 MakePair(6, 6.0),
																									 MakePair(7, 7.0),
																									 MakePair(8, 8.0),
																									 MakePair(9, 9.0),
																									 MakePair(10, 10.0)});
	ASSERT_EQ(rbt.GetSize(), 10);

	int cnt = 10;
	for (auto i = rbt.GetReverseBegin(); i != rbt.GetReverseEnd(); ++i)
	{
		ASSERT_EQ(cnt, i->m_First);
		(*i).m_Second = 0;
		cnt--;
	}

	auto iter = rbt.GetReverseBegin();
	for (int i = 10; i > 0; i--)
	{
		ASSERT_EQ(iter->m_First, i);
		ASSERT_EQ((*iter).m_Second, 0);
		++iter;
	}
	--iter;
	for (int i = 1; i <= 10; i++)
	{
		ASSERT_EQ(iter->m_First, i);
		ASSERT_EQ((*iter).m_Second, 0);
		--iter;
	}
	++iter;
	for (int i = 10; i > 0; i--)
	{
		ASSERT_EQ(iter.GetData()->m_First, i);
		ASSERT_EQ(iter.GetData()->m_Second, 0);
		++iter;
	}
}

TEST(RedBlackTreeIterator, ConstReverseIteratorTest)
{
	const Detail::RedBlackTree<Pair<const int, int>, KeyComparer<Less<int>>, KeyComparer<Equal<int>>> rbt({MakePair(1, 1.0),
																										   MakePair(2, 2.0),
																										   MakePair(3, 3.0),
																										   MakePair(4, 4.0),
																										   MakePair(5, 5.0),
																										   MakePair(6, 6.0),
																										   MakePair(7, 7.0),
																										   MakePair(8, 8.0),
																										   MakePair(9, 9.0),
																										   MakePair(10, 10.0)});
	ASSERT_EQ(rbt.GetSize(), 10);

	int cnt = 10;
	for (auto i = rbt.GetConstReverseBegin(); i != rbt.GetConstReverseEnd(); ++i)
	{
		ASSERT_EQ(cnt, i->m_First);
		cnt--;
	}

	auto iter = rbt.GetConstReverseBegin();
	for (int i = 10; i > 0; i--)
	{
		ASSERT_EQ(iter->m_First, i);
		ASSERT_EQ((*iter).m_Second, (double)i);
		++iter;
	}
	--iter;
	for (int i = 1; i <= 10; i++)
	{
		ASSERT_EQ(iter->m_First, i);
		ASSERT_EQ((*iter).m_Second, (double)i);
		--iter;
	}
	++iter;
	for (int i = 10; i > 0; i--)
	{
		ASSERT_EQ(iter.GetData()->m_First, i);
		ASSERT_EQ(iter.GetData()->m_Second, (double)i);
		++iter;
	}
}

TEST(RedBlackTreeIterator, PlusTest)
{
	Detail::RedBlackTree<int> rbt({0, 1, 2, 3, 4});
	auto iter1_1 = rbt.GetBegin() + 1;
	ASSERT_EQ(*iter1_1, 1);
	auto iter1_2 = rbt.GetBegin() + 2;
	ASSERT_EQ(*iter1_2, 2);

	auto iter3_1 = rbt.GetReverseBegin() + 1;
	ASSERT_EQ(*iter3_1, 3);
	auto iter3_2 = rbt.GetReverseBegin() + 2;
	ASSERT_EQ(*iter3_2, 2);

	const Detail::RedBlackTree<int>& crbt = rbt;
	auto iter2_1 = crbt.GetConstBegin() + 1;
	ASSERT_EQ(*iter2_1, 1);
	auto iter2_2 = crbt.GetConstBegin() + 2;
	ASSERT_EQ(*iter2_2, 2);

	auto iter4_1 = crbt.GetConstReverseBegin() + 1;
	ASSERT_EQ(*iter4_1, 3);
	auto iter4_2 = crbt.GetConstReverseBegin() + 2;
	ASSERT_EQ(*iter4_2, 2);
}

TEST(RedBlackTreeIterator, SubtractTest)
{
	Detail::RedBlackTree<int> rbt({0, 1, 2, 3, 4});
	auto iter1_1 = rbt.GetEnd() - 1;
	ASSERT_EQ(*iter1_1, 4);
	auto iter1_2 = rbt.GetEnd() - 2;
	ASSERT_EQ(*iter1_2, 3);

	auto iter3_1 = rbt.GetReverseEnd() - 1;
	ASSERT_EQ(*iter3_1, 0);
	auto iter3_2 = rbt.GetReverseEnd() - 2;
	ASSERT_EQ(*iter3_2, 1);

	const Detail::RedBlackTree<int>& crbt = rbt;
	auto iter2_1 = crbt.GetConstEnd() - 1;
	ASSERT_EQ(*iter2_1, 4);
	auto iter2_2 = crbt.GetConstEnd() - 2;
	ASSERT_EQ(*iter2_2, 3);

	auto iter4_1 = crbt.GetConstReverseEnd() - 1;
	ASSERT_EQ(*iter4_1, 0);
	auto iter4_2 = crbt.GetConstReverseEnd() - 2;
	ASSERT_EQ(*iter4_2, 1);
}

TEST(RedBlackTreeIterator, SelfIncreaseTest)
{
	Detail::RedBlackTree<int> rbt({0, 1, 2, 3, 4});
	auto iter1 = rbt.GetBegin();
	ASSERT_EQ(*iter1, 0);
	iter1 += 2;
	ASSERT_EQ(*iter1, 2);

	auto iter3 = rbt.GetReverseBegin();
	ASSERT_EQ(*iter3, 4);
	iter3 += 2;
	ASSERT_EQ(*iter3, 2);

	const Detail::RedBlackTree<int>& crbt = rbt;
	auto iter2 = crbt.GetConstBegin();
	ASSERT_EQ(*iter2, 0);
	iter2 += 2;
	ASSERT_EQ(*iter2, 2);

	auto iter4 = crbt.GetConstReverseBegin();
	ASSERT_EQ(*iter4, 4);
	iter4 += 2;
	ASSERT_EQ(*iter4, 2);
}

TEST(RedBlackTreeIterator, SelfDecreaseTest)
{
	Detail::RedBlackTree<int> rbt({0, 1, 2, 3, 4});
	auto iter1 = rbt.GetEnd();
	iter1 -= 2;
	ASSERT_EQ(*iter1, 3);

	auto iter3 = rbt.GetReverseEnd();
	iter3 -= 2;
	ASSERT_EQ(*iter3, 1);

	const Detail::RedBlackTree<int>& crbt = rbt;
	auto iter2 = crbt.GetConstEnd();
	iter2 -= 2;
	ASSERT_EQ(*iter2, 3);

	auto iter4 = crbt.GetConstReverseEnd();
	iter4 -= 2;
	ASSERT_EQ(*iter4, 1);
}

TEST(RedBlackTreeIterator, DistanceTest)
{
	Detail::RedBlackTree<int> rbt({0, 1, 2, 3, 4});
	ASSERT_EQ(rbt.GetSize(), 5);
	ASSERT_EQ(rbt.GetEnd() - rbt.GetBegin(), 5);
	ASSERT_EQ(rbt.GetReverseEnd() - rbt.GetReverseBegin(), 5);

	const Detail::RedBlackTree<int>& crbt = rbt;
	ASSERT_EQ(crbt.GetSize(), 5);
	ASSERT_EQ(crbt.GetConstEnd() - crbt.GetConstBegin(), 5);
	ASSERT_EQ(crbt.GetConstReverseEnd() - crbt.GetConstReverseBegin(), 5);
}