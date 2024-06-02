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
		: val(0), rel_func([](test_rbtree_object&) {})
	{
	}
	test_rbtree_object(int v)
		: val(v), rel_func([](test_rbtree_object&) {})
	{
	}
	test_rbtree_object(int v, const std::function<void(test_rbtree_object&)>& func)
		: val(v), rel_func(func)
	{
	}
	test_rbtree_object(const test_rbtree_object& o) noexcept
		: val(o.val), rel_func(o.rel_func)
	{
	}
	test_rbtree_object(test_rbtree_object&& o) noexcept
		: val(o.val), rel_func(std::move(o.rel_func))
	{
		o.rel_func = [](test_rbtree_object&) {};
	}
	~test_rbtree_object()
	{
		rel_func(*this);
	}
	int val;

	test_rbtree_object& operator=(const test_rbtree_object& o)
	{
		val = o.val;
		rel_func = o.rel_func;
		return *this;
	}

	test_rbtree_object& operator=(test_rbtree_object&& o)
	{
		val = o.val;
		rel_func = std::move(o.rel_func);
		o.rel_func = [](test_rbtree_object&) {};
		return *this;
	}

	bool operator<(const test_rbtree_object& o) const
	{
		return val < o.val;
	}

	bool operator==(const test_rbtree_object& o) const
	{
		return val == o.val;
	}

	bool operator!=(const test_rbtree_object& o) const
	{
		return val != o.val;
	}
};

bool operator==(const test_rbtree_object& o, int val)
{
	return o.val == val;
}

bool operator==(int val, const test_rbtree_object& o)
{
	return o.val == val;
}

TEST(RedBlackTree, FindTest)
{
	Detail::RedBlackTree<int> rbt1;
	ASSERT_EQ(rbt1.GetSize(), 0);
	ASSERT_EQ(rbt1.Find(0), nullptr);
	ASSERT_EQ(rbt1.Find(1), nullptr);

	rbt1.Insert(1);
	ASSERT_EQ(*rbt1.Find(1), 1);
}

TEST(RedBlackTree, InsertTest)
{
	Detail::RedBlackTree<Pair<const int, double>, KeyLess<Pair<const int, double>>, KeyEqual<Pair<const int, double>>> rbt1;
	auto res1 = rbt1.Insert(MakePair(1, 1.0));
	ASSERT_EQ(*(res1.m_First), MakePair(1, 1.0));
	ASSERT_TRUE(res1.m_Second);
	auto res2 = rbt1.Insert(MakePair(0, 0.0));
	ASSERT_EQ(*(res2.m_First), MakePair(0, 0.0));
	ASSERT_TRUE(res2.m_Second);
	ASSERT_EQ(rbt1.GetSize(), 2);
	ASSERT_EQ(*rbt1.Find(1), MakePair(1, 1.0));
	ASSERT_EQ(*rbt1.Find(0), MakePair(0, 0.0));

	auto res3 = rbt1.Insert(MakePair(0, 100.0));
	ASSERT_EQ(*(res3.m_First), MakePair(0, 0.0));
	ASSERT_FALSE(res3.m_Second);
	ASSERT_EQ(rbt1.GetSize(), 2);
	ASSERT_EQ(*rbt1.Find(0), MakePair(0, 0.0));

	ASSERT_EQ(rbt1.Find(-1), nullptr);
	auto res4 = rbt1.Insert(MakePair(-1, -1.0));
	ASSERT_EQ(*(res4.m_First), MakePair(-1, -1.0));
	ASSERT_TRUE(res4.m_Second);
	ASSERT_EQ(rbt1.GetSize(), 3);
	ASSERT_EQ(*rbt1.Find(-1), MakePair(-1, -1.0));
}

TEST(RedBlackTree, RemoveTest)
{
	Detail::RedBlackTree<test_rbtree_object> rbt1;
	int rel_cot = 0;
	int last_val = 0;
	auto rel_func = [&](test_rbtree_object& t) { rel_cot += 1; last_val=t.val; };
	rbt1.Insert(test_rbtree_object(1, rel_func));
	rbt1.Insert(test_rbtree_object(5, rel_func));

	ASSERT_EQ(rbt1.GetSize(), 2);
	ASSERT_EQ(rbt1.Find(1)->val, 1);
	ASSERT_EQ(rbt1.Find(5)->val, 5);

	rel_cot = 0;

	ASSERT_FALSE(rbt1.Remove(2));
	ASSERT_TRUE(rbt1.Remove(5));
	ASSERT_EQ(rel_cot, 1);
	ASSERT_EQ(last_val, 5);
	ASSERT_EQ(rbt1.GetSize(), 1);
	ASSERT_EQ(rbt1.Find(0), nullptr);
	ASSERT_EQ(rbt1.Find(1)->val, 1);
}

TEST(RedBlackTree, ReleaseTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_rbtree_object& o) {
		val_pool[o.val] += 1;
	};
	Detail::RedBlackTree<test_rbtree_object>* prbt = new Detail::RedBlackTree<test_rbtree_object>();
	for (int i = 0; i < test_size; i++)
	{
		prbt->Insert(test_rbtree_object(i, val_rel_func));
	}
	ASSERT_EQ(prbt->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(prbt->Find(test_rbtree_object(i))->val, i);
	}
	delete prbt;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(RedBlackTree, ClearTest)
{
	Detail::RedBlackTree<int> m;
	m.Insert(1);
	m.Insert(2);
	ASSERT_EQ(m.GetSize(), 2);
	ASSERT_EQ(*m.Find(1), 1);
	ASSERT_EQ(*m.Find(2), 2);

	m.Clear();

	ASSERT_EQ(m.GetSize(), 0);
	ASSERT_EQ(m.Find(1), nullptr);
	ASSERT_EQ(m.Find(2), nullptr);

	m.Insert(3);

	ASSERT_EQ(m.GetSize(), 1);
	ASSERT_EQ(m.Find(1), nullptr);
	ASSERT_EQ(m.Find(2), nullptr);
	ASSERT_EQ(*m.Find(3), 3);
}

TEST(RedBlackTree, CopyConstructionTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_rbtree_object& o) {
		val_pool[o.val] += 1;
	};
	Detail::RedBlackTree<test_rbtree_object>* pm1 = new Detail::RedBlackTree<test_rbtree_object>();
	for (int i = 0; i < test_size; i++)
	{
		pm1->Insert(test_rbtree_object(i, val_rel_func));
	}
	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->Find(test_rbtree_object(i))->val, i);
	}

	Detail::RedBlackTree<test_rbtree_object>* pm2 = new Detail::RedBlackTree<test_rbtree_object>(*pm1);

	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->Find(test_rbtree_object(i))->val, i);
	}

	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm2->Find(test_rbtree_object(i))->val, i);
	}

	delete pm1;
	delete pm2;

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(RedBlackTree, MoveConstructionTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_rbtree_object& o) {
		val_pool[o.val] += 1;
	};
	Detail::RedBlackTree<test_rbtree_object>* pm1 = new Detail::RedBlackTree<test_rbtree_object>();
	for (int i = 0; i < test_size; i++)
	{
		pm1->Insert(test_rbtree_object(i, val_rel_func));
	}
	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->Find(test_rbtree_object(i))->val, i);
	}

	Detail::RedBlackTree<test_rbtree_object>* pm2 = new Detail::RedBlackTree<test_rbtree_object>(std::move(*pm1));

	ASSERT_EQ(pm1->GetSize(), 0);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->Find(test_rbtree_object(i)), nullptr);
	}

	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm2->Find(test_rbtree_object(i))->val, i);
	}

	delete pm1;
	delete pm2;

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(RedBlackTree, CopyAssignmentTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_rbtree_object& o) {
		val_pool[o.val] += 1;
	};
	Detail::RedBlackTree<test_rbtree_object>* pm1 = new Detail::RedBlackTree<test_rbtree_object>();
	Detail::RedBlackTree<test_rbtree_object>* pm2 = new Detail::RedBlackTree<test_rbtree_object>();
	for (int i = 0; i < test_size; i++)
	{
		pm1->Insert(test_rbtree_object(i, val_rel_func));
	}
	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->Find(test_rbtree_object(i))->val, i);
	}

	*pm2 = *pm1;

	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->Find(test_rbtree_object(i))->val, i);
	}

	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm2->Find(test_rbtree_object(i))->val, i);
	}

	delete pm1;
	delete pm2;

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(RedBlackTree, MoveAssignmentTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_rbtree_object& o) {
		val_pool[o.val] += 1;
	};
	Detail::RedBlackTree<test_rbtree_object>* pm1 = new Detail::RedBlackTree<test_rbtree_object>();
	Detail::RedBlackTree<test_rbtree_object>* pm2 = new Detail::RedBlackTree<test_rbtree_object>();
	for (int i = 0; i < test_size; i++)
	{
		pm1->Insert(test_rbtree_object(i, val_rel_func));
	}
	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->Find(test_rbtree_object(i))->val, i);
	}

	*pm2 = std::move(*pm1);

	ASSERT_EQ(pm1->GetSize(), 0);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->Find(test_rbtree_object(i)), nullptr);
	}

	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm2->Find(test_rbtree_object(i))->val, i);
	}

	delete pm1;
	delete pm2;

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(RedBlackTree, AnotherAllocatorCopyConstructionTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_rbtree_object& o) {
		val_pool[o.val] += 1;
	};
	Detail::RedBlackTree<test_rbtree_object>* pm1 = new Detail::RedBlackTree<test_rbtree_object>();
	for (int i = 0; i < test_size; i++)
	{
		pm1->Insert(test_rbtree_object(i, val_rel_func));
	}
	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->Find(test_rbtree_object(i))->val, i);
	}

	Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, StdAllocator>* pm2 = new Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, StdAllocator>(*pm1);

	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->Find(test_rbtree_object(i))->val, i);
	}

	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm2->Find(test_rbtree_object(i))->val, i);
	}

	delete pm1;
	delete pm2;

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(RedBlackTree, AnotherAllocatorMoveConstructionTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_rbtree_object& o) {
		val_pool[o.val] += 1;
	};
	Detail::RedBlackTree<test_rbtree_object>* pm1 = new Detail::RedBlackTree<test_rbtree_object>();
	for (int i = 0; i < test_size; i++)
	{
		pm1->Insert(test_rbtree_object(i, val_rel_func));
	}
	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->Find(test_rbtree_object(i))->val, i);
	}

	Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, StdAllocator>* pm2 = new Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, StdAllocator>(std::move(*pm1));

	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->Find(test_rbtree_object(i))->val, i);
	}

	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm2->Find(test_rbtree_object(i))->val, i);
	}

	delete pm1;
	delete pm2;

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(RedBlackTree, AnotherAllocatorCopyAssignmentTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_rbtree_object& o) {
		val_pool[o.val] += 1;
	};
	Detail::RedBlackTree<test_rbtree_object>* pm1 = new Detail::RedBlackTree<test_rbtree_object>();
	Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, StdAllocator>* pm2 = new Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, StdAllocator>();
	for (int i = 0; i < test_size; i++)
	{
		pm1->Insert(test_rbtree_object(i, val_rel_func));
	}
	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->Find(test_rbtree_object(i))->val, i);
	}

	*pm2 = *pm1;

	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->Find(test_rbtree_object(i))->val, i);
	}

	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm2->Find(test_rbtree_object(i))->val, i);
	}

	delete pm1;
	delete pm2;

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(RedBlackTree, AnotherAllocatorMoveAssignmentTest)
{
	const int test_size = 1000;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_rbtree_object& o) {
		val_pool[o.val] += 1;
	};
	Detail::RedBlackTree<test_rbtree_object>* pm1 = new Detail::RedBlackTree<test_rbtree_object>();
	Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, StdAllocator>* pm2 = new Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, StdAllocator>();
	for (int i = 0; i < test_size; i++)
	{
		pm1->Insert(test_rbtree_object(i, val_rel_func));
	}
	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->Find(test_rbtree_object(i))->val, i);
	}

	*pm2 = std::move(*pm1);

	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->Find(test_rbtree_object(i))->val, i);
	}

	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm2->Find(test_rbtree_object(i))->val, i);
	}

	delete pm1;
	delete pm2;

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
	Detail::RedBlackTree<Pair<const int, double>, KeyLess<Pair<const int, double>>, KeyEqual<Pair<const int, double>>> m1;
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

	const Detail::RedBlackTree<Pair<const int, double>, KeyLess<Pair<const int, double>>, KeyEqual<Pair<const int, double>>> m2(m1);
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
	Detail::RedBlackTree<Pair<const int, double>, KeyLess<Pair<const int, double>>, KeyEqual<Pair<const int, double>>> m1;
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

	const Detail::RedBlackTree<Pair<const int, double>, KeyLess<Pair<const int, double>>, KeyEqual<Pair<const int, double>>> m2(m1);
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

TEST(RedBlackTree, SwapTest)
{
	const int test_size = 1000;
	int val_pool[2 * test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_rbtree_object& o) {
		val_pool[o.val] += 1;
	};
	Detail::RedBlackTree<test_rbtree_object>* pm1 = new Detail::RedBlackTree<test_rbtree_object>();
	Detail::RedBlackTree<test_rbtree_object>* pm2 = new Detail::RedBlackTree<test_rbtree_object>();
	for (int i = 0; i < test_size; i++)
	{
		pm1->Insert(test_rbtree_object(i, val_rel_func));
	}
	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->Find(test_rbtree_object(i))->val, i);
	}

	for (int i = test_size; i < 2 * test_size; i++)
	{
		pm2->Insert(test_rbtree_object(i, val_rel_func));
	}
	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = 2 * test_size - 1; i >= test_size; i--)
	{
		ASSERT_EQ(pm2->Find(test_rbtree_object(i))->val, i);
	}

	Detail::RedBlackTree<test_rbtree_object>* pm3 = new Detail::RedBlackTree<test_rbtree_object>(std::move(*pm1));

	ASSERT_EQ(pm3->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm3->Find(test_rbtree_object(i))->val, i);
	}

	*pm1 = std::move(*pm2);

	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = 2 * test_size - 1; i >= test_size; i--)
	{
		ASSERT_EQ(pm1->Find(test_rbtree_object(i))->val, i);
	}

	*pm2 = std::move(*pm3);

	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm2->Find(test_rbtree_object(i))->val, i);
	}

	delete pm1;
	delete pm2;
	delete pm3;

	for (int i = 0; i < 2 * test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(RedBlackTree, AnotherAllocatorSwapTest)
{
	const int test_size = 1000;
	int val_pool[2 * test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_rbtree_object& o) {
		val_pool[o.val] += 1;
	};
	Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, MemoryManagerAllocator>* pm1 = new Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, MemoryManagerAllocator>();
	Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, StdAllocator>* pm2 = new Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, StdAllocator>();
	for (int i = 0; i < test_size; i++)
	{
		pm1->Insert(test_rbtree_object(i, val_rel_func));
	}
	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->Find(test_rbtree_object(i))->val, i);
	}

	for (int i = test_size; i < 2 * test_size; i++)
	{
		pm2->Insert(test_rbtree_object(i, val_rel_func));
	}
	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = 2 * test_size - 1; i >= test_size; i--)
	{
		ASSERT_EQ(pm2->Find(test_rbtree_object(i))->val, i);
	}

	Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, MemoryManagerAllocator>* pm3 = new Detail::RedBlackTree<test_rbtree_object, Less<test_rbtree_object>, Equal<test_rbtree_object>, MemoryManagerAllocator>(std::move(*pm1));

	ASSERT_EQ(pm3->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm3->Find(test_rbtree_object(i))->val, i);
	}

	*pm1 = std::move(*pm2);

	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = 2 * test_size - 1; i >= test_size; i--)
	{
		ASSERT_EQ(pm1->Find(test_rbtree_object(i))->val, i);
	}

	*pm2 = std::move(*pm3);

	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm2->Find(test_rbtree_object(i))->val, i);
	}

	delete pm1;
	delete pm2;
	delete pm3;

	for (int i = 0; i < 2 * test_size; i++)
	{
		ASSERT_EQ(val_pool[i], 1);
	}
}
