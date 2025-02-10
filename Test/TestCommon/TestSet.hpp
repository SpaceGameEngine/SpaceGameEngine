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
#include "Container/Set.hpp"
#include <functional>
#include <cstring>
#include <vector>

using namespace SpaceGameEngine;

struct test_set_object
{
	std::function<void(test_set_object&)> rel_func;
	test_set_object()
		: val(0), rel_func([](test_set_object&) {})
	{
	}
	test_set_object(int v)
		: val(v), rel_func([](test_set_object&) {})
	{
	}
	test_set_object(int v, const std::function<void(test_set_object&)>& func)
		: val(v), rel_func(func)
	{
	}
	test_set_object(const test_set_object& o) noexcept
		: val(o.val), rel_func(o.rel_func)
	{
	}
	test_set_object(test_set_object&& o) noexcept
		: val(o.val), rel_func(std::move(o.rel_func))
	{
		o.rel_func = [](test_set_object&) {};
	}
	~test_set_object()
	{
		rel_func(*this);
	}
	int val;

	test_set_object& operator=(const test_set_object& o)
	{
		val = o.val;
		rel_func = o.rel_func;
		return *this;
	}

	test_set_object& operator=(test_set_object&& o)
	{
		val = o.val;
		rel_func = std::move(o.rel_func);
		o.rel_func = [](test_set_object&) {};
		return *this;
	}

	bool operator<(const test_set_object& o) const
	{
		return val < o.val;
	}

	bool operator==(const test_set_object& o) const
	{
		return val == o.val;
	}

	bool operator!=(const test_set_object& o) const
	{
		return val != o.val;
	}
};

TEST(Set, InitializerListConstructionTest)
{
	const int test_size = 10;
	int val_pool[test_size];
	memset(val_pool, 0, sizeof(val_pool));
	auto val_rel_func = [&](test_set_object& o) {
		val_pool[o.val] += 1;
	};

	Set<test_set_object>* pm = new Set<test_set_object>(
		{test_set_object(0, val_rel_func),
		 test_set_object(1, val_rel_func),
		 test_set_object(2, val_rel_func),
		 test_set_object(3, val_rel_func),
		 test_set_object(4, val_rel_func),
		 test_set_object(5, val_rel_func),
		 test_set_object(6, val_rel_func),
		 test_set_object(7, val_rel_func),
		 test_set_object(8, val_rel_func),
		 test_set_object(9, val_rel_func)});

	ASSERT_EQ(pm->GetSize(), test_size);
	for (int i = test_size - 1; i >= 0; i--)
	{
		ASSERT_EQ((*pm)[test_set_object(i)].val, i);
	}
	delete pm;
	for (int i = 0; i < test_size; i++)
	{
		// initializer_list can only return const variable, so move is useless
		ASSERT_EQ(val_pool[i], 2);
	}
}