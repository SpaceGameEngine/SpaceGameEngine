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
		ASSERT_EQ(key_pool[i], 2);	  //because Insert make a temporary instance
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
	SpaceGameEngine::MapImplement::RedBlackTree<int, double> m1;
	m1.Insert(1, 1.0);
	m1.Insert(2, 2.0);
	ASSERT_EQ(m1.GetSize(), 2);
	ASSERT_EQ(*m1.FindValueByKey(1), 1.0);
	ASSERT_EQ(*m1.FindValueByKey(2), 2.0);

	SpaceGameEngine::MapImplement::RedBlackTree<int, double> m2(m1);

	ASSERT_EQ(m1.GetSize(), 2);
	ASSERT_EQ(*m1.FindValueByKey(1), 1.0);
	ASSERT_EQ(*m1.FindValueByKey(2), 2.0);

	ASSERT_EQ(m2.GetSize(), 2);
	ASSERT_EQ(*m2.FindValueByKey(1), 1.0);
	ASSERT_EQ(*m2.FindValueByKey(2), 2.0);
}

TEST(RedBlackTree, MoveConstructionTest)
{
	SpaceGameEngine::MapImplement::RedBlackTree<int, double> m1;
	m1.Insert(1, 1.0);
	m1.Insert(2, 2.0);
	ASSERT_EQ(m1.GetSize(), 2);
	ASSERT_EQ(*m1.FindValueByKey(1), 1.0);
	ASSERT_EQ(*m1.FindValueByKey(2), 2.0);

	SpaceGameEngine::MapImplement::RedBlackTree<int, double> m2(std::move(m1));

	ASSERT_EQ(m1.GetSize(), 0);
	ASSERT_EQ(m1.FindValueByKey(1), nullptr);
	ASSERT_EQ(m1.FindValueByKey(2), nullptr);

	ASSERT_EQ(m2.GetSize(), 2);
	ASSERT_EQ(*m2.FindValueByKey(1), 1.0);
	ASSERT_EQ(*m2.FindValueByKey(2), 2.0);
}

TEST(RedBlackTree, CopyAssignmentTest)
{
	SpaceGameEngine::MapImplement::RedBlackTree<int, double> m1;
	SpaceGameEngine::MapImplement::RedBlackTree<int, double> m2;

	m1.Insert(1, 1.0);
	m1.Insert(2, 2.0);
	ASSERT_EQ(m1.GetSize(), 2);
	ASSERT_EQ(*m1.FindValueByKey(1), 1.0);
	ASSERT_EQ(*m1.FindValueByKey(2), 2.0);
	ASSERT_EQ(m2.GetSize(), 0);
	ASSERT_EQ(m2.FindValueByKey(1), nullptr);
	ASSERT_EQ(m2.FindValueByKey(2), nullptr);

	m2 = m1;

	ASSERT_EQ(m1.GetSize(), 2);
	ASSERT_EQ(*m1.FindValueByKey(1), 1.0);
	ASSERT_EQ(*m1.FindValueByKey(2), 2.0);

	ASSERT_EQ(m2.GetSize(), 2);
	ASSERT_EQ(*m2.FindValueByKey(1), 1.0);
	ASSERT_EQ(*m2.FindValueByKey(2), 2.0);
}

TEST(RedBlackTree, MoveAssignmentTest)
{
	SpaceGameEngine::MapImplement::RedBlackTree<int, double> m1;
	SpaceGameEngine::MapImplement::RedBlackTree<int, double> m2;

	m1.Insert(1, 1.0);
	m1.Insert(2, 2.0);
	ASSERT_EQ(m1.GetSize(), 2);
	ASSERT_EQ(*m1.FindValueByKey(1), 1.0);
	ASSERT_EQ(*m1.FindValueByKey(2), 2.0);
	ASSERT_EQ(m2.GetSize(), 0);
	ASSERT_EQ(m2.FindValueByKey(1), nullptr);
	ASSERT_EQ(m2.FindValueByKey(2), nullptr);

	m2 = std::move(m1);

	ASSERT_EQ(m1.GetSize(), 0);
	ASSERT_EQ(m1.FindValueByKey(1), nullptr);
	ASSERT_EQ(m1.FindValueByKey(2), nullptr);

	ASSERT_EQ(m2.GetSize(), 2);
	ASSERT_EQ(*m2.FindValueByKey(1), 1.0);
	ASSERT_EQ(*m2.FindValueByKey(2), 2.0);
}