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
#include <random>
#include "gtest/gtest.h"
#include "Container/PriorityQueue.hpp"

using namespace SpaceGameEngine;

struct test_priority_queue_object
{
	std::function<void(test_priority_queue_object&)> rel_func;
	test_priority_queue_object()
		: val(0), rel_func([](test_priority_queue_object&) {})
	{
	}
	test_priority_queue_object(int v)
		: val(v), rel_func([](test_priority_queue_object&) {})
	{
	}
	test_priority_queue_object(int v, const std::function<void(test_priority_queue_object&)>& func)
		: val(v), rel_func(func)
	{
	}
	test_priority_queue_object(const test_priority_queue_object& o) noexcept
		: val(o.val), rel_func(o.rel_func)
	{
	}
	test_priority_queue_object(test_priority_queue_object&& o) noexcept
		: val(o.val), rel_func(std::move(o.rel_func))
	{
		o.rel_func = [](test_priority_queue_object&) {};
	}
	~test_priority_queue_object()
	{
		rel_func(*this);
	}
	int val;

	test_priority_queue_object& operator=(const test_priority_queue_object& o)
	{
		val = o.val;
		rel_func = o.rel_func;
		return *this;
	}

	test_priority_queue_object& operator=(test_priority_queue_object&& o)
	{
		val = o.val;
		rel_func = std::move(o.rel_func);
		o.rel_func = [](test_priority_queue_object&) {};
		return *this;
	}

	bool operator<(const test_priority_queue_object& o) const
	{
		return val < o.val;
	}

	bool operator==(const test_priority_queue_object& o) const
	{
		return val == o.val;
	}

	bool operator!=(const test_priority_queue_object& o) const
	{
		return val != o.val;
	}
};

TEST(PriorityQueue, InstanceTest)
{
	PriorityQueue<int> pq1;
	PriorityQueue<int, Less<int>, StdAllocator> pq2({1, 2, 3});
	const PriorityQueue<int, Greater<int>, DefaultAllocator> pq3(10, 1);

	ASSERT_EQ(pq1.GetSize(), 0);
	ASSERT_EQ(pq2.GetSize(), 3);
	ASSERT_EQ(pq3.GetSize(), 10);

	Vector<int, StdAllocator>& l2 = pq2.GetImplement();
	ASSERT_EQ(l2.GetSize(), 3);
	ASSERT_EQ(*l2.GetBegin(), 1);
	ASSERT_EQ(*(l2.GetBegin() + 1), 2);
	ASSERT_EQ(*(l2.GetBegin() + 2), 3);

	const Vector<int>& v3 = pq3.GetImplement();
	ASSERT_EQ(v3.GetSize(), 10);
	for (int i = 0; i < 10; ++i)
		ASSERT_EQ(v3[i], 1);
}

TEST(PriorityQueue, PushAndPopTest)
{
	const int test_size = 1000;
	const int test_up_bound = 100;
	int val_cnt[test_up_bound];
	int val_pool[test_size];
	memset(val_cnt, 0, sizeof(val_cnt));
	memset(val_pool, 0, sizeof(val_pool));

	std::random_device rd;

	for (int i = 0; i < test_size; ++i)
		val_pool[i] = rd() % test_up_bound;

	PriorityQueue<int, Less<int>> pq1;
	ASSERT_EQ(pq1.GetSize(), 0);

	for (int i = 0; i < test_size; ++i)
		pq1.Push(val_pool[i]);
	ASSERT_EQ(pq1.GetSize(), test_size);

	int buf = pq1.GetTop();
	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_LE(buf, pq1.GetTop());
		val_cnt[pq1.GetTop()] += 1;
		pq1.Pop();
		ASSERT_EQ(pq1.GetSize(), test_size - 1 - i);
	}
	ASSERT_EQ(pq1.GetSize(), 0);

	PriorityQueue<int, Greater<int>> pq2;
	ASSERT_EQ(pq2.GetSize(), 0);

	for (int i = 0; i < test_size; ++i)
		pq2.Push(val_pool[i]);
	ASSERT_EQ(pq2.GetSize(), test_size);

	buf = pq2.GetTop();
	for (int i = 0; i < test_size; ++i)
	{
		ASSERT_GE(buf, pq2.GetTop());
		val_cnt[pq2.GetTop()] -= 1;
		pq2.Pop();
		ASSERT_EQ(pq2.GetSize(), test_size - 1 - i);
	}
	ASSERT_EQ(pq2.GetSize(), 0);

	for (int i = 0; i < test_up_bound; ++i)
		ASSERT_EQ(val_cnt[i], 0);
}
