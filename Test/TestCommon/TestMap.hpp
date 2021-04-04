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

using namespace SpaceGameEngine;

struct test_map_object
{
	std::function<void(test_map_object&)> rel_func;
	test_map_object()
		: val(0), rel_func([](test_map_object&) {})
	{
	}
	test_map_object(int v, const std::function<void(test_map_object&)>& func)
		: val(v), rel_func(func)
	{
	}
	~test_map_object()
	{
		rel_func(*this);
	}
	int val;
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