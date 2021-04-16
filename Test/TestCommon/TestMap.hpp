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
#include "Container/Map.hpp"
#include <functional>
#include <cstring>

using namespace SpaceGameEngine;

struct test_map_object
{
	std::function<void(test_map_object&)> rel_func;
	test_map_object()
		: val(0), rel_func([](test_map_object&) {})
	{
	}
	test_map_object(int v)
		: val(v), rel_func([](test_map_object&) {})
	{
	}
	test_map_object(int v, const std::function<void(test_map_object&)>& func)
		: val(v), rel_func(func)
	{
	}
	test_map_object(const test_map_object& o) noexcept
		: val(o.val), rel_func(o.rel_func)
	{
	}
	test_map_object(test_map_object&& o) noexcept
		: val(o.val), rel_func(std::move(o.rel_func))
	{
		o.rel_func = [](test_map_object&) {};
	}
	~test_map_object()
	{
		rel_func(*this);
	}
	int val;

	test_map_object& operator=(const test_map_object& o)
	{
		val = o.val;
		rel_func = o.rel_func;
		return *this;
	}

	test_map_object& operator=(test_map_object&& o)
	{
		val = o.val;
		rel_func = std::move(o.rel_func);
		o.rel_func = [](test_map_object&) {};
		return *this;
	}

	bool operator<(const test_map_object& o) const
	{
		return val < o.val;
	}

	bool operator==(const test_map_object& o) const
	{
		return val == o.val;
	}

	bool operator!=(const test_map_object& o) const
	{
		return val != o.val;
	}
};

TEST(RedBlackTree, FindValueByKeyTest)
{
	MapImplement::RedBlackTree<int, double> rbt1;
	ASSERT_EQ(rbt1.GetSize(), 0);
	ASSERT_EQ(rbt1.FindValueByKey(0), nullptr);
	ASSERT_EQ(rbt1.FindValueByKey(1), nullptr);

	rbt1.Insert(1, 2.0);
	ASSERT_EQ(*rbt1.FindValueByKey(1), 2.0);
}

TEST(RedBlackTree, InsertTest)
{
	MapImplement::RedBlackTree<int, double> rbt1;
	rbt1.Insert(1, 1.0);
	rbt1.Insert(0, 5.0);
	ASSERT_EQ(rbt1.GetSize(), 2);
	ASSERT_EQ(*rbt1.FindValueByKey(1), 1.0);
	ASSERT_EQ(*rbt1.FindValueByKey(0), 5.0);

	rbt1.Insert(0, 10.0);
	ASSERT_EQ(rbt1.GetSize(), 2);
	ASSERT_EQ(*rbt1.FindValueByKey(0), 10.0);

	ASSERT_EQ(rbt1.FindValueByKey(-1), nullptr);
	rbt1.Insert(-1, 3.0);
	ASSERT_EQ(rbt1.GetSize(), 3);
	ASSERT_EQ(*rbt1.FindValueByKey(-1), 3.0);
}

TEST(RedBlackTree, RemoveTest)
{
	MapImplement::RedBlackTree<int, test_map_object> rbt1;
	int rel_cot = 0;
	int last_val = 0;
	auto rel_func = [&](test_map_object& t) { rel_cot += 1; last_val=t.val; };
	rbt1.Insert(1, test_map_object(1, rel_func));
	rbt1.Insert(0, test_map_object(5, rel_func));

	ASSERT_EQ(rbt1.GetSize(), 2);
	ASSERT_EQ(rbt1.FindValueByKey(1)->val, 1);
	ASSERT_EQ(rbt1.FindValueByKey(0)->val, 5);

	rel_cot = 0;

	ASSERT_FALSE(rbt1.RemoveByKey(2));
	ASSERT_TRUE(rbt1.RemoveByKey(0));
	ASSERT_EQ(rel_cot, 1);
	ASSERT_EQ(last_val, 5);
	ASSERT_EQ(rbt1.GetSize(), 1);
	ASSERT_EQ(rbt1.FindValueByKey(0), nullptr);
	ASSERT_EQ(rbt1.FindValueByKey(1)->val, 1);
}

TEST(RedBlackTree, ReleaseTest)
{
	const int test_size = 1000;
	int key_pool[test_size];
	int val_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	auto key_rel_func = [&](test_map_object& o) {
		key_pool[o.val] += 1;
	};
	auto val_rel_func = [&](test_map_object& o) {
		val_pool[o.val] += 1;
	};
	SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object>* prbt = new SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object>();
	for (int i = 0; i < test_size; i++)
	{
		prbt->Insert(test_map_object(i, key_rel_func), test_map_object(i, val_rel_func));
	}
	ASSERT_EQ(prbt->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(prbt->FindValueByKey(test_map_object(i))->val, i);
	}
	delete prbt;
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 1);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(RedBlackTree, ClearTest)
{
	SpaceGameEngine::MapImplement::RedBlackTree<int, double> m;
	m.Insert(1, 1.0);
	m.Insert(2, 2.0);
	ASSERT_EQ(m.GetSize(), 2);
	ASSERT_EQ(*m.FindValueByKey(1), 1.0);
	ASSERT_EQ(*m.FindValueByKey(2), 2.0);

	m.Clear();

	ASSERT_EQ(m.GetSize(), 0);
	ASSERT_EQ(m.FindValueByKey(1), nullptr);
	ASSERT_EQ(m.FindValueByKey(2), nullptr);

	m.Insert(3, 3.0);

	ASSERT_EQ(m.GetSize(), 1);
	ASSERT_EQ(m.FindValueByKey(1), nullptr);
	ASSERT_EQ(m.FindValueByKey(2), nullptr);
	ASSERT_EQ(*m.FindValueByKey(3), 3.0);
}

TEST(RedBlackTree, CopyConstructionTest)
{
	const int test_size = 1000;
	int key_pool[test_size];
	int val_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	auto key_rel_func = [&](test_map_object& o) {
		key_pool[o.val] += 1;
	};
	auto val_rel_func = [&](test_map_object& o) {
		val_pool[o.val] += 1;
	};
	SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object>* pm1 = new SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object>();
	for (int i = 0; i < test_size; i++)
	{
		pm1->Insert(test_map_object(i, key_rel_func), test_map_object(i, val_rel_func));
	}
	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->FindValueByKey(test_map_object(i))->val, i);
	}

	SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object>* pm2 = new SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object>(*pm1);

	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->FindValueByKey(test_map_object(i))->val, i);
	}

	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm2->FindValueByKey(test_map_object(i))->val, i);
	}

	delete pm1;
	delete pm2;

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 2);
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(RedBlackTree, MoveConstructionTest)
{
	const int test_size = 1000;
	int key_pool[test_size];
	int val_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	auto key_rel_func = [&](test_map_object& o) {
		key_pool[o.val] += 1;
	};
	auto val_rel_func = [&](test_map_object& o) {
		val_pool[o.val] += 1;
	};
	SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object>* pm1 = new SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object>();
	for (int i = 0; i < test_size; i++)
	{
		pm1->Insert(test_map_object(i, key_rel_func), test_map_object(i, val_rel_func));
	}
	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->FindValueByKey(test_map_object(i))->val, i);
	}

	SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object>* pm2 = new SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object>(std::move(*pm1));

	ASSERT_EQ(pm1->GetSize(), 0);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->FindValueByKey(test_map_object(i)), nullptr);
	}

	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm2->FindValueByKey(test_map_object(i))->val, i);
	}

	delete pm1;
	delete pm2;

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 1);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(RedBlackTree, CopyAssignmentTest)
{
	const int test_size = 1000;
	int key_pool[test_size];
	int val_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	auto key_rel_func = [&](test_map_object& o) {
		key_pool[o.val] += 1;
	};
	auto val_rel_func = [&](test_map_object& o) {
		val_pool[o.val] += 1;
	};
	SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object>* pm1 = new SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object>();
	SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object>* pm2 = new SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object>();
	for (int i = 0; i < test_size; i++)
	{
		pm1->Insert(test_map_object(i, key_rel_func), test_map_object(i, val_rel_func));
	}
	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->FindValueByKey(test_map_object(i))->val, i);
	}

	*pm2 = *pm1;

	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->FindValueByKey(test_map_object(i))->val, i);
	}

	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm2->FindValueByKey(test_map_object(i))->val, i);
	}

	delete pm1;
	delete pm2;

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 2);
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(RedBlackTree, MoveAssignmentTest)
{
	const int test_size = 1000;
	int key_pool[test_size];
	int val_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	auto key_rel_func = [&](test_map_object& o) {
		key_pool[o.val] += 1;
	};
	auto val_rel_func = [&](test_map_object& o) {
		val_pool[o.val] += 1;
	};
	SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object>* pm1 = new SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object>();
	SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object>* pm2 = new SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object>();
	for (int i = 0; i < test_size; i++)
	{
		pm1->Insert(test_map_object(i, key_rel_func), test_map_object(i, val_rel_func));
	}
	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->FindValueByKey(test_map_object(i))->val, i);
	}

	*pm2 = std::move(*pm1);

	ASSERT_EQ(pm1->GetSize(), 0);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->FindValueByKey(test_map_object(i)), nullptr);
	}

	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm2->FindValueByKey(test_map_object(i))->val, i);
	}

	delete pm1;
	delete pm2;

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 1);
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(RedBlackTree, AnotherAllocatorCopyConstructionTest)
{
	const int test_size = 1000;
	int key_pool[test_size];
	int val_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	auto key_rel_func = [&](test_map_object& o) {
		key_pool[o.val] += 1;
	};
	auto val_rel_func = [&](test_map_object& o) {
		val_pool[o.val] += 1;
	};
	SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object>* pm1 = new SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object>();
	for (int i = 0; i < test_size; i++)
	{
		pm1->Insert(test_map_object(i, key_rel_func), test_map_object(i, val_rel_func));
	}
	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->FindValueByKey(test_map_object(i))->val, i);
	}

	SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object, Less<test_map_object>, StdAllocator>* pm2 = new SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object, Less<test_map_object>, StdAllocator>(*pm1);

	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->FindValueByKey(test_map_object(i))->val, i);
	}

	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm2->FindValueByKey(test_map_object(i))->val, i);
	}

	delete pm1;
	delete pm2;

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 2);
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(RedBlackTree, AnotherAllocatorMoveConstructionTest)
{
	const int test_size = 1000;
	int key_pool[test_size];
	int val_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	auto key_rel_func = [&](test_map_object& o) {
		key_pool[o.val] += 1;
	};
	auto val_rel_func = [&](test_map_object& o) {
		val_pool[o.val] += 1;
	};
	SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object>* pm1 = new SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object>();
	for (int i = 0; i < test_size; i++)
	{
		pm1->Insert(test_map_object(i, key_rel_func), test_map_object(i, val_rel_func));
	}
	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->FindValueByKey(test_map_object(i))->val, i);
	}

	SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object, Less<test_map_object>, StdAllocator>* pm2 = new SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object, Less<test_map_object>, StdAllocator>(std::move(*pm1));

	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->FindValueByKey(test_map_object(i))->val, i);
	}

	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm2->FindValueByKey(test_map_object(i))->val, i);
	}

	delete pm1;
	delete pm2;

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 2);	  //because const key can not be moved
		ASSERT_EQ(val_pool[i], 1);
	}
}

TEST(RedBlackTree, AnotherAllocatorCopyAssignmentTest)
{
	const int test_size = 1000;
	int key_pool[test_size];
	int val_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	auto key_rel_func = [&](test_map_object& o) {
		key_pool[o.val] += 1;
	};
	auto val_rel_func = [&](test_map_object& o) {
		val_pool[o.val] += 1;
	};
	SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object>* pm1 = new SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object>();
	SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object, Less<test_map_object>, StdAllocator>* pm2 = new SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object, Less<test_map_object>, StdAllocator>();
	for (int i = 0; i < test_size; i++)
	{
		pm1->Insert(test_map_object(i, key_rel_func), test_map_object(i, val_rel_func));
	}
	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->FindValueByKey(test_map_object(i))->val, i);
	}

	*pm2 = *pm1;

	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->FindValueByKey(test_map_object(i))->val, i);
	}

	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm2->FindValueByKey(test_map_object(i))->val, i);
	}

	delete pm1;
	delete pm2;

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 2);
		ASSERT_EQ(val_pool[i], 2);
	}
}

TEST(RedBlackTree, AnotherAllocatorMoveAssignmentTest)
{
	const int test_size = 1000;
	int key_pool[test_size];
	int val_pool[test_size];
	memset(key_pool, 0, sizeof(key_pool));
	memset(val_pool, 0, sizeof(val_pool));
	auto key_rel_func = [&](test_map_object& o) {
		key_pool[o.val] += 1;
	};
	auto val_rel_func = [&](test_map_object& o) {
		val_pool[o.val] += 1;
	};
	SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object>* pm1 = new SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object>();
	SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object, Less<test_map_object>, StdAllocator>* pm2 = new SpaceGameEngine::MapImplement::RedBlackTree<test_map_object, test_map_object, Less<test_map_object>, StdAllocator>();
	for (int i = 0; i < test_size; i++)
	{
		pm1->Insert(test_map_object(i, key_rel_func), test_map_object(i, val_rel_func));
	}
	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->FindValueByKey(test_map_object(i))->val, i);
	}

	*pm2 = std::move(*pm1);

	ASSERT_EQ(pm1->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm1->FindValueByKey(test_map_object(i))->val, i);
	}

	ASSERT_EQ(pm2->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ(pm2->FindValueByKey(test_map_object(i))->val, i);
	}

	delete pm1;
	delete pm2;

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(key_pool[i], 2);	  //because const key can not be moved
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
	SpaceGameEngine::MapImplement::RedBlackTree<int, double> m1;
	ASSERT_EQ(m1.GetSize(), 0);
	for (int i = 0; i < test_size; i++)
	{
		m1.Insert(i, 1);
	}
	ASSERT_EQ(m1.GetSize(), test_size);

	m1.ForEach([](Pair<const int, double>& d) {
		d.m_Second = d.m_First;
	});

	ASSERT_EQ(m1.GetSize(), test_size);
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(*m1.FindValueByKey(i), (double)i);
	}

	const SpaceGameEngine::MapImplement::RedBlackTree<int, double> m2(m1);
	ASSERT_EQ(m2.GetSize(), test_size);
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(*m2.FindValueByKey(i), (double)i);
	}

	m2.ForEach([&](const Pair<const int, double>& d) {
		ASSERT_EQ(d.m_First, d.m_Second);
		test_cnt[d.m_First] += 1;
	});

	ASSERT_EQ(m2.GetSize(), test_size);
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(*m2.FindValueByKey(i), (double)i);
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
	SpaceGameEngine::MapImplement::RedBlackTree<int, double> m1;
	ASSERT_EQ(m1.GetSize(), 0);
	for (int i = 0; i < test_size; i++)
	{
		m1.Insert(i, 1);
	}
	ASSERT_EQ(m1.GetSize(), test_size);

	m1.ReverseForEach([](Pair<const int, double>& d) {
		d.m_Second = d.m_First;
	});

	ASSERT_EQ(m1.GetSize(), test_size);
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(*m1.FindValueByKey(i), (double)i);
	}

	const SpaceGameEngine::MapImplement::RedBlackTree<int, double> m2(m1);
	ASSERT_EQ(m2.GetSize(), test_size);
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(*m2.FindValueByKey(i), (double)i);
	}

	m2.ReverseForEach([&](const Pair<const int, double>& d) {
		ASSERT_EQ(d.m_First, d.m_Second);
		test_cnt[d.m_First] += 1;
	});

	ASSERT_EQ(m2.GetSize(), test_size);
	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(*m2.FindValueByKey(i), (double)i);
	}

	for (int i = 0; i < test_size; i++)
	{
		ASSERT_EQ(test_cnt[i], 1);
	}
}

TEST(Map, InitializerListConstructionTest)
{
	Map<int, double> m({{1, 1.0},
						{2, 2.0},
						{3, 3.0}});
	ASSERT_EQ(m.GetSize(), 3);
}

TEST(MapIterator, GetBeginTest)
{
	Map<int, double> m({{1, 1.0},
						{2, 2.0},
						{3, 3.0}});
	ASSERT_EQ(m.GetSize(), 3);
	auto iter = m.GetBegin();
	ASSERT_TRUE((std::is_same_v<decltype(iter), Map<int, double>::Iterator>));
	ASSERT_EQ(iter->m_First, 1);
	ASSERT_EQ(iter->m_Second, 1.0);
}

TEST(MapIterator, GetEndTest)
{
	Map<int, double> m({{1, 1.0},
						{2, 2.0},
						{3, 3.0}});
	ASSERT_EQ(m.GetSize(), 3);
	auto iter = m.GetEnd();
	ASSERT_TRUE((std::is_same_v<decltype(iter), Map<int, double>::Iterator>));
	--iter;
	ASSERT_EQ(iter->m_First, 3);
	ASSERT_EQ(iter->m_Second, 3.0);
}

TEST(MapIterator, GetConstBeginTest)
{
	Map<int, double> m({{1, 1.0},
						{2, 2.0},
						{3, 3.0}});
	ASSERT_EQ(m.GetSize(), 3);
	auto iter = m.GetConstBegin();
	ASSERT_TRUE((std::is_same_v<decltype(iter), Map<int, double>::ConstIterator>));
	ASSERT_EQ(iter->m_First, 1);
	ASSERT_EQ(iter->m_Second, 1.0);
}

TEST(MapIterator, GetConstEndTest)
{
	Map<int, double> m({{1, 1.0},
						{2, 2.0},
						{3, 3.0}});
	ASSERT_EQ(m.GetSize(), 3);
	auto iter = m.GetConstEnd();
	ASSERT_TRUE((std::is_same_v<decltype(iter), Map<int, double>::ConstIterator>));
	--iter;
	ASSERT_EQ(iter->m_First, 3);
	ASSERT_EQ(iter->m_Second, 3.0);
}

TEST(MapIterator, GetReverseBeginTest)
{
	Map<int, double> m({{1, 1.0},
						{2, 2.0},
						{3, 3.0}});
	ASSERT_EQ(m.GetSize(), 3);
	auto iter = m.GetReverseBegin();
	ASSERT_TRUE((std::is_same_v<decltype(iter), Map<int, double>::ReverseIterator>));
	ASSERT_EQ(iter->m_First, 3);
	ASSERT_EQ(iter->m_Second, 3.0);
}

TEST(MapIterator, GetReverseEndTest)
{
	Map<int, double> m({{1, 1.0},
						{2, 2.0},
						{3, 3.0}});
	ASSERT_EQ(m.GetSize(), 3);
	auto iter = m.GetReverseEnd();
	ASSERT_TRUE((std::is_same_v<decltype(iter), Map<int, double>::ReverseIterator>));
	--iter;
	ASSERT_EQ(iter->m_First, 1);
	ASSERT_EQ(iter->m_Second, 1.0);
}

TEST(MapIterator, GetConstReverseBeginTest)
{
	Map<int, double> m({{1, 1.0},
						{2, 2.0},
						{3, 3.0}});
	ASSERT_EQ(m.GetSize(), 3);
	auto iter = m.GetConstReverseBegin();
	ASSERT_TRUE((std::is_same_v<decltype(iter), Map<int, double>::ConstReverseIterator>));
	ASSERT_EQ(iter->m_First, 3);
	ASSERT_EQ(iter->m_Second, 3.0);
}

TEST(MapIterator, GetConstReverseEndTest)
{
	Map<int, double> m({{1, 1.0},
						{2, 2.0},
						{3, 3.0}});
	ASSERT_EQ(m.GetSize(), 3);
	auto iter = m.GetConstReverseEnd();
	ASSERT_TRUE((std::is_same_v<decltype(iter), Map<int, double>::ConstReverseIterator>));
	--iter;
	ASSERT_EQ(iter->m_First, 1);
	ASSERT_EQ(iter->m_Second, 1.0);
}

TEST(MapIterator, IteratorTest)
{
	Map<int, double> m({{1, 1.0},
						{2, 2.0},
						{3, 3.0},
						{4, 4.0},
						{5, 5.0},
						{6, 6.0},
						{7, 7.0},
						{8, 8.0},
						{9, 9.0},
						{10, 10.0}});
	ASSERT_EQ(m.GetSize(), 10);

	int cnt = 1;
	for (auto i = m.GetBegin(); i != m.GetEnd(); ++i)
	{
		ASSERT_EQ(cnt, i->m_First);
		(*i).m_Second = 0;
		cnt++;
	}

	auto iter = m.GetBegin();
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
		ASSERT_EQ(iter->m_First, i);
		ASSERT_EQ((*iter).m_Second, 0);
		++iter;
	}
}

TEST(MapIterator, ConstIteratorTest)
{
	const Map<int, double> m({{1, 1.0},
							  {2, 2.0},
							  {3, 3.0},
							  {4, 4.0},
							  {5, 5.0},
							  {6, 6.0},
							  {7, 7.0},
							  {8, 8.0},
							  {9, 9.0},
							  {10, 10.0}});
	ASSERT_EQ(m.GetSize(), 10);

	int cnt = 1;
	for (auto i = m.GetConstBegin(); i != m.GetConstEnd(); ++i)
	{
		ASSERT_EQ(cnt, i->m_First);
		cnt++;
	}

	auto iter = m.GetConstBegin();
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
		ASSERT_EQ(iter->m_First, i);
		ASSERT_EQ((*iter).m_Second, (double)i);
		++iter;
	}
}

TEST(MapIterator, ReverseIteratorTest)
{
	Map<int, double> m({{1, 1.0},
						{2, 2.0},
						{3, 3.0},
						{4, 4.0},
						{5, 5.0},
						{6, 6.0},
						{7, 7.0},
						{8, 8.0},
						{9, 9.0},
						{10, 10.0}});
	ASSERT_EQ(m.GetSize(), 10);

	int cnt = 10;
	for (auto i = m.GetReverseBegin(); i != m.GetReverseEnd(); ++i)
	{
		ASSERT_EQ(cnt, i->m_First);
		(*i).m_Second = 0;
		cnt--;
	}

	auto iter = m.GetReverseBegin();
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
		ASSERT_EQ(iter->m_First, i);
		ASSERT_EQ((*iter).m_Second, 0);
		++iter;
	}
}

TEST(MapIterator, ConstReverseIteratorTest)
{
	const Map<int, double> m({{1, 1.0},
							  {2, 2.0},
							  {3, 3.0},
							  {4, 4.0},
							  {5, 5.0},
							  {6, 6.0},
							  {7, 7.0},
							  {8, 8.0},
							  {9, 9.0},
							  {10, 10.0}});
	ASSERT_EQ(m.GetSize(), 10);

	int cnt = 10;
	for (auto i = m.GetConstReverseBegin(); i != m.GetConstReverseEnd(); ++i)
	{
		ASSERT_EQ(cnt, i->m_First);
		cnt--;
	}

	auto iter = m.GetConstReverseBegin();
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
		ASSERT_EQ(iter->m_First, i);
		ASSERT_EQ((*iter).m_Second, (double)i);
		++iter;
	}
}